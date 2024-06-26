# Check if Admin-Privileges are available
function Test-IsAdmin {
    ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
}

# get SeTakeOwnership, SeBackup and SeRestore privileges, needs Admin privilege
function Get-SeTakeOwnershipPermissions {
    $import = '[DllImport("ntdll.dll")] public static extern int RtlAdjustPrivilege(ulong a, bool b, bool c, ref bool d);'
    $ntdll = Add-Type -Member $import -Name NtDll -PassThru
    $privileges = @{ SeTakeOwnership = 9; SeBackup =  17; SeRestore = 18 }
    foreach ($i in $privileges.Values) { $null = $ntdll::RtlAdjustPrivilege($i, 1, 0, [ref]0) }
}

# Reset Permissions of a Registry Key (recursive), Prerequsite: needs SeTakeOwnership Privileges!
# Example: Reset-RegistryKeyPermissions "HKLM" "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\demo.exe"
function Reset-RegistryKeyPermissions {
    # Default SID = Administrators, Recursive default true
    param($rootKey, $key, [System.Security.Principal.SecurityIdentifier]$sid = 'S-1-5-32-545', $recurse = $true)

    switch -regex ($rootKey) {
        'HKCU|HKEY_CURRENT_USER'    { $rootKey = 'CurrentUser' }
        'HKLM|HKEY_LOCAL_MACHINE'   { $rootKey = 'LocalMachine' }
        'HKCR|HKEY_CLASSES_ROOT'    { $rootKey = 'ClassesRoot' }
        'HKCC|HKEY_CURRENT_CONFIG'  { $rootKey = 'CurrentConfig' }
        'HKU|HKEY_USERS'            { $rootKey = 'Users' }
    }

    function Take-KeyPermissions {
        param($rootKey, $key, $sid, $recurse, $recurseLevel = 0)

        ### Step 2 - get ownerships of key - it works only for current key
        $regKey = [Microsoft.Win32.Registry]::$rootKey.OpenSubKey($key, 'ReadWriteSubTree', 'TakeOwnership')
        $acl = New-Object System.Security.AccessControl.RegistrySecurity
        $acl.SetOwner($sid)
        $regKey.SetAccessControl($acl)

        ### Step 3 - enable inheritance of permissions (not ownership) for current key from parent
        $acl.SetAccessRuleProtection($false, $false)
        $regKey.SetAccessControl($acl)

        ### Step 4 - recursively repeat steps for subkeys
        if ($recurse) {
            foreach($subKey in $regKey.OpenSubKey('').GetSubKeyNames()) {
                Take-KeyPermissions $rootKey ($key+'\'+$subKey) $sid $recurse ($recurseLevel+1)
            }
        }
    }
  Take-KeyPermissions $rootKey $key $sid $recurse
}

# Delete all ExploitGuard ProcessMitigations
function Remove-All-ProcessMitigations {

    if (!(Test-IsAdmin)){ throw "ERROR: No Administrator-Privileges detected!"; return }
    Get-SeTakeOwnershipPermissions
	
    Get-ChildItem -Path "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options" | ForEach-Object {
      $MitigationItem = $_; 
      $MitigationItemName = $MitigationItem.PSChildName

      # Some Entries are owned by TrustedInstaller and not modifyable, Reset-Permission takes ownership and enables inheritance
      Reset-RegistryKeyPermissions "HKLM" "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\$MitigationItemName"

      Try { 
            if ($MitigationItem.GetValue("MitigationOptions")) 
               { Write-Host "Removing MitigationOptions for:      " $MitigationItemName
                 Remove-ItemProperty -Path $MitigationItem.PSPath -Name "MitigationOptions" -ErrorAction Stop;          
               }
            if ($MitigationItem.GetValue("MitigationAuditOptions")) 
               { Write-Host "Removing MitigationAuditOptions for: " $MitigationItemName
                 Remove-ItemProperty -Path $MitigationItem.PSPath -Name "MitigationAuditOptions" -ErrorAction Stop;          
               }
            if ($MitigationItem.GetValue("UseFilter")) # Mitigation with FilterFullPath
               { Get-ChildItem -Path $MitigationItem.PSPath | ForEach-Object {
                    $FullPathItem = $_
                    if ($FullPathItem.GetValue("FilterFullPath")) {
                       Write-Host "Removing FullPathEntry:              " $MitigationItemName "-" $FullPathItem.GetValue("FilterFullPath")
                       Remove-Item -Path $FullPathItem.PSPath -ErrorAction Stop
                    }
                 }
                 Remove-ItemProperty -Path $MitigationItem.PSPath -Name "UseFilter" -ErrorAction Stop
               }
            if (($MitigationItem.SubKeyCount -eq 0) -and ($MitigationItem.ValueCount -eq 0)) 
               { Write-Host "Removing empty Entry:                " $MitigationItemName
                 Remove-Item -Path $MitigationItem.PSPath -ErrorAction Stop
               }
      } Catch { 
            Write-Host "ERROR:" $_.Exception.Message "- at ($MitigationItemName)" 
      }
    }
}


Remove-All-ProcessMitigations

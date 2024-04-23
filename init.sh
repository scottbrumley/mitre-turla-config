#/bin/bash

echo "#### Update APT ####"
sudo apt update
echo "#### Install Ansible and Collections ####"
sudo apt install -y ansible sshpass
ansible-galaxy collection install ansible.windows
ansible-galaxy collection install community.windows
ansible-galaxy collection install microsoft.ad
ansible-galaxy collection install chocolatey.chocolatey
ansible-galaxy collection install community.general

echo "#### Configure Timestamps for Ansible ####"
sudo sed -i 's/# some basic default values.../callback_whitelist = profile_tasks/' /etc/ansible/ansible.cfg

echo "#### Clone Adversary Emulation Library from GitHub ####"
git clone https://github.com/center-for-threat-informed-defense/adversary_emulation_library.git ../adversary_emulation_library

echo " "
echo "To test Linux servers try this command: ansible lin -i inventory -m ping --ask-pass -u adminuser"
echo " "
echo "To test Windows server try this command: ansible win -i inventory -m win_ping --ask-pass -u adminuser"
echo "If the environment was just built then login into each windows asset to kick of the first build script.  Then run this again"
echo " "
echo "To build the Support Network run: ansible-playbook -i inventory support.yml --ask-pass -u adminuser"

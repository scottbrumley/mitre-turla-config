#/bin/bash

ANSIBLE_HOST_KEY_CHECKING=False
printenv ANSIBLE_HOST_KEY_CHECKING

sudo apt update
sudo apt install -y ansible sshpass
# ansible-galaxy collection install chocolatey.chocolatey
ansible-galaxy collection install ansible.windows
ansible-galaxy collection install community.windows

git clone https://github.com/center-for-threat-informed-defense/adversary_emulation_library.git ../adversary_emulation_library

echo " "
echo "To test Linux servers try this command: ansible lin -i inventory -m ping --ask-pass -u adminuser"
echo " "
echo "To test Windows server try this command: ansible win -i inventory -m win_ping --ask-pass -u adminuser"
echo "If the environment was just built then login into each windows asset to kick of the first build script.  Then run this again"
echo " "
echo "To build the Support Network run: ansible-playbook -i inventory support.yml --ask-pass -u adminuser"

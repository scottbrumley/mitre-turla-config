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
echo "To build the whole environment run:"
echo "ansible-playbook -i inventory runall.yml --ask-pass -u adminuser"

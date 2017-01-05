#!/bin/bash

# run as sudo !
if [ "$(whoami)" != "root" ]; then
	echo "Sorry, you are not root. Please exec as root with sudo."
	exit 1
fi

# check if php is installed, install else
command -v make >/dev/null 2>&1 || { echo "installing make..."; apt-get install --force-yes --yes make >/dev/null; } && { echo "make is installed"; }

# check if php is installed, install else
command -v php >/dev/null 2>&1 || { echo "installing php5-cli..."; apt-get install --force-yes --yes php5-cli >/dev/null || apt-get install --force-yes --yes php7.0-mbstring php7.0-zip php7.0-xml; } && { echo "php-cli is installed"; } 

# check if dot is installed, install else
command -v dot >/dev/null 2>&1 || { echo "installing dot..."; apt-get install --force-yes --yes graphviz >/dev/null; } && { echo "dot is installed"; }

# copy bash completion file for gpnode
if [ -d "/usr/share/bash-completion/completions/" ]; then
	echo "install bash completion";
	cp share/gpnode_completion /usr/share/bash-completion/completions/gpnode
	cp share/gplib_completion /usr/share/bash-completion/completions/gplib
	cp share/gpcomp_completion /usr/share/bash-completion/completions/gpcomp
	cp share/gpproc_completion /usr/share/bash-completion/completions/gpproc
	cp share/gpdevice_completion /usr/share/bash-completion/completions/gpdevice
fi

# udev rules for dreamcam
sh -c 'echo "ATTR{idVendor}==\"04b4\", ATTR{idProduct}==\"1003\", MODE=\"666\"" > /etc/udev/rules.d/dreamcam.rules'
sh -c 'echo "ATTR{idVendor}==\"09fb\", ATTR{idProduct}==\"6001\", MODE=\"666\"" > /etc/udev/rules.d/usb_blaster.rules'
udevadm trigger

echo "Type '. ./setenv.sh' to set PATH temporary or add this to your bashrc file to add permanently GPStudio in your path"
echo "export PATH=\$PATH:$(pwd)/bin"
echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$(pwd)/bin"

exit

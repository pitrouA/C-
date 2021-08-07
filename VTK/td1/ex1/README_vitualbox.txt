Si vous utilisez Virtualbox vous pouvez  installer les extensions   qui permettent d'ajuster la taille de l'écran est de monter un répertoire partagé entre l'hôte(Mac OS ou Windows ou Linux) ———————
Marche à suivre:


installez  ubuntu sous virtualbox

ouvrez un xterm 


sudo apt install linux-headers-$(uname -r) build-essential dkms

reboot


dans VB: menu Devices/Insert Guest Additions CD image


puis dans le xterm: 

sudo adduser $USERNAME vboxsf


shutdown de  Ubuntu 

dans VB creer un dossier partagé permanent


relancer la machine virtuelle ubuntu



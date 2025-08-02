 
VirtualBox

Allow install of vbox linux additions tools :

Login as root;
Update your APT database with apt-get update;
Install the latest security updates with apt-get upgrade;
Install required packages with apt-get install build-essential module-assistant;
Configure your system for building kernel modules by running m-a prepare;
Click on Install Guest Additions… from the Devices menu, then run mount /media/cdrom.
Run sh /media/cdrom/VBoxLinuxAdditions.run, and follow the instructions on screen.


Allow creation of sym link in shared folder :

VBoxManage.exe setextradata YOUR_VM VBoxInternal2/SharedFoldersEnableSymlinksCreate/YOUR_SHARED_FOLDER 1


https://github.com/blchinezu/pocketbook-coolreader


sudo apt install build-essential cmake autoconf qtcreator libgtk2.0-dev libbz2-dev libcurl4-openssl-dev libgif-dev libjpeg-dev

emulator compilation :
sudo cp PBSDK/lib/libinkview.so /usr/lib/libinkview.so

mk -s system ~/PBDEV/system
sudo apt install gnome-themes-standard

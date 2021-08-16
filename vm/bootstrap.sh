#!/bin/bash

# Print commands and exit on errors
set -xe

apt-get update

DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" upgrade
DEBIAN_FRONTEND=noninteractive apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg \
    lsb-release \
    git \
    xinit \
    lxde-core \
    xfce4-terminal \
    mousepad \
    vim \
    curl \
    xterm

# Disable screensaver
apt-get -y remove light-locker

# to enable ndn user having uid=1000, we change vagrant uid
sed -i 's/1000/1100/g'  /etc/passwd /etc/shadow /etc/group /etc/gshadow
chown -R vagrant:vagrant /home/vagrant

# create ndn user
useradd -m -d /home/ndn -s /bin/bash -u 1000 ndn
echo "ndn:ndn" | chpasswd
echo "ndn ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/99_ndn
chmod 440 /etc/sudoers.d/99_ndn

# Ajusting LXDE:
# - Automatically log into the NDN user
# - include xfce4-terminal in the menu bar
# - disables screensaver
sed -i 's/^#autologin-user=$/autologin-user=ndn/' /etc/lightdm/lightdm.conf
sed -i '/id=lxde-x-www-browser.desktop/a\\t}\n\tButton {\n\t    id=xfce4-terminal.desktop' /etc/xdg/lxpanel/LXDE/panels/panel
sed -i '/@xscreensaver/d' /etc/xdg/lxsession/LXDE/autostart

# creating swap file, otherwise waf will complain about virtual memory exhausted
cat <<EOF >/etc/rc.local
#!/bin/bash

if [ ! -f /swapfile ]; then
   fallocate -l 1G /swapfile
   dd if=/dev/zero of=/swapfile bs=1024 count=1048576
   chmod 600 /swapfile
   mkswap /swapfile
fi
swapon /swapfile
EOF
chmod +x /etc/rc.local

# Docker
curl -fsSL https://download.docker.com/linux/debian/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg
echo \
  "deb [arch=amd64 signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/debian \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y docker-ce docker-ce-cli containerd.io
usermod -aG docker ndn

apt clean

# Pull docker image to be used in the tutorial
docker pull emrevoid/ndnsim:2.8-debug

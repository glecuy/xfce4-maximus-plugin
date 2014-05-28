xfce4-maximus
=============

maximus plugin for the Xfce panel. (netbook windows maximizer)

This is maximus plugin for the Xfce panel, version 4.8 or higher. It
permits to manage windows maximization and decorations.
It is mainly intended to be used on netbook or other small screen computers.
It tries to maximalize most applications window and merges title bar and 
panel bar to save vertical space.

Screenshot
==========
![netbook](https://raw.githubusercontent.com/glecuy/xfce4-maximus/master/screenshots/xfce4-maximus.png)


Build/Installation
==================
Configuration - build :

```sh
./autogen.sh --prefix=/usr

make 
```
To generate a distrib independent intallation archive :

```sh
make install_tar
```
To generate a a debian compatible intallation packet (.deb):

```sh
make deb
```

binary package for i386 architecture: [xfce4-maximus_i386.deb]



Information about original Maximus Package
------------------------------------------

It's a deamon used in Ubuntu Netbook Edition (10.04) See package description at [maximus]:



[xfce4-maximus_i386.deb]:http://frilouze.chez.com/deb/xfce4-maximus_i386.deb

[maximus]:https://launchpad.net/ubuntu/+source/maximus




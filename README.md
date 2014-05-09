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
============
```sh
./autogen.sh --prefix=/usr

make 

make install_tar
```

This target shall generate a distrib independent intallation archive ...

Original Maximus Package
------------------------

It's a deamon used in Ubuntu Netbook Edition (10.04) See package description at [site] [1]:


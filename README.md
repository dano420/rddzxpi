# RDDZ XPI
RDDZ XPI is an XPath and JSONiq interpreter tool. It supports XPath 3.0 expressions and can retrieve source code from remote url (or local file). It runs on Mac, Windows and Linux (developed with [Qt](https://www.qt.io) framework).


## Description
You're looking for an application who can interpret your XPath 3.0 expressions and functions and/or JSONiq ? RDDZ XPI is what you're looking for !  It's a GUI interface for the powerfull [xidel](https://github.com/benibela/xidel) project.

The interface is very simple
![RDDZ XPI interface](http://blog.rddz-tools.fr/files/rddzxpi/interface.png)

But so efficient
![RDDZ XPI in action](http://blog.rddz-tools.fr/files/rddzxpi/example.png)

## Installation

### Needed dependencies
You need to download specific binary/files for your platform and place it into the `dist` directory :
* [xidel](http://www.videlibri.de/xidel.html#downloads)

### System dependencies (especially for windows platform)
* [openssl](https://www.openssl.org/source/)

### Compilation

```
git clone https://github.com/rddztools/rddzxpi.git
cd rddzxpi
qt-creator rddz_xpi.pro
```

## Contact / Info
This project has been successfully built with QT 5.13.0 on Windows, Mac and Linux.

If you need help for anything XPath related, you can contact us through www.rddz.fr

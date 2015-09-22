# Installation from sources #

  1. Make sure that the following packages are installed:
    * libqt4-dev
    * sqlite
    * libqwt-dev (>=6.0)
    * libusb-dev
  1. Fetch sources from SVN repository
  1. Run _qmake_
  1. Run _make_

# Fix permissions of DS9490 USB adaptor #

Avoid auto-loading of ds2490.ko by adding to _/etc/modprobe.d/blacklist.conf_:

```
blacklist ds2490
```

Create /etc/udev/rules.d/80-iassist.rules

```
SUBSYSTEM=="usb",ACTION=="add",ATTR{idVendor}=="04fa",ATTR{idProduct}=="2490", MODE="666"
```

Restart udev and/or the whole system to apply changes.
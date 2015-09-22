**iButton Assist for Android**: We are about to move on smartphones. A new version of iAssist for Android phones can be downloaded for free from the [Google Play](https://play.google.com/store/apps/details?id=ch.ethz.iassist) store.

<a href='https://play.google.com/store/apps/details?id=ch.ethz.iassist'><img src='http://developer.android.com/images/brand/en_app_rgb_wo_45.png' alt='iButton Assist for Android' /></a>

# Rapid deployment of iButton miniature logger devices #

GUI application for the mass deployment of iButton miniature (temperature) logger devices. We are currently starting to go public with our software, several updates are coming up in the next few weeks.

For getting an idea of what this application does, please refer to the following publications:

  * _Matthias Keller and Guido Hungerbuehler and Oliver Knecht and Suhel Sheikh and Jan Beutel and Stefanie Gubler and Joel Fiddes and Stephan Gruber: Demo Abstract: iAssist – Rapid Deployment and Maintenance of Tiny Sensing Systems, Proc. of the 8th ACM Conference on Embedded Networked Sensor Systems (SenSys 2010), page 401--02, 2010_

  * _Stefanie Gubler and Joel Fiddes and Matthias Keller and Stephan Gruber: Scale-dependent Measurement and Analysis of Ground Surface Temperature Variability in Alpine Terrain, The Cryosphere, 5(2):431-443, 2011_

Easy way to get started:

```
qmake
make
./iAssist
```

Dependencies: Qt 4.7/4.8, QWT 6.0, libusb, sqlite
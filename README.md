# KonverterMidiUSB Host
USB to Serial Midi DIN bidrectionnal converter

This project based on Arduino Uno allows you to get a very economical USB MIDI converter, without any soldering.

The necessary components are as follows:
- An Arduino Uno
- An Arduino USB Host shield

![](https://d1xahwiwo4b49p.cloudfront.net/2538-large_default/usb-host-shield-support-google-android-adk-arduino.jpg)

- An Arduino  Midi shield (the simplest version)

![](http://img.banggood.com/thumb/water/oaupload/banggood/images/5E/59/01090afa-5632-4006-882b-e7f50ffca98d.jpg)

Before compiling, you must install the USB Host libray from the Arduino IDE, and copy the midiXParser library to your own Arduino/libraries path.

https://github.com/TheKikGen/midiXparser/archive/master.zip

Assemble the Uno Host shield, and Midi shield, then upload the firmware to the Uno. Don't forget to set the switch to OFF on the Midi shield, before uploading, then ON again to use it.

For greater simplicity, the HEX files compiled from the latest version are provided to be used with AVRDude.

Plug an USB keyboard in the host shield, a midi cable to the midi out and to a synth.
You are now able to use your favorite USB midi keyboard with any standard midi DIN device.



# KonverterMidiUSB
USB to Serial Midi bidrectionnal converter

This project based on Arduino Uno allows you to get a very economical USB MIDI converter, without any soldering.

The necessary components are as follows:
- An Arduino Uno
- An Arduino USB Host shield

![](https://d1xahwiwo4b49p.cloudfront.net/2538-large_default/usb-host-shield-support-google-android-adk-arduino.jpg)

- An Arduino  Midi shield (the simplest version)

![](https://www.google.fr/url?sa=i&source=images&cd=&ved=2ahUKEwjTj46b_qTjAhWIkxQKHSz1CU0QjRx6BAgBEAU&url=http%3A%2F%2Fgilles.thebault.free.fr%2Fspip.php%3Farticle54&psig=AOvVaw3KSrMTAAPDyTYYDcJU-C6-&ust=1562663726037331)

Before compiling, you must install the USB Host libray from the Arduino IDE.
Assemble the Uno Host shield, and Midi shield, then upload the firmware to the Uno.

Plug an USB keyboard in the host shield, a midi cable to the midi out and to a synth.
You are now able to use your favorite USB midi keyboard with any standard midi DIN device.



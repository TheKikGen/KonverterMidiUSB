# KonverterMidiUSB
USB to Serial Midi bidrectionnal converter

This project based on Arduino Uno allows you to have a very economical USB MIDI converter, without any soldering.

The necessary components are as follows:
- An Arduino Uno
- An Arduino USB Host shield
- An Arduino  Midi shield (the simplest version)

Before compiling, you must install the USB Host libray from the Arduino IDE.
Assemble the Uno Host shield, and Midi shield, then upload the firmware to the Uno.

Plug an USB keyboard in the host shield, a midi cable to the midi out and to a synth.
You are now able to use your favorite USB midi keyboard with any standard midi DIN device.


Getting up with Teensyduino could hardly be easier.
Moving to raw AVR, though, is a bit of a PITA.
Here's some research.


## Test Programs

Most of these programs come configured for the Teensy++2.0 but
check the Makefile to be sure.

* streamer: writes messages that hid_listen can read.  It shows that it
  takes around 1 second for usb_debug_only to connect, and that the first ~16 characters
  are buffered.

* watcher: shows status of D3.  Demonstrates simple I/O.

* pwm: shows how to use Clock1 to drive PWMs on pins OC1A and OC1B (PB5 and PB6)


## Thoughts

#### usb_debug_only

usb_debug_only lets you read your program's print statements.  Because it's
buffered, printing small amounts of data shouldn't slow you down too much.
Because it's meant to be small and unobtrusive, it can't read any input.
If you want full-duplex communication with your device, you need to use
http://www.pjrc.com/teensy/usb_serial.html and be ready for more issues.

I had an A3949 motor controller hooked to the teensy...  if the motor was
in use in a certain way, and I opened the serial console, it would fry itself
and sometimes take out some pins on the Teensy.  I think the USB code would
cause some pins to glitch, causing the A3949 to short.
I don't know if usb_serial is immune to this or not but the lesson is clear:
this does not happen when I'm using usb_debug_only.


#### automatic programming

In Teensyduino, you don't need to push the reset button to write new firmware.
This is because it continually listens for a reboot command over USB.
When the teensy_reboot utility sends it, teensyduino resets the device.
Bare metal, of course, doesn't allow stuff like that to run in the background.
No problem, just buy two teensys, connect one to the reset pin of the other,
then pass the -r flag to teensy_loader_cli.


#### makefile

'make program' patch has bee added to http://www.pjrc.com/teensy/loader_cli.html

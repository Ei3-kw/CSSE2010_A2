#!/usr/bin/fish
# Monitor the output of the program with screen
# screen sucks, it doesn't disconnect properly
#screen /dev/ttyUSB0 19200
# use tio instead
tio -b 19200 /dev/ttyUSB0
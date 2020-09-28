# Ground Station Control Panel

This is the repository of the Qt based control panel. It contains a interpreter that processes serial messages sent toi and from the ground station.

![Indev Image](/Documentation/img/20200723.png "In-Dev image 20/02/2020")

It conforms to the FCP document for FOSSASAT-2 as of:
- 26/06/2020 (RB)
- 23/07/2020 (RB)

## About the protocols
The ground station wraps the FCP packets with a header that specifies extra data such as direction (to or from ground stations), these are defined in the /docs/ directory.

The control panel's MainWindow() Qt object class is the main section of code that contains all the interpreting, message to command functions and switches.

## Navigating this repository.

Release 32-bit/ - This directory contains a prebuild 32-bit .exe for windows 10.
Ground Station Test Arduino Sketch - This directory contains the sketch used to simulate the ground station.
Documentation/ - This directory contains documents, at the moment it only contains information reguarding the ground station's protocol.
Control Panel Source Code/ - This directory contains the Qt C++ source code files and project files (including required libs).

## Versions
0.1.2 - Camera control, ephemerides controller matrix

0.1.3 - Compatability with 1B ui elements
	  - Wipe eeprom
	  - Record solar cells
	  - Camera Capture
	  - Picture burst
	  - Set ADCS controller
	  - Get statistics
	  - Set adcs ephemerides
	  - Get statistics

0.1.4 - Record solar cells 1B
	  - System information decoding 1b and 2
	  - get system information

0.1.5 - key entry now works with hex ascii e.g. 0x01,0x02,0x04.etc

0.2.0 - refactor entire system to accound for inbound and outbound datagram differences.

0.2.1 - removed "always on top" flag from windows
	  - fixed doppler timer being ran when disabled.
	  - changed doppler correction system to print a more descriptive method.
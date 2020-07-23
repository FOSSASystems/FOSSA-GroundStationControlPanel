# Ground Station Control Panel

This is the repository of the Qt based control panel. It contains a interpreter that processes serial messages sent toi and from the ground station.

![Indev Image](/Documentation/img/20200723.png "In-Dev image 20/02/2020")

It conforms to the FCP document for FOSSASAT-2 as of 26/06/2020.

## About the protocols
The ground station wraps the FCP packets with a header that specifies extra data such as direction (to or from ground stations), these are defined in the /docs/ directory.

The control panel's MainWindow() Qt object class is the main section of code that contains all the interpreting, message to command functions and switches.

## Navigating this repository.

Release 32-bit/ - This directory contains a prebuild 32-bit .exe for windows 10.
Ground Station Test Arduino Sketch - This directory contains the sketch used to simulate the ground station.
Documentation/ - This directory contains documents, at the moment it only contains information reguarding the ground station's protocol.
Control Panel Source Code/ - This directory contains the Qt C++ source code files and project files (including required libs).

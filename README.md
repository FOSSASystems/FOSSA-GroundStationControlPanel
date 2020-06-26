# Ground Station Control Panel

This is the repository of the Qt based control panel. It contains a interpreter that processes serial messages sent toi and from the ground station.

![Indev Image](/docs/img/26062020.png "In-Dev image 26/06/2020")

It conforms to the FCP document for FOSSASAT-2 as of 26/06/2020.

## About the protocols
The ground station wraps the FCP packets with a header that specifies extra data such as direction (to or from ground stations), these are defined in the /docs/ directory.

The control panel's MainWindow() Qt object class is the main section of code that contains all the interpreting, message to command functions and switches.

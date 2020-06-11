# FOSSA Ground Station Serial Protocol

### Terminology

In this document, the following terminology is used:
* __Ground Station__ - microcontroller unit with direct access to radio hardware (typically an Arduino or ESP32 board with SX127x/6x device), connected to PC via Serial port or similar.
* __Control Panel__ - PC-based application which controls the ground station.
* __Datagram__ - Protocol data unit of the Serial protocol for exchanging information between ground station and control panel. Not to be confused with FCP frame as described in [FCP specification](https://github.com/FOSSASystems/FOSSASAT-1/blob/master/FOSSA%20Documents/FOSSASAT-1%20Comms%20Guide.pdf) (section 3).

### Datagram structure

| Control | Length | Payload |
| ------- | ------ | ------- |
| 1 byte  | 1 byte | N bytes |

Each of the above sections is detailed below.

### Control byte

| Direction | Function ID |
| --------- | ----------- |
| 1 bit     | 7 bits      |

Direction bit set to __0__ marks datagrams sent from control panel to ground station.  
Direction bit set to __1__ marks datagrams sent from ground station to control panel.

Function ID specifies the type of data sent in the payload section.

* __0x00__ - FCP frame transfer. Payload is only the FCP frame to uplink when sent with direction = 0, or 2-byte RadioLib reception status code followed by the received FCP frame when sent with direction = 1.
* __0x01__ - Ground station configuration change request/result. Payload is the ground station configuration to set when sent with direction = 0, or configuration result when sent with direction = 1 (typically a 2-byte RadioLib status code).

### Length byte

The total length of payload section. The receiver shall wait for complete datagram reception before processing the payload. If the full datagram is not received within a reasonable timeout period, the datagram shall be dropped. Waiting for full datagram should not interfere with normal operation of the ground station or control panel. Length section is only mandatory for datagrams with payload.

### Payload

The payload section is specific for each function ID. Some function IDs may not have a payload section.

### Examples

* Control panel to ground station CMD_PING uplink request:  
`00 0b 46 4f 53 53 41 53 41 54 2d 32 00`  
Direction bit = 0 (control panel to ground station)  
Function ID = 0x00 (FCP frame transfer)  
Length = 0x0b (11 bytes)  
Payload = FCP Frame (`"FOSSASAT-2" <CMD_PING>`)

* Ground station to control panel RESP_PONG downlink report:  
`80 0d 00 00 46 4f 53 53 41 53 41 54 2d 32 20`  
Direction bit = 1 (ground station to control panel)  
Function ID = 0x00 (FCP frame transfer)  
Length = 0x0d (13 bytes)  
Reception status = 0 (RadioLib ERR_NONE status code)
Payload = FCP Frame (`"FOSSASAT-2" <RESP_PONG>`)

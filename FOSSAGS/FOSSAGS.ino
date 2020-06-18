/*
   FOSSA Ground Station Sketch

   To be used with FOSSA Ground Station Control Panel app.

   For details on the Serial protocol, see
   https://github.com/FOSSASystems/FOSSA-GroundStationControlPanel/blob/master/docs/Serial%20Protocol.md
*/

// include all libraries
#include <RadioLib.h>
#include <FOSSA-Comms.h>

// configuration
#define USE_SX126X                          // uncomment to use SX126x
#define SERIAL_PORT           Serial        // Serial port to use
#define DEFAULT_BAUDRATE      115200        // default baud rate

// pin definitions
#define CS                    10            // SPI chip select
#define DIO                   2             // DIO0 for SX127x, DIO1 for SX126x
#define NRST                  RADIOLIB_NC   // NRST pin (optional)
#define BUSY                  9             // BUSY pin (SX126x-only)

// default modem configuration
#define LORA_FREQUENCY        436.7         // MHz
#define FSK_FREQUENCY         436.9         // MHz
#define BANDWIDTH             125.0         // kHz
#define SPREADING_FACTOR      11            // -
#define CODING_RATE           8             // 4/8
#define SYNC_WORD             0x12          // used as LoRa "sync word", or twice repeated as FSK sync word (0x1212)
#define OUTPUT_POWER          20            // dBm
#define CURRENT_LIMIT         140           // mA
#define LORA_PREAMBLE_LEN     8             // symbols
#define BIT_RATE              9.6           // kbps
#define FREQ_DEV              5.0           // kHz SSB
#define RX_BANDWIDTH          39.0          // kHz SSB
#define FSK_PREAMBLE_LEN      16            // bits
#define DATA_SHAPING          0.5           // BT product
#define TCXO_VOLTAGE          1.6           // V
#define WHITENING_INITIAL     0x1FF         // initial whitening LFSR value

// FOSSA Ground Station Serial Protocol definitions
#define FGSP_PANEL_TO_STATION               (0 << 7)
#define FGSP_STATION_TO_PANEL               (1 << 7)
#define FGSP_OP_ID_HANDSHAKE                0x00
#define FGSP_OP_ID_FRAME_TRANSFER           0x01
#define FGSP_OP_ID_CONFIGURATION            0x02

// set up radio module
#ifdef USE_SX126X
SX1268 radio = new Module(CS, DIO, NRST, BUSY);
#else
SX1278 radio = new Module(CS, DIO, NRST, RADIOLIB_NC);
#endif

// flags
volatile bool interruptEnabled = true;
volatile bool transmissionReceived = false;

// datagram variables
bool receivingDatagram = false;
uint8_t controlByte, datagramLen;
uint8_t datagramBuff[256];
uint8_t datagramBuffPos = 0;

void sendDatagram(uint8_t ctrl, uint8_t len = 0, uint8_t* payload = NULL) {
  // create buffer
  uint8_t* buff = new uint8_t[2 + len];
  uint8_t* buffPtr = buff;

  // add control byte and length
  *buffPtr++ = (FGSP_STATION_TO_PANEL | ctrl);
  *buffPtr++ = len;

  // add payload
  if(payload != NULL) {
    memcpy(buffPtr, payload, len);
  }

  // send data
  for(uint16_t i = 0; i < ((uint16_t)2 + len); i++) {
    SERIAL_PORT.write(buff[i]);
  }

  // delete buffer
  delete[] buff;
}

void sendConfigResponse(int16_t state) {
  uint8_t buff[sizeof(state)];
  memcpy(buff, &state, sizeof(state));
  sendDatagram(FGSP_OP_ID_CONFIGURATION, sizeof(state), buff);
}

void sendFrameDatagram(int16_t state, uint8_t frameLen, uint8_t* frame) {
  // create buffer
  uint8_t* buff = new uint8_t[2 + frameLen];
  uint8_t* buffPtr = buff;

  // add RadioLib status code
  memcpy(buffPtr, &state, sizeof(state));
  buffPtr += sizeof(state);

  // add FCP buffer
  memcpy(buffPtr, frame, frameLen);
  buffPtr += frameLen;

  // send data
  sendDatagram(FGSP_OP_ID_FRAME_TRANSFER, 2 + frameLen, buff);

  // delete buffer
  delete[] buff;
}

void sendFrame(uint8_t* frame, uint8_t frameLen) {
  // disable reception interrupt
  interruptEnabled = false;
  #ifdef USE_SX126X
    radio.clearDio1Action();
  #else
    radio.clearDio0Action();
  #endif

  // send the uplink frame data
  radio.transmit(frame, frameLen);
  
  // for some reason, when using SX126x GFSK and listening after transmission,
  // the next packet received will have bad CRC,
  // and the data will be the transmitted packet
  // the only workaround seems to be resetting the module
  #if defined(USE_GFSK) && defined(USE_SX126X)
    radio.sleep(false);
    delay(10);
    setGFSK();
  #endif
  
  // set radio mode to reception
  #ifdef USE_SX126X
    radio.setDio1Action(onInterrupt);
  #else
    radio.setDio0Action(onInterrupt);
  #endif
  radio.startReceive();
  interruptEnabled = true;
}

void processDatagram() {
  // execute opID
  uint8_t opID = controlByte & 0x7F;
  switch(opID) {
    
    case FGSP_OP_ID_FRAME_TRANSFER:
      sendFrame(datagramBuff, datagramBuffPos + 1);
      break;
      
    case FGSP_OP_ID_CONFIGURATION: {
      // get the parameters
      uint8_t modem = 0;
      memcpy(&modem, datagramBuff + 0, sizeof(modem));
      float freq = 0;
      memcpy(&freq, datagramBuff + 1, sizeof(freq));
      int8_t power = 0;
      memcpy(&power, datagramBuff + 5, sizeof(power));
      float ocp = 0;
      memcpy(&ocp, datagramBuff + 6, sizeof(ocp));
      float loraBw = 0;
      memcpy(&loraBw, datagramBuff + 10, sizeof(loraBw));
      uint8_t loraSf = 0;
      memcpy(&loraSf, datagramBuff + 14, sizeof(loraSf));
      uint8_t loraCr = 0;
      memcpy(&loraCr, datagramBuff + 15, sizeof(loraCr));
      uint16_t loraPre = 0;
      memcpy(&loraPre, datagramBuff + 16, sizeof(loraPre));
      float gfskBr = 0;
      memcpy(&gfskBr, datagramBuff + 18, sizeof(gfskBr));
      float gfskFdev = 0;
      memcpy(&gfskFdev, datagramBuff + 22, sizeof(gfskFdev));
      float gfskRxbw = 0;
      memcpy(&gfskRxbw, datagramBuff + 26, sizeof(gfskRxbw));
      float gfskSh = 0;
      memcpy(&gfskSh, datagramBuff + 30, sizeof(gfskSh));
      uint16_t gfskPre = 0;
      memcpy(&gfskPre, datagramBuff + 34, sizeof(gfskPre));

      // set the configuration
      int16_t state = ERR_NONE;
      if(modem == 0x00) {
        state = radio.begin(freq, loraBw, loraSf, loraCr, SYNC_WORD, power, ocp, loraPre, TCXO_VOLTAGE);
      } else if(modem == 0x01) {
        state = radio.beginFSK(freq, gfskBr, gfskFdev, gfskRxbw, power, ocp, gfskPre, TCXO_VOLTAGE);
      } else {
        state = ERR_WRONG_MODEM;
      }

      // send the result
      sendConfigResponse(state);
    } break;
    
    case FGSP_OP_ID_HANDSHAKE:
      sendDatagram(FGSP_OP_ID_HANDSHAKE);
      break;
    
    default: 
      // TODO process unknown datagrams
      break;
  
  }
}

// radio ISR
void onInterrupt() {
  if (!interruptEnabled) {
    return;
  }

  transmissionReceived = true;
}

void setup() {
  // open serial port
  SERIAL_PORT.begin(DEFAULT_BAUDRATE);
  while(!SERIAL_PORT);

  // wait for the handshake datagram
  bool handshakeReceived = false;
  while(!handshakeReceived) {
    // read incoming bytes one at a time
    while(!SERIAL_PORT.available());
    uint8_t controlByte = SERIAL_PORT.read();

    // check opID
    if(controlByte == (FGSP_PANEL_TO_STATION | FGSP_OP_ID_HANDSHAKE)) {
      // got handshake, wait for the length byte
      // TODO timeout
      while(!SERIAL_PORT.available());
      SERIAL_PORT.read();
      
      // send response
      sendDatagram(FGSP_OP_ID_HANDSHAKE);
      handshakeReceived = true;
    }
  }

  // set default modem configuration
  int16_t state = radio.begin(LORA_FREQUENCY,
                              BANDWIDTH,
                              SPREADING_FACTOR,
                              CODING_RATE,
                              SYNC_WORD,
                              OUTPUT_POWER,
                              CURRENT_LIMIT,
                              LORA_PREAMBLE_LEN,
                              TCXO_VOLTAGE);
  if(state != ERR_NONE) {
    sendConfigResponse(state);
  }

  state = radio.setCRC(true);
  if(state != ERR_NONE) {
    sendConfigResponse(state);
  }

  // send configuration response
  sendConfigResponse(state);
}

void loop() {
  // check serial data
  if(SERIAL_PORT.available()) {
    // new byte, check datagram start
    if(!receivingDatagram) {
      // first byte, get control byte and wait for datagram length
      controlByte = SERIAL_PORT.read();

      // wait for datagram length
      // TODO timeout
      while(!SERIAL_PORT.available());
      datagramLen = SERIAL_PORT.read();

      // set dataram variables
      receivingDatagram = true;
      datagramBuffPos = 0;
    
    } else {
      // payload byte, add it to buffer
      datagramBuff[datagramBuffPos++] = SERIAL_PORT.read();
    
    }

    // check if this byte completes the current datagram
    // TODO timeout
    if((datagramLen == 0) || (datagramBuffPos == datagramLen)) {
      // got complete datagram, process it
      processDatagram();

      // reset datagram flag
      receivingDatagram = false;
    }
  }

  // check if new data were received
  if (transmissionReceived) {
    // disable reception interrupt
    interruptEnabled = false;
    transmissionReceived = false;

    // read received data
    size_t respLen = radio.getPacketLength();
    uint8_t* respFrame = new uint8_t[respLen];
    int16_t state = radio.readData(respFrame, respLen);

    // send it to control panel
    sendFrameDatagram(state, (uint8_t)respLen, respFrame);

    // enable reception interrupt
    delete[] respFrame;
    radio.startReceive();
    interruptEnabled = true;
  }
}

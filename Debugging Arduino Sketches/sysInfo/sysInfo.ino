void setup() {
  Serial.begin(9600);
}


void loop() {
  uint8_t sysInfo[] = {
    0x81, 0x25,  									  // 2 bytes
    'R', 'R', 										  // 2 bytes
    'F', 'O', 'S', 'S', 'A', 'S', 'A', 'T', '-', '2', // 10 bytes
    0x23, 0x17,
    0x00,                       // mppt output voltage
    0x83, 0xFF,                 // mppt output current
    0x27, 0xD3, 0x1E, 0x5F,     // onboard unix timestamp
    0x2B,                       // power configuration
    0x01, 0x00,                 // reset counter
    0x00,                       // XA volt
    0x00,                       // XB volt
    0x00,                       // ZA volt
    0x00,                       // ZB volt
    0x00,                       // y volt
    0xF9, 0x63,                 // batt temp
    0xFC, 0x09,                 // obc temp
    0x00, 0x00, 0x00, 0x00      // flash CRC
  };

  Serial.write(sysInfo, sysInfo[1] + 2);
  delay(3000);
}

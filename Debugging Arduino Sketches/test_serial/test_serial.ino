#include <SoftwareSerial.h>

SoftwareSerial SerialB(2, 3); //rx, tx

void setup()
{
	SerialB.begin(9600);
	
	// put your setup code here, to run once:
	Serial.begin(9600);
}

void loop()
{
	/* Press S to send messages from the arduino to the control panel.
	 *  
	// listen to the main com port 6 and echo to the serialb.
	while (Serial.available() > 0)
	{
		char v = Serial.read();
		if (v == 'S')
		{
			Serial.println("Wrote");

			
			SerialB.write(0x81);
			SerialB.write(0x0d);
			SerialB.write((uint8_t)0x00);
			SerialB.write((uint8_t)0x00); 
			SerialB.write(0x46);
			SerialB.write(0x4f);
			SerialB.write(0x53);
			SerialB.write(0x53);
			SerialB.write(0x41); 
			SerialB.write(0x53); 
			SerialB.write(0x41); 
			SerialB.write(0x54); 
			SerialB.write(0x2d); 
			SerialB.write(0x32); 
			SerialB.write(0x20);

			SerialB.write((uint8_t)0b10000000);
			SerialB.write((uint8_t)0x00);
		}
	}*/

	/*
	 * Wait for the handshake bytes and display if we receive them.
	 *
	 */
	/*while (SerialB.available() > 0)
	{
		uint8_t c = SerialB.read();
		
        // convert byte into 2 character hex (1 hex = 4 bits)
        char hexStr[2];
        sprintf(&(hexStr[0]), "%02x", (uint8_t)c);
        Serial.print(hexStr);
	}*/
	while (SerialB.available() > 0)
	{
		char c = SerialB.read();
		
        // convert byte into 2 character hex (1 hex = 4 bits)
        //char hexStr[2];
        //sprintf(&(hexStr[0]), "%02x\r\n", (uint8_t)c);
        Serial.print(c);
	}
}

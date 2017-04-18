/*
	White Card UID (Used to pass):14 0D 56 45 
	Blue magnetic buckle UID (Used to error):23 6D FB A4

	Purple magnetic buckle origin UID: 0B EA 3C BC

	sccot RFID:0B EA 3C BC 
	RFID Module PIN:
		Reset Pin (RST) :9 - Orange
		SS (SDA) :10 - Red
		MOSI : 11 - Brown
		MISO : 12 - Black
		SCK : 13 - White 
		GND : GND - gray
		3.3V : 3.3V - purple
	Servo:
		GND-GND(Brown) VCC-VCC(Red) 3-Signal(Orange)
	close LED: 6-GND(Black) 7-VCC (White)
	Open LED: 4-GND (Purple) 5-VCC (Orange)
	Switch: 8-VCC+pullup 0-GND

	Reference:
		https://github.com/miguelbalboa/rfid
		Arduino Servo Example
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>


#define SS_PIN 10
#define RST_PIN 9

#define Open 5
#define Close 7
#define Switch 8
#define Servo_Pin 3

#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF} //DEFINE NEW UID HERE . 

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
Servo lockservo;  // create servo object to control a servo
MFRC522::MIFARE_Key key; 


byte nuidPICC[4]; // Init array that will store new NUID 
byte LockPICC[4]={0x14,0x0D,0x56,0x45}; // Init Lock 
boolean state=false;
byte time;

void setup() 
{ 
	// Pinmode Servo Declare
	lockservo.attach(Servo_Pin); //Servo Declare
	lockservo.write(40);

	pinMode(Close, OUTPUT); // Close LED Declare
	pinMode(6, OUTPUT);digitalWrite(6,LOW); //Close GND Declare.
	digitalWrite(Close,HIGH); //Initial.

	pinMode(Open, OUTPUT);// Open LED High Declare
	pinMode(4, OUTPUT);digitalWrite(4,LOW); // Open LED GND Declare 
  
	digitalWrite(Open,LOW); //Initial

	pinMode(Switch, INPUT_PULLUP);
	pinMode(0, OUTPUT);
//----------------------------
	
	//Serial Declare
	Serial.begin(9600);
  	Serial.println(""); // Initial Space.
	SPI.begin(); // Init SPI bus
	rfid.PCD_Init(); // Init MFRC522 

	for (byte i = 0; i < 6; i++) 
	{
		key.keyByte[i] = 0xFF;
	}

	Serial.println(F("This code scan the MIFARE Classsic NUID."));
	if(digitalRead(Switch)==1)
		Serial.println("The Mode is Read");
	else
		Serial.println("The Mode is Write");
	
	Serial.print(F("Using the following key:"));
	printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
	time=millis(); //Initial time , unit is ms .
}
 
void loop() 
{
	if(state==true && millis()-time>=5000) //5000ms = 5s
	{
		time=millis(); // Reset new time.
		state=false;
		digitalWrite(Close,HIGH);
		digitalWrite(Open, LOW);
		lockservo.write(40);
	}
	// Look for new cards
	if ( ! rfid.PICC_IsNewCardPresent())
	return;

	// Verify if the NUID has been readed
	if ( ! rfid.PICC_ReadCardSerial())
	return;
  
	Serial.print(F("PICC type: "));
	MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
	Serial.println(rfid.PICC_GetTypeName(piccType));

	// Check is the PICC of Classic MIFARE type
	if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
	piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
	piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
	{
		Serial.println(F("Your tag is not of type MIFARE Classic."));
		return;
	}

	if (digitalRead(Switch)==1) //Read Mode
	{
		if (rfid.uid.uidByte[0] != nuidPICC[0] || 
		rfid.uid.uidByte[1] != nuidPICC[1] || 
		rfid.uid.uidByte[2] != nuidPICC[2] || 
		rfid.uid.uidByte[3] != nuidPICC[3] ) 
		{
			Serial.println(F("A new card has been detected."));

			// Store NUID into nuidPICC array
			for (byte i = 0; i < 4; i++) 
			{
			  nuidPICC[i] = rfid.uid.uidByte[i];
			}

			Serial.println(F("The NEW UID tag is:"));
			Serial.print(F("In hex: "));
			printHex(rfid.uid.uidByte, rfid.uid.size);
			Serial.println();
			Serial.print(F("In dec: "));
			printDec(rfid.uid.uidByte, rfid.uid.size);
			Serial.println();
			if(LockPICC[0] == nuidPICC[0] && LockPICC[1] == nuidPICC[1] && LockPICC[2] == nuidPICC[2] && LockPICC[3] == nuidPICC[3])
			{
       			Serial.println("Unlock!!!");
				digitalWrite(Close, LOW);
				digitalWrite(Open,HIGH);
				lockservo.write(120);
				state=true;
				time=millis(); // Reset new time.
			}
			else
			{
		        Serial.println("Unlock error!!!");
				digitalWrite(Close,HIGH);
				digitalWrite(Open, LOW);
				lockservo.write(40);
				state=false;
			}

		}
		else Serial.println(F("Card read previously."));
	}
	else
	{
		digitalWrite(Close, LOW);digitalWrite(Open,LOW); // No power is Write Mode.
		byte newUid[] = NEW_UID;
		if ( rfid.MIFARE_SetUid(newUid, (byte)4, true) ) 
		{
		    Serial.println(F("Wrote new UID to card."));
		}
		  
		  // Halt PICC and re-select it so DumpToSerial doesn't get confused
		rfid.PICC_HaltA();
		if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ) 
		{
		  return;
		}
		  
		// Dump the new memory contents
		Serial.println(F("New UID and contents:"));
		rfid.PICC_DumpToSerial(&(rfid.uid));
	}
	

	// Halt PICC
	rfid.PICC_HaltA();

	// Stop encryption on PCD
	rfid.PCD_StopCrypto1();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize)
{
	for (byte i = 0; i < bufferSize; i++) 
	{
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], HEX);
	}
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) 
{
	for (byte i = 0; i < bufferSize; i++) 
	{
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], DEC);
	}
}

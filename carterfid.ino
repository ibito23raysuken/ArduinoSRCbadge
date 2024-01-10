#include <MFRC522.h>
#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include "pitches.h" 
//ethernet config
// L'adresse MAC du shield

 //Parameters
const int buzPin 	= 2;
const int LED_RED =3;
const int LED_GREEN =4;

String rfidChar;

#define RST_PIN		7		// 
#define SS_PIN		6		//
byte nuidPICC[4];
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance
 ///////////////////////////////////////////////////////////////////////////////////////
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(169,254,196,17);  
IPAddress ip(169,254,196,18);
IPAddress myDns(169,254, 0, 1);
EthernetClient client;

unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

///////////////////////////////////////////////////////////////////////
void setup(){
    Serial.begin(9600);
    SPI.begin();			
////////////////////////////////////////////////////////////////////
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    mfrc522.PCD_Init();		// Init MFRC522
    Serial.println("Scan PICC to see UID, type, and data blocks...");   
    delay(1000);  //give the Ethernet shield a second to initialize:
}

void loop() {
  if ( !mfrc522.PICC_IsNewCardPresent())
    return;

  // Vérifier la présence d'un nouveau badge 
  if ( !mfrc522.PICC_ReadCardSerial())
    return;

  // Enregistrer l'ID du badge (4 octets) 
  for (byte i = 0; i < 4; i++) 
  {
    nuidPICC[i] = mfrc522.uid.uidByte[i];
  }
    /////////////////////////////////////////
  Ethernet.begin(mac, ip, myDns);
  delay(2000);
 // if you get a connection, report back via serial:
  if (client.connect(server, 8000)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /api/etudiant HTTP/1.1");
    client.println("Host:");
    client.println("http://169.254.196.17:8000");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
   delay(2000);
///////////////////////////////////////////////////////////////////////
  int len = client.available();
  if (len > 0) {
    byte buffer[80];
    if (len > 80) len = 80;
    client.read(buffer, len);
    if (printWebData) {
      Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  /////////////////////////////////////////
  Serial.println("Un badge est détecté");
  for (byte i = 0; i < 4; i++) 
  {
    rfidChar=rfidChar+nuidPICC[i]; 
  }
  //Serial.println(rfidChar);
  //test de carte
  if(rfidChar== "192015614"){
    playPassed();
  }
  else{
    playFailed();
  }
  rfidChar="";
  // Re-Init RFID
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD     
   
}

void ShowReaderDetails() {
	// Get the MFRC522 software version
	byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
	Serial.print("MFRC522 Software Version: 0x");
	Serial.print(v, HEX);
	if (v == 0x91)
		Serial.print(" = v1.0");
	else if (v == 0x92)
		Serial.print(" = v2.0");
	else
		Serial.print(" (unknown)");
        	Serial.println("");
	// When 0x00 or 0xFF is returned, communication probably failed
	if ((v == 0x00) || (v == 0xFF)) {
		Serial.println("WARNING: Communication failure, is the MFRC522 properly connected?");
	}
}

void playPassed() { /* function playPassed */
  digitalWrite(LED_GREEN, HIGH);
 	int melodyOn[] = {NOTE_C5, NOTE_C6, NOTE_D5, NOTE_A6};
 	int durationOn = 200;
 	for (int thisNote = 0; thisNote < 4; thisNote++) {
 			tone(buzPin, melodyOn[thisNote], durationOn);
 			delay(200);
 	}
      digitalWrite(LED_GREEN, LOW);   // turn the LED off by making the voltage LOW
}

void playFailed() { /* function playFailed */
  digitalWrite(LED_RED, HIGH);      
 	int melodyOff[] = {NOTE_C3, NOTE_D3};
 	int durationOff = 200;
 	for (int thisNote = 0; thisNote < 2; thisNote++) {
 			tone(buzPin, melodyOff[thisNote], durationOff);
 	}
  digitalWrite(LED_RED, LOW);
}
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define Sensor_Input 5
#define RST_PIN D3
#define SS_PIN D8

int Sensor_Type = 1;

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
// Type : 0 Movement Sensor | Type: 1 RFID Sensor 





void Movement_Sensor();
void RFID_Sensor();
String printHex(byte *buffer, byte bufferSize);

void setup() {
  Serial.begin(115200);
  Serial.print("\n");

  SPI.begin();
  rfid.PCD_Init();
  // TODO : FIND API WITH WEBSERVER 

  // TODO : CONNECT TO WEBSOCKET API
  Serial.print("calibrating Device ");
  for(int i = 0; i < 10; i++){
    Serial.print(".");
    delay(1000);
  }
}

void loop() {
  switch (Sensor_Type)
  {
  case 0:
      Movement_Sensor();
    break;
  case 1:
      RFID_Sensor();
    break;
  }
}

void Movement_Sensor(){
  Serial.println("SENSOR ACTIVE");
  //Time to Start all components on ESP

  pinMode(Sensor_Input, INPUT);
  int count_bewegung = 0;

  while(true){
    if (digitalRead(Sensor_Input) == 1){
        Serial.println("Deine Fette Mutter hat sich endlich bewegt");
        Serial.println(count_bewegung++);
        // TODO : WEBSOCKET INFO  | SOME CODE TO API 
        delay(500);
      }
      delay(30);
  }  
}

void RFID_Sensor(){
  while (true)
  {
    if (!rfid.PICC_IsNewCardPresent()) return;
    if (!rfid.PICC_ReadCardSerial()) return;
    Serial.println(printHex(rfid.uid.uidByte, rfid.uid.size));
 
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

String printHex(byte *buffer, byte bufferSize) {
  String id = "";
  for (byte i = 0; i < bufferSize; i++) {
    id += buffer[i] < 0x10 ? "0" : "";
    id += String(buffer[i], HEX);
  }
  return id;
}




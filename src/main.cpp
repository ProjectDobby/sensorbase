#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include "Keypad.h"
#include <WebSocketClient.h>
#include <strings.h>

#define Sensor_Input 5
#define RST_PIN D3
#define SS_PIN D8
#define DHTPIN D6

int Sensor_Type = 1;
int Setup = 0;
// Type : 0 Movement Sensor | Type: 1 RFID Sensor | Type : 2 Keypad 

String handler = "";
String Type = "";
String Scope = "";
String JSON = "";

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
String printHex(byte *buffer, byte bufferSize);

const byte ROWS = 4;
const byte COLS = 4; 

char *strcat(char *dest,const char *src);

char path[] = "/";
char host[] = "192.168.43.56";

WebSocketClient webSocketClient;
WiFiClient client;

char keys[ROWS][COLS] = {
  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};

byte rowPins[ROWS] = {D1,D2,D3,D4};
byte colPins[COLS] = {D5,D6,D7,D8};

Keypad pad = Keypad( makeKeymap(keys),rowPins, colPins,ROWS,COLS);

int pad_count = 0;

void Movement_Sensor();
void RFID_Sensor();
void Connect_to_Wlan();
void PinPad();
void Websocket_Connection();
void Send_JSON_Data();

void setup() {
  Serial.begin(115200);
  Serial.print("\n");

  SPI.begin();
  rfid.PCD_Init();

  Serial.print("Calibrating Device ");
  for(int i = 0; i < 5; i++){
    Serial.print(".");
    delay(1000);
  }
  
  Serial.print(" Calibrated");
  Connect_to_Wlan();
  Websocket_Connection();
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
  case 2:
      PinPad();
    break;
  }
}

void Movement_Sensor(){
  pinMode(Sensor_Input, INPUT);
  int count_bewegung = 0;

  handler = "door";
  Type = "Movement-Sensor";
  Scope = "register";

  Send_JSON_Data();

  Scope = "inform";

  while(true){
    if (digitalRead(Sensor_Input) == 1){
        Serial.println("Movement");
        Serial.println(count_bewegung++);
        Send_JSON_Data();
        delay(5000);
      }
      delay(30);
  }  
}

void RFID_Sensor(){
  if(Setup == 0){
    handler = "rfid";
    Type = "RFID-Reader";
    Scope = "register";
    Send_JSON_Data();
    Scope = "inform";
    Setup++;
  }
  

  while (true)
  {
    if (!rfid.PICC_IsNewCardPresent()) return;
    if (!rfid.PICC_ReadCardSerial()) return;
    Serial.println(printHex(rfid.uid.uidByte, rfid.uid.size));
    
    JSON = ",\"RFIDstring\":\"" + printHex(rfid.uid.uidByte, rfid.uid.size) + "\"";
    Send_JSON_Data();

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

void Connect_to_Wlan(){
  Serial.println("\nConnect to Wlan");
  WiFi.begin("4 Euro pro MB","Welches Passwort?");  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected!");
  Serial.print("\nIP address: ");
  Serial.print(WiFi.localIP());
}

void PinPad(){
  pad_count = 0;
  handler = "pinpad";
  Type = "Pinpad";
  Scope = "register";

  Send_JSON_Data();

  Scope = "inform";
  JSON = "";
  while(true){
    char key[4];
    key[pad_count] = pad.getKey();

    if(key[pad_count] != NO_KEY){
      
      pad_count++;
    }

    if(pad_count == 4){
      for(int i = 0; i < 4; i++){
        Serial.print(key[i]);
      }

      String keyo(key[0]);
      String keyt(key[1]);
      String keyd(key[2]);
      String keyf(key[3]);

      JSON = ",\"enterdPin\":\"" + keyo + keyt + keyd + keyf + "\"";
      Serial.println(JSON);
      Send_JSON_Data();
      Serial.println();
      pad_count = 0;
    }
    delay(20);
  }
}

void Websocket_Connection(){
  if (client.connect(host, 8080)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
  }
 
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
  }
}

void Send_JSON_Data(){
  webSocketClient.sendData("{\"handler\":\"" + handler + "\",\"deviceType\":\"" + Type + "\"," + "\"deviceMac\":\"" + WiFi.macAddress() + "\"," + "\"details\":{" + "\"scope\":\"" + Scope + "\"" + JSON + "}}");
}
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "WebSocketClient.h"
#include <ESP8266WiFi.h>
#include "Keypad.h"
#include "DHT.h"
#include "Adafruit_Sensor.h"

#define Sensor_Input 5
#define RST_PIN D3
#define SS_PIN D8
#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

int Sensor_Type = 3;

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
String printHex(byte *buffer, byte bufferSize);
// Type : 0 Movement Sensor | Type: 1 RFID Sensor | Type : 2 Keypad | Type 3 : Temperature Sensor

const byte ROWS = 4;
const byte COLS = 4; 


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
void Temperature_Sensor();

void setup() {
  Serial.begin(115200);
  Serial.print("\n");

  SPI.begin();
  rfid.PCD_Init();

  Serial.print("calibrating Device ");
  for(int i = 0; i < 10; i++){
    Serial.print(".");
    delay(1000);
  }
  Connect_to_Wlan();
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
  case 3:
      Temperature_Sensor();
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
        delay(5000);
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

void Connect_to_Wlan(){
  WiFi.begin("AP-PI1","");
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  Serial.print("Connectet!");
} 

void Connect_to_Websocket(){
  WebSocketClient web;
  web.connect("","",8080);
}

void PinPad(){
  pad_count = 0;
  Serial.print("Test1:");
  Serial.println(pad_count);

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

      Serial.println();
      pad_count = 0;
    }
    delay(20);
  }
}

void Temperature_Sensor(){
  int timeSinceLastRead = 0;
  while(true){
      if(timeSinceLastRead > 2000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100;
  }
}
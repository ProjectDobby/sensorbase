#include <Arduino.h>
#define Sensor_Input 5
int count_bewegung = 0;
int Sensor_Type = 0;

// Type : 0 Movement Sensor

void Movement_Sensor();

void setup() {
  Serial.begin(115200);
  Serial.print("\n");
  
  //Time to Start all components on ESP
  Serial.print("calibrating sensor ");
  for(int i = 0; i < 10; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println("SENSOR ACTIVE");

  pinMode(Sensor_Input, INPUT);

  // TODO : FIND API WITH WEBSERVER 

  // TODO : CONNECT TO WEBSOCKET API
}

void loop() {
  switch (Sensor_Type)
  {
  case 0:
      Movement_Sensor();
    break;
  }
}

void Movement_Sensor(){
  while(true){
    if(digitalRead(Sensor_Input) == 1){
        Serial.println("Deine Fette Mutter hat sich endlich bewegt");
        Serial.println(count_bewegung++);
        // TODO : WEBSOCKET INFO  | SOME CODE TO API 
        delay(500);
      }
      delay(30);
  }  
}









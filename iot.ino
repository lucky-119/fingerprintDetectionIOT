#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include "ESP8266.h"

int ID=0;

#define KEY_ENROLL 7
#define BUZZER 6

#define SSID "IOTPROJECT"     
#define PASS "123456789"      
#define IP ""
String msg = "GET /update?key="; 

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial);
ESP8266 ESP;

void beep(int duration = 100){
  digitalWrite(BUZZER, HIGH);
  delay(duration);
  digitalWrite(BUZZER, LOW);
  delay(duration);
}

void setup(){
  pinMode(KEY_ENROLL, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(57600);
  Serial.print('Hello');
  finger.begin(57600);

  if (finger.verifyPassword()) {
    beep();
  } else {
    while (1) { beep(50); }
  }

  ESP.preInit();
  while(ESP.connectWiFi(SSID, PASS)){
    beep(400);
    beep(100);
  }
  
  beep(1000);
  beep();
}

void updateLevel(String success, int confidentLevel, int ID){
  String cmd = msg;
  cmd += "&field1=";
  cmd += success;
  cmd += "&field2=";
  cmd += confidentLevel;
  cmd += "&field3=";
  cmd += ID;
  cmd += "\r\n";
  if(ESP.get(IP, 80, cmd)){
  }
}

void loop(){
  String dataString = "";
  char a[20];
  int i=0;
  if(!digitalRead(KEY_ENROLL)){
    enrollFinger();
    
    while(!digitalRead(KEY_ENROLL))
      delay(10);
  }
  if(finger.getImage() == FINGERPRINT_OK){
    int t=getFingerprintIDez();
    if(t>ID or t<=0){
      beep(2000);
      updateLevel("ABSENT", LOW, -1);
      }else{
      a[i]=char(t);
      i+=1;
      dataString+=String(t)+",Present";
      Serial.println(dataString);
      updateLevel("PRESENT", finger.confidence, t);
      beep(200);
      beep(100);
    }
  }
  delay(100);/*
  beep(3000);
  int flag=0;
  for(int x=0;x<ID;x=x+1){
    flag=0;
    for(int j=0;j<i;j=j+1){
       if(x==a[j])
       {
        flag=1;
        break;
       }
    }
    if(flag==0){
    dataString+=String(x)+",Absent";
    }
  }
  delay(1000);*/
}

void waitNoFinger(){
  while(finger.getImage() == FINGERPRINT_OK)
    delay(100);
}

uint8_t enrollFinger() {
  int p = -1;
  
  do {
    p = finger.getImage();
  } while (p != FINGERPRINT_OK);

  p = finger.image2Tz(1);
  if(p != FINGERPRINT_OK){
    return -1;
  }
  
  waitNoFinger();
  do {
    p = finger.getImage();
  }while (p != FINGERPRINT_OK);

  p = finger.image2Tz(2);
  if(p != FINGERPRINT_OK){
    return -1;
  }  
  p = finger.createModel();
  ID=ID+1;
  p = finger.storeModel(ID);
  if (p == FINGERPRINT_OK) {
    beep(500);
    beep(100);
    return 0;
  } else {
    beep(2000);
    return -1;
  }
}

int getFingerprintIDez() {
  uint8_t p;
  do{
    p = finger.getImage();
  }while(p != FINGERPRINT_OK);

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  return finger.fingerID; 
}

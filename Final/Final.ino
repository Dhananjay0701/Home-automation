/////// Header Files

// IRsend
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

// WiFiManager
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

// Google Asst
#include<ESP8266WiFi.h>
#include<ESP8266HTTPClient.h>

/////// Global Variables

// IRsend
const uint16_t kIrLed = 4;
IRPanasonicAc ac(kIrLed);

// Google Asst
const char* site = "http://gosmartsoftsolutions.com/AutoStart/testv1.txt";
const char* siteSet = "http://gosmartsoftsolutions.com/AutoStart/testv1.php?ACcode=";
const char* sitePow = "http://gosmartsoftsolutions.com/AutoStart/testv1pow.txt";
const char* siteTemp = "http://gosmartsoftsolutions.com/AutoStart/testv1temp.txt";
const char* siteRelay = "http://gosmartsoftsolutions.com/AutoStart/testv1relay.txt";

const uint16_t relay1 = 14;
const uint16_t relay2 = 12;
const uint16_t relay3 = 13;
const uint16_t relay4 = 15;

/////// Setup Functions

void wifiManagerSetup() {
  WiFi.printDiag(Serial);
 
  WiFiManager wifiManager;
 
  wifiManager.autoConnect();
  
  if (WiFi.status()!=WL_CONNECTED) {
    Serial.println("failed to connect, finishing setup anyway");
  } 
  else {
    Serial.print("local ip: ");
    Serial.println(WiFi.localIP());
  }
}

void irSendSetup() {
  ac.begin();
}
 
void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  irSendSetup();
  wifiManagerSetup();
}

////// Helper Functions

void printState() {
  // Display the settings.
  Serial.println("Panasonic A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());
  // Display the encoded IR sequence.
  unsigned char* ir_code = ac.getRaw();
  Serial.print("IR Code: 0x");
  for (uint8_t i = 0; i < kPanasonicAcStateLength; i++)
    Serial.printf("%02X", ir_code[i]);
  Serial.println();
}

void ACOn(int temp) {
    Serial.println("Default state of the remote.");
    printState();
    Serial.println("Setting desired state for A/C.");
    ac.setModel(kPanasonicRkr);
    ac.on();
    ac.setFan(kPanasonicAcFanMax);
    ac.setMode(kPanasonicAcCool);
    ac.setTemp(temp);
    ac.setSwingVertical(kPanasonicAcSwingVAuto);
    ac.setSwingHorizontal(kPanasonicAcSwingHAuto);
    #if SEND_PANASONIC_AC
        Serial.println("Sending IR command to A/C ...");
        ac.send();
    #endif  // SEND_PANASONIC_AC
        printState();
        delay(100);
}

void ACOff(int temp) {

    Serial.println("Default state of the remote.");
    printState();
    Serial.println("Setting desired state for A/C.");
    ac.setModel(kPanasonicRkr);
    ac.off();
    ac.setFan(kPanasonicAcFanMax);
    ac.setMode(kPanasonicAcCool);
    ac.setTemp(temp);
    ac.setSwingVertical(kPanasonicAcSwingVAuto);
    ac.setSwingHorizontal(kPanasonicAcSwingHAuto);
    #if SEND_PANASONIC_AC
        Serial.println("Sending IR command to A/C ...");
        ac.send();
    #endif  // SEND_PANASONIC_AC
        printState();
        delay(100);
}

int getTemp() {
    int temp = 16;
    if(WiFi.status() == WL_CONNECTED){    
        HTTPClient http;

        http.begin(siteTemp);
        int httpCode = http.GET();

        if(httpCode > 0){
            String payload = http.getString();
            int len = payload.length() + 1;
            char ans[len];
            payload.toCharArray(ans, len);
            
            temp = atoi(ans);     
        }    
        http.end();
    }
    return temp;
}

bool getPow() {
    int pow = 1;
    if(WiFi.status() == WL_CONNECTED){    
        HTTPClient http;

        http.begin(sitePow);
        int httpCode = http.GET();

        if(httpCode > 0){
            String payload = http.getString();
            int len = payload.length() + 1;
            char ans[len];
            payload.toCharArray(ans, len);
            
            pow = atoi(ans);     
        }    
        http.end();
        switch(pow) {
            case 1:
                return true;
            case 0:
                return false;
            default:
                return true;
        }
    }
    return true;
}

void updateStatus(String str){
    if(WiFi.status() == WL_CONNECTED){    
        HTTPClient http;

        http.begin(siteSet + str);
        int httpCode = http.GET();
        Serial.println("Updated String http Code ..." + String(httpCode));

        http.end();
    } 
}

void relayCheck(){
  if(WiFi.status() == WL_CONNECTED){    
    HTTPClient http;
    
    http.begin(siteRelay);
    int httpCode = http.GET();
    
    if(httpCode > 0){
      String payload = http.getString();
      int len = payload.length() + 1;
      char ans[len];
      payload.toCharArray(ans, len);
      
      if(ans[0] == '0'){
        digitalWrite(relay1, HIGH);
        Serial.println("Ne_0");
      }
      else {
        digitalWrite(relay1, LOW);
        Serial.println("Ye_0");
      }

      if(ans[1] == '0'){
        digitalWrite(relay2, HIGH);
        Serial.println("Ne_1");
      }
      else {
        digitalWrite(relay2, LOW);
        Serial.println("Ye_1");
      }

      if(ans[2] == '0'){
        digitalWrite(relay3, HIGH);
        Serial.println("Ne_2");
      }
      else {
        digitalWrite(relay3, LOW);
        Serial.println("Ye_2");
      }

      if(ans[3] == '0'){
        digitalWrite(relay4, HIGH);
        Serial.println("Ne_3");
      }
      else {
        digitalWrite(relay4, LOW);
        Serial.println("Ye_3");
      }
    }
    
    http.end();
  }
  delay(100);
}

/* 
  0 = TV Off
*/
void ACRemoteCheck() {
  if(WiFi.status() == WL_CONNECTED){    
    HTTPClient http;

    http.begin(site);
    int httpCode = http.GET();

    if(httpCode > 0){
      String payload = http.getString();
      int len = payload.length() + 1;
      char ans[len];
      payload.toCharArray(ans, len);

      if(ans[0] == '1'){
        digitalWrite(LED_BUILTIN, LOW);
        int temp = getTemp();

        if(getPow())
          ACOn(temp);
        else
          ACOff(temp);

        ans[0] = 0;
        Serial.println("Updated String ..." + String(ans));
        updateStatus(String(ans));
        digitalWrite(LED_BUILTIN, HIGH);
      }      
    }    
    http.end();
  }  
  delay(100);
}

void loop() {
  relayCheck();
  ACRemoteCheck();
}

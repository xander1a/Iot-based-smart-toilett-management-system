
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <LCD_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
Servo myservo; 
/////////////////////////////////////////////////////////
#define HOST "itaratec.xyz"
//#define WIFI_SSID "A"            // WIFI SSID here                                   
//#define WIFI_PASSWORD "12345678"  

#define WIFI_SSID "A"            // WIFI SSID here                                   
#define WIFI_PASSWORD "1234567890" 
String sendval, sendval2, sendval3,  postData, postData2;
//////////////////////////////////////////////////////////////
int lcdColumns = 16;
int lcdRows = 2;
String data="";
int led =A0;
LCD_I2C lcd(0x27, 16, 2); 
#define SS_PIN D4
#define RST_PIN D3
const int buto=D0;  
byte readCard[4];
String tag = "";
MFRC522 rfid(SS_PIN, RST_PIN);
String card="";
boolean getID();
int buttonState = 0;

 WiFiClient client;
HTTPClient http;  


void setup() {


  Serial.begin(9600);
  SPI.begin();
  myservo.attach(D8); 
  pinMode(led,OUTPUT);
  pinMode(buto,INPUT);
  rfid.PCD_Init();
  lcd.begin(); // If you are using more I2C devices using the Wire library use lcd.begin(false)
                 // this stop the library(LCD_I2C) from calling Wire.begin()
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Smart public");
  lcd.setCursor(5, 1);
  lcd.print("toilette");
  delay(3000);
     myservo.write(0); 

  ///////////////////////////////////////////////////////////////////
WiFi.mode(WIFI_STA);           
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
Serial.print("Connecting to ");
Serial.print(WIFI_SSID);
while (WiFi.status() != WL_CONNECTED) 
{ Serial.print(".");
    delay(500); }

Serial.println();
Serial.print("Connected to ");
Serial.println(WIFI_SSID);
Serial.print("IP Address is : ");
Serial.println(WiFi.localIP());    //print local IP address
delay(30);

////////////////////////////////////////////////////// 
}

void loop() {
  
    
buttonState=digitalRead(buto);
if(buttonState==1){
   lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Place Your Card");
 cardd();
 Serial.println(buttonState);
  }
  else{
    
   lcd.setCursor(0, 0);
   lcd.print("Please wait");   
   lcd.setCursor(2, 1);
   lcd.print("Ocupied");   
   Serial.println(buttonState);
    
    }

}
void cardd(){
  
   if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    http1(tag);
    
    if (tag == "1193818595") {
      Serial.println("Access Granted!");
      
      delay(100);
    } else {
      Serial.println("Access Denied!");
      digitalWrite(D8, HIGH);
      delay(2000);
      digitalWrite(D8, LOW);
    }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

//http1(tag);
  
  }
  void http1(String card){
sendval =   card;  


postData = "sendval=" + sendval ,//+ "&sendval2=" + sendval2 ;
//postData2 = "sendval2=" + sendval2 ,//+ "&sendval2=" + sendval2 ;
http.begin(client,"http://itaratec.xyz/Smart_toilet/data.php");//http://localhost/kitchen/data_write.php
//http.begin(client,"http://192.168.43.197/dbin/dbwrite.php");
http.addHeader("Content-Type", "application/x-www-form-urlencoded");            //Specify content-type header

  
 
int httpCode = http.POST(postData);   // Send POST request to php file and store server response code in variable named httpCode
Serial.println("Values are, sendval = " + sendval);// + " and sendval2 = " +sendval2 );
delay(100);

// if connection eatablished then do this
if (httpCode == 200) { Serial.println("Values uploaded successfully."); 
Serial.println(httpCode); 
String webpage = http.getString();    // Get html webpage output and store it in a string
Serial.println(webpage + "\n"); 

StaticJsonDocument<96> doc;

DeserializationError error = deserializeJson(doc, webpage);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());

Serial.println("Not granted");  
   // myservo.write(90); 
     digitalWrite(led,HIGH); 
     lcd.clear();
     lcd.setCursor(0,1);
     lcd.print("Not granted") ;
   
      lcd.setCursor(0, 0);
     lcd.print("Not Registered");
 
  return;
}

const char* cstatus = doc["cstatus"]; // "Bimenyimana"
int balance = doc["balance"]; // 1300
Serial.print(balance);
Serial.print(cstatus);
if(balance>100){
   lcd.clear();
 Serial.println("Access granted"); 
      myservo.write(90); 
      delay(2000);
      myservo.write(0);    
    digitalWrite(led,LOW); 
         lcd.setCursor(0, 1);
     lcd.print("balce:"); 
          lcd.setCursor(9, 1);
     lcd.print(balance);
      lcd.setCursor(0, 0);
     lcd.print( "Granted");
  
}
else{
   lcd.clear();
  Serial.println("Not granted");  
    // myservo.write(90); 
     digitalWrite(led,HIGH); 
     lcd.clear();
     lcd.setCursor(0,1);
     lcd.print("balce:") ;
     lcd.setCursor(9, 1);
     lcd.print(balance);

      lcd.setCursor(0, 0);
     lcd.print("Please Add Money");
  }
}

// if failed to connect then return and restart

else { 
  Serial.println(httpCode); 
  Serial.println("Failed to upload values. \n"); 
  http.end(); 
  return; 
}
////////////////////////////////////////////////////////////////////
}

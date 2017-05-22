#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <stdlib.h>
#include <stdio.h>

#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D

MicroOLED oled(PIN_RESET, DC_JUMPER);  // I2C Example


const char* ssid = "";
const char* password = "";

const char* host = "api.kraken.com";
const int httpsPort = 443;

void initDisplay() { 
  oled.begin();
  oled.clear(PAGE); // Clear the display's internal memory
  oled.clear(ALL);
  oled.setFontType(0);
}

void clearDisplay() { 
  oled.clear(PAGE);
  oled.setCursor(0, 0); 
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  initDisplay();
}

  WiFiClientSecure client;

void connectToKraken() { 
  while (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
  }
}

void printDouble( double val, byte precision){
 // prints val with number of decimal places determine by precision
 // precision is a number from 0 to 6 indicating the desired decimial places
 // example: lcdPrintDouble( 3.1415, 2); // prints 3.14 (two decimal places)

 if(val < 0.0){
   Serial.print('-');
   val = -val;
 }

 Serial.print (int(val));  //prints the int part
 if( precision > 0) {
   Serial.print("."); // print the decimal point
   unsigned long frac;
   unsigned long mult = 1;
   byte padding = precision -1;
   while(precision--)
    mult *=10;

   if(val >= 0)
    frac = (val - int(val)) * mult;
   else
    frac = (int(val)- val ) * mult;
   unsigned long frac1 = frac;
   while( frac1 /= 10 )
    padding--;
   while(  padding--)
   Serial.print("0");
   Serial.print(frac,DEC) ;
 }
}

void printDoubleOled( double val, byte precision){
 // prints val with number of decimal places determine by precision
 // precision is a number from 0 to 6 indicating the desired decimial places
 // example: lcdPrintDouble( 3.1415, 2); // prints 3.14 (two decimal places)

 if(val < 0.0){
   oled.print('-');
   val = -val;
 }

 oled.print (int(val));  //prints the int part
 if( precision > 0) {
   oled.print("."); // print the decimal point
   unsigned long frac;
   unsigned long mult = 1;
   byte padding = precision -1;
   while(precision--)
    mult *=10;
   if(val >= 0)
    frac = (val - int(val)) * mult;
   else
    frac = (int(val)- val ) * mult;
   unsigned long frac1 = frac;
   while( frac1 /= 10 )
    padding--;
   while(  padding--)
    oled.print("0");
   oled.print(frac,DEC) ;
 }
}


void getQuote(String pair) { 
  connectToKraken();
  String url = "/0/public/Ticker?pair="+pair;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266PriceDisplay\r\n" +
               "Connection: close\r\n\r\n");  
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if(line.startsWith("{\"error")) { 
      StaticJsonBuffer<1024> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(line,10);
      if(root.success()) {   
        double price = strtod(root["result"][pair]["c"][0], NULL)*1000;
        // printDouble(price,5);  
        oled.println(pair);
        oled.print(" ");     
        printDoubleOled(price,5);
        oled.println("");
      }
    }
  }
}



void loop() {
  while( 1==1 ) { 
    clearDisplay();

    getQuote("XXRPXXBT");
    getQuote("XETHXXBT");
    
    oled.display();
    delay(2500);
  }
}

#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LoRa.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define SS 15
#define RST 16
#define DIO0 2
#define SCREEN_ADDRESS 0x3C 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,&Wire, OLED_RESET);
String host="script.google.com";
String GAS_ID = "AKfycbwH-49PJ2q5cSQjQm2LnSNuc8x4Z0e9fsC8Tqp1KS6KSoHBZTJcEvq6mKxfdaorZC13Fw";
const char * ssid = "Sim";
const char * password = "11111111";
int httpsPort = 443;
WiFiClientSecure client;
void setup() {
  Wire.begin(0,2);//sck to d4 and sda to d3
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  LoRa.setPins(SS, RST, DIO0);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  while (!Serial);
  Serial.println("Receiver Host");
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    while (1);
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi Connected !!!");
}
void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Receiving Data: ");
    while (LoRa.available()) {
      String data = LoRa.readString();
      Serial.println(data);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0); 
      display.print(data);
      display.display();
      String a=data.substring(13,18);
      String b=data.substring(28,33);
      sendData(a, b);
    }
  }
  
}
void sendData(String a, String b) {
  client.setInsecure();
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------
 
  //----------------------------------------Processing data and sending data
  String string_temperature =  String(a);
  String string_humidity =  String(b); 
  String url = "/macros/s/" + GAS_ID + "/exec?value1=" + string_temperature + "&value2=" + string_humidity;
  Serial.print("requesting URL: ");
  Serial.println(url);
 
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");
 
  Serial.println("request sent");
  
} 
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> 
#include <LoRa.h>

#define SS 10
#define RST 9
#define DIO0 2   
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
const int LM35_PIN = A0;
const int PulseWire = A1;   
int Threshold = 550;                                       
PulseSensorPlayground pulseSensor;    
void setup() {
  Serial.begin(9600);
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);
  pulseSensor.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
  }
}

void loop() {
  int lm35Value = analogRead(LM35_PIN);
  float temperatureF = (lm35Value * 0.00488) * 205.0;
  if (temperatureF >= 0 && temperatureF <= 99) {
    Serial.print("Temperature: ");
    Serial.print(temperatureF);
    Serial.println("F");
    float myBPM = pulseSensor.getBeatsPerMinute();
    Serial.print("BPM: ");                        
    Serial.println(myBPM/2.811);
    String Message = "Temperature: " + String(temperatureF)+" F\n"+"Pulse: "+String(myBPM/2.811)+" BPM";
    LoRa.beginPacket();
    LoRa.print(Message);
    LoRa.endPacket();
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Temperature: ");
    display.print(temperatureF);
    display.print("F");
    display.setCursor(0,15);
    display.print("Pulse: ");
    display.print(myBPM/2.811);
    display.print("BPM");
    display.display();
    }
  delay(2000);
}

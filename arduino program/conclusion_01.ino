#include <Adafruit_NeoPixel.h>
#include <dht11.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "U8glib.h"

int ledpin = 2;
int ledcount = 60;
Adafruit_NeoPixel strip(ledcount, ledpin, NEO_GRB + NEO_KHZ800);

dht11 DHT;
#define DHT11_PIN 6
int chk;
float humidity;
float temperature;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
void u8g_prepare(void) {
  u8g.setFont(u8g_font_6x10); 
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

//screen
void drawMainStream(int x, int y, bool isLarge) {
  u8g.firstPage();
  do {
    u8g_prepare();
    if (isLarge) {
      u8g.setFont(u8g_font_10x20); 
      u8g.drawStr(x, y, "MAIN STREAM");
    } else {
      u8g.setFont(u8g_font_6x10); 
      u8g.drawStr(x, y, "main stream");
    }
  } while (u8g.nextPage());
}

SoftwareSerial mySerial(10, 11); // RX, TX
DFRobotDFPlayerMini dfPlayer;

int mp3State = 0; // 0stop，1start1，2start2

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  strip.begin();
  strip.show();

  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");

  if (!dfPlayer.begin(mySerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Please recheck the connection!"));
    Serial.println(F("2. Please insert the SD card!"));
    while (true);
  }
  dfPlayer.volume(20);

  if (u8g.getMode() == U8G_MODE_R3G3B2)
    u8g.setColorIndex(1); 
  else if (u8g.getMode() == U8G_MODE_GRAY2BIT)
    u8g.setColorIndex(1); 
  else if (u8g.getMode() == U8G_MODE_BW)
    u8g.setColorIndex(1); 
  u8g_prepare();
}



void loop() {
  
  drawMainStream(8, 10, false); 
  drawMainStream(8, 25, false);
  drawMainStream(8, 40, false); 
  drawMainStream(20, 30, true); 
  drawMainStream(20, 33, true); 
  drawMainStream(20, 39, true); 
  drawMainStream(20, 33, true); 
  delay(500); 
  u8g.firstPage();
  do {
    u8g.setColorIndex(0); 
    u8g.drawBox(0, 0, 128, 64);
  } while (u8g.nextPage());
  delay(500); 

  Serial.print("DHT11, \t");
  chk = DHT.read(DHT11_PIN);    
  switch (chk) {
    case DHTLIB_OK:
      Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
  }
  humidity = DHT.humidity;
  temperature = DHT.temperature;

  if (humidity > 63) {
    Serial.println(humidity);
    Serial.println(temperature);
    if (mp3State != 2) {
      dfPlayer.play(2);
      mp3State = 2;
    }
    flashLED(strip.Color(0, 50, 255), 500);
  } else {
    Serial.println(humidity);
    Serial.println(temperature);
    if (mp3State != 1) {
      dfPlayer.play(1);
      mp3State = 1;
    }
    flashLED(strip.Color(0, 255, 20), 1000);
  }
}

void flashLED(uint32_t color, int delayTime) {
  for (int i = 0; i < ledcount; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
  delay(delayTime);

  for (int i = 0; i < ledcount; i++) {
    strip.setPixelColor(i, 0); 
  }
  strip.show();
  delay(delayTime);
}

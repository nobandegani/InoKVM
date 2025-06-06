// --------------------------------------------- use display ---------------------------------------------
#define USE_DISPLAY False

// --------------------------------------------- headers ---------------------------------------------
#ifdef USE_DISPLAY
  #include <SPI.h>
  #include <Wire.h>

  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
#endif

// --------------------------------------------- defs ---------------------------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C



#ifdef USE_DISPLAY
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
#endif

// ---------------------------------------------setup_display ---------------------------------------------
void setup_display(){
  #ifdef USE_DISPLAY
    Serial.println("Display start");
    Wire.begin(47, 21);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.println("display started");
    display.display();
  #endif
}
	
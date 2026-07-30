#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;
const int OLED_ADDRESS = 0x3C;

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

void DisplayText(const char* text) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.println("C-D 2000");
  display.setTextSize(1);
  display.println(text);
  display.display();

   Serial.println("Display updated to " + String(text) + " successfully");
}

void setup() {
   // pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("Display initialization failed");

    while (true) {
      // Stop here because the display could not be initialized.
    }
  }
 
  DisplayText("OLED connected using a function to help in future messages!");
}

void loop() {
  //this was for the blinking LED
/*  digitalWrite(9, HIGH);
  Serial.println("Cat Deterrent 2000 is alive");
  delay(500);

  digitalWrite(9, LOW);
  delay(500);
  */


}




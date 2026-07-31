#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
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

const int DFPLAYER_RX_PIN = 10;
const int DFPLAYER_TX_PIN = 11;
const int BUTTON_PIN = 2;

const unsigned long BUTTON_DEBOUNCE_MS = 50;
const unsigned long SOUND_INTERVAL_MS = 5000;

// Sound numbers correspond to files in the microSD card's /mp3 folder:
// /mp3/0001.mp3, /mp3/0002.mp3, and so on.
const int HISS_SOUND = 1;
const int GET_OFF_OF_THERE_SOUND = 2;
const int LUCY_GET_OUT_SOUND = 3;
const int UH_UH_UH_SOUND = 4;
const int WHATCHA_DOING_SOUND = 5;

const int SOUND_TRACKS[] = {
    HISS_SOUND,
    GET_OFF_OF_THERE_SOUND,
    LUCY_GET_OUT_SOUND,
    UH_UH_UH_SOUND,
    WHATCHA_DOING_SOUND
};
const int SOUND_COUNT = sizeof(SOUND_TRACKS) / sizeof(SOUND_TRACKS[0]);

// SoftwareSerial arguments are (Uno RX pin, Uno TX pin).
// DFPlayer TX connects to pin 10.
// Pin 11 connects through a 1 kOhm resistor to DFPlayer RX.
SoftwareSerial dfPlayerSerial(DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
DFRobotDFPlayerMini dfPlayer;

bool soundLoopRunning = false;
int nextSoundIndex = 0;
int lastButtonReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastButtonChangeAt = 0;
unsigned long lastSoundStartedAt = 0;

const __FlashStringHelper* SoundName(int trackNumber) {
  switch (trackNumber) {
    case HISS_SOUND:
      return F("Hiss");
    case GET_OFF_OF_THERE_SOUND:
      return F("Get off there");
    case LUCY_GET_OUT_SOUND:
      return F("Lucy get out");
    case UH_UH_UH_SOUND:
      return F("Uh uh uh");
    case WHATCHA_DOING_SOUND:
      return F("Whatcha doing");
    default:
      return F("Unknown sound");
  }
}

void DisplayMessage(
    const __FlashStringHelper* status,
    const __FlashStringHelper* detail = nullptr) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.println(F("C-D 2000"));
  display.setTextSize(1);
  display.println(status);

  if (detail != nullptr) {
    display.println(detail);
  }

  display.display();
}

void PlayNextSound() {
  int trackNumber = SOUND_TRACKS[nextSoundIndex];
  dfPlayer.playMp3Folder(trackNumber);

  Serial.print(F("Playing sound number "));
  Serial.println(trackNumber);
  DisplayMessage(F("Playing"), SoundName(trackNumber));

  nextSoundIndex = (nextSoundIndex + 1) % SOUND_COUNT;
  lastSoundStartedAt = millis();
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Display initialization failed"));

    while (true) {
      // Stop here because the display could not be initialized.
    }
  }

  DisplayMessage(F("Starting"), F("DFPlayer..."));

  Serial.println(F("Starting OLED and DFPlayer..."));

  dfPlayerSerial.begin(9600);

  // Give the DFPlayer time to start and recognize the microSD card.
  delay(1000);

  // This MP3-TF-16P V3.0 is a DFPlayer-compatible clone. Some clones do not
  // return the status acknowledgement expected by the DFRobot library.
  // Disable acknowledgements, but still send the normal reset command.
  if (!dfPlayer.begin(dfPlayerSerial, false, true)) {
    Serial.println(F("DFPlayer compatibility-mode setup failed."));
    DisplayMessage(F("DFPlayer failed"));

    while (true) {
      // Stop here because commands cannot be sent to the DFPlayer.
    }
  }

  Serial.println(F("DFPlayer commands enabled (no-ack compatibility mode)."));

  // Volume ranges from 0 (silent) to 30 (maximum). Start conservatively.
  dfPlayer.volume(20);
  delay(200);

  Serial.println(F("Ready. Press the button to start the sound loop."));
  DisplayMessage(F("Ready"), F("Press button"));
}

void loop() {
  int buttonReading = digitalRead(BUTTON_PIN);

  // A mechanical button briefly flickers between HIGH and LOW when pressed.
  // Only accept a change after it has remained stable for 50 milliseconds.
  if (buttonReading != lastButtonReading) {
    lastButtonChangeAt = millis();
  }

  if (millis() - lastButtonChangeAt >= BUTTON_DEBOUNCE_MS &&
      buttonReading != stableButtonState) {
    stableButtonState = buttonReading;

    // INPUT_PULLUP means a pressed button reads LOW.
    if (stableButtonState == LOW) {
      soundLoopRunning = !soundLoopRunning;

      if (soundLoopRunning) {
        Serial.println(F("Sound loop started."));
        nextSoundIndex = 0;
        PlayNextSound();
      } else {
        dfPlayer.stop();
        Serial.println(F("Sound loop stopped."));
        DisplayMessage(F("Loop stopped"), F("Press to start"));
      }
    }
  }

  lastButtonReading = buttonReading;

  if (soundLoopRunning &&
      millis() - lastSoundStartedAt >= SOUND_INTERVAL_MS) {
    PlayNextSound();
  }
}

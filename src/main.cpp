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
const int MOTION_SENSOR_PIN = 5;

const unsigned long BUTTON_DEBOUNCE_MS = 50;
const unsigned long MOTION_SENSOR_WARMUP_MS = 30000;

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

int nextSoundIndex = 0;
int lastButtonReading = HIGH;
int stableButtonState = HIGH;
bool motionDetectionStarted = false;
bool previousMotionState = LOW;
unsigned long lastButtonChangeAt = 0;
unsigned long motionDetectionCount = 0;

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

  // Keep the session count on the bottom eight-pixel text row.
  display.setCursor(0, SCREEN_HEIGHT - 8);
  display.print(F("Motion count: "));
  display.print(motionDetectionCount);

  display.display();
}

void InitializeMotionSensor() {
  // The HC-SR501 OUT pin is a digital signal: HIGH means motion detected.
  pinMode(MOTION_SENSOR_PIN, INPUT);

  Serial.println(F("Motion sensor warming up for 30 seconds..."));
  DisplayMessage(F("Motion sensor"), F("Warming up..."));

  // PIR sensors need time to establish a stable infrared baseline after power-up.
  delay(MOTION_SENSOR_WARMUP_MS);

  Serial.println(F("Motion sensor ready on pin 5."));
  DisplayMessage(F("Motion sensor"), F("Motion sensor ready"));
  
}

bool MotionDetected() {
  return digitalRead(MOTION_SENSOR_PIN) == HIGH;
}

void PlayNextSound() {
  int trackNumber = SOUND_TRACKS[nextSoundIndex];
  dfPlayer.playMp3Folder(trackNumber);

  Serial.print(F("Playing sound number "));
  Serial.println(trackNumber);

  nextSoundIndex = (nextSoundIndex + 1) % SOUND_COUNT;
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

  InitializeMotionSensor();

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

  // Record the current PIR state so startup does not look like a new event.
  previousMotionState = MotionDetected();

  Serial.println(F("Ready. Press the button to arm motion detection."));
  DisplayMessage(F("Motion disabled"), F("Press to arm"));
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
      motionDetectionStarted = !motionDetectionStarted;

      if (motionDetectionStarted) {
        Serial.println(F("Motion detection armed."));
        DisplayMessage(F("Motion armed"), F("Watching..."));
      } else {
        dfPlayer.stop();
        Serial.println(F("Motion detection disabled."));
        DisplayMessage(F("Motion disabled"), F("Press to arm"));
      }
    }
  }

  lastButtonReading = buttonReading;

  bool currentMotionState = MotionDetected();

  // Trigger once when the PIR changes from no motion (LOW) to motion (HIGH).
  if (motionDetectionStarted &&
      currentMotionState == HIGH &&
      previousMotionState == LOW) {
    motionDetectionCount++;
    Serial.println(F("Motion detected."));
    DisplayMessage(F("Motion detected!!"), F("Is that Lucy?!?"));
    PlayNextSound();
  }

  previousMotionState = currentMotionState;
}

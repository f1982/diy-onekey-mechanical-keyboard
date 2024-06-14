#include "Keyboard.h"

// Key definitions
#define BUTTON_KEY KEY_F19

// Pin definitions
#define BUTTON_PIN 9
#define LED_PIN 6

// Fade control parameters
const int FADE_DURATION = 2000;  // Total fade duration in milliseconds
const int FADE_STEPS = 255;      // Number of fade steps (8-bit PWM)
const int INTERVAL = FADE_DURATION / FADE_STEPS; // Interval for each fade step

// Button helper class for handling press/release and debouncing
class Button {
public:
  const char key;
  const uint8_t pin;

  Button(uint8_t k, uint8_t p) : key(k), pin(p), lastPressed(0), pressed(false) {}

  void press(boolean state) {
    if (state == pressed || (millis() - lastPressed <= debounceTime)) {
      return;
    }

    lastPressed = millis();
    state ? Keyboard.press(key) : Keyboard.release(key);
    pressed = state;
  }

  void update() {
    press(!digitalRead(pin));
  }

private:
  const unsigned long debounceTime = 30;
  unsigned long lastPressed;
public:
  boolean pressed;
};

// Button objects
// You can add more key configuration if you want make multiple keys keyboard
Button buttons[] = {
  {BUTTON_KEY, BUTTON_PIN},
};

const uint8_t NumButtons = sizeof(buttons) / sizeof(Button);

// LED control variables
int brightness = 255;
bool fading = false;
unsigned long previousMillis = 0;

void setup() {
  pinMode(1, INPUT_PULLUP);
  if (!digitalRead(1)) {
    failsafe();
  }

  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);

  for (int i = 0; i < NumButtons; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
}

void loop() {
  for (int i = 0; i < NumButtons; i++) {
    buttons[i].update();
    if (buttons[i].pressed == true) {
      brightness = 255;
      analogWrite(LED_PIN, brightness);
      fading = true;
      previousMillis = millis();
    }
  }

  if (fading) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= INTERVAL) {
      previousMillis = currentMillis;
      if (brightness > 0) {
        brightness--;
        analogWrite(LED_PIN, brightness);
      } else {
        fading = false;
        analogWrite(LED_PIN, 0);
      }
    }
  }
}

void failsafe() {
  while (true) {}
}
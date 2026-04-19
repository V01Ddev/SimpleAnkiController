/*
 * Anki Controller by https://v01d.dev
 * 
 * PS GAMEPAD MODE
 * {SQUARE=1, X=2, CIRCLE=3, TRIANGLE=4, L1=5, R1=6, L2=7, R2=8,
 * Select=9, Start=10, L3=11, R3=12, PS=13}
 */

#include <Arduino.h>
#include <BleGamepad.h>

// BUTTONS
#define X_BUTTON      33
#define CIRCLE_BUTTON 25
#define SQUARE_BUTTON 32

#define NUM_BUTTONS 3

int buttonsPins[NUM_BUTTONS] = {
  X_BUTTON,
  CIRCLE_BUTTON,
  SQUARE_BUTTON
};

const char* buttonNames[NUM_BUTTONS] = {
  "X",
  "CIRCLE",
  "SQUARE"
};

int PS1GamepadButtons[NUM_BUTTONS] = {2, 3, 1};
int PCGamepadButtons[NUM_BUTTONS]  = {1, 2, 4};

typedef enum { PS1, PC } GamepadModes;
GamepadModes gamepadMode = PS1;

BleGamepad bleGamepad("Anki Controller", "v01d.dev");
BleGamepadConfiguration bleGamepadConfig;

bool wasConnected = false;

void setup() {
  delay(1000);
  Serial.begin(115200);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonsPins[i], INPUT_PULLUP);
  }

  bleGamepadConfig.setAutoReport(false);
  bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  bleGamepadConfig.setVid(0x054c);
  bleGamepadConfig.setPid(0x0ce6);

  // Disable axes to prevent mouse cursor movement on Windows
  bleGamepadConfig.setWhichAxes(false, false, false, false, false, false, false, false);
  bleGamepadConfig.setWhichSimulationControls(false, false, false, false, false);
  bleGamepadConfig.setHatSwitchCount(0);

  bleGamepad.begin(&bleGamepadConfig);

  Serial.println("[BLE] Advertising... waiting for connection.");
}

void loop() {
  if (bleGamepad.isConnected()) {

    if (!wasConnected) {
      Serial.println("[BLE] Connected!");
      wasConnected = true;
    }

    switch (gamepadMode) {
      case PS1:
        for (int i = 0; i < NUM_BUTTONS; i++) {
          if (!digitalRead(buttonsPins[i])) {
            Serial.printf("[BUTTON] %s pressed (PS1 mode)\n", buttonNames[i]);
            bleGamepad.press(PS1GamepadButtons[i]);
          } else {
            bleGamepad.release(PS1GamepadButtons[i]);
          }
        }
        break;

      case PC:
        for (int i = 0; i < NUM_BUTTONS; i++) {
          if (!digitalRead(buttonsPins[i])) {
            Serial.printf("[BUTTON] %s pressed (PC mode)\n", buttonNames[i]);
            bleGamepad.press(PCGamepadButtons[i]);
          } else {
            bleGamepad.release(PCGamepadButtons[i]);
          }
        }
        break;
    }

    bleGamepad.sendReport();

  } else {
    if (wasConnected) {
      Serial.println("[BLE] Disconnected. Advertising again...");
      wasConnected = false;
    }
  }
}

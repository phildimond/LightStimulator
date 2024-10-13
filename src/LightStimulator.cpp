/*
 Name:    SkinLight.ino
 Created: 10/9/2024 6:24:02 PM
 Author:  Phillip
*/

#include <Arduino.h>

// define led according to pin diagram in article
const int buzzer = D0;                  // Buzzer unit
const int mainLed = D1;                 // Main LED array
const int led = D10;                    // there is no LED_BUILTIN available for the XIAO ESP32C3.
const int switchIn = D6;                // User switch

// Program constants
const int switchDebounceTime = 50;      // Switch debounce time in milliseconds
const int lightOnOffSwitchTime = 1000;  // Time switch must be on to turn light on or off
const int buzzTime = 250;               // How long to buzz for
const int buzzerTone = 150;             // PWM value for the desired buzzer tone
const int lightTimerMilliseconds = 180 * 1000; // Three minute light timer

// Globals
int switchState = 0;
int switchStatePrev = 0;
int switchChanged = 0;
unsigned long switchDebounceStartTime = 0;
unsigned long switchOnTime = 0;
int lightState = 0;
unsigned long buzzerTimeStart = 0;
unsigned long lightTimerStart = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin led as an output
  pinMode(led, OUTPUT);
  pinMode(mainLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(switchIn, INPUT);
  Serial.begin(115200);
}

// the loop function runs over and over again until power down or resetkia
void loop() {

  // Check the switch value & do switch debouncing and set start time
  int sw = digitalRead(switchIn);
  if (sw != switchState) {
    if (switchDebounceStartTime == 0) {
      // switch state just changed, start debouncing
      switchDebounceStartTime = millis(); // Serial.print("Start debouncing\r\n");
    } else if (millis() - switchDebounceStartTime >= switchDebounceTime) {
      // Switch state has stayed unchanged for for the debounce time. Record the change and act on it.
      switchDebounceStartTime = 0;
      switchStatePrev = switchState;
      switchState = sw;
      //Serial.print("Debounced! Switch is ");
      //Serial.print(switchState);
      //Serial.print(" and previous state was ");
      //Serial.println(switchStatePrev);
      switchChanged = 1;
    }
  } else {
    // switch hasn't changed or bounced. Restart debouncer
    if (switchDebounceStartTime > 0) { 
      //Serial.print("Bounce...\r\n"); 
      switchDebounceStartTime = 0;
    }
  }

  // Process switch changes
  if (switchChanged) {
    switchChanged = 0;

    if (switchState == 1) {
      Serial.println("Switch turned on.");
      if (lightState == 0) {
        lightState = 1; //Serial.println("Turned the main LED on.");
        // Start the buzzer
        buzzerTimeStart = millis();
        analogWrite(buzzer, buzzerTone); // Serial.println("Buzzer on.");
        // Start the light timer
        lightTimerStart = millis();
      } else {
        lightState = 0; // Serial.println("Turned the main LED off.");
        lightTimerStart = 0; // Serial.println("Stopped the light timer.");
      }
    } // else { Serial.println("Switch turned off."); }
  }
  
  // Manage the state of the main LED
  if (lightState == 1) {
    digitalWrite(mainLed, HIGH);  // turn the main LED on
    digitalWrite(led, HIGH);    // turn the LED on 
  }
  else {
    digitalWrite(mainLed, LOW);   // turn the main LED off
    digitalWrite(led, LOW);     // turn the LED off
  }

  // Manage the light timer
  if ((lightTimerStart > 0) && (millis() - lightTimerStart >= lightTimerMilliseconds)) {
    lightTimerStart = millis(); // Serial.println("Restarted the light timer.");
    analogWrite(buzzer, buzzerTone); // Turn the buzzer On
    buzzerTimeStart = millis(); // Serial.println("Turned the  buzzer on.");
  }

  // Manage the buzzer operation
  if ((buzzerTimeStart > 0) && (millis() - buzzerTimeStart >= buzzTime)) {
    analogWrite(buzzer, 0);   // Turn the buzzer off
    buzzerTimeStart = 0; // Serial.println("Buzzer off.");
  }
  
}

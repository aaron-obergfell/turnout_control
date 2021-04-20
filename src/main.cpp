#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>

int numberOfServos = 2;
Servo servos[2];
int servoPins[] = {10, 11};
int inputPullupPins[] = {52, 53};
int minAngles[] = {0, 0};
int minLEDPins[] = {25, 23};
int maxAngles[] = {60, 100};
int maxLEDPins[] = {24, 22};
int oldValues[] = {1, 1};
int newValues[] = {1, 1};
boolean wasPressedInfos[] = {false, false};
double myTime = 0;
double starts[] = {0,0};
double duration = 1000;
boolean positions[] = {false, false};
int eepromAddresses[] = {0,1};

void setup()
{
  for(int i = 0; i < numberOfServos; i++) {
    servos[i].attach(servoPins[i]);
    if (EEPROM.read(eepromAddresses[i]) == 1) {
      servos[i].write(maxAngles[i]);
      positions[i] = true;
    } else {
      servos[i].write(minAngles[i]);
      positions[i] = false;
    }
    pinMode(minLEDPins[i], OUTPUT);
    pinMode(maxLEDPins[i], OUTPUT);
    pinMode(inputPullupPins[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
}

void loop() {
  myTime = millis();

  for(int i = 0; i < 2; i++) {
    newValues[i] = digitalRead(inputPullupPins[i]);
    if (wasPressedInfos[i] && myTime - starts[i] < duration) {
      if (positions[i]) {
        servos[i].write(minAngles[i] + (maxAngles[i] - minAngles[i]) * (myTime - starts[i]) / duration);
      } else {
        servos[i].write(maxAngles[i] + (minAngles[i] - maxAngles[i]) * (myTime - starts[i]) / duration);
      }
      if (((int)(myTime - starts[i])) %  200 < 100) {
        digitalWrite(maxLEDPins[i], LOW);
        digitalWrite(minLEDPins[i], HIGH);
      } else {
        digitalWrite(maxLEDPins[i], HIGH);
        digitalWrite(minLEDPins[i], LOW);
      }
    } else {
      wasPressedInfos[i] = false;
      if (!wasPressedInfos[i] && newValues[i] != oldValues[i] && newValues[i] == 0) {
        starts[i] = myTime;
        wasPressedInfos[i] = true;
        Serial.println("Schalter wurde gedrueckt");
        positions[i] = !positions[i];
        if (positions[i]) {
          EEPROM.update(eepromAddresses[i], 1);
        } else if (!positions[i]) {
          EEPROM.update(eepromAddresses[i], 0);
        }
      }
    }
    if (positions[i] && !wasPressedInfos[i]) {
      digitalWrite(maxLEDPins[i], HIGH);
      digitalWrite(minLEDPins[i], LOW);
    } else if (!positions[i] && !wasPressedInfos[i]) {
      digitalWrite(maxLEDPins[i], LOW);
      digitalWrite(minLEDPins[i], HIGH);
    }
  }
}

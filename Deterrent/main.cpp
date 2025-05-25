#include <Arduino.h>

#define LED1 13
#define LED2 14
#define LED3 26
#define LED4 27
#define BUZZER 12
#define TRIGGER_PIN 0

const unsigned long DURATION = 15000;
bool triggered = false;
int lastFreq = -1;  // Start with an invalid frequency

int leds[] = {LED1, LED2, LED3, LED4};

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  pinMode(BUZZER, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  randomSeed(analogRead(0));
}

void turnOffAllLEDs() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], LOW);
  }
}

void disorientPattern(unsigned long start) {
  while (millis() - start < DURATION) {
    int mode = random(4);
    for (int i = 0; i < 4; i++) {
      digitalWrite(leds[i], (random(0, 4) <= mode) ? HIGH : LOW);
    }
    delay(random(20, 150));
    turnOffAllLEDs();
    delay(random(50, 300));
  }
}

void chasePattern(unsigned long start) {
  while (millis() - start < DURATION) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(leds[i], HIGH);
      delay(150);
      digitalWrite(leds[i], LOW);
    }
  }
}

void bouncePattern(unsigned long start) {
  while (millis() - start < DURATION) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(leds[i], HIGH);
      delay(100);
      digitalWrite(leds[i], LOW);
    }
    for (int i = 2; i > 0; i--) {
      digitalWrite(leds[i], HIGH);
      delay(100);
      digitalWrite(leds[i], LOW);
    }
  }
}

void alternatePattern(unsigned long start) {
  while (millis() - start < DURATION) {
    digitalWrite(leds[0], HIGH);
    digitalWrite(leds[2], HIGH);
    delay(300);
    digitalWrite(leds[0], LOW);
    digitalWrite(leds[2], LOW);

    digitalWrite(leds[1], HIGH);
    digitalWrite(leds[3], HIGH);
    delay(300);
    digitalWrite(leds[1], LOW);
    digitalWrite(leds[3], LOW);
  }
}

void runRandomPattern(unsigned long start) {
  int pattern = random(4);
  switch (pattern) {
    case 0: disorientPattern(start); break;
    case 1: chasePattern(start); break;
    case 2: bouncePattern(start); break;
    case 3: alternatePattern(start); break;
  }
}

int getNewFrequency() {
  int newFreq;

  // Keep picking until we get a different value from last time
  do {
    newFreq = random(200, 251); // 200–250 Hz
  } while (newFreq == lastFreq);

  lastFreq = newFreq;
  return newFreq;
}

void loop() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(TRIGGER_PIN);

  if (lastState == HIGH && currentState == LOW && !triggered) {
    triggered = true;
    unsigned long startTime = millis();

    // Generate and play new buzzer tone
    int freq = getNewFrequency();
    tone(BUZZER, freq);

    // Run LED pattern
    runRandomPattern(startTime);

    // Stop after 15 seconds
    noTone(BUZZER);
    turnOffAllLEDs();
    triggered = false;
  }

  lastState = currentState;
}

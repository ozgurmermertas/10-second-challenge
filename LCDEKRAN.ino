#include <TM1637Display.h>

#define CLK 8
#define DIO 9
#define BUTTON 2

TM1637Display display(CLK, DIO);

byte state = 0;  // 0: sıfır, 1: çalışıyor, 2: durdu

unsigned long startTime = 0;
unsigned long elapsedTime = 0;

bool buttonState = HIGH;
bool lastReading = HIGH;
unsigned long lastDebounceTime = 0;

const unsigned long debounceDelay = 50;

void setup() {
  pinMode(BUTTON, INPUT);

  display.setBrightness(7);
  display.showNumberDecEx(0, 0b01000000, true);

  lastReading = digitalRead(BUTTON);
  buttonState = lastReading;
}

void loop() {
  bool reading = digitalRead(BUTTON);

  if (reading != lastReading) {
    lastDebounceTime = millis();
    lastReading = reading;
  }

  if (millis() - lastDebounceTime >= debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Sadece butona basıldığı anda işlem yap
      if (buttonState == LOW) {
        if (state == 0) {
          // İlk basış: Başlat
          startTime = millis();
          state = 1;
        }
        else if (state == 1) {
          // İkinci basış: Durdur
          elapsedTime += millis() - startTime;
          state = 2;
        }
        else {
          // Üçüncü basış: Sıfırla
          elapsedTime = 0;
          startTime = 0;
          state = 0;
          display.showNumberDecEx(0, 0b01000000, true);
        }
      }
    }
  }

  if (state == 1) {
    unsigned long totalTime =
        elapsedTime + millis() - startTime;

    int centiseconds = totalTime / 10;

    if (centiseconds > 9999) {
      centiseconds = 9999;
    }

    display.showNumberDecEx(
      centiseconds,
      0b01000000,
      true
    );
  }
}
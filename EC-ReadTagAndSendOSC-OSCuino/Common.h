
#pragma once

#define CTRL_LED 13

void errorBlink(unsigned int ledNum, unsigned int delayMs) {
  
  digitalWrite(ledNum, HIGH);
  delay(delayMs / 2);
  digitalWrite(ledNum, LOW);
  delay(delayMs / 2);
  
}


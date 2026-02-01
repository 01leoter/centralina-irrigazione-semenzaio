// Controllo lampeggio led
void controlloLED(unsigned long currentMillis) {
  // Verifica lo stato FISICO del relè (LOW = acceso, HIGH = spento)
  bool relayActive = (digitalRead(relayPin) == LOW);
  
  if (relayActive) {
    // LED FISSO quando irrigazione è attiva
    digitalWrite(ledPin, HIGH);
  } else if (WiFi.getMode() == WIFI_AP) {
    // LAMPEGGIO VELOCE quando AP è attivo e relè spento (500ms)
    if (currentMillis - previousMillis >= intervalAP) {
      previousMillis = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
  } else {
    // LAMPEGGIO LENTO in standby (200ms ON / 3000ms OFF)
    if (currentMillis - previousMillis <= intervalNormalON) {
      digitalWrite(ledPin, HIGH);
    } else if (currentMillis - previousMillis <= intervalNormalON + intervalNormalOFF) {
      digitalWrite(ledPin, LOW);
    } else {
      previousMillis = currentMillis;
    }
  }
}

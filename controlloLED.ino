// Controllo lampeggio led
void controlloLED(unsigned long currentMillis) {
  if(relayState || relayStateManual){
    digitalWrite(ledPin, HIGH);
  }
  else{
    if (WiFi.getMode() == WIFI_AP) {
      // Se in modalità AP, lampeggia ogni 500 ms
      if (currentMillis - previousMillis >= intervalAP) {
        previousMillis = currentMillis; // Salva il tempo dell'ultimo lampeggio
        // Cambia lo stato del LED (se acceso, spegni, se spento, accendi)
        digitalWrite(ledPin, !digitalRead(ledPin));
      }
    }
    else {
      // Se non in modalità AP, accendi per 500 ms e poi spegni per 3000 ms
      if (currentMillis - previousMillis <= intervalNormalON) {
        digitalWrite(ledPin, HIGH); // Accendi il LED
      } else if (currentMillis - previousMillis <= intervalNormalON + intervalNormalOFF) {
        digitalWrite(ledPin, LOW); // Spegni il LED
      } else {
        previousMillis = currentMillis; // Reimposta il tempo dell'ultimo cambiamento
      }
    }
  }
}
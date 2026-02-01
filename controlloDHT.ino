// Gestisce i dati del DHT
void controlloDHT() {
  if (currentMillis - previousMillisSensor >= intervalSensor) {
    previousMillisSensor = currentMillis;

    temperature = dht.getTemperature();
    humidity = dht.getHumidity();

    // Validazione lettura sensore
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Errore durante la lettura del sensore DHT!");
      return;
    }
    
    // Validazione range realistici per DHT11
    if (temperature < -40 || temperature > 80) {
      Serial.println("Temperatura fuori range valido DHT11");
      return;
    }
    
    if (humidity < 0 || humidity > 100) {
      Serial.println("Umidità fuori range valido");
      return;
    }

    // Verifica e aggiornamento temperatura massima
    if (temperature > temp_max) {
      temp_max = temperature;
      ora_temp_max = hour();
      salvaInEEPROM(0, temp_max);
      salvaInEEPROM(4 * sizeof(int), ora_temp_max);
    }

    // Verifica e aggiornamento temperatura minima
    if (temperature < temp_min) {
      temp_min = temperature;
      ora_temp_min = hour();
      salvaInEEPROM(sizeof(int), temp_min);
      salvaInEEPROM(5 * sizeof(int), ora_temp_min);
    }

    // Verifica e aggiornamento umidità massima
    if (humidity > umid_max) {
      umid_max = humidity;
      ora_umid_max = hour();
      salvaInEEPROM(2 * sizeof(int), umid_max);
      salvaInEEPROM(6 * sizeof(int), ora_umid_max);
    }

    // Verifica e aggiornamento umidità minima
    if (humidity < umid_min) {
      umid_min = humidity;
      ora_umid_min = hour();
      salvaInEEPROM(3 * sizeof(int), umid_min);
      salvaInEEPROM(7 * sizeof(int), ora_umid_min);
    }

    // Aggiorna le temperature giornaliere
    // TimeLib: 1=Domenica, 2=Lunedì, 3=Martedì, ..., 7=Sabato
    // Nostro array: 0=Lunedì, 1=Martedì, ..., 6=Domenica
    int currentDayIndex = (weekday() == 1) ? 6 : weekday() - 2;
    weeklyTemperatures[currentDayIndex].temperature = temperature;
    weeklyTemperatures[currentDayIndex].hour = hour();
    weeklyTemperatures[currentDayIndex].valid = true;
  }
}

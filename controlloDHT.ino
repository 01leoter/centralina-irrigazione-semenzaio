// Gestische i dati del DHT
void controlloDHT() {
  if (currentMillis - previousMillisSensor >= intervalSensor) {
    previousMillisSensor = currentMillis;
    EEPROM.begin(512); // Inizializzazione della libreria EEPROM

    temperature = dht.getTemperature();
    humidity = dht.getHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Errore durante la lettura del sensore DHT!");
      return;
    }

    // Verifica e aggiornamento dei valori massimi e minimi
  if (temperature > temp_max) {
    temp_max = temperature;
    ora_temp_max = hour();

    // Salvataggio nella EEPROM
    EEPROM.begin(512);
    EEPROM.put(0, temp_max);
    EEPROM.put(4 * sizeof(int), ora_temp_max);
    EEPROM.commit();
    EEPROM.end();
  }

  if (temperature < temp_min) {
    temp_min = temperature;
    ora_temp_min = hour();

    // Salvataggio nella EEPROM
    EEPROM.begin(512);
    EEPROM.put(sizeof(int), temp_min);
    EEPROM.put(5 * sizeof(int), ora_temp_min);
    EEPROM.commit();
    EEPROM.end();
  }

  if (humidity > umid_max) {
    umid_max = humidity;
    ora_umid_max = hour();

    // Salvataggio nella EEPROM
    EEPROM.begin(512);
    EEPROM.put(2 * sizeof(int), umid_max);
    EEPROM.put(6 * sizeof(int), ora_umid_max);
    EEPROM.commit();
    EEPROM.end();
  }

  if (humidity < umid_min) {
    umid_min = humidity;
    ora_umid_min = hour();

    // Salvataggio nella EEPROM
    EEPROM.begin(512);
    EEPROM.put(3 * sizeof(int), umid_min);
    EEPROM.put(7 * sizeof(int), ora_umid_min);
    EEPROM.commit();
    EEPROM.end();
  }

    // Aggiorna le temperature giornaliere
    int currentDayIndex = weekday() - 1; // Ottieni l'indice del giorno della settimana (da 0 a 6)
    weeklyTemperatures[currentDayIndex].temperature = temperature;
    weeklyTemperatures[currentDayIndex].hour = hour();
  }
}
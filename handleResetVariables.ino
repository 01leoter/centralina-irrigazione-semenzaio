// Resetta i valori salvati dopo il click del pulsante nel webserver
void handleResetVariables() {
  temperature = dht.getTemperature();
  humidity = dht.getHumidity();
  // Reimposta tutte le variabili
  temp_max = temperature;
  temp_min = temperature;
  umid_max = humidity;
  umid_min = humidity;
  ora_temp_max = hour();
  ora_temp_min = hour();
  ora_umid_max = hour();
  ora_umid_min = hour();

  // Salvataggio delle variabili aggiornate nella EEPROM
  EEPROM.begin(512); // Inizializzazione della libreria EEPROM
  EEPROM.put(0, temp_max);
  EEPROM.put(sizeof(int), temp_min);
  EEPROM.put(2 * sizeof(int), umid_max);
  EEPROM.put(3 * sizeof(int), umid_min);
  EEPROM.put(4 * sizeof(int), ora_temp_max);
  EEPROM.put(5 * sizeof(int), ora_temp_min);
  EEPROM.put(6 * sizeof(int), ora_umid_max);
  EEPROM.put(7 * sizeof(int), ora_umid_min);
  EEPROM.commit(); // Salvataggio effettivo dei dati nella EEPROM
  EEPROM.end(); // Fine utilizzo della EEPROM

  // Invia conferma al client
  server.send(200, "text/plain", "Tutte le variabili sono state azzerate!");
}
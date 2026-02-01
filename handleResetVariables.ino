// Azzera i valori min/max salvati
void handleResetVariables() {
  temp_max = -127;
  temp_min = 127;
  umid_max = 0;
  umid_min = 100;
  ora_temp_max = 0;
  ora_temp_min = 0;
  ora_umid_max = 0;
  ora_umid_min = 0;
  
  // Azzera anche le temperature settimanali
  for (int i = 0; i < 7; i++) {
    weeklyTemperatures[i].temperature = 0;
    weeklyTemperatures[i].hour = 0;
    weeklyTemperatures[i].valid = false;
  }

  // Salva in EEPROM
  EEPROM.begin(512);
  EEPROM.put(0, temp_max);
  EEPROM.put(sizeof(int), temp_min);
  EEPROM.put(2 * sizeof(int), umid_max);
  EEPROM.put(3 * sizeof(int), umid_min);
  EEPROM.put(4 * sizeof(int), ora_temp_max);
  EEPROM.put(5 * sizeof(int), ora_temp_min);
  EEPROM.put(6 * sizeof(int), ora_umid_max);
  EEPROM.put(7 * sizeof(int), ora_umid_min);
  EEPROM.commit();
  EEPROM.end();
  
  logEvent("Valori min/max azzerati");

  server.sendHeader("Location", "/");
  server.send(303);
}

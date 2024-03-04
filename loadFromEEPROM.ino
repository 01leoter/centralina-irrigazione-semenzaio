// Carica i valori dalla EEPROM
void loadFromEEPROM() {
  // Lettura delle variabili dalla EEPROM al caricamento dello sketch
  EEPROM.begin(512); // Inizializzazione della libreria EEPROM
  EEPROM.get(0, temp_max);
  EEPROM.get(sizeof(int), temp_min);
  EEPROM.get(2 * sizeof(int), umid_max);
  EEPROM.get(3 * sizeof(int), umid_min);
  EEPROM.get(4 * sizeof(int), ora_temp_max);
  EEPROM.get(5 * sizeof(int), ora_temp_min);
  EEPROM.get(6 * sizeof(int), ora_umid_max);
  EEPROM.get(7 * sizeof(int), ora_umid_min);
  for (int i = 0; i < 7; i++) {
    EEPROM.get((8+i) * sizeof(int), weeklySchedule[i].startHour);
    EEPROM.get((15+i) * sizeof(int), weeklySchedule[i].startMinute);
    EEPROM.get((22+i) * sizeof(int), weeklySchedule[i].endHour);
    EEPROM.get((29+i) * sizeof(int), weeklySchedule[i].endMinute);
  }
  EEPROM.commit();
  EEPROM.end();
}
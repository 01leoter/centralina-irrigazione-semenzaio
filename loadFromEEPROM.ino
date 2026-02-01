// Carica i valori dalla EEPROM
void loadFromEEPROM() {
  EEPROM.begin(512);
  EEPROM.get(0, temp_max);
  EEPROM.get(sizeof(int), temp_min);
  EEPROM.get(2 * sizeof(int), umid_max);
  EEPROM.get(3 * sizeof(int), umid_min);
  EEPROM.get(4 * sizeof(int), ora_temp_max);
  EEPROM.get(5 * sizeof(int), ora_temp_min);
  EEPROM.get(6 * sizeof(int), ora_umid_max);
  EEPROM.get(7 * sizeof(int), ora_umid_min);
  
  for (int i = 0; i < 7; i++) {
    EEPROM.get((8 + i) * sizeof(int), weeklySchedule[i].startHour);
    EEPROM.get((15 + i) * sizeof(int), weeklySchedule[i].startMinute);
    EEPROM.get((22 + i) * sizeof(int), weeklySchedule[i].durationMinutes);
  }
  
  EEPROM.end();
  
  // Sincronizza con RTC se disponibile
  if (rtc.begin() && !rtc.lostPower()) {
    DateTime now = rtc.now();
    // Imposta data e ora completa da RTC
    setTime(now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
    logEvent("Data e ora ripristinate da RTC");
    Serial.print("RTC: ");
    Serial.print(now.day());
    Serial.print("/");
    Serial.print(now.month());
    Serial.print("/");
    Serial.print(now.year());
    Serial.print(" ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.println(now.minute());
  } else {
    logEvent("ATTENZIONE: RTC non disponibile o ha perso alimentazione");
  }
  
  logEvent("Dati caricati da EEPROM");
}

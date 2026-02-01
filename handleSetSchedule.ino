// Imposta la programmazione di irrigazione
void handleSetSchedule() {
  EEPROM.begin(512);
  
  for (int i = 0; i < 7; i++) {
    weeklySchedule[i].startHour = server.arg("start_hour_" + String(i)).toInt();
    weeklySchedule[i].startMinute = server.arg("start_minute_" + String(i)).toInt();
    weeklySchedule[i].durationMinutes = server.arg("duration_" + String(i)).toInt();
    
    EEPROM.put((8 + i) * sizeof(int), weeklySchedule[i].startHour);
    EEPROM.put((15 + i) * sizeof(int), weeklySchedule[i].startMinute);
    EEPROM.put((22 + i) * sizeof(int), weeklySchedule[i].durationMinutes);
  }
  
  EEPROM.commit();
  EEPROM.end();
  
  logEvent("Programmazione irrigazione aggiornata");

  server.send(200, "text/plain", "Orari impostati con successo!");
}

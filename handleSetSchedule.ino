// Imposta la programmazione di irrigazione
void handleSetSchedule() {
  for (int i = 0; i < 7; i++) {
    weeklySchedule[i].startHour = server.arg("start_hour_" + String(i)).toInt();
    weeklySchedule[i].startMinute = server.arg("start_minute_" + String(i)).toInt();
    weeklySchedule[i].endHour = server.arg("end_hour_" + String(i)).toInt();
    weeklySchedule[i].endMinute = server.arg("end_minute_" + String(i)).toInt();
    
    EEPROM.begin(512);
    EEPROM.put((8+i) * sizeof(int), weeklySchedule[i].startHour);
    EEPROM.put((15+i) * sizeof(int), weeklySchedule[i].startMinute);
    EEPROM.put((22+i) * sizeof(int), weeklySchedule[i].endHour);
    EEPROM.put((29+i) * sizeof(int), weeklySchedule[i].endMinute);
    EEPROM.commit();
    EEPROM.end();
  }

  server.send(200, "text/plain", "Orari impostati con successo!");
}
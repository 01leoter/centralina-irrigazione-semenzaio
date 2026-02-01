// Imposta l'orario corrente e lo salva su RTC
void handleSetCurrentTime() {
  int currentHour = server.arg("current_hour").toInt();
  int currentMinute = server.arg("current_minute").toInt();
  int currentDayIndex = server.arg("current_day").toInt(); // 0=Lunedì, 6=Domenica

  // TimeLib usa: 1=Domenica, 2=Lunedì, 3=Martedì, ..., 7=Sabato
  // Noi usiamo: 0=Lunedì, 1=Martedì, ..., 6=Domenica
  // Conversione: Lunedì(0)→2, Martedì(1)→3, ..., Domenica(6)→1
  int timeDayOfWeek = (currentDayIndex == 6) ? 1 : currentDayIndex + 2;

  // Calcola una data fittizia con il giorno della settimana corretto
  // Usiamo gennaio 2024 come riferimento (1 gen 2024 = lunedì)
  int refDay = 1 + currentDayIndex; // 1=Lun, 2=Mar, ..., 7=Dom
  
  // Salva su RTC con data completa
  DateTime now = DateTime(2024, 1, refDay, currentHour, currentMinute, 0);
  rtc.adjust(now);
  
  // Imposta TimeLib
  setTime(currentHour, currentMinute, 0, refDay, 1, 2024);
  
  logEvent("Orario impostato: " + String(currentHour) + ":" + String(currentMinute) + " - " + giorni[currentDayIndex]);
  
  Serial.print("Debug - Giorno impostato: ");
  Serial.print(giorni[currentDayIndex]);
  Serial.print(" (index=");
  Serial.print(currentDayIndex);
  Serial.print(", weekday()=");
  Serial.print(weekday());
  Serial.println(")");

  server.send(200, "text/plain", "Orario impostato con successo!");
}

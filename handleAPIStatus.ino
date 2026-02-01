// API REST per monitoraggio remoto
void handleAPIStatus() {
  bool relayActive = (digitalRead(relayPin) == LOW);
  
  // TimeLib: 1=Domenica, 2=Lunedì, 3=Martedì, ..., 7=Sabato
  // Nostro array: 0=Lunedì, 1=Martedì, ..., 6=Domenica
  int currentDayIndex = (weekday() == 1) ? 6 : weekday() - 2;
  
  String json = "{";
  json += "\"temperatura\":" + String(temperature) + ",";
  json += "\"umidita\":" + String(humidity) + ",";
  json += "\"relay_attivo\":" + String(relayActive) + ",";
  json += "\"modalita_manuale\":" + String(manualMode) + ",";
  json += "\"manuale_on\":" + String(manualModeOn) + ",";
  json += "\"temp_max\":" + String(temp_max) + ",";
  json += "\"temp_min\":" + String(temp_min) + ",";
  json += "\"umid_max\":" + String(umid_max) + ",";
  json += "\"umid_min\":" + String(umid_min) + ",";
  json += "\"ora_attuale\":\"" + String(hour()) + ":" + String(minute()) + "\",";
  json += "\"giorno_settimana\":\"" + giorni[currentDayIndex] + "\"";
  json += "}";
  
  server.send(200, "application/json", json);
}

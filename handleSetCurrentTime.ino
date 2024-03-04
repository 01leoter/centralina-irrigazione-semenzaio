// Gestisce la richiesta di impostazione dell'ora e del giorno attuali
void handleSetCurrentTime() {
  if (server.hasArg("current_hour") && server.hasArg("current_minute") && server.hasArg("current_day")) {
    int currentHour = server.arg("current_hour").toInt();
    int currentMinute = server.arg("current_minute").toInt();
    int desiredDay = server.arg("current_day").toInt(); // Giorno desiderato della settimana

    // Calcola la differenza di giorni tra il giorno corrente e il giorno desiderato
    int currentDay = weekday() - 1; // Converti da 1-7 a 0-6 (Lunedì = 1)
    int dayDifference = desiredDay - currentDay;

    // Se la differenza è negativa, aggiungi 7 giorni per ottenere una differenza positiva equivalente
    if (dayDifference < 0) {
      dayDifference += 7;
    }

    // Aggiungi la differenza di giorni al tempo corrente per impostare il giorno desiderato
    adjustTime(dayDifference * SECS_PER_DAY);

    // Imposta l'ora attuale
    setTime(currentHour, currentMinute, 0, day(), month(), year());

    server.send(200, "text/plain", "Orario e giorno attuali impostati con successo!");
  } else {
    server.send(400, "text/plain", "Parametri mancanti");
  }
}
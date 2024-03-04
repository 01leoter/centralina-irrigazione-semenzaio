// Gestisce il relè
void updateRelayState() {
  int currentDayIndex = weekday() - 1; // Ottieni l'indice del giorno della settimana (da 0 a 6)
  int currentHour = hour();
  int currentMinute = minute();

  // Controlla se l'ora attuale è compresa negli orari di accensione e spegnimento per il giorno corrente
  bool shouldBeOn = (currentHour > weeklySchedule[currentDayIndex].startHour || (currentHour == weeklySchedule[currentDayIndex].startHour && currentMinute >= weeklySchedule[currentDayIndex].startMinute)) &&
                    (currentHour < weeklySchedule[currentDayIndex].endHour || (currentHour == weeklySchedule[currentDayIndex].endHour && currentMinute < weeklySchedule[currentDayIndex].endMinute));

  if (shouldBeOn && !relayState) {
    // Se dovrebbe essere acceso ma non lo è, accendi il relè
    digitalWrite(relayPin, LOW);
    relayState = true; // Aggiorna lo stato del relè
  } else if (!shouldBeOn && relayState) {
    // Se dovrebbe essere spento ma non lo è, spegni il relè
    digitalWrite(relayPin, HIGH);
    relayState = false; // Aggiorna lo stato del relè
  }
}
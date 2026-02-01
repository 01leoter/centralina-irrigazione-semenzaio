// Gestisce il relè in modalità automatica
void updateRelayState() {
  // Se siamo in modalità manuale ON, non fare nulla (resta acceso fino a comando manuale)
  if (manualMode && manualModeOn) {
    return;
  }
  
  // TimeLib: 1=Domenica, 2=Lunedì, 3=Martedì, ..., 7=Sabato
  // Nostro array: 0=Lunedì, 1=Martedì, ..., 6=Domenica
  int currentDayIndex = (weekday() == 1) ? 6 : weekday() - 2;
  
  int currentMinutes = hour() * 60 + minute();
  int startMinutes = weeklySchedule[currentDayIndex].startHour * 60 + 
                     weeklySchedule[currentDayIndex].startMinute;
  int endMinutes = startMinutes + weeklySchedule[currentDayIndex].durationMinutes;

  // Se siamo in modalità manuale OFF, controlla se è arrivato l'orario di STOP
  if (manualMode && !manualModeOn) {
    if (currentMinutes >= manualStopTime) {
      // È arrivato l'orario di STOP, riprendi il controllo automatico
      manualMode = false;
      logEvent("Orario di STOP raggiunto - riprendo controllo automatico");
    } else {
      // Non è ancora l'orario di STOP, resta spento
      return;
    }
  }

  // Controllo automatico normale
  bool shouldBeOn = false;
  
  // Se la durata è 0, non irrigare
  if (weeklySchedule[currentDayIndex].durationMinutes > 0) {
    if (endMinutes >= 1440) {
      // Caso attraverso mezzanotte (es. 23:30 durata 60min → fine 00:30)
      endMinutes = endMinutes - 1440;  // Normalizza
      shouldBeOn = (currentMinutes >= startMinutes || currentMinutes < endMinutes);
    } else {
      // Caso normale
      shouldBeOn = (currentMinutes >= startMinutes && currentMinutes < endMinutes);
    }
  }

  // Aggiorna lo stato del relè in base alla programmazione
  if (shouldBeOn && !relayState) {
    digitalWrite(relayPin, LOW);  // LOW = acceso
    relayState = true;
    logEvent("Irrigazione ATTIVATA (automatico)");
  } else if (!shouldBeOn && relayState) {
    digitalWrite(relayPin, HIGH);  // HIGH = spento
    relayState = false;
    logEvent("Irrigazione DISATTIVATA (automatico)");
  }
}

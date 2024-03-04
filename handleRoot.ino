void handleRoot() {
  // Ottieni l'orario e il giorno della settimana attuali
  int currentHour = hour();
  int currentMinute = minute();
  int currentDayIndex = weekday() - 1; // Ottieni l'indice del giorno della settimana (da 0 a 6)
  String currentDay = giorni[currentDayIndex]; // Ottieni il nome completo del giorno della settimana

  // Genera la pagina HTML per impostare l'ora e il giorno attuali
  String timeContent = "<div class='section'><h2>Imposta l'orario e il giorno attuali</h2>";
  timeContent += "<form action='/imposta_orario' method='POST'>";
  timeContent += "<label>Ora attuale:</label><input type='number' name='current_hour' min='0' max='23' value='" + String(currentHour) + "'><br>"; // Campo di input per l'ora attuale
  timeContent += "<label>Minuto attuale:</label><input type='number' name='current_minute' min='0' max='59' value='" + String(currentMinute) + "'><br>"; // Campo di input per il minuto attuale
  timeContent += "<label>Giorno attuale:</label><select name='current_day'>";
  for (int i = 0; i < 7; i++) {
    // Seleziona il giorno attuale nell'elenco
    String selected = (i == currentDayIndex) ? "selected" : "";
    timeContent += "<option value='" + String(i) + "' " + selected + ">" + giorni[i] + "</option>";
  }
  timeContent += "</select><br><input type='submit' value='Salva'></form></div>";

  // Componi la stringa con l'orario e il giorno attuali
  String currentTime = "<div class='section'><h2>Orario attuale</h2><p>" + String(currentHour) + ":" + String(currentMinute) + " - " + currentDay + "</p></div>";

  String setIrrigazione = "<div class='section'><h2>Imposta l'irrigazione</h2><form action='/set_schedule' method='POST'>";
  for (int i = 0; i < 7; i++) {
    setIrrigazione += "<div class='day'>";
    setIrrigazione += "<h3>" + String(giorni[i]) + "</h3>";
    setIrrigazione += "<div><label>Accensione:</label><input type='number' name='start_hour_" + String(i) + "' min='0' max='23' value='" + String(weeklySchedule[i].startHour) + "'>";
    setIrrigazione += ":<input type='number' name='start_minute_" + String(i) + "' min='0' max='59' value='" + String(weeklySchedule[i].startMinute) + "'></div>";
    setIrrigazione += "<div><label>Spegnimento:</label><input type='number' name='end_hour_" + String(i) + "' min='0' max='23' value='" + String(weeklySchedule[i].endHour) + "'>";
    setIrrigazione += ":<input type='number' name='end_minute_" + String(i) + "' min='0' max='59' value='" + String(weeklySchedule[i].endMinute) + "'></div>";
    setIrrigazione += "</div>";
  }
  setIrrigazione += "<input type='submit' value='Salva'></form></div>";

  String dhtContent = "<div class='section'><h2>Dati ambientali</h2>";
  dhtContent += "<p><strong>Temperatura:</strong> " + String(temperature) + " &deg;C</p>";
  dhtContent += "<p><strong>Umidità:</strong> " + String(humidity) + " %</p>";
  dhtContent += "<h3>Dati storici</h3>";
  dhtContent += "<p><strong>Temperatura massima:</strong> " + String(temp_max) + " &deg;C  alle ore: " + String(ora_temp_max) + "</p>";
  dhtContent += "<p><strong>Temperatura minima:</strong> " + String(temp_min) + " &deg;C  alle ore: " + String(ora_temp_min) + "</p>";
  dhtContent += "<p><strong>Umidità massima:</strong> " + String(umid_max) + " %  alle ore: " + String(ora_umid_max) + "</p>";
  dhtContent += "<p><strong>Umidità minima:</strong> " + String(umid_min) + " %  alle ore: " + String(ora_umid_min) + "</p></div>";

  // Calcola la media della temperatura dell'ultima settimana
  float weeklyTemperatureAverage = calculateWeeklyTemperatureAverage();

  // Aggiungi la media della temperatura alla pagina HTML
  String weeklyTemperatureContent = "<div class='section'><h3>Media della temperatura dell'ultima settimana</h3><p>" + String(weeklyTemperatureAverage, 1) + " &deg;C</p></div>";

  // Aggiungi il pulsante per il reset dei dati salvati
  String resetButton = "<div class='section'><form id='resetForm' action='/reset_variables' method='GET'><input type='submit' value='Azzera valori' onclick='return confirmReset()'></form></div>";

  // Aggiungi il footer
  String footer = "<footer><p>Leonardo Terenzi - 2024 - V 1.0</p></footer>";

  // Componi la pagina HTML completa
  String content = "<html><head><meta charset='UTF-8'><style>";
  content += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 0; background-color: #f5f5f5; }";
  content += ".container { max-width: 800px; margin: 20px auto; padding: 20px; background-color: #fff; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
  content += ".section { margin-bottom: 20px; padding-bottom: 20px; border-bottom: 1px solid #ddd; }";
  content += "h1, h2, h3 { color: #333; }";
  content += "label { display: inline-block; width: 120px; }";
  content += "input[type='number'] { width: 60px; }";
  content += "input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }";
  content += ".day { margin-bottom: 10px; }";
  content += "</style></head><body>";
  content += "<div class='container'>";
  content += "<h1>CENTRALINA IRRIGAZIONE</h1>";
  content += currentTime; // Aggiungi l'orario e il giorno attuali
  content += dhtContent;
  content += "<form id='resetForm' action='/reset_variables' method='GET'><input type='submit' value='Azzera valori' onclick='return confirmReset()'></form>";  // Aggiungi lo script JavaScript per la conferma
  content += "<script>";
  content += "function confirmReset() {";
  content += "  return confirm('Sei sicuro di voler azzerare tutti i valori?');";
  content += "}";
  content += "</script>";  content += weeklyTemperatureContent; // Aggiungi la media della temperatura dell'ultima settimana
  content += timeContent; // Aggiungi il form per impostare l'ora e il giorno attuali
  content += setIrrigazione;
  content += footer; // Aggiungi il footer
  content += "</div></body></html>";

  server.send(200, "text/html", content);
}

// Pagina principale web - ottimizzata con PROGMEM
const char HTML_HEADER[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<style>
body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 10px; background-color: #f5f5f5; }
.container { max-width: 800px; margin: 10px auto; padding: 20px; background-color: #fff; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); border-radius: 8px; }
.section { margin-bottom: 20px; padding-bottom: 20px; border-bottom: 1px solid #ddd; }
h1 { color: #2c5f2d; margin-bottom: 10px; }
h2 { color: #333; font-size: 1.3em; margin-bottom: 10px; }
h3 { color: #555; font-size: 1.1em; }
label { display: inline-block; width: 120px; font-weight: 500; }
input[type='number'] { width: 60px; padding: 5px; border: 1px solid #ddd; border-radius: 4px; }
input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; margin-top: 10px; }
input[type='submit']:hover { background-color: #45a049; }
.day { margin-bottom: 15px; padding: 10px; background-color: #f9f9f9; border-radius: 4px; }
.status { padding: 10px; border-radius: 4px; margin-bottom: 10px; }
.status.on { background-color: #d4edda; color: #155724; }
.status.off { background-color: #f8d7da; color: #721c24; }
footer { text-align: center; color: #888; margin-top: 20px; padding-top: 20px; border-top: 1px solid #ddd; font-size: 0.9em; }
</style>
</head>
<body>
<div class='container'>
)";

const char HTML_FOOTER[] PROGMEM = R"(
<footer><p>Leonardo Terenzi - 2026 - V 2.0 </p></footer>
</div>
</body>
</html>
)";

void handleRoot() {
  String html;
  html.reserve(3072); // Pre-alloca memoria
  
  // Header
  html += FPSTR(HTML_HEADER);
  html += F("<h1>🌱 CENTRALINA IRRIGAZIONE</h1>");
  
  // Stato irrigazione
  int currentHour = hour();
  int currentMinute = minute();
  
  // TimeLib: 1=Domenica, 2=Lunedì, 3=Martedì, ..., 7=Sabato
  // Nostro array: 0=Lunedì, 1=Martedì, ..., 6=Domenica
  // Conversione: weekday()==1 → index 6 (Domenica), weekday()==2 → index 0 (Lunedì), ecc.
  int currentDayIndex = (weekday() == 1) ? 6 : weekday() - 2;
  String currentDay = giorni[currentDayIndex];
  
  html += F("<div class='section'><h2>Stato Sistema</h2>");
  html += F("<p><strong>Orario attuale:</strong> ");
  if (currentHour < 10) html += "0";
  html += String(currentHour) + ":";
  if (currentMinute < 10) html += "0";
  html += String(currentMinute) + " - " + currentDay + "</p>";
  
  // Stato irrigazione - verifica lo stato FISICO del relè
  bool relayActive = (digitalRead(relayPin) == LOW);
  
  if (relayActive) {
    html += F("<div class='status on'><strong>🚿 IRRIGAZIONE ATTIVA</strong>");
    if (manualMode && manualModeOn) {
      html += F(" (MODALITÀ MANUALE - premi lungo per spegnere)");
    }
    html += F("</div>");
  } else {
    html += F("<div class='status off'><strong>⏸️ IRRIGAZIONE DISATTIVATA</strong>");
    if (manualMode && !manualModeOn) {
      int currentDayIndex = (weekday() == 1) ? 6 : weekday() - 2;
      int startMinutes = weeklySchedule[currentDayIndex].startHour * 60 + 
                        weeklySchedule[currentDayIndex].startMinute;
      int stopMinutes = startMinutes + weeklySchedule[currentDayIndex].durationMinutes;
      int stopHour = stopMinutes / 60;
      int stopMinute = stopMinutes % 60;
      
      // Gestisci overflow oltre mezzanotte
      if (stopHour >= 24) stopHour -= 24;
      
      html += F(" (MANUALE - riprende alle ");
      if (stopHour < 10) html += "0";
      html += String(stopHour) + ":";
      if (stopMinute < 10) html += "0";
      html += String(stopMinute) + ")";
    }
    html += F("</div>");
  }
  html += F("</div>");
  
  // Dati ambientali
  html += F("<div class='section'><h2>🌡️ Dati Ambientali</h2>");
  html += F("<p><strong>Temperatura:</strong> ") + String(temperature) + F(" °C</p>");
  html += F("<p><strong>Umidità:</strong> ") + String(humidity) + F(" %</p>");
  html += F("<h3>Dati Storici</h3>");
  html += F("<p><strong>Temp. massima:</strong> ") + String(temp_max) + F(" °C alle ore ") + String(ora_temp_max) + F("</p>");
  html += F("<p><strong>Temp. minima:</strong> ") + String(temp_min) + F(" °C alle ore ") + String(ora_temp_min) + F("</p>");
  html += F("<p><strong>Umidità massima:</strong> ") + String(umid_max) + F(" % alle ore ") + String(ora_umid_max) + F("</p>");
  html += F("<p><strong>Umidità minima:</strong> ") + String(umid_min) + F(" % alle ore ") + String(ora_umid_min) + F("</p>");
  
  // Media settimanale
  float weeklyAvg = calculateWeeklyTemperatureAverage();
  if (weeklyAvg != -999) {
    html += F("<p><strong>Media temp. settimanale:</strong> ") + String(weeklyAvg, 1) + F(" °C</p>");
  }
  
  html += F("<form action='/reset_variables' method='GET'>");
  html += F("<input type='submit' value='Azzera Valori' onclick='return confirm(\"Sei sicuro di voler azzerare tutti i valori?\")'>");
  html += F("</form></div>");
  
  // Imposta orario
  html += F("<div class='section'><h2>⏰ Imposta Orario</h2>");
  html += F("<form action='/imposta_orario' method='POST'>");
  html += F("<label>Ora:</label><input type='number' name='current_hour' min='0' max='23' value='") + String(currentHour) + F("'><br>");
  html += F("<label>Minuto:</label><input type='number' name='current_minute' min='0' max='59' value='") + String(currentMinute) + F("'><br>");
  html += F("<label>Giorno:</label><select name='current_day'>");
  for (int i = 0; i < 7; i++) {
    String selected = (i == currentDayIndex) ? "selected" : "";
    html += F("<option value='") + String(i) + "' " + selected + ">" + giorni[i] + F("</option>");
  }
  html += F("</select><br><input type='submit' value='Salva'></form></div>");
  
  // Programmazione irrigazione
  html += F("<div class='section'><h2>💧 Programmazione Irrigazione</h2>");
  html += F("<form action='/set_schedule' method='POST'>");
  for (int i = 0; i < 7; i++) {
    html += F("<div class='day'><h3>") + String(giorni[i]) + F("</h3>");
    html += F("<div><label>Inizio:</label>");
    html += F("<input type='number' name='start_hour_") + String(i) + F("' min='0' max='23' value='") + String(weeklySchedule[i].startHour) + F("'>");
    html += F(":<input type='number' name='start_minute_") + String(i) + F("' min='0' max='59' value='") + String(weeklySchedule[i].startMinute) + F("'></div>");
    html += F("<div><label>Durata:</label>");
    html += F("<select name='duration_") + String(i) + F("'>");
    
    // Opzioni durata: 0, 5, 10, 15, ..., 60 minuti
    for (int dur = 0; dur <= 60; dur += 5) {
      String selected = (dur == weeklySchedule[i].durationMinutes) ? "selected" : "";
      html += F("<option value='") + String(dur) + "' " + selected + ">" + String(dur) + F(" min</option>");
    }
    
    html += F("</select></div>");
    html += F("</div>");
  }
  html += F("<input type='submit' value='Salva Programmazione'></form></div>");
  
  // Footer
  html += FPSTR(HTML_FOOTER);
  
  server.send(200, F("text/html"), html);
}

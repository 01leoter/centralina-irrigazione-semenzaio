// Calcola la media settimanale di temperatura
float calculateWeeklyTemperatureAverage() {
  float totalTemperature = 0;
  int count = 0;

  for (int i = 0; i < 7; i++) {
    if (weeklyTemperatures[i].temperature != 0) {
      totalTemperature += weeklyTemperatures[i].temperature;
      count++;
    }
  }

  if (count > 0) {
    return totalTemperature / count;
  } else {
    return 0; // Non ci sono dati disponibili per calcolare la media
  }
}
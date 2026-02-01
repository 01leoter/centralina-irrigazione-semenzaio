#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <TimeLib.h>
#include <DHTesp.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Bounce2.h>

/*
 * MAPPATURA PIN per ESP8266:
 * Se la tua scheda NON riconosce D3, D5, D6, D4, usa i numeri GPIO:
 * 
 * NodeMCU/Wemos D1 Mini:     Generic ESP8266:
 * D1 = GPIO5 (RTC SCL)       GPIO5 (RTC SCL)
 * D2 = GPIO4 (RTC SDA)       GPIO4 (RTC SDA)
 * D3 = GPIO0 (Pulsante)      GPIO0 (Pulsante)
 * D4 = GPIO2 (DHT Data)      GPIO2 (DHT Data)
 * D5 = GPIO14 (LED)          GPIO14 (LED)
 * D6 = GPIO12 (Relè)         GPIO12 (Relè)
 */

ESP8266WebServer server(80);
RTC_DS3231 rtc;
Bounce button = Bounce();

// Pin configuration
const int buttonPin = 0;   // D3 = GPIO0
const int ledPin = 14;     // D5 = GPIO14
const int relayPin = 12;   // D6 = GPIO12
const int dhtPin = 2;      // D4 = GPIO2

// Relay state management
bool relayState = false;           // Stato del relè (true = acceso)
bool manualMode = false;           // Modalità manuale attiva
bool manualModeOn = false;         // true = acceso manualmente, false = spento manualmente
unsigned long manualStopTime = 0;  // Orario (in minuti) fino a cui restare spenti se spento manualmente

// WiFi AP configuration
const char *ssid = "Irrigazione semenzaio";
const char *password = "";
bool apMode = false;
unsigned long apStartTime;

// Timing variables
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long intervalAP = 500;
unsigned long intervalNormalON = 200;
unsigned long intervalNormalOFF = 3000;
unsigned long intervalSensor = 6000;
unsigned long previousMillisSensor = 0;
unsigned long pressStartTime = 0;

// EEPROM management
unsigned long lastEEPROMSave = 0;
const unsigned long EEPROM_SAVE_INTERVAL = 60000; // Salva max 1 volta al minuto

// Days of the week
String giorni[] = {"Lunedì", "Martedì", "Mercoledì", "Giovedì", "Venerdì", "Sabato", "Domenica"};

// Schedule structure
struct Schedule {
  int startHour;
  int startMinute;
  int durationMinutes;  // Durata in minuti (0-60, step di 5)
};

Schedule weeklySchedule[7];

// DHT sensor
DHTesp dht;
int temperature;
int humidity;
int temp_max = -127;   // Valore impossibilmente basso
int temp_min = 127;    // Valore impossibilmente alto
int umid_max = 0;
int umid_min = 100;
int ora_temp_max;
int ora_temp_min;
int ora_umid_max;
int ora_umid_min;

// Weekly temperature tracking
struct DailyTemperature {
  int temperature;
  int hour;
  bool valid;
};

DailyTemperature weeklyTemperatures[7];

// Function to sync time from RTC
time_t syncRTC() {
  DateTime now = rtc.now();
  return now.unixtime();
}

// Safe EEPROM save with rate limiting
void salvaInEEPROM(int address, int value) {
  if (millis() - lastEEPROMSave > EEPROM_SAVE_INTERVAL) {
    EEPROM.begin(512);
    EEPROM.put(address, value);
    EEPROM.commit();
    EEPROM.end();
    lastEEPROMSave = millis();
  }
}

// Log events for debugging
void logEvent(String event) {
  Serial.print("[");
  if (hour() < 10) Serial.print("0");
  Serial.print(hour());
  Serial.print(":");
  if (minute() < 10) Serial.print("0");
  Serial.print(minute());
  Serial.print("] ");
  Serial.println(event);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // Pin setup
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  
  // IMPORTANTE: Inizializza PRIMA il pin, POI le variabili
  digitalWrite(relayPin, HIGH);  // HIGH = relè SPENTO (logica invertita)
  digitalWrite(ledPin, LOW);
  
  // Inizializza stati
  relayState = false;
  manualMode = false;
  manualModeOn = false;
  manualStopTime = 0;
  
  // Button setup with debouncing
  button.attach(buttonPin, INPUT_PULLUP);
  button.interval(50); // 50ms debounce
  
  // WiFi off by default
  WiFi.mode(WIFI_OFF);
  
  // RTC initialization
  if (!rtc.begin()) {
    Serial.println("ERRORE: RTC non trovato!");
    logEvent("RTC non trovato - verificare collegamenti");
  } else {
    if (rtc.lostPower()) {
      logEvent("RTC ha perso alimentazione - configurare orario");
    }
    setSyncProvider(syncRTC);
    setSyncInterval(60); // Sincronizza ogni minuto
    logEvent("RTC inizializzato correttamente");
  }
  
  // DHT sensor setup
  dht.setup(dhtPin, DHTesp::DHT11);
  
  // Web server routes
  server.on("/", handleRoot);
  server.on("/imposta_orario", HTTP_POST, handleSetCurrentTime);
  server.on("/set_schedule", HTTP_POST, handleSetSchedule);
  server.on("/reset_variables", HTTP_GET, handleResetVariables);
  server.on("/api/status", HTTP_GET, handleAPIStatus);
  server.begin();
  
  // Load data from EEPROM
  loadFromEEPROM();
  
  logEvent("Sistema avviato");
}

void loop() {
  currentMillis = millis();
  server.handleClient();
  button.update();
  
  // Check if AP mode should be disabled after 5 minutes
  if (apMode && millis() - apStartTime >= 300000) {
    WiFi.softAPdisconnect(true);
    apMode = false;
    logEvent("Modalità AP disattivata (timeout)");
  }
  
  // Button handling
  if (button.fell()) {
    pressStartTime = millis();
  }
  
  if (button.rose()) {
    unsigned long pressDuration = millis() - pressStartTime;
    
    if (pressDuration >= 3000) {
      // Long press - Comando manuale
      bool currentState = (digitalRead(relayPin) == LOW); // true se acceso
      
      if (currentState) {
        // Il relè è ACCESO, lo spengo
        digitalWrite(relayPin, HIGH);
        relayState = false;
        
        if (manualMode && manualModeOn) {
          // Sto spegnendo un relè che avevo acceso manualmente
          manualMode = false;
          manualModeOn = false;
          logEvent("MANUALE: relè spento - torno in automatico");
        } else {
          // Sto spegnendo un relè acceso dall'automatico
          // Resto spento fino all'orario di STOP programmato (inizio + durata)
          int currentDayIndex = (weekday() == 1) ? 6 : weekday() - 2;
          int startMinutes = weeklySchedule[currentDayIndex].startHour * 60 + 
                            weeklySchedule[currentDayIndex].startMinute;
          manualStopTime = startMinutes + weeklySchedule[currentDayIndex].durationMinutes;
          manualMode = true;
          manualModeOn = false;
          logEvent("MANUALE: relè spento - riprendo all'orario di stop");
        }
      } else {
        // Il relè è SPENTO, lo accendo
        digitalWrite(relayPin, LOW);
        relayState = true;
        manualMode = true;
        manualModeOn = true;
        logEvent("MANUALE: relè acceso - resto acceso fino a spegnimento manuale");
      }
    } else {
      // Short press - activate AP mode
      WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
      WiFi.softAP(ssid, password);
      apMode = true;
      apStartTime = millis();
      logEvent("Modalità AP attivata");
    }
  }
  
  // LED control
  controlloLED(currentMillis);
  
  // Relay control (only if not in manual mode)
  updateRelayState();
  
  // DHT sensor reading
  controlloDHT();
}

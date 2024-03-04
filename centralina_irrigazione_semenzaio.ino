#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <TimeLib.h>
#include <DHTesp.h>
#include <EEPROM.h>

ESP8266WebServer server(80);

const int buttonPin = D3; // Pin del pulsante
const int ledPin = D5; // Pin per l'indicatore LED
const int relayPin = D6; // Pin del relè per l'irrigazione
const int dhtPin = D4;

bool lastButtonState = true; // Lo stato precedente del pulsante
bool buttonPressed = false; // Flag che indica se il pulsante è stato premuto
bool relayState = false; // Lo stato corrente del relè
bool relayStateManual = false; // Lo stato corrente del relè

const char *ssid = "Centralina irrigazione"; // Nome della rete AP
const char *password = ""; // Password della rete AP

unsigned long currentMillis = 0;
unsigned long previousMillis = 0; // Variabile per tenere traccia del tempo trascorso
unsigned long intervalAP = 500; // Lampeggio led in modalità AP 
unsigned long intervalNormalON = 200; // Lampeggio led in modalità operativa
unsigned long intervalNormalOFF = 3000; // Lampeggio led in modalità operativa
unsigned long intervalSensor = 6000; // Intervallo lettura DHT
unsigned long previousMillisSensor = 0;
unsigned long pressStartTime = 0; // Il momento in cui è iniziata la pressione del pulsante

bool apMode = false; // Variabile di stato per indicare se l'ESP è in modalità AP
unsigned long apStartTime; // Timestamp di quando è iniziata la modalità AP

// Variabile globale per memorizzare i nomi dei giorni della settimana
String giorni[] = {"Lunedì", "Martedì", "Mercoledì", "Giovedì", "Venerdì", "Sabato", "Domenica"};

// Struttura per memorizzare gli orari di accensione e spegnimento per ciascun giorno
struct Schedule {
  int startHour;
  int startMinute;
  int endHour;
  int endMinute;
};

Schedule weeklySchedule[7]; // Array per memorizzare gli orari per ogni giorno della settimana

DHTesp dht;

int temperature;
int humidity;
int temp_max;
int temp_min;
int umid_max;
int umid_min;
int ora_temp_max;
int ora_temp_min;
int ora_umid_max;
int ora_umid_min;

struct DailyTemperature {
  int temperature;
  int hour;
};

DailyTemperature weeklyTemperatures[7];


void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(ledPin, LOW); 
  WiFi.mode(WIFI_OFF);
  digitalWrite(relayPin, HIGH); 

  dht.setup(dhtPin, DHTesp::DHT11);

  server.on("/", handleRoot);
  server.on("/imposta_orario", HTTP_POST, handleSetCurrentTime);
  server.on("/set_schedule", HTTP_POST, handleSetSchedule);
  server.on("/reset_variables", HTTP_GET, handleResetVariables);
  server.begin(); // Avvia il server

  loadFromEEPROM();
}


void loop() {
  currentMillis = millis();
  server.handleClient(); // Gestisci le richieste del client
  // Leggi lo stato del pulsante
  bool buttonState = digitalRead(buttonPin);
  // Controlla se è necessario disattivare la modalità AP dopo 10 minuti
  if (apMode && millis() - apStartTime >= 300000) { // 300000 millisecondi = 5 minuti
    WiFi.softAPdisconnect(true); // Disconnetti la modalità AP
    apMode = false; // Imposta il flag di modalità AP su false
  }

  // Se il pulsante è stato premuto
  if (buttonState != lastButtonState) {
    // Se il pulsante è stato premuto
    if (buttonState == LOW) {
      // Registra il momento in cui è iniziata la pressione
      pressStartTime = millis();
      buttonPressed = true; // Imposta il flag a true
    } else { // Se il pulsante è stato rilasciato
      // Se il pulsante è stato premuto per più di 3 secondi
      if (buttonPressed && (millis() - pressStartTime >= 3000)) {
        // Cambia lo stato del relè
        digitalWrite(relayPin, !digitalRead(relayPin));
        relayStateManual = !digitalRead(relayPin);
      } else { // Se è stata una pressione breve
        WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
        WiFi.softAP(ssid, password); // Avvia la modalità AP
        apMode = true; // Imposta il flag di modalità AP su true
        apStartTime = millis(); // Registra il tempo di inizio della modalità AP
      }
      buttonPressed = false; // Resetta il flag
    }
  }
  // Salva lo stato del pulsante per il prossimo ciclo
  lastButtonState = buttonState;

  controlloLED(millis());

  updateRelayState();

  controlloDHT();
}
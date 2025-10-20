#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// Configuração dos pinos
#define ONE_WIRE_BUS 2         // DS18B20 conectado no GPIO2
#define HUMIDITY_PIN 34        // Sensor capacitivo umidade - ADC1 CH6
#define PH_PIN 35              // Sensor de pH - ADC1 CH7
#define GAS_PIN 32             // Sensor MQ135 - ADC1 CH4
#define FAN_PIN 15             // Ventilador
#define PUMP_PIN 4             // Bomba de água
#define LED_PIN 13             // Led status

// Limites do sistema
const float TEMP_MIN = 55.0;
const float TEMP_MAX = 65.0;
const float HUM_MIN = 40.0;
const float HUM_MAX = 60.0;
const float PH_MIN = 6.5;
const float PH_MAX = 8.0;
const int GAS_ODOR_THRESHOLD = 350;  // Threshold MQ135

// WiFi + MQTT
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";
const char* mqtt_server = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient client(espClient);

// Objetos sensores
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 3000;  // Em ms

// Função para conectar WiFi
void setupWifi() {
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// Reconexão MQTT
void reconnect() {
  while (!client.connected()) {
    if (client.connect("CompostechUnit01")) {
      // Conectado ao broker MQTT
    } else {
      delay(5000);
    }
  }
}

// Leitura do sensor de umidade capacitiva (simplificado)
float readHumidity() {
  int raw = analogRead(HUMIDITY_PIN);
  float voltage = (raw / 4095.0) * 3.3;
  // Converter voltagem para porcentagem via calibração
  float humidity = map(voltage * 1000, 0, 3300, 0, 100);
  return humidity;
}

// Leitura sensor pH (simplificado)
float readPH() {
  int raw = analogRead(PH_PIN);
  float voltage = (raw / 4095.0) * 3.3;
  float phValue = (voltage * 3.5); // Ajustar calibração conforme hardware
  return phValue;
}

// Leitura sensor MQ135 (simplificado)
int readGas() {
  int raw = analogRead(GAS_PIN);
  return raw;  // Calibrar para PPM conforme sensores reais
}

// Função média móvel para filtragem básica (exemplo)
float movingAverage(float newValue, float* samples, int size) {
  static int index = 0;
  static int count = 0;

  samples[index] = newValue;
  index = (index + 1) % size;
  if (count < size) count++;

  float sum = 0;
  for (int i = 0; i < count; i++) sum += samples[i];
  return sum / count;
}

// Variables para suavização
float tempSamples[10] = {0};
float humSamples[10] = {0};
float phSamples[10] = {0};
float gasSamples[10] = {0};

void setup() {
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(FAN_PIN, LOW);
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Compostech Init");

  tempSensor.begin();

  setupWifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = now;

    tempSensor.requestTemperatures();
    float tempRaw = tempSensor.getTempCByIndex(0);
    float humRaw = readHumidity();
    float phRaw = readPH();
    int gasRaw = readGas();

    // Filtragem média móvel
    float tempF = movingAverage(tempRaw, tempSamples, 10);
    float humF = movingAverage(humRaw, humSamples, 10);
    float phF = movingAverage(phRaw, phSamples, 10);
    float gasF = movingAverage((float)gasRaw, gasSamples, 10);

    // Controle automático dos atuadores:
    bool fanOn = false;
    bool pumpOn = false;

    if (tempF > TEMP_MAX) {
      digitalWrite(FAN_PIN, HIGH);
      fanOn = true;
    } else if (tempF < TEMP_MIN) {
      digitalWrite(FAN_PIN, LOW);
      fanOn = false;
    }

    if (humF < HUM_MIN) {
      digitalWrite(PUMP_PIN, HIGH);
      pumpOn = true;
    } else if (humF > HUM_MAX) {
      digitalWrite(PUMP_PIN, LOW);
      pumpOn = false;
    }

    if (gasF >= GAS_ODOR_THRESHOLD) {
      digitalWrite(FAN_PIN, HIGH);
      digitalWrite(PUMP_PIN, HIGH);
      fanOn = true;
      pumpOn = true;
      client.publish("/compostech/alert/odor", "Alta concentração gas");
    }

    if (phF < PH_MIN || phF > PH_MAX) {
      client.publish("/compostech/alert/ph", "PH fora do ideal");
    }

    // Publicação MQTT
    char msg[50];
    snprintf(msg, sizeof(msg), "%.2f", tempF);
    client.publish("/compostech/data/temperature", msg);
    snprintf(msg, sizeof(msg), "%.2f", humF);
    client.publish("/compostech/data/humidity", msg);
    snprintf(msg, sizeof(msg), "%.2f", phF);
    client.publish("/compostech/data/ph", msg);
    snprintf(msg, sizeof(msg), "%d", (int)gasF);
    client.publish("/compostech/data/gas", msg);

    // Atualizar display LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("Tmp: %.1f C\n", tempF);
    lcd.print("Hum: "); lcd.print(humF); lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.printf("pH: %.2f ", phF);
    lcd.print(fanOn ? "FAN ON " : "FAN OFF");
    lcd.print(pumpOn ? "PUMP ON" : "PUMP OFF");

    // LED indicador de status
    if (fanOn || pumpOn) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

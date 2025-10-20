#include <WiFi.h>
#include <PubSubClient.h>
// Bibliotecas dos sensores e atuadores podem ser específicas conforme modelo
#include <DS18B20.h>    // Temperatura
#include <CapacitiveSensor.h> // Umidade
#include <MQ135.h>      // Gás
#include <LiquidCrystal_I2C.h> // LCD via I2C

// Definir pinos físicos
#define PIN_TEMP D2
#define PIN_HUM A0
#define PIN_PH  A1
#define PIN_GAS A2
#define PIN_FAN D3
#define PIN_PUMP D4
#define PIN_LED D5

// Limites biológicos
const float TEMP_MIN = 55.0;
const float TEMP_MAX = 65.0;
const float HUM_MIN = 40.0;
const float HUM_MAX = 60.0;
const float PH_MIN = 6.5;
const float PH_MAX = 8.0;
const int MQ135_ODOR_THRESHOLD = 350; // calibrar via testes

// Rede e MQTT
const char* ssid = "SeuSSID";
const char* password = "SuaSenha";
const char* mqtt_server = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient client(espClient);

// Instâncias sensores
DS18B20 tempSensor(PIN_TEMP);
CapacitiveSensor humSensor(PIN_HUM);
MQ135 gasSensor(PIN_GAS);
LiquidCrystal_I2C lcd(0x27, 20, 4); // Endereço pode variar

// Funções utilitárias
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("CompostechUnit01")) break;
    delay(5000);
  }
}

// Setup inicial
void setup() {
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Iniciando sistema...");
}

// Função para filtrar ruído (média móvel simples)
float readFiltered(CapacitiveSensor& s, int n=5) {
  float soma = 0;
  for(int i=0; i<n; i++) soma += s.read();
  return soma/n;
}

// Função principal do loop
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Leitura dos sensores com filtragem
  float temp = tempSensor.getTempC();
  float hum  = readFiltered(humSensor);
  float ph   = analogRead(PIN_PH) / 50.0; // ajustar calibração
  int   gas  = gasSensor.getPPM();

  // Controle inteligente dos atuadores
  bool fanStatus = false;
  bool pumpStatus = false;

  if (temp > TEMP_MAX) {
    digitalWrite(PIN_FAN, HIGH);
    fanStatus = true;
  } else {
    digitalWrite(PIN_FAN, LOW);
    fanStatus = false;
  }

  if (hum < HUM_MIN) {
    digitalWrite(PIN_PUMP, HIGH);
    pumpStatus = true;
  } else if (hum > HUM_MAX) {
    digitalWrite(PIN_PUMP, LOW);
    pumpStatus = false;
  }

  if (gas >= MQ135_ODOR_THRESHOLD) {
    digitalWrite(PIN_FAN, HIGH);
    digitalWrite(PIN_PUMP, HIGH);
    client.publish("/compostech/alert/odor", "Alerta de odor/amonia!");
  }

  if (ph < PH_MIN || ph > PH_MAX) {
    client.publish("/compostech/alert/ph", "Alerta pH fora do ideal");
  }

  // Publicação dos dados via MQTT
  char tempStr[10], humStr[10], phStr[10], gasStr[10];
  sprintf(tempStr, "%.2f", temp);
  sprintf(humStr, "%.2f", hum);
  sprintf(phStr, "%.2f", ph);
  sprintf(gasStr, "%d", gas);

  client.publish("/compostech/data/temperature", tempStr);
  client.publish("/compostech/data/humidity", humStr);
  client.publish("/compostech/data/ph", phStr);
  client.publish("/compostech/data/gas", gasStr);

  // Interface LCD local
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Temp:"); lcd.print(temp); lcd.print("C");
  lcd.setCursor(0,1); lcd.print("Hum:"); lcd.print(hum); lcd.print("%");
  lcd.setCursor(0,2); lcd.print("pH:");  lcd.print(ph);
  lcd.setCursor(0,3); lcd.print(fanStatus ? "Vent.:ON " : "Vent.:OFF ");
  lcd.print(pumpStatus ? "Agua:ON" : "Agua:OFF");

  // LED status
  if (gas >= MQ135_ODOR_THRESHOLD || ph < PH_MIN || ph > PH_MAX) {
    digitalWrite(PIN_LED, HIGH); // Vermelho alerta
  } else {
    digitalWrite(PIN_LED, LOW);  // Verde operação normal
  }

  // Intervalo de ciclo
  delay(3000);

  // Segurança: watchdog timer (pseudocódigo, usar biblioteca específica conforme MCU)
  // if (travamentoDetectado) ESP.restart();
}

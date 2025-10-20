// Compostech Firmware - Pseudocódigo Estruturado
// Conceito: Controle automatizado e inteligente da compostagem com IoT

#include <Sensor.h>      // Classe base para sensores
#include <Actuator.h>    // Classe base para atuadores
#include <WiFiManager.h>
#include <MQTTClient.h>
#include <DataLogger.h>

// Constantes ideais
const float TEMP_MIN = 55.0;     // temperatura mínima ideal °C
const float TEMP_MAX = 65.0;     
const float HUM_MIN = 40.0;      // umidade mínima %
const float HUM_MAX = 60.0;
const float PH_MIN = 6.5;
const float PH_MAX = 8.0;

// Instâncias de sensores
TemperatureSensor tempSensor(D2);
HumiditySensor humSensor(A0);
GasSensor mq135(A1);
PHSensor phSensor(A2);

// Instâncias de atuadores
Fan ventilador(D3);
WaterPump bomba(D4);
LEDStatus indicador(D5);

// Comunicação
WiFiManager wifi;
MQTTClient mqtt("broker_server", 1883);
DataLogger logger("/data/log.txt");

unsigned long lastUpdate = 0;
const unsigned long INTERVAL = 3000; // 3 segundos

void setup() {
    wifi.connect("SSID", "senha");
    mqtt.connect("CompostechUnit01");

    tempSensor.calibrate();
    humSensor.calibrate();
    phSensor.calibrate();

    logger.initialize();
    indicador.setStatus(BLUE); // Sistema inicializando
}

void loop() {
    unsigned long now = millis();

    // Execução não bloqueante
    if (now - lastUpdate >= INTERVAL) {
        lastUpdate = now;

        float temp = tempSensor.read();
        float hum  = humSensor.read();
        float ph   = phSensor.read();
        float gas  = mq135.read();

        // Controle inteligente
        if (temp > TEMP_MAX) ventilador.on();
        else if (temp < TEMP_MIN) ventilador.off();

        if (hum < HUM_MIN) bomba.on();
        else if (hum > HUM_MAX) bomba.off();

        if (gas >= THRESHOLD_ODOR) { // prevenção de odores
            ventilador.on();
            bomba.on();
            mqtt.publish("alert/odor", "High ammonia detected");
        }

        // Status pH
        if (ph < PH_MIN || ph > PH_MAX)
            mqtt.publish("alert/ph", "pH irregular - verificar nutrientes");

        // Envio de dados
        mqtt.publish("data/temperature", String(temp));
        mqtt.publish("data/humidity", String(hum));
        mqtt.publish("data/ph", String(ph));
        mqtt.publish("data/gas", String(gas));

        logger.save(temp, hum, ph, gas);
        indicador.setStatus(GREEN);
    }

    // Modo econômico
    if (!wifi.isConnected()) sleepMode();
}

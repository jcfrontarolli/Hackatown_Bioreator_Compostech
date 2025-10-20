import threading
import time
from flask import Flask, jsonify, render_template, request
import paho.mqtt.client as mqtt

app = Flask(__name__)

# Estado global protegido por lock
sensor_data = {
    "temperature": None,
    "humidity": None,
    "ph": None,
    "gas": None,
    "alerts": []
}
data_lock = threading.Lock()

MQTT_BROKER = 'broker.hivemq.com'
MQTT_PORT = 1883
MQTT_TOPICS = [
    ("compostech/data/temperature", 0),
    ("compostech/data/humidity", 0),
    ("compostech/data/ph", 0),
    ("compostech/data/gas", 0),
    ("compostech/alert/odor", 0),
    ("compostech/alert/ph", 0)
]

# MQTT Callback para recepção de mensagens
def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode()

    with data_lock:
        if topic == "compostech/data/temperature":
            sensor_data["temperature"] = float(payload)
        elif topic == "compostech/data/humidity":
            sensor_data["humidity"] = float(payload)
        elif topic == "compostech/data/ph":
            sensor_data["ph"] = float(payload)
        elif topic == "compostech/data/gas":
            sensor_data["gas"] = int(payload)
        elif topic == "compostech/alert/odor":
            if payload not in sensor_data["alerts"]:
                sensor_data["alerts"].append("Odor Alert: " + payload)
        elif topic == "compostech/alert/ph":
            if payload not in sensor_data["alerts"]:
                sensor_data["alerts"].append("pH Alert: " + payload)

# Inicializa o cliente MQTT e conecta
def mqtt_loop():
    client = mqtt.Client()
    client.on_message = on_message
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.subscribe(MQTT_TOPICS)
    client.loop_forever()

# Rota raiz que mostra a página do dashboard
@app.route('/')
def index():
    return render_template('dashboard.html')

# API para obter dados sensoriais e alertas atuais
@app.route('/api/sensordata', methods=['GET'])
def get_sensor_data():
    with data_lock:
        current_data = sensor_data.copy()
    return jsonify(current_data)

# API para limpar alertas existentes
@app.route('/api/clearalerts', methods=['POST'])
def clear_alerts():
    with data_lock:
        sensor_data["alerts"].clear()
    return jsonify({"status": "Alerts cleared successfully"})

if __name__ == "__main__":
    # Executa MQTT em thread separada para não bloquear Flask
    mqtt_thread = threading.Thread(target=mqtt_loop)
    mqtt_thread.daemon = True
    mqtt_thread.start()

    # Executa aplicação Flask no host para acesso local/rede
    app.run(host='0.0.0.0', port=5000, debug=False)

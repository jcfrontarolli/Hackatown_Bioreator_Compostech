from flask import Flask, jsonify, render_template
from threading import Thread, Lock
import paho.mqtt.client as mqtt
import time

app = Flask(__name__)

# Dados globais e lock para concorrência segura
sensor_data = {
    "temperature": None,
    "humidity": None,
    "ph": None,
    "gas": None,
    "alerts": []
}
data_lock = Lock()

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPICS = [
    ("compostech/data/temperature", 0),
    ("compostech/data/humidity", 0),
    ("compostech/data/ph", 0),
    ("compostech/data/gas", 0),
    ("compostech/alert/odor", 0),
    ("compostech/alert/ph", 0)
]

# Callback MQTT ao receber mensagem
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
            add_alert("Odor Alert: " + payload)
        elif topic == "compostech/alert/ph":
            add_alert("pH Alert: " + payload)

def add_alert(message):
    if message not in sensor_data["alerts"]:
        sensor_data["alerts"].append(message)

def clear_alerts():
    sensor_data["alerts"].clear()

# Thread para gerenciar conexão MQTT e loop
def mqtt_thread():
    client = mqtt.Client()
    client.on_message = on_message
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.subscribe(MQTT_TOPICS)
    client.loop_forever()

@app.route('/')
def index():
    return render_template('dashboard.html')

@app.route('/api/sensordata')
def api_sensordata():
    with data_lock:
        data_copy = sensor_data.copy()
    return jsonify(data_copy)

@app.route('/api/clearalerts', methods=['POST'])
def api_clearalerts():
    with data_lock:
        clear_alerts()
    return jsonify({"status": "alerts cleared"})

if __name__ == "__main__":
    # Inicializa thread do MQTT para receber dados em background
    t = Thread(target=mqtt_thread)
    t.start()
    # Inicializa Flask (debug=False para produção)
    app.run(host='0.0.0.0', port=5000, debug=False)

import threading

class DataSource:
    def __init__(self):
        self._lock = threading.Lock()
        self._data = {
            "temperature": None,
            "humidity": None,
            "ph": None,
            "gas": None,
            "alerts": []
        }

    def update_sensor(self, sensor_name: str, value):
        with self._lock:
            if sensor_name in self._data:
                self._data[sensor_name] = value

    def add_alert(self, alert_message: str):
        with self._lock:
            if alert_message not in self._data["alerts"]:
                self._data["alerts"].append(alert_message)

    def clear_alerts(self):
        with self._lock:
            self._data["alerts"].clear()

    def get_all_data(self):
        with self._lock:
            # Retorna uma cópia para evitar uso concorrente
            return self._data.copy()

# Instância única para todo sistema (singleton-like)
data_source_instance = DataSource()

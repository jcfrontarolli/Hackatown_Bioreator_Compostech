# Compostech — Sistema Inteligente de Monitoramento e Automação de Compostagem

O **Compostech** é um sistema integrado de monitoramento ambiental desenvolvido para medir **umidade** e **temperatura** de compostagem orgânica em tempo real.  
Combina **sensores físicos (Arduino + YL-69 + ADS1115)**, **broker MQTT**, **API Flask**, e **dashboard web interativo** para visualização e controle remoto.

## Arquitetura do Sistema

### Camadas principais

| Camada | Componente | Função |
|--------|-------------|--------|
| 1. Física (Hardware) | `Sensor YL-69 + ADS1115` | Captura da umidade real do composto. |
| 2. Coleta (Gateway) | `dashboard.py` | Lê dados via Serial e publica em MQTT. |
| 3. Backend (Servidor Flask) | `app.py` + `data_source.py` | Fornece API REST e serve o dashboard. |
| 4. Armazenamento Local | `data.json` | Registra leituras de sensores. |
| 5. Interface Web | `dashboard.html` | Exibe gráficos e status em tempo real. |


## Instalação e Execução

### Requisitos

- Python 3.7+
- Arduino IDE
- Broker MQTT (ex: Mosquitto)
- Navegador moderno (Chrome/Edge/Firefox)

---

### 🪫 1. Configuração do Arduino

1. Abra o arquivo [`hardware/Sensor-umidade-YL-69.cpp`](hardware/Sensor-umidade-YL-69.cpp) na **Arduino IDE**.
2. Conecte o sensor YL-69 ao módulo **ADS1115**:
   - A0 → Saída do YL-69  
   - SDA → A4  
   - SCL → A5  
   - VCC → 3.3V  
   - GND → GND
3. Faça o upload do código para a placa (Arduino Uno ou compatível).
4. Teste a leitura no **Monitor Serial** (9600 baud).

---

### 2. Configuração do Gateway MQTT

O arquivo [`backend/dashboard.py`](backend/dashboard.py) lê dados da porta serial e publica via MQTT.

#### Ajuste a porta serial:
```python

SERIAL_PORT = '/dev/ttyACM0'  # Ajuste conforme seu sistema

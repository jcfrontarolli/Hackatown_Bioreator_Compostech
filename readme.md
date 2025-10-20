Compostech Bio Solutions ♻️
Sistema integrado para compostagem automatizada e inteligente que combina hardware embarcado, conectividade IoT e dashboard web para otimizar o processo de compostagem.

📋 Visão Geral
O Compostech é uma solução completa que monitora e controla automaticamente o processo de compostagem através de:

Redução de odores através de controle ambiental preciso

Aceleração do ciclo biológico com condições otimizadas

Monitoramento em tempo real com dados auditáveis

Alertas proativos para manutenção preventiva

🏗️ Arquitetura do Sistema
🔧 Firmware (ESP32/STM32)
Monitoramento: Sensores de temperatura (DS18B20), umidade (capacitivo), pH (analógico) e gás (MQ135)

Controle: Ventilador e bomba de irrigação automáticos

Conectividade: Publicação de dados via MQTT

Robustez: Média móvel para filtragem e watchdog timer

⚙️ Backend (Flask)
API REST: Endpoints para dados sensoriais e gestão de alertas

MQTT Client: Thread dedicada para consumo de mensagens

Segurança: Locks para proteção de estado global

Módulos:

app.py: Servidor web principal

data_source.py: Abstração de operações de dados

🖥️ Dashboard Web
Interface Responsiva: Visualização em tempo real

Atualização Automática: Fetch API periódico

Gestão de Alertas: Lista clara com opção de limpeza

Transparência: Indicador de último horário de atualização

🚀 Como Executar
Configuração do Firmware
cpp
// Configure no código:
const char* WIFI_SSID = "sua_rede";
const char* WIFI_PASS = "sua_senha";
const char* MQTT_BROKER = "seu_broker";

# Instale dependências
pip install -r requirements.txt

# Configure variáveis de ambiente
export MQTT_BROKER="seu_broker"
export MQTT_TOPIC="compostech/data"

# Execute o servidor
python app.py
 Acesso ao Dashboard
Abra o navegador e acesse: http://localhost:5000

📁 Estrutura do Projeto
text
compostech-bio-solutions/
├── firmware/
│   ├── src/
│   │   ├── main.cpp
│   │   ├── sensors.h
│   │   └── mqtt_client.h
│   └── platformio.ini
├── backend/
│   ├── app.py
│   ├── data_source.py
│   ├── requirements.txt
│   └── config.py
├── web/
│   └── dashboard.html
└── README.md
🛠️ Tecnologias Utilizadas
Hardware: ESP32/STM32, DS18B20, MQ135, sensores capacitivos

Firmware: C++ (PlatformIO/Arduino)

Backend: Python, Flask, Paho-MQTT

Frontend: HTML5, CSS3, JavaScript (Fetch API)

Protocolos: MQTT, HTTP/REST

✅ Boas Práticas Implementadas
Separação de Responsabilidades: Firmware (publicação), Backend (subscrição + API), Frontend (consumo)

Concorrência Segura: Locks para evitar race conditions

Qualidade de Dados: Filtros para dados sensoriais

Performance: Atualizações assíncronas

Manutenibilidade: Design modular e extensível

🔮 Roadmap
Integração de mais sensores (CO2, NH3)

Machine Learning para otimização de parâmetros

App móvel nativo

Relatórios PDF automáticos

Integração com APIs de gestão ambiental
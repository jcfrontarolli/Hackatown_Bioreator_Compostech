Compostech Bio Solutions
Visão Geral
Compostech é um sistema integrado para compostagem automatizada e inteligente, combinando firmware embarcado em microcontrolador, conectividade IoT via MQTT, backend web em Flask e interface de dashboard web. O objetivo é otimizar o processo de compostagem reduzindo odores, acelerando o ciclo biológico e fornecendo dados auditáveis para usuários e gestão ambiental com estatísticas e alertas em tempo real.

Estrutura do Sistema
Firmware (ESP32/STM32)
Monitora sensores de temperatura (DS18B20), umidade (capacitivo), pH (analógico) e gás (MQ135).

Controla ventilador e bomba de irrigação automaticamente conforme variáveis ambientais.

Publica dados e alertas via MQTT ao broker configurado.

Implementação robusta de média móvel para filtragem e watchdog timer para estabilidade.

Backend (Flask - app.py + data_source.py)
Executa servidor web que expos dados da compostagem via APIs REST.

Consome mensagens MQTT em uma thread dedicada atualizando estado global protegido por lock.

Fornece endpoints para leitura de dados sensoriais e gerenciamento de alertas.

O módulo data_source.py abstrai operações de dados garantindo segurança e consistência.

Dashboard Web (dashboard.html)
Página responsiva para visualização em tempo real dos dados dos sensores.

Atualização periódica por fetch API aos endpoints do backend.

Visualiza alertas em lista clara, com botão para limpeza manual.

Indica último horário de atualização para controle e transparência.

Como Executar
Configure credenciais Wi-Fi e broker MQTT no firmware e backend.

Compile e execute o firmware no ESP32/STM32, garantindo conexão Wi-Fi.

Execute o backend Flask (app.py) para iniciar o servidor web e thread MQTT.

Acesse o dashboard no browser pelo endereço do servidor para monitorar em tempo real.

Boas Práticas
Separação clara de responsabilidades entre firmware (publicação MQTT), backend (subscrição MQTT + API REST) e front-end (consumo API).

Uso de locks para evitar condições de corrida.

Filtros simples para garantir qualidade dos dados sensoriais.

Atualizações assíncronas para garantir desempenho e responsividade.

Design modular e extensível para integrar futuros sensores, atuadores e funções analíticas.

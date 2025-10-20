Estrutura geral do firmware
O firmware opera em três camadas principais:

Aquisição de dados (sensoriamento)
Responsável por ler continuamente variáveis ambientais críticas:

Temperatura (Sensor DS18B20): indica a atividade microbiana.

Umidade (Sensor capacitivo): garante hidratação adequada do composto (40–60%).

pH (Sensor analógico): monitora equilíbrio químico do processo.

Gás MQ135: detecta amônia e gases resultantes de anaerobiose, indicando risco de odor.

Processamento e controle autônomo
Utiliza algoritmos embarcados para comparar valores lidos com faixas ideais:

Se a temperatura ultrapassa 65 °C, o ventilador é acionado para resfriamento e oxigenação.

Se a umidade cai abaixo de 40 %, a bomba de água é ativada por tempo controlado.

Se há alta emissão de gás (MQ135), o sistema simultaneamente liga o ventilador e envia alerta ao servidor MQTT.

Se o pH sai da faixa 6,5–8, é registrado aviso para possível intervenção manual.

Comunicação e registro de dados
Cada leitura é armazenada em um buffer local e enviada via Wi-Fi (protocolo MQTT) para a plataforma SaaS Compostech Cloud.

Relatórios em tempo real são gerados no dashboard web, convertendo dados em indicadores ESG e créditos de carbono.

Mesmo offline, o firmware mantém logs em memória e sincroniza automaticamente ao restabelecer comunicação.

Ciclo operacional e lógica
O firmware segue um loop assíncrono não bloqueante, com intervalos de leitura ajustáveis (geralmente 3 s a 5 s).
Cada iteração do loop passa por quatro etapas:

Leitura de sensores → Captura e conversão analógica-digital.

Filtragem de ruído → Média móvel ou filtro de Kalman para estabilidade.

Análise de estado → Cálculo de desvios em relação às faixas-alvo.

Ação autônoma → Controle proporcional de ventilação, umidificação ou alarme.

Esse mecanismo transforma a compostagem em um processo preditivo, prevenindo falhas antes que odores ou desequilíbrios ocorram. O sistema não espera o erro — ele age preventivamente com base nas tendências detectadas nas leituras sucessivas.

Interação com o usuário
Interface local: o firmware controla um display LCD 16x2 (ou tela 20x4) que exibe temperatura, pH, status dos atuadores e alertas em tempo real.

Aplicativo móvel: permite monitorar gráficos históricos, receber notificações automáticas e ajustar parâmetros (por exemplo, tempo máximo da bomba ou gatilho do ventilador).

Modo educativo: exibe mensagens de feedback (“atividade microbiana alta”, “composto equilibrado”) para engajar o usuário e reforçar entendimento ambiental.

Segurança, otimização e confiabilidade
Watchdog Timer reinicia o sistema em caso de travamento.

Detecção de falhas (fail-safe): sensores desconectados ou leituras inválidas acionam modos seguros (desligar bomba, ativar ventilador).

Economia de energia: o microcontrolador entra em sleep mode entre ciclos prolongados quando a compostagem entra em fase de estabilização.

Código limpo e modular: cada sensor é implementado como uma classe independente, seguindo o padrão Object-Oriented Embedded Programming.

Fluxo de funcionamento simplificado
Inicialização → calibragem de sensores → conexão Wi-Fi/MQTT.

Loop contínuo de 3 s:
a. Coleta de temperatura, umidade, pH e MQ135.
b. Aplicação de filtros e cálculo de médias.
c. Avaliação lógica e acionamento dos atuadores.
d. Publicação de dados no dashboard.

Registro local e atualização do status LED (azul = inicialização, verde = operação normal, vermelho = alerta).

O firmware da Compostech, portanto, é um sistema embarcado ciberfísico completo: sensoriamento ambiental + controle inteligente + conectividade. Ele permite que o processo de compostagem ocorra de forma automatizada, eficiente e inodora, eliminando a necessidade de intervenção humana contínua e garantindo que cada ciclo produza adubo de alta qualidade, com monitoramento totalmente auditável.

STM32 TinyML Weather Monitoring

Projeto de monitoramento de temperatura e umidade utilizando a placa **NUCLEO-H755ZI-Q**, o microcontrolador **STM32H755** e o sensor **DHT11**.

O objetivo é desenvolver um sistema embarcado capaz de coletar dados ambientais, processar essas informações e evoluir gradualmente para a execução de um modelo de Machine Learning diretamente no microcontrolador.

Hardware utilizado

* NUCLEO-H755ZI-Q
* STM32H755
* Sensor DHT11
* Cabo USB para programação, depuração e comunicação serial

Estado atual do projeto

Atualmente, o firmware é capaz de:

* Ler temperatura e umidade do sensor DHT11
* Realizar uma leitura por segundo
* Enviar os dados pela USART3
* Armazenar as últimas 10 temperaturas
* Trabalhar com janelas de 10 amostras
* Calcular a média de temperatura de cada janela
* Utilizar os LEDs da placa como resposta simples às leituras

Funcionamento

O sensor DHT11 é lido aproximadamente uma vez por segundo.

Exemplo:


Temperatura: 25 C | Umidade: 61 %
Temperatura: 25 C | Umidade: 61 %
Temperatura: 26 C | Umidade: 60 %


As temperaturas são armazenadas em grupos de 10 amostras:


25 25 26 25 26 26 25 25 26 26


Após completar a janela, o programa calcula a média:

Media: 25.50 C


Fluxo atual:


DHT11
  ↓
Leitura de temperatura e umidade
  ↓
Amostragem de 1 Hz
  ↓
Janela com 10 temperaturas
  ↓
Processamento
  ↓
Cálculo da média

Comunicação serial

A comunicação com o computador é feita pela USART3, utilizando a porta serial virtual do ST-LINK.

Configuração:


Baud Rate: 115200
Data bits: 8
Stop bits: 1
Paridade: nenhuma


Exemplo de saída:


========================================
 STM32H755 - DHT11 - Fase 2 TinyML
========================================
Amostragem: 1 Hz
Janela: 10 temperaturas
========================================

Amostra[0]: Temperatura: 25 C | Umidade: 61 %
Amostra[1]: Temperatura: 25 C | Umidade: 61 %
Amostra[2]: Temperatura: 26 C | Umidade: 60 %

Buffer completo: 25 25 26 25 26 26 25 25 26 26

Media: 25.50 C


 Estrutura do projeto


H755_TinyMl_temperatura/
│
├── CM4/
├── CM7/
├── Common/
├── Drivers/
├── .settings/
│
├── .gitignore
├── .mxproject
├── .project
├── H755_TinyMl_temperatura.ioc
├── README.md
└── serial.ps1


O código principal utilizado atualmente está no projeto do **Cortex-M7**.

Tecnologias utilizadas

* C
* STM32CubeIDE
* STM32CubeMX
* STM32 HAL
* Git
* GitHub

Etapas concluídas

* [x] Configuração inicial do STM32H755
* [x] Comunicação serial pela USART3
* [x] Integração com o sensor DHT11
* [x] Leitura de temperatura
* [x] Leitura de umidade
* [x] Amostragem de 1 Hz
* [x] Buffer com 10 temperaturas
* [x] Cálculo da média

Próximas etapas

* [ ] Implementar novas características dos dados
* [ ] Calcular temperatura mínima e máxima
* [ ] Calcular variação
* [ ] Calcular desvio padrão
* [ ] Coletar dados para criação do dataset
* [ ] Exportar e analisar os dados
* [ ] Treinar um modelo de Machine Learning
* [ ] Avaliar o modelo
* [ ] Quantizar o modelo para INT8
* [ ] Converter o modelo para TensorFlow Lite Micro
* [ ] Executar inferência no STM32H755
* [ ] Medir tempo de inferência
* [ ] Medir uso de RAM
* [ ] Medir uso de memória Flash

Objetivo final

O objetivo final é construir todo o fluxo, desde a leitura do sensor até a inferência diretamente no STM32H755:


DHT11
  ↓
Coleta de dados
  ↓
Processamento
  ↓
Extração de características
  ↓
Dataset
  ↓
Treinamento
  ↓
Quantização INT8
  ↓
TensorFlow Lite Micro
  ↓
Inferência no STM32H755


O projeto também pretende avaliar aspectos importantes da execução embarcada, como:

* Latência
* Uso de RAM
* Uso de Flash
* Quantização
* Otimização

 Autor

**Ian Godoi**

Estudante de Ciência da Computação com interesse em:

* Sistemas Embarcados
* TinyML
* Processamento Digital de Sinais
* Machine Learning
* C/C++
* STM32

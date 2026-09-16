\# STM32 TinyML Weather Monitoring



TinyML project developed for the \*\*STM32H755\*\* microcontroller using the \*\*NUCLEO-H755ZI-Q\*\* development board and a \*\*DHT11 temperature and humidity sensor\*\*.



The project explores the complete TinyML pipeline on an embedded system, starting with sensor data acquisition and feature extraction and progressing toward machine learning inference directly on the microcontroller.



\## Current Status



\*\*Phase 2 — Data Acquisition and Feature Extraction\*\*



The current firmware:



\* Reads temperature and humidity from a DHT11 sensor

\* Samples the sensor at \*\*1 Hz\*\*

\* Sends measurements through \*\*USART3\*\*

\* Stores temperature samples in a \*\*10-sample window\*\*

\* Calculates the mean temperature of each window

\* Uses the onboard LEDs for simple rule-based behavior

\* Prepares the data pipeline for future TinyML inference



Example serial output:



```text

========================================

&#x20;STM32H755 - DHT11 - Phase 2 TinyML

========================================

Sampling: 1 Hz

Window: 10 temperatures

========================================



Sample\[0]: Temperature: 25 C | Humidity: 61 %

Sample\[1]: Temperature: 25 C | Humidity: 61 %

Sample\[2]: Temperature: 26 C | Humidity: 60 %



...



Buffer complete: 25 25 26 25 26 26 25 25 26 26

Mean: 25.50 C

```



\## Hardware



\* \*\*STMicroelectronics NUCLEO-H755ZI-Q\*\*

\* \*\*STM32H755ZI\*\*



&#x20; \* Arm Cortex-M7

&#x20; \* Arm Cortex-M4

\* \*\*DHT11 temperature and humidity sensor\*\*

\* USB connection for programming, debugging and serial communication



\## Software



\* STM32CubeIDE

\* STM32CubeMX

\* STM32 HAL

\* C

\* Git / GitHub



\## Project Structure



```text

H755\_TinyMl\_temperatura/

│

├── CM4/

│

├── CM7/

│   └── Core/

│       ├── Inc/

│       └── Src/

│

├── Common/

├── Drivers/

├── .settings/

│

├── .gitignore

├── .mxproject

├── .project

├── H755\_TinyMl\_temperatura.ioc

└── serial.ps1

```



\## Data Acquisition Pipeline



The current processing pipeline is:



```text

DHT11

&#x20; │

&#x20; ▼

Temperature + Humidity

&#x20; │

&#x20; ▼

1 sample / second

&#x20; │

&#x20; ▼

10-sample temperature window

&#x20; │

&#x20; ▼

Feature Extraction

&#x20; │

&#x20; ▼

Mean Temperature

```



Each window contains approximately \*\*10 seconds of temperature data\*\*.



For example:



```text

24 24 24 25 25 25 25 26 26 27

```



is transformed into a feature such as:



```text

Mean = 25.10 °C

```



This is the first step toward converting raw sensor data into features suitable for a machine learning model.



\## Serial Communication



USART3 is configured for communication through the ST-LINK Virtual COM Port.



Configuration:



```text

Baud rate: 115200

Data bits: 8

Stop bits: 1

Parity: None

```



Sensor measurements and calculated features can therefore be monitored directly from a serial terminal.



\## TinyML Roadmap



The project will progressively evolve from traditional embedded programming to an embedded machine learning application.



\* \[x] Read temperature and humidity from DHT11

\* \[x] Send sensor data through UART

\* \[x] Implement fixed sampling frequency

\* \[x] Create a sample window

\* \[x] Calculate the first feature: mean

\* \[ ] Add additional statistical features

\* \[ ] Collect and export a dataset

\* \[ ] Analyze and preprocess the dataset

\* \[ ] Train a machine learning model

\* \[ ] Evaluate model accuracy

\* \[ ] Quantize the model to INT8

\* \[ ] Convert the model for TensorFlow Lite Micro

\* \[ ] Deploy the model to the STM32H755

\* \[ ] Run inference on the Cortex-M7

\* \[ ] Measure inference latency

\* \[ ] Measure RAM and Flash usage

\* \[ ] Compare rule-based logic with TinyML inference



\## Goal



The main goal of this project is to understand how a complete TinyML system is built on a microcontroller:



```text

Sensor

&#x20;  ↓

Data Acquisition

&#x20;  ↓

Signal / Data Processing

&#x20;  ↓

Feature Extraction

&#x20;  ↓

Dataset

&#x20;  ↓

Machine Learning

&#x20;  ↓

Model Quantization

&#x20;  ↓

Embedded Deployment

&#x20;  ↓

Inference on STM32

```



Rather than starting directly with a trained model, the project is being developed incrementally to understand each stage of the embedded machine learning pipeline.



\## Development Board



The project uses the \*\*NUCLEO-H755ZI-Q\*\*, based on the dual-core STM32H755 microcontroller.



The architecture provides:



```text

STM32H755

├── Cortex-M7

└── Cortex-M4

```



Future versions of the project may explore how the two cores can be used for different parts of the sensing and inference pipeline.



\## Future Work



Future development will focus on:



\* Additional feature extraction

\* Environmental data collection

\* Dataset generation

\* Machine learning model training

\* INT8 quantization

\* TensorFlow Lite for Microcontrollers

\* Embedded inference

\* Performance benchmarking

\* RAM and Flash optimization

\* Inference latency analysis



\## Author



\*\*Ian Godoi\*\*



Computer Science student interested in:



\* Embedded Systems

\* TinyML

\* Digital Signal Processing

\* Machine Learning

\* C/C++

\* STM32



\## Repository



`stm32-tinyml-weather`




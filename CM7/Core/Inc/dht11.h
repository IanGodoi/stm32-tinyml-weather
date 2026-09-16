#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"

/* Pino onde o DATA do DHT11 está conectado */
#define DHT11_DATA_Pin       GPIO_PIN_3
#define DHT11_DATA_GPIO_Port GPIOA

void DHT11_Init(void);

uint8_t DHT11_Read(uint8_t *temperature,
                   uint8_t *humidity);

#endif

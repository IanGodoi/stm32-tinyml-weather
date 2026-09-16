/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : DHT11 + USART3 + TinyML Features - NUCLEO-H755ZI-Q
  ******************************************************************************
  */
/* USER CODE END Header */


/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "dht11.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */


/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */


/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/*
 * Quantidade de temperaturas armazenadas.
 *
 * Como fazemos uma leitura por segundo:
 *
 * 10 amostras = aproximadamente 10 segundos.
 */
#define WINDOW_SIZE 10U


/*
 * Sincronização entre Cortex-M7 e Cortex-M4.
 */
#define DUAL_CORE_BOOT_SYNC_SEQUENCE

#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U)
#endif

#endif

/* USER CODE END PD */


/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */


/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;


/* USER CODE BEGIN PV */

/*
 * Buffer que guarda as últimas 10 temperaturas.
 *
 * Inicialmente:
 *
 * [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
 */
float temp_buffer[WINDOW_SIZE] = {0};


/*
 * Indica onde a próxima temperatura
 * deverá ser armazenada.
 *
 * Vai de:
 *
 * 0 até 9
 */
uint8_t buffer_index = 0;

/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_USART3_UART_Init(void);


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */


  /* USER CODE BEGIN Boot_Mode_Sequence_0 */

#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)

  int32_t timeout;

#endif

  /* USER CODE END Boot_Mode_Sequence_0 */


  /* USER CODE BEGIN Boot_Mode_Sequence_1 */

#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)

  /*
   * Espera o Cortex-M4 inicializar
   * e entrar em STOP mode.
   */

  timeout = 0xFFFF;

  while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET)
         && (timeout-- > 0))
  {
  }


  if (timeout < 0)
  {
    Error_Handler();
  }

#endif

  /* USER CODE END Boot_Mode_Sequence_1 */


  /*
   * Inicializa a HAL.
   */
  HAL_Init();


  /* USER CODE BEGIN Init */

  /* USER CODE END Init */


  /*
   * Configura o clock do sistema.
   */
  SystemClock_Config();


  /* USER CODE BEGIN Boot_Mode_Sequence_2 */

#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)

  /*
   * Habilita o clock do Hardware Semaphore.
   */
  __HAL_RCC_HSEM_CLK_ENABLE();


  /*
   * Libera o Cortex-M4.
   */
  HAL_HSEM_FastTake(HSEM_ID_0);

  HAL_HSEM_Release(
      HSEM_ID_0,
      0
  );


  /*
   * Espera o Cortex-M4 acordar.
   */
  timeout = 0xFFFF;

  while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET)
         && (timeout-- > 0))
  {
  }


  if (timeout < 0)
  {
    Error_Handler();
  }

#endif

  /* USER CODE END Boot_Mode_Sequence_2 */


  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */


  /*
   * Inicializa os periféricos.
   */
  MX_GPIO_Init();

  MX_USART3_UART_Init();


  /* USER CODE BEGIN 2 */


  /*
   * =====================================================
   * INICIALIZAÇÃO DO DHT11
   * =====================================================
   */

  DHT11_Init();


  /*
   * Temperatura atual.
   */
  uint8_t temperature = 0;


  /*
   * Umidade atual.
   */
  uint8_t humidity = 0;


  /*
   * Usado para montar mensagens que serão
   * enviadas pela USART3.
   */
  char mensagem[160];


  /*
   * Guarda o instante em que fizemos
   * a última leitura do sensor.
   */
  uint32_t last_sample_time = 0;


  /* USER CODE END 2 */


  /*
   * =====================================================
   * LEDs DA NUCLEO
   * =====================================================
   */

  BSP_LED_Init(LED_GREEN);

  BSP_LED_Init(LED_YELLOW);

  BSP_LED_Init(LED_RED);


  /*
   * Botão USER.
   */
  BSP_PB_Init(
      BUTTON_USER,
      BUTTON_MODE_EXTI
  );


  /*
   * Mensagem inicial.
   */
  {
      char inicio[] =
          "\r\n"
          "========================================\r\n"
          " STM32H755 - DHT11 - Fase 2 TinyML\r\n"
          "========================================\r\n"
          "Amostragem: 1 Hz\r\n"
          "Janela: 10 temperaturas\r\n"
          "========================================\r\n\r\n";


      HAL_UART_Transmit(
          &huart3,
          (uint8_t *)inicio,
          sizeof(inicio) - 1,
          HAL_MAX_DELAY
      );
  }


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

      /*
       * =====================================================
       * UMA LEITURA POR SEGUNDO
       * =====================================================
       */

      if ((HAL_GetTick() - last_sample_time) >= 1000U)
      {

          /*
           * Guarda o horário desta leitura.
           */
          last_sample_time = HAL_GetTick();


          /*
           * =================================================
           * 1. LEITURA DO DHT11
           * =================================================
           */

          if (DHT11_Read(
                  &temperature,
                  &humidity))
          {

              /*
               * =================================================
               * 2. GUARDA A TEMPERATURA NO BUFFER
               * =================================================
               *
               * Exemplo:
               *
               * Amostra 0:
               *
               * [24, 0, 0, 0, 0, 0, 0, 0, 0, 0]
               *
               *
               * Amostra 1:
               *
               * [24, 24, 0, 0, 0, 0, 0, 0, 0, 0]
               *
               *
               * ...
               */


              temp_buffer[buffer_index] =
                  (float)temperature;


              /*
               * Mostra no terminal qual posição
               * estamos preenchendo.
               */

              snprintf(
                  mensagem,
                  sizeof(mensagem),

                  "Amostra[%u]: Temperatura: %u C | Umidade: %u %%\r\n",

                  (unsigned int)buffer_index,

                  (unsigned int)temperature,

                  (unsigned int)humidity
              );


              HAL_UART_Transmit(
                  &huart3,

                  (uint8_t *)mensagem,

                  strlen(mensagem),

                  HAL_MAX_DELAY
              );


              /*
               * Passa para a próxima posição.
               */
              buffer_index++;


              /*
               * =================================================
               * 3. AS 10 TEMPERATURAS FORAM COLETADAS?
               * =================================================
               */

              if (buffer_index >= WINDOW_SIZE)
              {

                  /*
                   * Mostra que a janela está completa.
                   */

                  char texto[] =
                      "\r\nBuffer completo: ";


                  HAL_UART_Transmit(
                      &huart3,

                      (uint8_t *)texto,

                      sizeof(texto) - 1,

                      HAL_MAX_DELAY
                  );


                  /*
                   * =================================================
                   * MOSTRA AS 10 TEMPERATURAS
                   * =================================================
                   */

                  for (uint8_t i = 0;
                       i < WINDOW_SIZE;
                       i++)
                  {

                      snprintf(
                          mensagem,

                          sizeof(mensagem),

                          "%u ",

                          (unsigned int)
                          temp_buffer[i]
                      );


                      HAL_UART_Transmit(
                          &huart3,

                          (uint8_t *)mensagem,

                          strlen(mensagem),

                          HAL_MAX_DELAY
                      );
                  }


                  /*
                   * Nova linha.
                   */
                  char nova_linha[] =
                      "\r\n";


                  HAL_UART_Transmit(
                      &huart3,

                      (uint8_t *)nova_linha,

                      sizeof(nova_linha) - 1,

                      HAL_MAX_DELAY
                  );


                  /*
                   * =================================================
                   * FEATURE 1: MÉDIA
                   * =================================================
                   *
                   * Agora começamos de fato a transformar
                   * os dados brutos em características.
                   *
                   *
                   * Exemplo:
                   *
                   * 24 24 24 25 25 25 25 26 26 27
                   *
                   *                ↓
                   *
                   *              média
                   *
                   */


                  float soma = 0.0f;


                  for (uint8_t i = 0;
                       i < WINDOW_SIZE;
                       i++)
                  {

                      soma += temp_buffer[i];

                  }


                  /*
                   * Calcula a média.
                   */

                  float media =
                      soma / (float)WINDOW_SIZE;


                  /*
                   * Por enquanto não usamos %f no printf.
                   *
                   * Transformamos:
                   *
                   * 24.70
                   *
                   * em:
                   *
                   * parte inteira = 24
                   * centésimos    = 70
                   */


                  uint32_t media_inteira =
                      (uint32_t)media;


                  uint32_t media_centesimos =
                      (uint32_t)(
                          (
                              media -
                              (float)media_inteira
                          )
                          * 100.0f
                          + 0.5f
                      );


                  /*
                   * Corrige um possível arredondamento:
                   *
                   * por exemplo:
                   *
                   * 24.999...
                   */

                  if (media_centesimos >= 100U)
                  {

                      media_inteira++;

                      media_centesimos = 0U;

                  }


                  snprintf(
                      mensagem,

                      sizeof(mensagem),

                      "Media: %lu.%02lu C\r\n",

                      (unsigned long)
                      media_inteira,

                      (unsigned long)
                      media_centesimos
                  );


                  HAL_UART_Transmit(
                      &huart3,

                      (uint8_t *)mensagem,

                      strlen(mensagem),

                      HAL_MAX_DELAY
                  );


                  /*
                   * Separador.
                   */

                  char separador[] =
                      "----------------------------------------\r\n\r\n";


                  HAL_UART_Transmit(
                      &huart3,

                      (uint8_t *)separador,

                      sizeof(separador) - 1,

                      HAL_MAX_DELAY
                  );


                  /*
                   * =================================================
                   * REINICIA A JANELA
                   * =================================================
                   *
                   * Agora as próximas 10 temperaturas
                   * substituirão as anteriores.
                   */

                  buffer_index = 0;

              }


              /*
               * =================================================
               * 4. REGRA TRADICIONAL DA FASE 1
               * =================================================
               *
               * Ainda mantemos a regra antiga
               * somente para comparação.
               *
               *
               * Temperatura > 25 °C
               *
               *       ↓
               *
               * LED verde ligado
               *
               */


              if (temperature > 25U)
              {

                  BSP_LED_On(LED_GREEN);

              }

              else
              {

                  BSP_LED_Off(LED_GREEN);

              }

          }

          else
          {

              /*
               * =================================================
               * ERRO NA LEITURA DO DHT11
               * =================================================
               */

              char erro[] =
                  "Erro ao ler DHT11\r\n";


              HAL_UART_Transmit(
                  &huart3,

                  (uint8_t *)erro,

                  sizeof(erro) - 1,

                  HAL_MAX_DELAY
              );


              BSP_LED_Off(LED_GREEN);

          }

      }


      /*
       * Pequena espera para evitar que o processador
       * execute este while milhões de vezes sem necessidade.
       *
       * IMPORTANTE:
       *
       * Isso NÃO significa que estamos lendo
       * o DHT11 a cada 10 ms.
       *
       * A leitura continua limitada pelo HAL_GetTick()
       * para uma vez por segundo.
       */

      HAL_Delay(10);


      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */

  }

  /* USER CODE END 3 */

}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  /*
   * Configuração da alimentação.
   */

  HAL_PWREx_ConfigSupply(
      PWR_DIRECT_SMPS_SUPPLY
  );


  /*
   * Configuração do regulador interno.
   */

  __HAL_PWR_VOLTAGESCALING_CONFIG(
      PWR_REGULATOR_VOLTAGE_SCALE3
  );


  while (!__HAL_PWR_GET_FLAG(
              PWR_FLAG_VOSRDY))
  {
  }


  /*
   * Oscilador interno HSI.
   */

  RCC_OscInitStruct.OscillatorType =
      RCC_OSCILLATORTYPE_HSI;


  RCC_OscInitStruct.HSIState =
      RCC_HSI_DIV1;


  RCC_OscInitStruct.HSICalibrationValue =
      RCC_HSICALIBRATION_DEFAULT;


  RCC_OscInitStruct.PLL.PLLState =
      RCC_PLL_NONE;


  if (HAL_RCC_OscConfig(
          &RCC_OscInitStruct)
      != HAL_OK)
  {

    Error_Handler();

  }


  /*
   * Clocks dos barramentos.
   */

  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK |
      RCC_CLOCKTYPE_SYSCLK |
      RCC_CLOCKTYPE_PCLK1 |
      RCC_CLOCKTYPE_PCLK2 |
      RCC_CLOCKTYPE_D3PCLK1 |
      RCC_CLOCKTYPE_D1PCLK1;


  RCC_ClkInitStruct.SYSCLKSource =
      RCC_SYSCLKSOURCE_HSI;


  RCC_ClkInitStruct.SYSCLKDivider =
      RCC_SYSCLK_DIV1;


  RCC_ClkInitStruct.AHBCLKDivider =
      RCC_HCLK_DIV1;


  RCC_ClkInitStruct.APB3CLKDivider =
      RCC_APB3_DIV1;


  RCC_ClkInitStruct.APB1CLKDivider =
      RCC_APB1_DIV1;


  RCC_ClkInitStruct.APB2CLKDivider =
      RCC_APB2_DIV1;


  RCC_ClkInitStruct.APB4CLKDivider =
      RCC_APB4_DIV1;


  if (HAL_RCC_ClockConfig(
          &RCC_ClkInitStruct,
          FLASH_LATENCY_1)
      != HAL_OK)
  {

    Error_Handler();

  }

}


/**
  * @brief USART3 Initialization Function
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /*
   * USART3 conectada ao
   * Virtual COM Port do ST-LINK.
   *
   * PD8 = TX
   * PD9 = RX
   */

  huart3.Instance =
      USART3;


  huart3.Init.BaudRate =
      115200;


  huart3.Init.WordLength =
      UART_WORDLENGTH_8B;


  huart3.Init.StopBits =
      UART_STOPBITS_1;


  huart3.Init.Parity =
      UART_PARITY_NONE;


  huart3.Init.Mode =
      UART_MODE_TX_RX;


  huart3.Init.HwFlowCtl =
      UART_HWCONTROL_NONE;


  huart3.Init.OverSampling =
      UART_OVERSAMPLING_16;


  huart3.Init.OneBitSampling =
      UART_ONE_BIT_SAMPLE_DISABLE;


  huart3.Init.ClockPrescaler =
      UART_PRESCALER_DIV1;


  huart3.AdvancedInit.AdvFeatureInit =
      UART_ADVFEATURE_NO_INIT;


  if (HAL_UART_Init(
          &huart3)
      != HAL_OK)
  {

    Error_Handler();

  }


  if (HAL_UARTEx_SetTxFifoThreshold(
          &huart3,
          UART_TXFIFO_THRESHOLD_1_8)
      != HAL_OK)
  {

    Error_Handler();

  }


  if (HAL_UARTEx_SetRxFifoThreshold(
          &huart3,
          UART_RXFIFO_THRESHOLD_1_8)
      != HAL_OK)
  {

    Error_Handler();

  }


  if (HAL_UARTEx_DisableFifoMode(
          &huart3)
      != HAL_OK)
  {

    Error_Handler();

  }

}


/**
  * @brief GPIO Initialization Function
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};


  /*
   * Habilita clocks dos GPIOs.
   */

  __HAL_RCC_GPIOC_CLK_ENABLE();

  __HAL_RCC_GPIOA_CLK_ENABLE();

  __HAL_RCC_GPIOB_CLK_ENABLE();

  __HAL_RCC_GPIOD_CLK_ENABLE();

  __HAL_RCC_GPIOG_CLK_ENABLE();


  /*
   * USB Power Enable inicialmente LOW.
   */

  HAL_GPIO_WritePin(
      USB_OTG_FS_PWR_EN_GPIO_Port,

      USB_OTG_FS_PWR_EN_Pin,

      GPIO_PIN_RESET
  );


  /*
   * Ethernet:
   *
   * PC1
   * PC4
   * PC5
   */

  GPIO_InitStruct.Pin =
      GPIO_PIN_1 |
      GPIO_PIN_4 |
      GPIO_PIN_5;


  GPIO_InitStruct.Mode =
      GPIO_MODE_AF_PP;


  GPIO_InitStruct.Pull =
      GPIO_NOPULL;


  GPIO_InitStruct.Speed =
      GPIO_SPEED_FREQ_HIGH;


  GPIO_InitStruct.Alternate =
      GPIO_AF11_ETH;


  HAL_GPIO_Init(
      GPIOC,
      &GPIO_InitStruct
  );


  /*
   * Ethernet:
   *
   * PA1
   * PA2
   * PA7
   */

  GPIO_InitStruct.Pin =
      GPIO_PIN_1 |
      GPIO_PIN_2 |
      GPIO_PIN_7;


  GPIO_InitStruct.Mode =
      GPIO_MODE_AF_PP;


  GPIO_InitStruct.Pull =
      GPIO_NOPULL;


  GPIO_InitStruct.Speed =
      GPIO_SPEED_FREQ_HIGH;


  GPIO_InitStruct.Alternate =
      GPIO_AF11_ETH;


  HAL_GPIO_Init(
      GPIOA,
      &GPIO_InitStruct
  );


  /*
   * Ethernet:
   *
   * PB13
   */

  GPIO_InitStruct.Pin =
      GPIO_PIN_13;


  GPIO_InitStruct.Mode =
      GPIO_MODE_AF_PP;


  GPIO_InitStruct.Pull =
      GPIO_NOPULL;


  GPIO_InitStruct.Speed =
      GPIO_SPEED_FREQ_HIGH;


  GPIO_InitStruct.Alternate =
      GPIO_AF11_ETH;


  HAL_GPIO_Init(
      GPIOB,
      &GPIO_InitStruct
  );


  /*
   * USB Power Enable.
   */

  GPIO_InitStruct.Pin =
      USB_OTG_FS_PWR_EN_Pin;


  GPIO_InitStruct.Mode =
      GPIO_MODE_OUTPUT_PP;


  GPIO_InitStruct.Pull =
      GPIO_NOPULL;


  GPIO_InitStruct.Speed =
      GPIO_SPEED_FREQ_LOW;


  HAL_GPIO_Init(
      USB_OTG_FS_PWR_EN_GPIO_Port,
      &GPIO_InitStruct
  );


  /*
   * USB Overcurrent.
   */

  GPIO_InitStruct.Pin =
      USB_OTG_FS_OVCR_Pin;


  GPIO_InitStruct.Mode =
      GPIO_MODE_IT_RISING;


  GPIO_InitStruct.Pull =
      GPIO_NOPULL;


  HAL_GPIO_Init(
      USB_OTG_FS_OVCR_GPIO_Port,
      &GPIO_InitStruct
  );


  /*
   * USB:
   *
   * PA8
   * PA11
   * PA12
   */

  GPIO_InitStruct.Pin =
      GPIO_PIN_8 |
      GPIO_PIN_11 |
      GPIO_PIN_12;


  GPIO_InitStruct.Mode =
      GPIO_MODE_AF_PP;


  GPIO_InitStruct.Pull =
      GPIO_NOPULL;


  GPIO_InitStruct.Speed =
      GPIO_SPEED_FREQ_LOW;


  GPIO_InitStruct.Alternate =
      GPIO_AF10_OTG1_FS;


  HAL_GPIO_Init(
      GPIOA,
      &GPIO_InitStruct
  );


  /*
   * Ethernet:
   *
   * PG11
   * PG13
   */

  GPIO_InitStruct.Pin =
      GPIO_PIN_11 |
      GPIO_PIN_13;


  GPIO_InitStruct.Mode =
      GPIO_MODE_AF_PP;


  GPIO_InitStruct.Pull =
      GPIO_NOPULL;


  GPIO_InitStruct.Speed =
      GPIO_SPEED_FREQ_HIGH;


  GPIO_InitStruct.Alternate =
      GPIO_AF11_ETH;


  HAL_GPIO_Init(
      GPIOG,
      &GPIO_InitStruct
  );

}


/**
  * @brief Error Handler
  * @retval None
  */
void Error_Handler(void)
{

  __disable_irq();


  while (1)
  {
  }

}


#ifdef USE_FULL_ASSERT

/**
  * @brief Reports assertion errors.
  */
void assert_failed(
    uint8_t *file,
    uint32_t line)
{

}

#endif

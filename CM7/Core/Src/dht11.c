#include "dht11.h"


/* =========================================================
 * Delay em microssegundos usando o contador de ciclos
 * do Cortex-M7.
 * =========================================================
 */

static void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;

    uint32_t cycles =
        us * (SystemCoreClock / 1000000U);

    while ((uint32_t)(DWT->CYCCNT - start) < cycles)
    {
    }
}


/* =========================================================
 * Configura o pino DATA como saída Open Drain
 * =========================================================
 */

static void DHT11_SetPinOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(DHT11_DATA_GPIO_Port,
                  &GPIO_InitStruct);
}


/* =========================================================
 * Configura o pino DATA como entrada
 * =========================================================
 */

static void DHT11_SetPinInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(DHT11_DATA_GPIO_Port,
                  &GPIO_InitStruct);
}


/* =========================================================
 * Espera o pino chegar em determinado estado.
 *
 * Retorna:
 * 1 = conseguiu
 * 0 = timeout
 * =========================================================
 */

static uint8_t DHT11_WaitForState(GPIO_PinState state,
                                  uint32_t timeout_us)
{
    uint32_t start = DWT->CYCCNT;

    uint32_t timeout_cycles =
        timeout_us * (SystemCoreClock / 1000000U);

    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port,
                            DHT11_DATA_Pin) != state)
    {
        if ((uint32_t)(DWT->CYCCNT - start) >
            timeout_cycles)
        {
            return 0;
        }
    }

    return 1;
}


/* =========================================================
 * Inicialização do contador DWT
 * =========================================================
 */

void DHT11_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    DWT->CYCCNT = 0;

    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


/* =========================================================
 * Envia o sinal inicial para o DHT11
 * =========================================================
 */

static uint8_t DHT11_Start(void)
{
    /* STM32 assume controle da linha */
    DHT11_SetPinOutput();

    /* DATA = 0 */
    HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port,
                      DHT11_DATA_Pin,
                      GPIO_PIN_RESET);

    /*
     * O DHT11 exige pelo menos cerca de 18 ms
     * em nível baixo.
     */
    HAL_Delay(18);

    /* Libera a linha */
    HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port,
                      DHT11_DATA_Pin,
                      GPIO_PIN_SET);

    delay_us(30);

    /* Agora o DHT11 controla a linha */
    DHT11_SetPinInput();


    /*
     * DHT11 responde:
     *
     * LOW  ~80 us
     * HIGH ~80 us
     */

    if (!DHT11_WaitForState(GPIO_PIN_RESET, 100))
        return 0;

    if (!DHT11_WaitForState(GPIO_PIN_SET, 100))
        return 0;

    if (!DHT11_WaitForState(GPIO_PIN_RESET, 100))
        return 0;

    return 1;
}


/* =========================================================
 * Lê um bit
 * =========================================================
 */

static uint8_t DHT11_ReadBit(void)
{
    /*
     * Cada bit começa com aproximadamente
     * 50 us em LOW.
     */

    if (!DHT11_WaitForState(GPIO_PIN_SET, 100))
        return 0;

    /*
     * Após 40 us:
     *
     * se já caiu para LOW -> bit 0
     * se continua HIGH    -> bit 1
     */

    delay_us(40);

    if (HAL_GPIO_ReadPin(DHT11_DATA_GPIO_Port,
                         DHT11_DATA_Pin) == GPIO_PIN_SET)
    {
        /*
         * Espera terminar o pulso HIGH
         */
        DHT11_WaitForState(GPIO_PIN_RESET, 100);

        return 1;
    }

    return 0;
}


/* =========================================================
 * Lê 8 bits
 * =========================================================
 */

static uint8_t DHT11_ReadByte(void)
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        value <<= 1;

        value |= DHT11_ReadBit();
    }

    return value;
}


/* =========================================================
 * Leitura completa do DHT11
 * =========================================================
 */

uint8_t DHT11_Read(uint8_t *temperature,
                   uint8_t *humidity)
{
    uint8_t data[5];

    if (!DHT11_Start())
    {
        return 0;
    }


    for (uint8_t i = 0; i < 5; i++)
    {
        data[i] = DHT11_ReadByte();
    }


    /*
     * Verifica checksum
     */
    if ((uint8_t)(data[0] +
                  data[1] +
                  data[2] +
                  data[3]) != data[4])
    {
        return 0;
    }


    /*
     * DHT11:
     *
     * data[0] = umidade inteira
     * data[1] = umidade decimal
     * data[2] = temperatura inteira
     * data[3] = temperatura decimal
     * data[4] = checksum
     */

    *humidity = data[0];
    *temperature = data[2];

    return 1;
}

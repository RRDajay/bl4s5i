#include "main.h"
#include "gpio.h"
#include "adc.h"
#include "dfsdm.h"
#include "i2c.h"
#include "octospi.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Initializes the hardware
 * 
 */
HAL_StatusTypeDef setupPeripherals()
{
  auto status = HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_DFSDM1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_OCTOSPI1_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_USB_Init();
  return status;
}

/**
 * @brief Blinks the led every 1000ms. Indicates if the board is still running
 * 
 * @param param 
 */
static void heartBeatTask(void *param) {
  for(;;) {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    vTaskDelay(1000);
  }
}

int main() {

  setupPeripherals();

  BaseType_t result = xTaskCreate(heartBeatTask, "heartBeatTask", 128, NULL, 1, NULL);

  vTaskStartScheduler();

  while(1); // We shouldn't go here
}
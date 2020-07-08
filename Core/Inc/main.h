#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "string.h"

#define USART1_BUFFER_LENGTH 256
#define USART2_BUFFER_LENGTH 256

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define onboard_led_Pin GPIO_PIN_13
#define onboard_led_GPIO_Port GPIOC
#define sound_power_Pin GPIO_PIN_4
#define sound_power_GPIO_Port GPIOA
#define sw_btn_Pin GPIO_PIN_2
#define p2_0_Pin GPIO_PIN_3
#define p2_4_Pin GPIO_PIN_5
#define ean_Pin GPIO_PIN_8
#define bt_reset GPIO_PIN_8
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

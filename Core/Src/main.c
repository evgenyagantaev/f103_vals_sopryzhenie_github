/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

#include "ssd1306.h"
#include "stdio.h"

#include "usart1_buffer_interface.h"
#include "usart2_buffer_interface.h"
#include "interface_board_interface.h"
#include "vest_interface.h"
#include "pulse_impact_interface.h"
#include "power_button_interface.h"

//#define EEPROM
#ifdef EEPROM
#define CONFIGURE_PA2_AS_INPUT
#define CONFIGURE_WRITE_EEPROM
#define DEBUG_STOP_WITH_BLINK
#endif

//#define REFLASH
#ifdef REFLASH
#define CONFIGURE_PA2_AS_INPUT
#define CONFIGURE_WRITE_FLASH
#define DEBUG_STOP_WITH_BLINK
#endif

//#define PC_TEST_MODE
#ifdef PC_TEST_MODE
#define CONFIGURE_PA2_AS_INPUT
#define CONFIGURE_NORMAL_JOB
#define DEBUG_STOP_WITH_BLINK
#endif

#define NORMAL
#ifdef NORMAL
#define CONFIGURE_NORMAL_JOB
#endif


void SystemClock_Config(void);


char usart1_buffer[2][USART1_BUFFER_LENGTH];
int usart1_write_buffer = 0;
int usart1_read_buffer = 1;
int usart1_write_index = 0;
int usart1_new_message_ready_flag = 0;
int usart1_message_length = 0;
int usart1_old_message_saved = 1;
int usart1_message_lost = 0;
int usart1_received_messages = 0;
int usart1_processed_messages = 0;

char usart2_buffer[2][USART2_BUFFER_LENGTH];
int usart2_write_buffer = 0;
int usart2_read_buffer = 1;
int usart2_write_index = 0;
int usart2_new_message_ready_flag = 0;
int usart2_message_length = 0;
int usart2_old_message_saved = 1;
int usart2_message_lost = 0;
int usart2_received_messages = 0;
int usart2_processed_messages = 0;

int pulse_pain = 0;

#define WEAK_TEST
//#define STRONG_PAIN

//*************************
int localization = -1;		// 0-levaya ruka, 1-pravaya ruka, 8-levaya noga, 9-pravaya noga, 14-zhivot, 15-poyasnica
//*************************
#ifdef WEAK_TEST
int prim_k = 8;
int prim_l = 200;
int prim_n = 13;
//*************************
int second_k = 8;
int second_l = 200;
int second_n = 2000;
//*************************
int puls_k = 8;
int puls_l = 200;
int puls_n = 13;
//*************************
#endif
#ifdef STRONG_PAIN
int prim_k = 128;
int prim_l = 200;
int prim_n = 5;
//*************************
int second_k = 13;
int second_l = 200;
int second_n = 2000;
//*************************
int puls_k = 8;
int puls_l = 200;
int puls_n = 13;
//*************************
#endif

int impact_automat_state = 0;

int main(void)
{
	//uint32_t delay_start_tick = 0;

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();


	/* Initialize all configured peripherals */
	MX_GPIO_Init();

	HAL_GPIO_WritePin(GPIOB, sw_btn_Pin, GPIO_PIN_SET);
	HAL_Delay(100);


	HAL_GPIO_WritePin(GPIOA, bt_reset, GPIO_PIN_RESET);  // bluetooth hard reset
	HAL_GPIO_WritePin(GPIOB, sw_btn_Pin, GPIO_PIN_RESET);

	MX_I2C1_Init();

	//****************************************************************

	ssd1306_set_i2c_port(&hi2c1, 1);
	ssd1306_Init();
	HAL_Delay(100);

	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
	HAL_Delay(100);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(100);

	pulse_impact_object_init();

	ssd1306_SetCursor(0,0);
	ssd1306_WriteString("VALS", Font_16x26, White);
	ssd1306_UpdateScreen();
	HAL_Delay(100);

	//****************************************************************

	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();

	// debug *********************************************

	//HAL_GPIO_WritePin(GPIOB, p2_0_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(GPIOB, p2_4_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(GPIOB, ean_Pin, GPIO_PIN_RESET);  // 1 1 0 normal mode
	HAL_GPIO_WritePin(GPIOA, bt_reset, GPIO_PIN_RESET);  // bluetooth hard reset
	HAL_Delay(50);

#ifdef CONFIGURE_PA2_AS_INPUT

	// configure pa2 (usart2 tx) as input
	 GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

#ifdef CONFIGURE_WRITE_EEPROM
	// configure write eeprom
	HAL_GPIO_WritePin(GPIOB, p2_0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, p2_4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, ean_Pin, GPIO_PIN_RESET);  // 0 0 1 write flash; 0 1 0 write eeprom; 1 1 0 normal
#endif

#ifdef CONFIGURE_WRITE_FLASH
	// configure write flash
	HAL_GPIO_WritePin(GPIOB, p2_0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, p2_4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, ean_Pin, GPIO_PIN_SET);  // 0 0 1 write flash; 0 1 0 write eeprom; 1 1 0 normal
#endif


#ifdef CONFIGURE_NORMAL_JOB
	// configure normal job
	HAL_GPIO_WritePin(GPIOB, p2_0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, p2_4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, ean_Pin, GPIO_PIN_RESET);  // 0 0 1 write flash; 0 1 0 write eeprom; 1 1 0 normal
#endif

	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOA, bt_reset, GPIO_PIN_SET);        // bluetooth power on reset
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOB, sw_btn_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	//HAL_GPIO_WritePin(GPIOB, sw_btn_Pin, GPIO_PIN_RESET);

#ifdef DEBUG_STOP_WITH_BLINK
	// stop after bt mode configuration
	while(1)
	{
		HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
	}
#endif

	usart1_object_init();
	//usart2_object_init();
	interface_board_object_init();
	vest_object_init();



	//int counter = 0;
	//char message[64];

	//ssd1306_Fill(Black);
	//ssd1306_UpdateScreen();
	//HAL_Delay(100);

	/* Disable SysTick Interrupt */
	//SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

	//usart2_buffer_obj_write_reset();
	vest_new_message_received_flag_reset();

	// DEBUG sound
	/*
	while(1)
	{
		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);
		HAL_Delay(3000);

		power_button_action();

		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
		HAL_Delay(4);
		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);
		HAL_Delay(3000);

		power_button_action();
	}
	//*/
	// DEBUG



	// main loop
	while (1)
	{

		if(usart2_new_message_ready_flag)
		{

			char vest_aux_message[128];

			usart2_new_message_ready_flag = 0;
			usart2_processed_messages++;

			// copy message
			usart2_old_message_saved = 0;
			int i;
			for(i=0; i<=usart2_message_length; i++)
				vest_aux_message[i] = usart2_buffer[usart2_read_buffer][i];
			usart2_old_message_saved = 1;


			vest_receive_message(vest_aux_message);
			vest_new_message_received_flag_set();
		}

		usart1_buffer_action();
		//usart2_buffer_action();
		interface_board_action();
		vest_action();
		//pulse_impact_action();
		power_button_action();
		impact_action();

		// DEBUG *****************************************************
		// repeating wound
		/*
		if((HAL_GetTick() - delay_start_tick) > 9000)
		{

			impact_automat_state = 1;

			if(localization == 15)
				localization = 14;
			else if(localization == 14)
				localization = 0;
			else if(localization == 0)
				localization = 1;
			else if(localization == 1)
				localization = 8;
			else if(localization == 8)
				localization = 9;
			else if(localization == 9)
				localization = 15;

			delay_start_tick = HAL_GetTick();

		}
		//*/
		// DEBUG *****************************************************
	}

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

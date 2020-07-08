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

//#define EEPROM
#ifdef EEPROM
#define CONFIGURE_PA2_AS_INPUT
#define CONFIGURE_WRITE_EEPROM
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

int vest_interface_condition = 0;   // ozhidanie adresa (0); adres poluchen (1)

int pulse_pain = 0;

//*************************
int localization = 15;
//*************************
int prim_k = 128;
int prim_l = 200;
int prim_n = 3;
//*************************
int second_k = 8;
int second_l = 200;
int second_n = 2000;
//*************************
int puls_k = 8;
int puls_l = 200;
int puls_n = 11;
//*************************

int wound_action = 0;

int main(void)
{
	int i = 0;

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

#ifdef CONFIGURE_NORMAL_JOB
	// configure normal job
	HAL_GPIO_WritePin(GPIOB, p2_0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, p2_4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, ean_Pin, GPIO_PIN_RESET);  // 0 0 1 write flash; 0 1 0 write eeprom; 1 1 0 normal
#endif

	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOA, bt_reset, GPIO_PIN_SET);        // bluetooth power on reset
	HAL_Delay(1000);

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

	// prepare go to standby mode command
	char message[64];
	int16_t aux;
	uint8_t check_sum;
	uint16_t command_length;
	uint8_t op_code;
	uint8_t parameter;
	int message_length;

	command_length = 0x02;
	message_length = command_length + 4;
	op_code = 0x1c; // command id (visibility setting)
	parameter = 0x01; // parameter (go to standby mode)
	message[0] = 0xaa; // sync word
	message[1] = (uint8_t)(command_length >> 8); // length high
	message[2] = (uint8_t)command_length; // length low
	message[3] = op_code; // command id
	message[4] = parameter; // parameter (go to standby mode)
	check_sum = 0;
	for(i=0; i<(command_length + 2); i++)
	{
		check_sum = (uint8_t)(check_sum + message[1+i]);
	}
	aux = (int16_t)0x0000 - check_sum;
	check_sum = (uint8_t)aux;
	message[message_length - 1] = check_sum; // check sum
	// send standby command
	HAL_UART_Transmit(&huart2, (uint8_t *)message, message_length, 500);

	HAL_Delay(2500);

	// prepare read device name command
	command_length = 0x01;
	message_length = command_length + 4;
	op_code = 0x07; // command id (read device name)
	message[0] = 0xaa; // sync word
	message[1] = (uint8_t)(command_length >> 8); // length high
	message[2] = (uint8_t)command_length; // length low
	message[3] = op_code; // command id
	check_sum = 0;
	for(i=0; i<(command_length + 2); i++)
	{
		check_sum = (uint8_t)(check_sum + message[1+i]);
	}
	aux = (int16_t)0x0000 - check_sum;
	check_sum = (uint8_t)aux;
	message[message_length - 1] = check_sum; // check sum

	// prepare read all paired devices
	command_length = 0x01;
	message_length = command_length + 4;
	op_code = 0x0c; // command id (read all paired devices)
	message[0] = 0xaa; // sync word
	message[1] = (uint8_t)(command_length >> 8); // length high
	message[2] = (uint8_t)command_length; // length low
	message[3] = op_code; // command id
	check_sum = 0;
	for(i=0; i<(command_length + 2); i++)
	{
		check_sum = (uint8_t)(check_sum + message[1+i]);
	}
	aux = (int16_t)0x0000 - check_sum;
	check_sum = (uint8_t)aux;
	message[message_length - 1] = check_sum; // check sum
	HAL_UART_Transmit(&huart2, (uint8_t *)message, message_length, 500);

	/*
	while (1)
	{

		HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_RESET);
		HAL_Delay(3500);
		HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_SET);
		HAL_Delay(3500);
		HAL_UART_Transmit(&huart2, (uint8_t *)message, 5, 500);
	}
	//*/

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

	// debug *********************************************
	//****************************************************
	/*
	char aux_message[32];
	unsigned int r_pick_counter = 0;
	uint32_t delay_counter;
	while(1)
	{
		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);

		delay_counter = 0;
		while(delay_counter < 4250)
		{
			delay_counter++;
		}

		//sprintf(aux_message, "R -> %u", (unsigned int)r_pick_counter);
		//ssd1306_SetCursor(0,0);
		//ssd1306_WriteString("           ", Font_11x18, White);
		//ssd1306_SetCursor(0,0);
		//ssd1306_WriteString(aux_message, Font_11x18, White);
		//ssd1306_UpdateScreen();

		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);

		delay_counter = 0;
		while(delay_counter < 4250)
		{
			delay_counter++;
		}

		//sprintf(aux_message, "R -> %u", (unsigned int)r_pick_counter);
		//ssd1306_SetCursor(0,0);
		//ssd1306_WriteString("           ", Font_11x18, White);
		//ssd1306_SetCursor(0,0);
		//ssd1306_WriteString(aux_message, Font_11x18, White);
		//ssd1306_UpdateScreen();
	}
	//*/
	//****************************************************


	/* Infinite loop */
	while (1)
	{
		/*
		HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_RESET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_SET);
		HAL_Delay(500);
		//*/

		/*
		counter++;
		sprintf(message, "%d", counter);
		ssd1306_SetCursor(0,30);
		ssd1306_WriteString("           ", Font_11x18, White);
		ssd1306_SetCursor(0,30);
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_UpdateScreen();
		//*/

		if(usart2_new_message_ready_flag)
		{

			if (vest_interface_condition == 0)
			{
				char vest_aux_message[64];

				usart2_new_message_ready_flag = 0;
				usart2_processed_messages++;

				// copy message
				usart2_old_message_saved = 0;
				int i;
				for(i=0; (i<=usart2_message_length)&&(i<64); i++)
					vest_aux_message[i] = usart2_buffer[usart2_read_buffer][i];
				usart2_old_message_saved = 1;


				// debug
				//vest_aux_message[usart2_message_length-2] = 0;
				//vest_aux_message[11] = 0;

				ssd1306_SetCursor(0,44);
				ssd1306_WriteString("           ", Font_11x18, White);
				ssd1306_SetCursor(0,44);
				ssd1306_WriteString(vest_aux_message, Font_11x18, White);
				ssd1306_UpdateScreen();

				int offset = 5;
				vest_receive_address(&(vest_aux_message[5+offset]));
				vest_new_message_received_flag_set();
			}
			else
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
		}

		usart1_buffer_action();
		//usart2_buffer_action();
		interface_board_action();
		vest_action();
		//pulse_impact_action();
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

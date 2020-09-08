#include "interface_board_obj.h"
#include "interface_board_interface.h"
#include "usart1_buffer_interface.h"
#include "usart.h"
#include "gpio.h"
#include "math.h"

#include "power_button_interface.h"

#include "ssd1306.h"
#include "stdio.h"

extern int pulse_pain;
//*************************
extern int localization;
//*************************
extern int prim_k;
extern int prim_l;
extern int prim_n;
//*************************
extern int second_k;
extern int second_l;
extern int second_n;
//*************************
extern int puls_k;
extern int puls_l;
extern int puls_n;
//*************************

extern int manzheta_k;

extern int wound_action;
extern int stop_previous_impact;
extern int impact_automat_state;

extern int band_mode;
extern int head_impact;
extern int debug_flag;

extern int chaotic_impact_array[6];

void interface_board_new_message_received_flag_set()
{
	interface_board_new_message_received_flag = 1;
}


void interface_board_new_message_received_flag_reset()
{
	interface_board_new_message_received_flag = 0;
}
int interface_board_new_message_received_flag_get()
{
	return interface_board_new_message_received_flag;
}

void interface_board_object_init()
{
	interface_board_new_message_received_flag_reset();
}


void interface_board_action()
{
	char local_message[128];

	// DEBUG ************************************************************************
	/*
	while(1)
	{

		if((GPIOB->IDR & GPIO_PIN_11) != (uint32_t)GPIO_PIN_RESET) // high level, ready state
		{
			char local_message[128];

			sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", 15, 8, 200, 5);
			HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);

			HAL_Delay(1);
		}
	}
	//*/
	// DEBUG ************************************************************************

//#define TEST_WITHOUT_SLIB

#ifndef TEST_WITHOUT_SLIB
	if(interface_board_new_message_received_flag_get())
#else
	if(debug_flag)
#endif
	{


		interface_board_new_message_received_flag_reset();

		usart1_buffer_get_message(input_message);

		//  tXXXhXXXzXgXsXXX\r\n
		//  где:
		//  t - идентификатор бойца, в которого попали (то есть собственный идентификатор) в формате: 7 и 6 биты - номер команды: 0 - красные, 1 - зеленые, 2 - синие; 5-0 бит - номер бойца в команде;
		//  h - процент оставшейся после попадания жизни от 0 до 100;
		//  z - зона поражения (0 - тело, 1 - голова, 2 - руки, 3 - ноги);
		//  g - тип оружия, из которого стреляли (0 - автомат, 1 - пистолет, 2 - винтовка, 3 - другое);
		//  s - идентификатор стрелявшего бойца (формат такой же как и для идентификатора бойца в поле "t").

		// novaya tablica
		// 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka

		unsigned int t, h, z, g, s;

#ifndef TEST_WITHOUT_SLIB
		if((input_message[0] == 't')&&(input_message[4] == 'h')&&(input_message[8] == 'z')&&(input_message[10] == 'g')&&(input_message[12] == 's'))
#else
		if(debug_flag)
#endif
		{
			sscanf(input_message, "t%3uh%3uz%1ug%1us%3u\r\n", &t, &h, &z, &g, &s);


			if(h == 0)
			{
				// отработка летального поражения
				sprintf(local_message, "smoke\r\n");
				HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
			}

			pulse_pain = 0;

#ifndef TEST_WITHOUT_SLIB
			if(g == 0) // avtomat
#else
			if(debug_flag)
#endif
			{
#ifndef TEST_WITHOUT_SLIB
				if(z==0) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
#else
				if(debug_flag)
#endif
				{
					debug_flag = 0;

#ifdef TEST_WITHOUT_SLIB
					head_impact = 1;
					localization = -1;
#else
					localization = 15;
#endif
				}
				else if(z==1) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					head_impact = 1;

					localization = -1;


					// sound
					HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
					HAL_Delay(1);
					HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);
				}
				else if(z==2) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 1;
				}
				else if(z==3) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 9;
				}
				else if(z==4) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 14;
				}
				else if(z==5) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 0;
				}
				else if(z==6) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 8;
				}
			}
			else if(g == 1) // pistolet
			{
				if(z==0) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 15;
				}
				else if(z==1) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = -1;
					// sound
					HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
					HAL_Delay(1);
					HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);
				}
				else if(z==2) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 1;
				}
				else if(z==3) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 9;
				}
				else if(z==4) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 14;
				}
				else if(z==5) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 0;
				}
				else if(z==6) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 8;
				}
			}
			else if(g == 2) // vintovka
			{
				if(z==0) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 15;
				}
				else if(z==1) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = -1;
					// sound
					HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
					HAL_Delay(2);
					HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);
				}
				else if(z==2) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 1;
				}
				else if(z==3) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 9;
				}
				else if(z==4) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 14;
				}
				else if(z==5) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 0;
				}
				else if(z==6) // 1 - golova, 0 - spina, 4 - grud, 6 - levaya noga, 3 - pravaya noga, 5 - levaya ruka, 2 - pravaya ruka
				{
					localization = 8;
				}
			}// end if(g == 0) // avtomat

			if(localization == -1)
				head_impact = 1;
			else
				head_impact = 0;
			impact_automat_state = 1;   // start new impact

		}// end if((input_message[0] == 't')&&(input_message[4] == 'h')&&(input_message[8] == 'z')&&(input_message[10] == 'g')&&(input_message[12] == 's'))




	}// end if(interface_board_new_message_received_flag_get())

}// end void interface_board_action()


void impact_action()
{
	char local_message[128];
	static uint32_t start_delay_tick = 0;

	static int num_of_iterations;
	static int partial_n;

	if(impact_automat_state == 1)
	{
		pulse_pain = 0;

		while((GPIOB->IDR & GPIO_PIN_11) == (uint32_t)GPIO_PIN_RESET);

		// sound
		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);

		if(band_mode)
		{
			localization = 12;
			sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, manzheta_k, prim_l, prim_n);
			HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
		}
		else
		{
			if(head_impact)
			{
				localization = chaotic_impact_array[HAL_GetTick()%6]; //
				sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, prim_k, prim_l, prim_n);
				HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
			}
			else
			{
				sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, prim_k, prim_l, prim_n);
				HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
			}
		}

		start_delay_tick = HAL_GetTick();
		impact_automat_state = 2;
	}
	else if(impact_automat_state == 2)
	{
		// pause 500 ms
		if((HAL_GetTick() - start_delay_tick) > 700)
			impact_automat_state = 3;
	}
	else if(impact_automat_state == 3)
	{
		// obschaya dlitelnost 3 sec
		// kolichestvo iteracij second_k - 3
		// n = 170 sootvetstvuet 1 sec
		// n v odnoj iteracii = (3 * 170) / (second_k - 3)

		num_of_iterations = second_k - 3;
		partial_n = round((3.0 * 170.0) / ((double)second_k - 3.0));

		impact_automat_state = 4;
	}
	else if(impact_automat_state == 4)
	{
		if(num_of_iterations < 0)
		{
			num_of_iterations = 23;
			partial_n = 42;
			impact_automat_state = 5;
		}
		else
		{
			if((GPIOB->IDR & GPIO_PIN_11) != (uint32_t)GPIO_PIN_RESET)
			{
				if(band_mode)
				{
					localization = 12;
					sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, manzheta_k - num_of_iterations, second_l, partial_n);
					HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
				}
				else
				{
					if(head_impact)
					{
						localization = chaotic_impact_array[num_of_iterations%6]; //
						sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, prim_k, prim_l, prim_n);
						HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
					}
					else
					{
						sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, second_k - num_of_iterations, second_l, partial_n);
						HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
					}
				}

				num_of_iterations--;
			}
		}
	}
	else if(impact_automat_state == 5)
	{
		if(num_of_iterations < 0)
		{
			impact_automat_state = 0;
			pulse_pain = 1;

			if(head_impact)
				localization = 15;
		}
		else
		{
			if((GPIOB->IDR & GPIO_PIN_11) != (uint32_t)GPIO_PIN_RESET)
			{
				if(band_mode)
				{
					localization = 12;
					sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, manzheta_k, second_l, partial_n);
					HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
				}
				else
				{
					if(head_impact)
					{
						localization = chaotic_impact_array[num_of_iterations%6]; //
						sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, prim_k, prim_l, prim_n);
						HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
					}
					else
					{
						sprintf(local_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, second_k, second_l, partial_n);
						HAL_UART_Transmit(&huart3, (uint8_t *)local_message, strlen(local_message), 500);
					}
				}

				num_of_iterations--;
			}
		}

	}
}








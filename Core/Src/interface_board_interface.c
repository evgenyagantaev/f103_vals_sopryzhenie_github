#include "interface_board_obj.h"
#include "interface_board_interface.h"
#include "usart1_buffer_interface.h"
#include "usart.h"

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
extern int wound_action;

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
	//if(interface_board_new_message_received_flag_get())
	if(wound_action)
	{
		//debug debug debug
		wound_action = 0;


		interface_board_new_message_received_flag_reset();

		usart1_buffer_get_message(input_message);

		//  tXXXhXXXzXgXsXXX\r\n
		//  где:
		//  t - идентификатор бойца, в которого попали (то есть собственный идентификатор) в формате: 7 и 6 биты - номер команды: 0 - красные, 1 - зеленые, 2 - синие; 5-0 бит - номер бойца в команде;
		//  h - процент оставшейся после попадания жизни от 0 до 100;
		//  z - зона поражения (0 - тело, 1 - голова, 2 - руки, 3 - ноги);
		//  g - тип оружия, из которого стреляли (0 - автомат, 1 - пистолет, 2 - винтовка, 3 - другое);
		//  s - идентификатор стрелявшего бойца (формат такой же как и для идентификатора бойца в поле "t").

		unsigned int t, h, z, g, s;

		//if((input_message[0] == 't')&&(input_message[4] == 'h')&&(input_message[8] == 'z')&&(input_message[10] == 'g')&&(input_message[12] == 's'))
		if(1)
		{
			sscanf(input_message, "t%3uh%3uz%1ug%1us%3u\r\n", &t, &h, &z, &g, &s);

			char int_board_aux_message[128];

			if(h == 0)
			{
				// отработка летального поражения

			}



			//if(g == 0) // avtomat
			if(1)
			{
				//if(z==0) // tors
				if(1)
				{
					// debug debug****************************
					/*
					ssd1306_Fill(Black);
					int counter = 0;
					while(1)
					{
						sprintf(int_board_aux_message, "hello %3d  ", counter);
						ssd1306_SetCursor(0,0);
						ssd1306_WriteString(int_board_aux_message, Font_11x18, White);
						ssd1306_UpdateScreen();

						sprintf(int_board_aux_message, "hello %3d\r\n", counter);
						HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);


						counter++;

						power_button_action();

						HAL_Delay(2000);

					}
					//*/
					//*****************************************


					// debug delay*****************************
					//HAL_Delay(3000);
					ssd1306_SetCursor(0,0);
					ssd1306_WriteString("SHOCK", Font_16x26, White);
					ssd1306_UpdateScreen();
					// debug delay*****************************


					// sound
					HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);

					sprintf(int_board_aux_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, prim_k, prim_l, prim_n);// levaya ruka pervichnyj
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);

					// sound
					HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);

					HAL_Delay(500);
					sprintf(int_board_aux_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f1\r\n", localization, second_k, second_l, 500);// levaya ruka vtorichnyj
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					HAL_Delay(2800);
					sprintf(int_board_aux_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f0\r\n", localization, second_k, second_l, second_n);// levaya ruka vtorichnyj
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c14k024l0200d05n2000p01000m001f0

					// debug debug debug
					HAL_Delay(12000);
					pulse_pain = 1;

				}
				else if(z==1) // golova
				{
					strcpy(int_board_aux_message, "e1c14k064l0200d05n0003p01000m001f0\r\n"); // slabenkiy
					//strcpy(int_board_aux_message, "v1c00n001l10000d01000\r\n");  // DEBUG VIBRA
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
				}
				else if(z==2) // ruki
				{
					//e1c00k128l0200d05n0009p01000m001f0
					strcpy(int_board_aux_message, "e1c00k128l0200d05n0005p01000m001f0\r\n");// levaya ruka pervichnyj
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					HAL_Delay(300);
					strcpy(int_board_aux_message, "e1c00k12l0200d05n2000p01000m001f0\r\n");// levaya ruka vtorichnyj
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c00k024l0200d05n2000p01000m001f0
				}
				else if(z==3) // nogi
				{
					//e1c08k128l0200d05n0010p01000m001f0
					strcpy(int_board_aux_message, "e1c08k128l0200d05n0005p01000m001f0\r\n");// levaya noga pervichnyj
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					HAL_Delay(300);
					strcpy(int_board_aux_message, "e1c08k14l0200d05n2000p01000m001f0\r\n");// levaya noga vtorichnyj
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c08k026l0200d05n2000p01000m001f0
				}
			}
			else if(g == 1) // pistolet
			{
				if(z==0) // tors
				{
					//e1c14k128l0200d05n0003p01000m001f0
					strcpy(int_board_aux_message, "e1c14k024l0200d05n0003p01000m001f0\r\n");
					//strcpy(int_board_aux_message, "v1c00n001l10000d01000\r\n");  // DEBUG VIBRA
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c14k012l0200d05n2000p01000m001f0
				}
				else if(z==1) // golova
				{
					// отработка локализации поражения
				}
				else if(z==2) // ruki
				{
					//e1c00k128l0200d05n0003p01000m001f0
					strcpy(int_board_aux_message, "e1c00k024l0200d05n0003p01000m001f0\r\n");
					//strcpy(int_board_aux_message, "v1c00n001l10000d01000\r\n");  // DEBUG VIBRA
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c00k012l0200d05n2000p01000m001f0
				}
				else if(z==3) // nogi
				{
					//e1c08k128l0200d05n0005p01000m001f0
					strcpy(int_board_aux_message, "e1c08k024l0200d05n0005p01000m001f0\r\n");
					//strcpy(int_board_aux_message, "v1c00n001l10000d01000\r\n");  // DEBUG VIBRA
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c08k014l0200d05n2000p01000m001f0
				}
			}
			else if(g == 2) // vintovka
			{
				if(z==0) // tors
				{
					//e1c14k128l0200d05n0006p01000m001f0
					strcpy(int_board_aux_message, "e1c14k024l0200d05n0006p01000m001f0\r\n");
					//strcpy(int_board_aux_message, "v1c00n001l10000d01000\r\n");  // DEBUG VIBRA
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c14k018l0200d05n2000p01000m001f0
				}
				else if(z==1) // golova
				{
					// отработка локализации поражения
				}
				else if(z==2) // ruki
				{
					//e1c00k128l0200d05n0006p01000m001f0
					strcpy(int_board_aux_message, "e1c00k024l0200d05n0006p01000m001f0\r\n");
					//strcpy(int_board_aux_message, "v1c00n001l10000d01000\r\n");  // DEBUG VIBRA
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c00k018l0200d05n2000p01000m001f0
				}
				else if(z==3) // nogi
				{
					//e1c08k128l0200d05n0007p01000m001f0
					strcpy(int_board_aux_message, "e1c08k024l0200d05n0007p01000m001f0\r\n");
					//strcpy(int_board_aux_message, "v1c00n001l10000d01000\r\n");  // DEBUG VIBRA
					HAL_UART_Transmit(&huart3, (uint8_t *)int_board_aux_message, strlen(int_board_aux_message), 500);
					//e1c08k020l0200d05n2000p01000m001f0
				}
			}

			//HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);


			// формирования управляющей строки для генератора
		}


	}
}


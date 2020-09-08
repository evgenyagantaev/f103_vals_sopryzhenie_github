#include "vest_obj.h"
#include "vest_interface.h"
#include "usart2_buffer_interface.h"
#include "pulse_impact_interface.h"

#include "ssd1306.h"
#include "stdio.h"
#include "usart.h"

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

extern int vest_interface_condition;   // ozhidanie adresa (0); adres poluchen (1)

extern int band_mode;
extern int debug_flag;
extern int head_impact;

extern int chaotic_impact_array[6];

void vest_new_message_received_flag_set()
{
	vest_new_message_received_flag = 1;
}


void vest_new_message_received_flag_reset()
{
	vest_new_message_received_flag = 0;
}
int vest_new_message_received_flag_get()
{
	return vest_new_message_received_flag;
}

void vest_object_init()
{
	vest_new_message_received_flag_reset();
	r_pick_counter = 0;
	vest_address_received_flag = 0;

	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(100);

	ssd1306_SetCursor(0,0);
	ssd1306_WriteString("ready      ", Font_11x18, White);
	ssd1306_UpdateScreen();

	/*
	sprintf(aux_message, "AT+AB Bond 0080E1FD2CE9 0000\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)aux_message, strlen(aux_message), 500);  // for production board
	HAL_Delay(5000);
	sprintf(aux_message, "AT+AB SPPConnect 0080E1FD2CE9\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)aux_message, strlen(aux_message), 500);  // for production board
	HAL_Delay(2000);
	//*/
}


void vest_receive_address(char *address_string)
{
	int i;

	for(i=0; i<12; i++)
		vest_address_string[i] = address_string[i];
	vest_address_string[12] = 0;

}

void vest_receive_message(char *message_string)
{
	int i;
	int length = strlen(message_string);

	if (length < VEST_MESSAGE_LENGTH)
	{
		for(i=0; i<length; i++)
			vest_message[i] = message_string[i];
		vest_message[length] = 0;
	}
	else  // error
	{
		ssd1306_SetCursor(0,0);
		ssd1306_WriteString("error      ", Font_11x18, White);
		ssd1306_SetCursor(0,22);
		ssd1306_WriteString("vest_recv_m", Font_11x18, White);
		ssd1306_SetCursor(0,44);
		ssd1306_WriteString("overflow   ", Font_11x18, White);
		ssd1306_UpdateScreen();
	}

}



void vest_action()
{
	//if(vest_new_message_received_flag_get())
	if((HAL_GetTick() - pulse_timeout_marker) > PULSE_TIMEOUT)
	{
		vest_new_message_received_flag_reset();
		pulse_timeout_marker = HAL_GetTick();


		//usart2_buffer_get_message(vest_message);

		//if((vest_message[0] == 'R') && strlen(vest_message) == 3) // new R-marker
		{

			pulse_impact_new_r_pick_detected_flag_set();

			r_pick_counter++;

			/*
			sprintf(aux_message, "R -> %u", (unsigned int)r_pick_counter);
			ssd1306_SetCursor(0,0);
			ssd1306_WriteString("           ", Font_11x18, White);
			ssd1306_SetCursor(0,0);
			ssd1306_WriteString(aux_message, Font_11x18, White);
			ssd1306_UpdateScreen();
			//*/



			if(pulse_pain)
			{

				// heart beat
				HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
				HAL_Delay(2);
				HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);
				HAL_Delay(2);
				HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
				HAL_Delay(4);
				HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);

				if(localization != -1)
				{
					if(band_mode)
					{
						localization = 12;
						sprintf(aux_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f2\r\n", localization, manzheta_k, puls_l, puls_n);
						HAL_UART_Transmit(&huart3, (uint8_t *)aux_message, strlen(aux_message), 500);
					}
					else
					{
						if(head_impact)
						{
							localization = chaotic_impact_array[HAL_GetTick()%6]; //
							sprintf(aux_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f2\r\n", localization, puls_k, puls_l, puls_n);
							HAL_UART_Transmit(&huart3, (uint8_t *)aux_message, strlen(aux_message), 500);
						}
						else
						{
							sprintf(aux_message, "e1c%02dk%03dl%04dd05n%04dp01000m001f2\r\n", localization, puls_k, puls_l, puls_n);
							HAL_UART_Transmit(&huart3, (uint8_t *)aux_message, strlen(aux_message), 500);
						}
					}
				}
			}

			/*
			HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_RESET);
			uint32_t delay_counter = 0;
			while(delay_counter < 4250)
			{
				delay_counter++;
			}
			HAL_GPIO_WritePin(sound_power_GPIO_Port, sound_power_Pin, GPIO_PIN_SET);
			//*/
		}
		if((vest_message[0] == 'c') && (vest_message[1] == '1')) // new status string
		{
			int heart_rate = 0;
			int aux = 0;
			aux = (int)vest_message[3] - 48;
			heart_rate += (aux*100);
			aux = (int)vest_message[4] - 48;
			heart_rate += (aux*10);
			aux = (int)vest_message[5] - 48;
			heart_rate += aux;

			/*
			sprintf(aux_message, "H -> %u", (unsigned int)heart_rate);
			ssd1306_SetCursor(0,22);
			ssd1306_WriteString("           ", Font_11x18, White);
			ssd1306_SetCursor(0,22);
			ssd1306_WriteString(aux_message, Font_11x18, White);
			ssd1306_UpdateScreen();
			//*/
		}

		if((vest_message[0] == 'b') && (vest_message[1] == 'a') && (vest_message[2] == 'n') && band_mode == 0) // switch to band mode
		{
			band_mode = 1;
			//debug************************
			debug_flag = 1;
		}


	}


}


#include "vest_obj.h"
#include "vest_interface.h"
#include "usart2_buffer_interface.h"
#include "pulse_impact_interface.h"

#include "ssd1306.h"
#include "stdio.h"
#include "usart.h"

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

	sprintf(aux_message, "AT+AB Bond 0080E1FD2CE9 0000\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)aux_message, strlen(aux_message), 500);  // for production board
	HAL_Delay(5000);
	sprintf(aux_message, "AT+AB SPPConnect 0080E1FD2CE9\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)aux_message, strlen(aux_message), 500);  // for production board
	HAL_Delay(2000);
}


void vest_action()
{
	if(vest_new_message_received_flag_get())
	{
		vest_new_message_received_flag_reset();

		usart2_buffer_get_message(vest_message);

		if((vest_message[0] == 'R') && strlen(vest_message) == 3) // new R-marker
		{
			pulse_impact_new_r_pick_detected_flag_set();

			r_pick_counter++;

			sprintf(aux_message, "R -> %u", (unsigned int)r_pick_counter);
			ssd1306_SetCursor(0,0);
			ssd1306_WriteString("           ", Font_11x18, White);
			ssd1306_SetCursor(0,0);
			ssd1306_WriteString(aux_message, Font_11x18, White);
			ssd1306_UpdateScreen();
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

			sprintf(aux_message, "H -> %u", (unsigned int)heart_rate);
			ssd1306_SetCursor(0,22);
			ssd1306_WriteString("           ", Font_11x18, White);
			ssd1306_SetCursor(0,22);
			ssd1306_WriteString(aux_message, Font_11x18, White);
			ssd1306_UpdateScreen();
		}

	}


}


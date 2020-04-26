#include "pulse_impact_obj.h"
#include "pulse_impact_interface.h"

#include "usart.h"



void pulse_impact_object_init()
{
	pulse_pain_mode_flag = 1;
	pulse_pain_localization = 0;
	pulse_impact_new_r_pick_detected_flag = 0;
	r_pick_time_marker = 0;
}



void pulse_impact_action()
{
	if(pulse_pain_mode_flag)
	{
		if(pulse_impact_new_r_pick_detected_flag)
		{
			// drop flag
			pulse_impact_new_r_pick_detected_flag = 0;
			// set time-marker
			r_pick_time_marker = HAL_GetTick();
		}
		else if(r_pick_time_marker != 0)
		{
			if((HAL_GetTick() - r_pick_time_marker) > PULSE_WAVE_DELAY)
			{
				// drop time marker
				r_pick_time_marker = 0;
				// create text command of pulse impact
				//sscanf(usart_buffer, "e%1dc%2dk%3dl%4dd%2dn%4dp%5dm%3df%1d\r\n",
					   //&on_off_flag, &channel, &NUMBER_OF_CHARGE_PULSES, &DISCHARGE_IMPULSE_LENGTH,
					   //&DELAY_LENGTH, &CHOCK_LENGTH, &aux, &aux, &FORM);
				sprintf(pulse_impact_command, "e%01dc%02dk%03dl%04dd%02dn%04dp%05dm%03df%01d\r\n", 1, 0, 27, 50, 10, 3, 0, 0, 0);
				// send command
				HAL_UART_Transmit(&huart3, (uint8_t *)pulse_impact_command, strlen(pulse_impact_command), 500);
			}
		}
	}
}


void pulse_impact_new_r_pick_detected_flag_set()
{
	pulse_impact_new_r_pick_detected_flag = 1;
}
void pulse_impact_new_r_pick_detected_flag_drop()
{
	pulse_impact_new_r_pick_detected_flag = 0;
}


void pulse_pain_mode_flag_set()
{
	pulse_pain_mode_flag = 1;
}
void pulse_pain_mode_flag_drop()
{
	pulse_pain_mode_flag = 0;
}



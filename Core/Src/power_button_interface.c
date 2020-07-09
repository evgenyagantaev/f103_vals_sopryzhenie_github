#include "power_button_obj.h"
#include "power_button_interface.h"

void power_button_action()
{
	if(button_tick_start_marker > 0)  // uzhe videli, chto knopka nazhata
	{
		if((GPIOA->IDR & power_button_pin) == (uint32_t)GPIO_PIN_RESET) // knopka nazhata
		{
			if((HAL_GetTick() - button_tick_start_marker) > POWER_OFF_INTERVAL) // nazhata bolshe 2 sekund
			{
				HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_SET); // led off
				HAL_GPIO_WritePin(GPIOA, power_control_pin, GPIO_PIN_RESET); // power off

				while(1);

			}
		}
		else // knopka ne nazhata
		{
			button_tick_start_marker = 0;
		}

	}
	else  // esche ne videli, chto knopka nazhata
	{
		if((GPIOA->IDR & power_button_pin) == (uint32_t)GPIO_PIN_RESET) // knopka nazhata
		{
			// power button is pressed
			button_tick_start_marker = HAL_GetTick();
		}
	}
}



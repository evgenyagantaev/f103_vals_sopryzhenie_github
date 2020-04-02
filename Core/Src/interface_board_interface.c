#include "interface_board_obj.h"
#include "interface_board_interface.h"
#include "usart1_buffer_interface.h"

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
	if(interface_board_new_message_received_flag_get())
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

		unsigned int t, h, z, g, s;

		if((input_message[0] == 't')&&(input_message[4] == 'h')&&(input_message[8] == 'z')&&(input_message[10] == 'g')&&(input_message[12] == 's'))
		{
			sscanf(input_message, "t%3uh%3uz%1ug%1us%3u\r\n", &t, &h, &z, &g, &s);

			if(h == 0)
			{
				// отработка летального поражения
			}
			else
			{
				if(g == 0)
				{
					// отработка типа оружия, то есть, степени воздействия
				}

				if(z==0)
				{
					// отработка локализации поражения
				}

				// формирования управляющей строки для генератора
			}
		}


	}
}


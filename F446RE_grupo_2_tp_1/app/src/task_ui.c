/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "task_ui.h"
#include "task_led.h"
/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_            (1)
#define QUEUE_ITEM_SIZE_         (sizeof(msg_event_t))

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static QueueHandle_t hao_hqueue;

/********************** external data definition *****************************/

//extern SemaphoreHandle_t hsem_button;
//extern SemaphoreHandle_t hsem_led;

extern ao_led_handle_t led_red;
extern ao_led_handle_t led_green;
extern ao_led_handle_t led_blue;

/********************** internal functions definition ************************/

/********************** external functions definition ************************/


void task_ao_ui_init(void)
{
	hao_hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while(NULL == hao_hqueue) { }

	BaseType_t status;
	status = xTaskCreate(task_ui, "task_ao_ui", 128, NULL, tskIDLE_PRIORITY, NULL);
	while (pdPASS != status) { }
}

void task_ui(void *argument)
{

	while (true) {

		msg_event_t event_msg;

		if (pdPASS == xQueueReceive(hao_hqueue, &event_msg, portMAX_DELAY)) {

			switch (event_msg) {

				case MSG_EVENT_BUTTON_PULSE:
					LOGGER_INFO("[UI] led red %d", AO_LED_MESSAGE_ON);
					ao_led_send(&led_red, AO_LED_MESSAGE_ON);
					break;
				case MSG_EVENT_BUTTON_SHORT:
					LOGGER_INFO("[UI] led green");
					ao_led_send(&led_green, AO_LED_MESSAGE_ON);
					break;
				case MSG_EVENT_BUTTON_LONG:
					LOGGER_INFO("[UI] led blue");
					ao_led_send(&led_blue, AO_LED_MESSAGE_ON);
					break;
				default:
					break;
			}
		}
		LOGGER_INFO("[UI] led activate");
	}
}

bool ao_ui_send_event(msg_event_t msg) {

	BaseType_t status = xQueueSend(hao_hqueue, &msg, 0);
	if (status != pdPASS) {

		LOGGER_INFO("[UI] Cola llena: evento %d perdido", msg);
	} else {

		LOGGER_INFO("[UI] Evento enviado: %d", msg);
	}
	return (status == pdPASS);
}

/********************** end of file ******************************************/

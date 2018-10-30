/*
    ВНИМАНИЕ! 
Для выполнения этого задания у вас должен быть способ передавать с платы на компьютер 
сообщения через интерфейс UART. Для этого к плате stm32f0discovery должен быть подключен UART-USB адаптер.
PB6 - это UART TX
PB7 - это UART RX
*/ 

// подключение библиотек
#include <stdio.h>
#include "xtimer.h"
#include "timex.h"
#include "periph/adc.h"
#include <math.h>

char stack_adc[THREAD_STACKSIZE_MEDIUM];
char stack_blink_pc8[THREAD_STACKSIZE_MEDIUM];

static kernel_pid_t thread_blink_pid;

float valueVoltage = 0.0f;

void * thread_task(void *arg)
{
	int sample = 0;

	adc_init(ADC_LINE(0));

	msg_t msg;


	while(true)
	{
		sample = adc_sample(ADC_LINE(0), ADC_RES_12BIT);
		valueVoltage = sample / (pow(2, 12) -1) * 100;
		
		if(sample >= 2096 / 2)
		{
			msg.content.value = true;
		}
		else
		{
			msg.content.value = false;
		}

		msg_send(&msg, thread_blink_pid);

		printf("ADC: %d VOLTAGE: %f\r\n", sample, valueVoltage);
		xtimer_usleep(100000);
	}

	return NULL;
}

void * blink_thread_task(void *arg)
{
	(void)arg;
	msg_t msg;

	msg_receive(&msg);

	xtimer_ticks32_t timer = xtimer_now(); 
	gpio_init(GPIO_PIN(PORT_C, 8), GPIO_OUT);

	while(msg.content.value)
	{
		xtimer_periodic_wakeup(&timer, 100 * US_PER_MS);
		gpio_toggle(GPIO_PIN(PORT_C, 8));
	}
}


int main(void)
{

    thread_create(stack_adc, sizeof(stack_adc),
		    THREAD_PRIORITY_MAIN - 1,
		    THREAD_CREATE_STACKTEST,
		    thread_task,
		    NULL, "thread adc"
		    );

    thread_blink_pid = thread_create(stack_blink_pc8, sizeof(stack_blink_pc8),
		    THREAD_PRIORITY_MAIN - 1,
		    THREAD_CREATE_STACKTEST,
		    blink_thread_task,
		    NULL, "thread blink task"
		    );
    return 0;
}

/*
Задание 1: переключите разрешение АЦП на 12 бит. 1+
Задание 2: сделайте так, чтобы в консоль выводилась строка "ADC: xxx VOLTAGE: yyy ",
            где xxx - число, получаемое с АЦП (тип int), yyy - измеренное напряжение в вольтах (тип float) 2+
Задание 3: вынесите чтение АЦП в отдельный поток. После этого добавьте другой поток, который будет 
            включать светодиод PC8, если АЦП измерил напряжение больше половины от максимального, 
            и выключать, если меньше.
Задание 4: модифицирейте код предудщего решения так, чтобы ручка потенциометра задавала период моргания светодиода. 
            одно крайнее положение ручки должно соответствовать периоду 100 мс, другое - 2 с.
*/

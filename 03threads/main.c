// Подключение библиотек
#include "thread.h"
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

char stack_one[THREAD_STACKSIZE_MEDIUM];
char stack_two[THREAD_STACKSIZE_MEDIUM];

#define INTERVAL_BLUE 100 * US_PER_MS
#define INTERVAL_GREEN 500 * US_PER_MS

struct blinkData{
	gpio_t pin;
	uint32_t period;
};

void *thread_handler(void *arg)
{
    struct blinkData * data = (struct blinkData *) arg;

    gpio_t pin = data->pin;
    uint32_t period = data->period;

    gpio_init(pin, GPIO_OUT);

    xtimer_ticks32_t last_wakeup_one = xtimer_now();

    while(1){
    	xtimer_periodic_wakeup(&last_wakeup_one, period);
	gpio_toggle(pin);
    }
    return NULL;
}



int main(void)
{

    struct blinkData blueData = {GPIO_PIN(PORT_C, 8), INTERVAL_BLUE};
    struct blinkData greenData = {GPIO_PIN(PORT_C, 9), INTERVAL_GREEN };

    thread_create(stack_one, sizeof(stack_one),
                    THREAD_PRIORITY_MAIN-1,
                    THREAD_CREATE_STACKTEST,
                    thread_handler,
                    &blueData, "thread_one");

    thread_create(stack_two, sizeof(stack_two),
		    THREAD_PRIORITY_MAIN-1,
		    THREAD_CREATE_STACKTEST,
		    thread_handler,
		    &greenData, "thread two"
		    );
    return 0;
}

/*
    Задание 1. Создайте второй поток, который будет моргать другим светодиодом с другой частотой. 
                Обратите внимание, что не рекомендуется создавать потоки с одинаковым приоритетом!
*/

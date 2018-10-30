// Подключение библиотек
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

#define INTERVAL_FIX 50000
xtimer_ticks32_t lastActivate;

// Обработчик прерывания по нажатию кнопки

void btn_handler_both(void *arg)
{
   (void)arg;

    if(xtimer_now().ticks32 - lastActivate.ticks32 < INTERVAL_FIX)
	   return;

    if(gpio_read(GPIO_PIN(PORT_A, 0)) == 1)
    {
	gpio_toggle(GPIO_PIN(PORT_C, 8));
    }
    else
    {
	gpio_toggle(GPIO_PIN(PORT_C, 8));
    }
}

int main(void)
{
  // Инициализация пина PA0, к которому подключена кнопка, как источника прерывания.
  // GPIO_RISING - прерывание срабатывает по фронту
  // btn_handler - имя функции обработчика прерывания
  // NULL - ничего не передаем в аргументах
  gpio_init_int(GPIO_PIN(PORT_A, 0), GPIO_IN, GPIO_BOTH, btn_handler_both, NULL);
  // Инициализация пина PC8 на выход
  gpio_init(GPIO_PIN(PORT_C, 8), GPIO_OUT);

  lastActivate = xtimer_now();
  while(1){
  }
  return 0;
}

/*
Задание 1. Подавите дребезг контакта кнопки. Для этого запомните время, когда вызвалось прерывание, 
            и при последующем вызове, если не прошло 100 мс, не переключайте светодиод. 1+
Задание 2. Сделайте так, чтобы при удержании кнопки светодиод светился, а при отпускании - гас. 
            В этом случае нужно заставить прерывание срабатывать и на фронт, и на спад. 
            Это задается при инициализации прерывания макросом GPIO_BOTH. 
            Обработчик прерывания должен узнать, его вызвал фронт или спад.
            Для этого можно прочитать значение пина с кнопкой функцией gpio_read. 
            Если прочитали высокое состояние - то это был фронт, если низкое - это был спад.   
*/

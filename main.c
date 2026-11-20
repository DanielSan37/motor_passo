#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor.h"

/*void app_main(void)
{
    const char *nome[3] = { "A", "B", "C" };

    motor_gpio_init();      // começa com a fase A ligada
    motor_set_modo(2);      // passo completo: A -> B -> C
    motor_set_sentido(1);

    while (1) {
        int32_t p = motor_get_posicao();
        int f = ((p / 2) % 3 + 3) % 3;          // 0 = A, 1 = B, 2 = C
        printf("Fase %s ligada\n", nome[f]);
        vTaskDelay(pdMS_TO_TICKS(15000));
        motor_passo();
    }
}*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor.h"

void app_main(void)
{
    motor_gpio_init();
    motor_set_modo(1);       // teste em passo completo (7,5°)
    motor_set_sentido(1);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));   // provisório: será o timer na Etapa 2
        motor_passo();
        /*printf("posicao = %ld  (%.2f graus)\n",
               (long)motor_get_posicao(), motor_get_posicao() * 3.75f);*/
    }
}
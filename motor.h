#ifndef MOTOR_H_
#define MOTOR_H_
#include <stdint.h>

void motor_gpio_init(void);    // configura os pinos das fases
void motor_passo(void);        // avança um estado na tabela (será chamada pela ISR)

// Parâmetros que a interface web vai alterar no futuro
void motor_set_modo(uint8_t incremento);   // 2 = 7,5°  | 1 = 3,75°
void motor_set_sentido(int8_t sentido);    // +1 / -1
int32_t motor_get_posicao(void);           // em unidades de 3,75°

#endif //MOTOR_H_
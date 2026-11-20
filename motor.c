#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor.h"
#include "soc/soc.h"            // REG_WRITE, BIT()
#include "soc/gpio_reg.h"       // GPIO_OUT_W1TS_REG, GPIO_ENABLE_W1TS_REG, GPIO_FUNC0_OUT_SEL_CFG_REG
#include "soc/gpio_sig_map.h"   // SIG_GPIO_OUT_IDX
#include "soc/io_mux_reg.h"     // PIN_FUNC_SELECT, PIN_FUNC_GPIO, PIN_INPUT_DISABLE...
#include "soc/gpio_periph.h"    // GPIO_PIN_MUX_REG[]

// Bits de pull-up e pull-down do IO_MUX_x_REG (TRM: campos FUN_WPU e FUN_WPD)
#ifndef FUN_PU
#define FUN_PU  BIT(8)
#endif
#ifndef FUN_PD
#define FUN_PD  BIT(7)
#endif

// ---------- Pinos (IN1 de cada ponte; IN2 em GND; EN no jumper por enquanto) ----------
#define PINO_A  25
#define PINO_B  26
#define PINO_C  27

#define FA  BIT(PINO_A)
#define FB  BIT(PINO_B)
#define FC  BIT(PINO_C)
#define MASCARA_FASES  (FA | FB | FC)

// ---------- Tabela de estados (meio passo); passo completo = índices pares ----------
static const uint32_t seq[6] = { FA, FA | FB, FB, FB | FC, FC, FC | FA };

static volatile uint8_t  idx        = 0;
static volatile int8_t   incremento = 2;   // 2 = 7,5° | 1 = 3,75°
static volatile int8_t   sentido    = 1;   // +1 / -1
static volatile int32_t  posicao    = 0;   // unidades de 3,75°

// ---------- Configura UM pino como saída GPIO simples ----------
static void config_saida(int pino)
{
    uint32_t iomux = GPIO_PIN_MUX_REG[pino];   // endereço do IO_MUX_x_REG deste pino

    // Portão 2 – IO_MUX: função GPIO, sem entrada, sem pull-up/down
    PIN_FUNC_SELECT(iomux, PIN_FUNC_GPIO);     // MCU_SEL = 2
    PIN_INPUT_DISABLE(iomux);                  // FUN_IE  = 0
    REG_CLR_BIT(iomux, FUN_PU | FUN_PD);       // FUN_WPU = 0, FUN_WPD = 0                   // FUN_WPD = 0

    // Portão 1 – GPIO Matrix: pino comandado pelo GPIO_OUT_REG
    REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG + 4 * pino, SIG_GPIO_OUT_IDX);

    // Garante nível 0 ANTES de habilitar (fase desligada ao ligar a saída)
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(pino));

    // Portão 3 – habilita o driver de saída
    REG_WRITE(GPIO_ENABLE_W1TS_REG, BIT(pino));
}

// ---------- Escreve um estado da tabela nas 3 fases ----------
static inline void aplica_estado(uint32_t estado)
{
    REG_WRITE(GPIO_OUT_W1TC_REG, MASCARA_FASES & ~estado);  // desliga as que saem
    REG_WRITE(GPIO_OUT_W1TS_REG, estado);                   // liga as que entram
}

// ---------- Funções públicas ----------
void motor_gpio_init(void)
{
    config_saida(PINO_A);
    config_saida(PINO_B);
    config_saida(PINO_C);
    aplica_estado(seq[idx]);
}

void motor_passo(void)
{
    int8_t d = incremento * sentido;   // -2, -1, +1 ou +2
    idx = (idx + 6 + d) % 6;
    posicao += d;
    aplica_estado(seq[idx]);
}

void motor_set_modo(uint8_t inc)    { incremento = (inc == 1) ? 1 : 2; }
void motor_set_sentido(int8_t s)    { sentido = (s < 0) ? -1 : 1; }
int32_t motor_get_posicao(void)     { return posicao; }
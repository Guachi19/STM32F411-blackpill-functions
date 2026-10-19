#include <stdint.h>

#define REG(addr) (*(volatile uint32_t *)(addr))

#define RCC_AHB1ENR  REG(0x40023830)
#define GPIOC_MODER  REG(0x40020800)
#define GPIOC_BSRR   REG(0x40020818)

#define SYST_CSR     REG(0xE000E010)
#define SYST_RVR     REG(0xE000E014)
#define SYST_CVR     REG(0xE000E018)

static volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) { ms_ticks++; }

static void delay_ms(uint32_t ms)
{
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms);
}

int main(void)
{
    // SysTick a 1 ms (HSI = 16 MHz)
    SYST_RVR = 16000 - 1;
    SYST_CVR = 0;
    SYST_CSR = 7;                       // reloj del core + interrupción + enable

    RCC_AHB1ENR |= (1U << 2);           // reloj de GPIOC
    GPIOC_MODER &= ~(3U << (13 * 2));
    GPIOC_MODER |=  (1U << (13 * 2));   // PC13 salida

    while (1)
    {
        GPIOC_BSRR = (1U << (13 + 16)); // LED encendido (activo en bajo)
        delay_ms(1000);
        GPIOC_BSRR = (1U << 13);        // LED apagado
        delay_ms(1000);
    }
}
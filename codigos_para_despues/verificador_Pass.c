/**
 ******************************************************************************
 * @file           : main.c
 * @author         : JULIAN BAEZ CAICEDO
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f411xe.h"

#define MASCARA_SEGMENTOS ((1U << 12) | \
                           (1U << 6)  | \
                           (1U << 7)  | \
                           (1U << 13) | \
                           (1U << 10) | \
                           (1U << 9)  | \
                           (1U << 5))

#define MASCARA_TRANS ((1U << 10) | \
                      (1U << 8)  | \
                      (1U << 9)  | \
                      (1U << 6))

#define LED_PIN 5U

volatile uint8_t digitoActual = 0;
volatile uint16_t contador = 0;
volatile uint32_t tiempo_ms = 0;


 //Palabra
volatile uint32_t tiempo_exti1 = 0;
volatile uint32_t tiempo_exti4 = 0;

volatile uint8_t ocurrido_exti1 = 0;
volatile uint8_t ocurrido_exti4 = 0;

volatile uint8_t mostrar_palabra = 0;
volatile uint32_t inicio_palabra = 0;
//palabra

void configurarEXTI(void);
void configurarTIM11(void); //LED BLINKY COPIAR Y PEGAR EN FUTUROS CODIGOS
void configurarGPIO_Blinky(void); //LED BLINKY COPIAR Y PEGAR EN FUTUROS CODIGOS
void numero1(void);
void numero2(void);
void numero3(void);
void numero4(void);
void numero5(void);
void numero6(void);
void numero7(void);
void numero8(void);
void numero9(void);
void numero0(void);
void letraA(void);
void letraP(void);
void mostrarNumero(uint8_t);




int main()
{

    // Habilitar los relojes para GPIOA, GPIOB, GPIOC, TIM2 y SYSCFG

    RCC->AHB1ENR |= (1u<<0); // Activa el reloj del GPIOA
    RCC->AHB1ENR |= (1u<<1); // Activa el reloj del GPIOB
    RCC->AHB1ENR |= (1u<<2); // Activa el reloj del GPIOC
    RCC->APB1ENR |= (1U<<0); // Activa el reloj del TIM2
    RCC->APB2ENR |= (1U<<14); // Activa el reloj del SYSCFG

    // Configurar los pines PA

    GPIOA->MODER &= ~((3U << 2)|(3U << 8)); // Configura PA1 y PA4 como entrada

    // Configurar los pines PB

    GPIOB->MODER &= ~((3U << 24) | //Limpia el pin PB12 == A
                  (3U << 12) | //Limpia el pinPB6 == B
                  (3U << 14) | //Limpia el pin PB7 == C
                  (3U << 26) | //Limpia el pin PB13 == D
                  (3U << 20) | //Limpia el pin PB10 == E
                  (3U << 18) | //Limpia el pin PB9 == F
                  (3U << 10)); //Limpia el pin PB5 == G
                  
    GPIOB->MODER |=  ((1U << 24) | //Habilita el pin PB12 == A
                  (1U << 12) | //Habilita el pinPB6 == B
                  (1U << 14) | //Habilita el pin PB7 == C
                  (1U << 26) | //Habilita el pin PB13 == D
                  (1U << 20) | //Habilita el pin PB10 == E
                  (1U << 18) | //Habilita el pin PB9 == F
                  (1U << 10)); //Habilita el pin PB5 == G

    // Configurar los pines PC
    
    GPIOC->MODER &= ~((3U << 20) | //Limpia el pin PC10 == DP1
                    (3U << 16)| //Limpia el pin PC8 == DP2
                    (3U << 18)| //Limpia el pin PC9 == DP3
                    (3U << 12)); //Limpia el pin PC6 == DP4

    GPIOC->MODER |=  ((1U << 20) | //Habilita el pin PC10 == DP1
                    (1U << 16)| //Habilita el pin PC8 == DP2
                    (1U << 18)| //Habil ita el pin PC9 == DP3
                    (1U << 12)); //Habilita el pin PC6 == DP4  
                    
    //APAGAMOS TODOS LOS TRANSISTORES Y SEGMENTOS, PREPARAMOS LOS TIMER Y HABILITAMOS INTERRUPCIONES

    GPIOC->ODR &= ~MASCARA_TRANS; //Apagamos todos los transistores
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; //Apagamos todos los segmentos
    
     // Configurar TIM2
    TIM2->PSC = 15999U;
    TIM2->ARR = 1U;

    // Preparar el temporizador
    TIM2->CNT = 0;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF; //limpia la bandera de actualización

    configurarEXTI(); // Configura las interrupciones externas.

    // Habilitar interrupción de actualización
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn); // Habilitar interrupciones del TIM2 en el NVIC

    // Iniciar TIM2
    TIM2->CR1 |= TIM_CR1_CEN;

    configurarGPIO_Blinky(); //LED BLINKY COPIAR Y PEGAR EN FUTUROS CODIGOS
    configurarTIM11(); //LED BLINKY COPIAR Y PEGAR EN FUTUROS CODIGOS

    while(1){

    }
}


void configurarEXTI(void)
{
    // Seleccionar PA1 como fuente de EXTI1
    SYSCFG->EXTICR[0] &= ~(0xFU << 4);

    // Seleccionar PA4 como fuente de EXTI4
    SYSCFG->EXTICR[1] &= ~(0xFU << 0);

    // EXTI1 (PA1): detectar flancos de subida
    EXTI->RTSR |= (1U << 1);

    // EXTI4 (PA4): detectar flancos de bajada
    EXTI->FTSR |= (1U << 4);

    // Desactivar los flancos opuestos
    EXTI->FTSR &= ~(1U << 1);
    EXTI->RTSR &= ~(1U << 4);

    // Limpiar banderas pendientes
    EXTI->PR = (1U << 1) | (1U << 4);

    // Habilitar las líneas EXTI1 y EXTI4
    EXTI->IMR |= (1U << 1) | (1U << 4);

    // Configurar prioridades
    NVIC_SetPriority(EXTI1_IRQn, 2);
    NVIC_SetPriority(EXTI4_IRQn, 2);

    // Habilitar interrupciones en el NVIC
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
}

void EXTI1_IRQHandler(void)
{
    if (EXTI->PR & (1U << 1))
    {
        if (!mostrar_palabra)
        {
            contador++;

            tiempo_exti1 = tiempo_ms;
            ocurrido_exti1 = 1;

            if (ocurrido_exti4 &&
                (uint32_t)(tiempo_exti1 - tiempo_exti4) <= 300U) //Tiempo para ejecutar la palabra
            {
                contador = 0; //reinicia el contador
                mostrar_palabra = 1;
                inicio_palabra = tiempo_ms;

                ocurrido_exti1 = 0;
                ocurrido_exti4 = 0;
            } //verifica si ocurre EXTI1 Y EXTI4
            else if (ocurrido_exti4)
            {
                ocurrido_exti4 = 0;
            }
        }

        EXTI->PR = (1U << 1);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI->PR & (1U << 4))
    {
        if (!mostrar_palabra)
        {
            if (contador > 0)
            {
                contador--;
            }

            tiempo_exti4 = tiempo_ms;
            ocurrido_exti4 = 1;

            if (ocurrido_exti1 &&
                (uint32_t)(tiempo_exti4 - tiempo_exti1) <= 300U) //Tiempo de ejecución de la IR
            {
                contador = 0; //reinicio de contador 
                mostrar_palabra = 1; //Define cuando se activa la palabra
                inicio_palabra = tiempo_ms;

                ocurrido_exti1 = 0;
                ocurrido_exti4 = 0;
            } //VERIFICA SI OCURRE EXTI1 Y EXTI4
            else if (ocurrido_exti1)
            {
                ocurrido_exti1 = 0;
            }
        }

        EXTI->PR = (1U << 4);
    }
}



void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) // Verifica si la bandera de actualización está activa
    {
        TIM2->SR &= ~TIM_SR_UIF; // Limpia la bandera de actualización
        tiempo_ms += 2;

        // Comprobar si terminó el tiempo de mostrar PASS
        if (mostrar_palabra && (uint32_t)(tiempo_ms - inicio_palabra) >= 1000U)
        {
            mostrar_palabra = 0; 
            contador = 0; //Interrupción de la palabra
        }

            if (contador >= 10000){
                contador = 0;
            }
        

        GPIOC->ODR &= ~MASCARA_TRANS; // Apaga todos los transistores

switch (digitoActual)
{
    case 0:
        if (mostrar_palabra)
            letraP();
        else
            mostrarNumero((contador / 1000) % 10);

        GPIOC->ODR |= (1U << 6);
        break;

    case 1:
        if (mostrar_palabra)
            letraA();
        else
            mostrarNumero((contador / 100) % 10);

        GPIOC->ODR |= (1U << 9);
        break;

    case 2:
        if (mostrar_palabra)
            mostrarNumero(5);
        else
            mostrarNumero((contador / 10) % 10);

        GPIOC->ODR |= (1U << 8);
        break;

    case 3:
        if (mostrar_palabra)
            mostrarNumero(5);
        else
            mostrarNumero(contador % 10);

        GPIOC->ODR |= (1U << 10);
        break;
}

digitoActual++;

if (digitoActual >= 4)
{
    digitoActual = 0;
}   
}
}

void numero1()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=   (1U<<6)|
                    (1U<<7); //Enciende los segmentos B y C
}

void numero2()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=    (1U<<12)|
                    (1U<<6)|
                    (1U<<13)|
                    (1U<<10)|
                    (1U<<5); //Enciende los segmentos A, B, D, E y G
}

void numero3()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=    (1U<<12)|
                    (1U<<6)|
                    (1U<<7)|
                    (1U<<13)|
                    (1U<<5); //Enciende los segmentos A, B, C, D y G
}

void numero4()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=    (1U<<6)|
                    (1U<<7)|
                    (1U<<5)|
                    (1U<<9); //Enciende los segmentos B, C, F y G

}

void numero5()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB ->ODR |=    (1U<<12)|
                    (1U<<7)|
                    (1U<<9)|
                    (1U<<5)|
                    (1U<<13); //Enciende los segmentos A, C, D, F y G
}

void numero6()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=    (1U<<12)|
                    (1U<<13)|
                    (1U<<10)|
                    (1U<<7)|
                    (1U<<9)|
                    (1U<<5); //Enciende los segmentos A,C, D, E, F y G
}

void numero7()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=    (1U<<12)|
                    (1U<<6)|
                    (1U<<7); //Enciende los segmentos A, B y C
}

void numero8()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |= MASCARA_SEGMENTOS; //Enciende todos los segmentos
}

void numero9()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=    (1U<<12)|
                    (1U<<6)|
                    (1U<<7)|
                    (1U<<9)|
                    (1U<<5); //Enciende los segmentos A, B, C, F y G
}

void numero0 ()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=    (1U<<12)|
                    (1U<<6)|
                    (1U<<7)|
                    (1U<<13)|
                    (1U<<10)|
                    (1U<<9); //Enciende los segmentos A, B, C, D, E y F

}

void letraA()
{
    GPIOB->ODR &= ~MASCARA_SEGMENTOS; // Apaga todos los segmentos

    GPIOB->ODR |=   (1U<<12)|
                    (1U<<6)|
                    (1U<<7)|
                    (1U<<9)|
                    (1U<<10)|
                    (1U<<5); //Enciende A, B, C, E, F, G
}

void letraP(){

    GPIOB->ODR &= ~MASCARA_SEGMENTOS; 

    GPIOB->ODR |=   (1U<<12)|
                    (1U<<6)|
                    (1U<<9)|
                    (1U<<10)|
                    (1U<<5); //Enciende A, B, E, F, G

}

void mostrarNumero(uint8_t numero)
{
    switch (numero)
    {
        case 0:
            numero0();
            break;
        case 1:
            numero1();
            break;
        case 2:
            numero2();
            break;
        case 3:
            numero3();
            break;
        case 4:
            numero4();
            break;
        case 5:
            numero5();
            break;
        case 6:
            numero6();
            break;
        case 7:
            numero7();
            break;
        case 8:
            numero8();
            break;
        case 9:
            numero9();
            break;
        default:
            // Manejo de error si el número no es válido
            break;
    }
}

//LED BLINKY COPIAR Y PEGAR EN FUTUROS CODIGOS TODAS LAS FUNCIONES QUE SIGAN DESPUES DE ESTA LINEA
//RECORDATORIO DOBLE PORQUE NO SÉ LEER Y LUEGO VOY A ESTAR BUSCANDO ESTO EN EL FUTURO Y NO LO VOY A ENCONTRAR
void configurarTIM11(void)
{
    // Habilitar reloj de TIM11 (APB2)
    RCC->APB2ENR |= (1U << 18);

    // Detener el temporizador
    TIM11->CR1 = 0;

    // Configurar interrupción periódica
    TIM11->PSC = 15999;
    TIM11->ARR = 499;

    // Reiniciar contador
    TIM11->CNT = 0;

    // Limpiar bandera de actualización
    TIM11->SR = 0;

    // Habilitar interrupción de actualización
    TIM11->DIER |= TIM_DIER_UIE;

    // TIM2 tiene prioridad superior
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 2);

    // Habilitar interrupción de TIM11
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

    // Iniciar temporizador
    TIM11->CR1 |= TIM_CR1_CEN;
}

void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
    if (TIM11->SR & TIM_SR_UIF)
    {
        TIM11->SR &= ~TIM_SR_UIF;

        GPIOH->ODR ^= GPIO_ODR_OD1;
    }
}

void configurarGPIO_Blinky(void)
{
    // Activar reloj de GPIOH
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;

    // Configurar PH1 como salida
    GPIOH->MODER &= ~GPIO_MODER_MODE1;
    GPIOH->MODER |= GPIO_MODER_MODE1_0;

    // Salida push-pull
    GPIOH->OTYPER &= ~GPIO_OTYPER_OT1;

    // Sin pull-up ni pull-down
    GPIOH->PUPDR &= ~GPIO_PUPDR_PUPD1;
}

//POR FAVOR NO PONGAS MÁS FUNCIONES ABAJO PERO SI ERES TARADO, RECUERDA SOLO LLEVARTE HASTA AQUÍ
//RECORDATORIO DOBLE PQ ME CONOZCO LOL

//INCLUYE UNA FUNCIÓN DONDE SE ACTIVA EXTI 1 Y EXTI 4 AL MISMO TIEMPO, PRUEBALA :D
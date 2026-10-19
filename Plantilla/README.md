# Plantilla STM32F411CEU6 Black Pill

Plantilla bare-metal para la STM32F411CEU6. El ejemplo de `Src/main.c` hace parpadear el LED conectado a PC13 usando HSI a 16 MHz y SysTick.

## Herramientas

En Ubuntu/Debian:

```sh
sudo apt install cmake make gcc-arm-none-eabi binutils-arm-none-eabi openocd
```

Para depurar desde VS Code instala las extensiones CMake Tools y Cortex-Debug. Asegúrate de que `cmake`, `make`, `arm-none-eabi-gcc` y `openocd` estén disponibles en el `PATH`. El preset usa `Unix Makefiles`, por lo que esta configuración está preparada para Linux.

## Compilar

Desde la carpeta del proyecto:

```sh
cmake --preset Debug
cmake --build --preset Debug
```

Se generan `build/Debug-make/firmware.elf`, `firmware.hex`, `firmware.bin` y `firmware.map`. En VS Code, ejecuta la tarea **Compilar** o inicia **Depurar Black Pill** con F5.

## Programar con ST-Link

Es el método recomendado para desarrollo: funciona con el microcontrolador en blanco, permite depurar y no requiere entrar al bootloader. Ejecuta la tarea **Compilar y subir** o utiliza F5 para cargar y depurar.

Conecta el ST-Link a la placa:

| ST-Link | STM32F411CEU6 |
| --- | --- |
| SWDIO | SWDIO / PA13 |
| SWCLK | SWCLK / PA14 |
| GND | GND |
| VTref / 3V3 | 3V3 de la placa, solo como referencia si la placa ya tiene alimentación |
| NRST (opcional) | NRST |

No conectes 5 V a los pines de señal de la placa. Mantén BOOT0 en nivel bajo para el arranque normal desde Flash.

## USB de la placa

El conector USB-C (o Micro-USB, según la revisión de la Black Pill) no programa por sí mismo el firmware. Puede usarse para alimentación y para el USB del programa; para cargar por USB hace falta que el micro tenga un bootloader DFU compatible y poner la placa en modo bootloader. La disponibilidad y el procedimiento dependen de la revisión de la placa y del bootloader del STM32F411. En ese caso se necesita además la herramienta DFU correspondiente; este proyecto no configura ese flujo.

Para esta plantilla y para depurar, usa ST-Link. USB DFU es una alternativa cuando ya confirmaste que el bootloader de tu variante lo soporta y no necesitas depuración SWD.
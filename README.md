# odometry-module
High-level system monitoring and odometry module running in FreeRTOS on the NUCLEO-F767ZI STM32F767ZIT6 (part of my cbr-hacks).

READTHIS:
  * http://lwip.wikia.com/wiki/Maximizing_throughput

TODO:
  * Document the toolchain in use (currently gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2)
  * Figure out what I want the FreeRTOS configASSERT() macro to do
  * Migrate to use the MPU (memory protection unit) FreeRTOS API/implementation
  * Migrate FreeRTOS API usages to use the static allocation scheme
  * Migrate the vendor packages to a git repo and make them a submodule of this repo
  * Handle error bits in the IRQ receivers (like in UART drivers)
  * Do I need some sort of mutual exclusion around the UART1/debug output if used in multiple tasks (or just queue up the tx data)?
  * Write my own `src/modules/ethernetif.c` implementation

#include <stdint.h>
#include "minemu/user_abi.h"

int32_t minemu_hello_uart_putchar(uint32_t byte);

void minemu_user_main(void) {
    static const char message[] = "hello world\n";
    
    (void)ioctl
    (
        MINEMU_FD_UART_OUT, MINEMU_IOCTL_UART_WRITE,
        (void *)message, sizeof(message) - 1U
    );

    for (;;) {
        __asm__ volatile("nop");
    }
}
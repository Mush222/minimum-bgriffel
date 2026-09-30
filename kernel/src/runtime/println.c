#include "minemu/platform.h"
#include "minemu/trace.h"

void println(const char message[])
{
    for (size_t index = 0; message[index] != '\0'; ++index) {
        MINEMU_UART0->tx_data = (uint8_t)message[index];
    }
    MINEMU_UART0->tx_data = '\n';
}
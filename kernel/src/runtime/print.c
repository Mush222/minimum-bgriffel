#include "minemu/platform.h"
#include "minemu/trace.h"

void print(const char message[])
{
    for (size_t index = 0; message[index] != '\0'; ++index) {
        MINEMU_UART0->tx_data = (uint8_t)message[index];
    }
}
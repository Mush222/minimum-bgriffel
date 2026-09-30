#include "minemu/platform.h"
#include "minemu/trace.h"

char readChar()
{
    return MINEMU_UART0->rx_data;
    //QUESTION: Should I have this function do the interupts since writing in the console should automatically send data to read via interpts,
    // or should it just be MIMENU_UART0->rx_data?
}
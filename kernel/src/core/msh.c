#include <stdint.h>
#include "minemu/boot.h"
#include "minemu/platform.h"
#include "minemu/trap.h"
#include "minemu/irq.h"
#include "minemu/println.h"
#include "minemu/print.h"
#include "minemu/readChar.h"
#include "minemu/trace.h"

static volatile uint32_t interrupt_count;

void minemu_irq_trampoline(void) __attribute__((noreturn));

char answer[20];
char c = 'c';
size_t index = 0;
char messyDataArray[100];
size_t messyIndex = 0;


char isNewCommand = 'y';
char pressedEnter = 'n';
//Checks if theres a new command so msh> should be printed again. Can be y or n

struct minemu_trap_frame *minemu_irq_dispatch(struct minemu_trap_frame *frame) {
    uint32_t source = (uint32_t)frame->exception_id;
    ++interrupt_count;
    if (source == MINEMU_IRQ_SYSTICK) 
    {
        MINEMU_SYSTICK->ack = MINEMU_SYSTICK_ACK;
    } 
    else if (source == MINEMU_IRQ_UART0) 
    {
        char c = readChar();
        if(c == '\r' || c == '\n')
        {
            pressedEnter = 'y';
        }
        else if(messyIndex < sizeof(messyDataArray) - 1)
        {
            messyDataArray[messyIndex++] = c;
            messyDataArray[messyIndex] = '\0';
        }
        
        //CONTINUE HERE, READ ONE CHAR AT A TIME AND STORE IT IN ARRAY
    } else if (source == MINEMU_IRQ_UART1) 
    {
        (void)MINEMU_UART1->rx_data;
    } else if (source == MINEMU_IRQ_BLOCK) 
    {
        MINEMU_BLOCK->ack = MINEMU_BLOCK_ACK;
    }
    MINEMU_INTERRUPT->eoi = source;
    return frame;
}

void minemu_kernel_main(const struct minemu_boot_info *boot_info) 
{
    (void)boot_info;
    MINEMU_INTERRUPT->enable =(UINT32_C(1) << MINEMU_IRQ_UART0);
    MINEMU_UART0->control = MINEMU_UART_CONTROL_RX_IRQ_ENABLE;

     __asm__ volatile("cpsie i" : : : "memory");
    for (;;) 
    {
        if(isNewCommand == 'y')
        {
            print("msh> ");
            isNewCommand = 'n';
        }
        
        
        if(pressedEnter == 'y')
        {
            pressedEnter = 'n';
            char cleanDataArray[30] = {0}; 
            size_t cleanIndex = 0;
            
            for(size_t i = 0; i < sizeof(messyDataArray) && messyDataArray[i] != '\0'; i++)
            {
                if(messyDataArray[i] == '\b' || messyDataArray[i] == 127)
                {
                    if(cleanIndex > 0){cleanIndex--;}
                }
                else if (messyDataArray[i] != '\r' && messyDataArray[i] != '\n' && messyDataArray[i] != '\0')
                {
                    cleanDataArray[cleanIndex++] = messyDataArray[i];
                }
            }
            
            cleanDataArray[cleanIndex] = '\0';

            if((cleanDataArray[0] == 'e' && cleanDataArray[1] == 'c' && cleanDataArray[2] == 'h' && cleanDataArray[3] == 'o' && cleanDataArray[4] == ' ') || (cleanDataArray[0] == 'e' && cleanDataArray[1] == 'c' && cleanDataArray[2] == 'h' && cleanDataArray[3] == 'o' && cleanDataArray[4] == '\0'))
            {
                println(&cleanDataArray[5]);
            }
            else if (cleanIndex > 0)
            {
                print("command not found: ");
                println(cleanDataArray);
            }

            for (size_t i = 0; i < sizeof(messyDataArray); i++)
            {
                messyDataArray[i] = '\0';
            }

            messyIndex = 0;
            isNewCommand = 'y';
        }


        __asm__ volatile("nop");
    }
    //TURN THIS INTO LOOP AND CONTINUE WHAT OTHER COMMENT SAID

   

}
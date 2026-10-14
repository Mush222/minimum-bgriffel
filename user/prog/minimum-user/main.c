//Bradley Griffel
//I pledge my honor that I have abided by the Stevens Honor System.
#include <stdint.h>
#include <stdbool.h>
#include "minemu/user_abi.h"

bool overlong = false;

static void write_text(char *text, uint32_t length)
{
    (void)ioctl(
        MINEMU_FD_UART_OUT,
        MINEMU_IOCTL_UART_WRITE,
        (void *)text,
        length
    );
}

void minemu_user_main(void)
{
    static char prompt[] = "msh> ";

    char c;
    char line[26];
    uint32_t length = 0;

    write_text(prompt, 5);

    for (;;)
    {
        int32_t result = ioctl(
            MINEMU_FD_UART_IN,
            MINEMU_IOCTL_UART_READ,
            (void *)&c,
            1U
        );

        if(result == 1)
        {
            if(c == '\n')
            {
                if(overlong)
                {
                    overlong = false;
                    length = 0;
                    write_text(prompt, 5);
                    continue;
                }
                
                line[length] = '\0';

                uint32_t start = 0;

                // Ignore spaces before the command
                while(start < length && line[start] == ' ')
                {
                    start++;
                }

                // Empty or all-space command
                if(start == length)
                {
                    length = 0;
                    write_text(prompt, 5);
                    continue;
                }

                // Check for "echo"
                if(start + 4 <= length &&
                line[start] == 'e' &&
                line[start + 1] == 'c' &&
                line[start + 2] == 'h' &&
                line[start + 3] == 'o' &&
                (start + 4 == length || line[start + 4] == ' '))
                {
                    uint32_t textStart = start + 4;

                    // Skip spaces immediately after echo
                    while(textStart < length && line[textStart] == ' ')
                    {
                        textStart++;
                    }

                    char output[26];
                    uint32_t outputLength = 0;

                    for(uint32_t i = textStart; i < length; i++)
                    {
                        if(line[i] == ' ')
                        {
                            // Only keep one space between words
                            if(outputLength > 0 &&
                            output[outputLength - 1] != ' ')
                            {
                                output[outputLength++] = ' ';
                            }
                        }
                        else
                        {
                            output[outputLength++] = line[i];
                        }
                    }

                    // Remove trailing space
                    if(outputLength > 0 &&
                    output[outputLength - 1] == ' ')
                    {
                        outputLength--;
                    }

                    if(outputLength > 0)
                    {
                        write_text(output, outputLength);
                    }

                    static char newline[] = "\n";
                    write_text(newline, 1);
                }
                else
                {
                    static char error[] = "command not found: ";
                    write_text(error, sizeof(error) - 1U);

                    // Print only the first word
                    uint32_t commandEnd = start;

                    while(commandEnd < length &&
                        line[commandEnd] != ' ')
                    {
                        commandEnd++;
                    }

                    write_text(&line[start], commandEnd - start);

                    static char newline[] = "\n";
                    write_text(newline, 1);
                }

                length = 0;
                write_text(prompt, 5);
            }
            else if(c == '\b' || c == 127)
            {
                if(length > 0)
                {
                    length--;
                }
            }
            else if(!overlong)
            {
                if(length < 25)
                {
                    line[length] = c;
                    length++;
                }
                else
                {
                    overlong = true;
                }
            }
        }
    }
}
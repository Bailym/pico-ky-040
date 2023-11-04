#include "ky-040.h"
#include <stdio.h>

int main()
{
    stdio_init_all();
    Encoder_Init();

    while(1)
    {
        tEncoderDirection direction = GetAveragedEncoderDirection();
        printf("Encoder direction: %d\n", direction); 
        sleep_ms(10);
    }
    
    return 0;
}
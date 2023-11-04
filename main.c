#include "ky-040.h"
#include <stdio.h>

// EXAMPLE CODE USING THE LIBRARY

void encoderPulsedCallback(tEncoderDirection direction)
{
    if(direction == ENCODER_DIRECTION_CLOCKWISE)
    {
        printf("Clockwise\n"); 
    }
    else if(direction == ENCODER_DIRECTION_COUNTERCLOCKWISE)
    {
        printf("Counter Clockwise\n"); 
    }
    else
    {
        printf("Stopped\n");
    }
}

void encoderSwitchPressedCallback(void)
{
    printf("Switch Pressed\n");
}

int main()
{
    stdio_init_all();
    Encoder_Init();

    SetEncoderPulsedCallback(encoderPulsedCallback);
    SetEncoderSwitchPressedCallback(encoderSwitchPressedCallback);

    while(1)
    {
        // Do nothing - Registered callbacks will be called when encoder is pulsed or switch is pressed.
    }

    return 0;
}
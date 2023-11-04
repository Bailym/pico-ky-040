#include "ky-040.h"
#include <stdio.h>

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

    }

    return 0;
}
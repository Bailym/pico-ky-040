#ifndef KY_040_H
#define KY_040_H

#include "pico/stdlib.h"

typedef enum
{
    ENCODER_DIRECTION_CLOCKWISE = 1,
    ENCODER_DIRECTION_STOPPED = 0,
    ENCODER_DIRECTION_COUNTERCLOCKWISE = -1
} tEncoderDirection;

void Encoder_Init(void);
bool Encoder_TaskInterruptWrapper(repeating_timer_t *rt);
bool PrintEncoderPositionInterruptWrapper(repeating_timer_t *rt);
tEncoderDirection GetAveragedEncoderDirection(void);
bool GetEncoderSwitchPressed(void);

#endif // KY_040_H
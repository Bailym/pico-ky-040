#include "ky-040.h"
#include "hardware/timer.h"
#include <stdio.h>

#define ENCODER_READ_INTERVAL_MS 10
#define ENCODER_PINS_DEBOUNCE_READS 3
#define ENCODER_SWITCH_DEBOUNCE_TIME_MS 500
#define NUM_ENCODER_AVERAGE_DIRECTION_READINGS 10

static repeating_timer_t encoderTaskTimer;

typedef enum
{
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} tGpioPinState;

typedef enum
{
    KY_040_PIN_A = 6, // Pico Pin GPIO6 -> CLK on KY-040
    KY_040_PIN_B = 7, // Pico Pin GPIO7 -> DT on KY-040
    KY_040_PIN_SW = 8 // Pico Pin GPIO8 -> Marked SW on KY-040
} tKy040Pin;

static tGpioPinState lastStateA;
static tGpioPinState lastStateB;
static tGpioPinState lastStateSW;
static int encoderPosition = 0;
static tEncoderDirection averagedDirection = ENCODER_DIRECTION_CLOCKWISE;
static bool encoderSwitchPressed = false;
static tEncoderDirection previousDirectionReadings[NUM_ENCODER_AVERAGE_DIRECTION_READINGS];
static int currentDirectionReadingsPositionToWrite = 0;

static tGpioPinState GetEncoderPinState(tKy040Pin pin)
{
    return (tGpioPinState)gpio_get(pin);
}

void Encoder_Init(void)
{
    // Setup GPIOs
    gpio_init(KY_040_PIN_A);
    gpio_init(KY_040_PIN_B);
    gpio_init(KY_040_PIN_SW);
    gpio_set_dir(KY_040_PIN_A, GPIO_IN);
    gpio_set_dir(KY_040_PIN_B, GPIO_IN);
    gpio_set_dir(KY_040_PIN_SW, GPIO_IN);

    // Initial states
    lastStateA = GetEncoderPinState(KY_040_PIN_A);
    lastStateB = GetEncoderPinState(KY_040_PIN_B);
    lastStateSW = GetEncoderPinState(KY_040_PIN_SW);

    // Setup interrupt timer
    add_repeating_timer_ms(ENCODER_READ_INTERVAL_MS, Encoder_TaskInterruptWrapper, NULL, &encoderTaskTimer);
}

static bool CheckForNoisyReadings()
{
    tGpioPinState encoderPinAReadings[ENCODER_PINS_DEBOUNCE_READS];
    tGpioPinState encoderPinBReadings[ENCODER_PINS_DEBOUNCE_READS];

    for (int i = 0; i < ENCODER_PINS_DEBOUNCE_READS; i++)
    {
        encoderPinAReadings[i] = GetEncoderPinState(KY_040_PIN_A);
        encoderPinBReadings[i] = GetEncoderPinState(KY_040_PIN_B);
    }

    for (int i = 1; i < ENCODER_PINS_DEBOUNCE_READS; i++)
    {
        bool encoderPinAHasNoisyReading = encoderPinAReadings[i] != encoderPinAReadings[0];
        bool encoderPinBHasNoisyReading = encoderPinBReadings[i] != encoderPinBReadings[0];

        if (encoderPinAHasNoisyReading || encoderPinBHasNoisyReading)
        {
            return true;
        }
    }

    return false;
}

static void UpdateAverageReadings(tEncoderDirection direction)
{
    previousDirectionReadings[currentDirectionReadingsPositionToWrite] = direction;
    currentDirectionReadingsPositionToWrite = (currentDirectionReadingsPositionToWrite + 1) % NUM_ENCODER_AVERAGE_DIRECTION_READINGS;

    int runningTotalForDirectionReadings = 0;
    for (int i = 0; i < NUM_ENCODER_AVERAGE_DIRECTION_READINGS; i++)
    {
        runningTotalForDirectionReadings += previousDirectionReadings[i];
    }
    if (runningTotalForDirectionReadings > 0)
    {
        averagedDirection = ENCODER_DIRECTION_CLOCKWISE;
    }
    else if (runningTotalForDirectionReadings < 0)
    {
        averagedDirection = ENCODER_DIRECTION_COUNTERCLOCKWISE;
    }
    else
    {
        averagedDirection = ENCODER_DIRECTION_STOPPED;
    }
}

static void Encoder_Task(void)
{
    if (CheckForNoisyReadings())
    {
        return;
    }

    tGpioPinState currentStateA = GetEncoderPinState(KY_040_PIN_A);
    tGpioPinState currentStateB = GetEncoderPinState(KY_040_PIN_B);
    tGpioPinState currentStateSW = GetEncoderPinState(KY_040_PIN_SW);

    bool aChanged = currentStateA != lastStateA;
    bool bChanged = currentStateB != lastStateB;
    tEncoderDirection currentEncoderDirection;

    bool encoderPositionChanged = aChanged || bChanged;

    if (encoderPositionChanged)
    {
        bool aStateChangedFirst = aChanged && !bChanged;
        bool bStateChangedFirst = bChanged && !aChanged;

        if (aStateChangedFirst)
        {
            encoderPosition++;
            currentEncoderDirection = ENCODER_DIRECTION_CLOCKWISE;
        }
        else if (bStateChangedFirst)
        {
            encoderPosition--;
            currentEncoderDirection = ENCODER_DIRECTION_COUNTERCLOCKWISE;
        }
    }
    else
    {
        currentEncoderDirection = ENCODER_DIRECTION_STOPPED;
    }

    UpdateAverageReadings(currentEncoderDirection);

    encoderSwitchPressed = (currentStateSW == GPIO_LOW && lastStateSW == GPIO_HIGH);

    lastStateA = currentStateA;
    lastStateB = currentStateB;
    lastStateSW = currentStateSW;
}

bool Encoder_TaskInterruptWrapper(repeating_timer_t *rt)
{
    Encoder_Task();
    return true;
}

// debug
static void PrintEncoderPosition(void)
{
    printf("Encoder position: %d\n", encoderPosition);
}

bool PrintEncoderPositionInterruptWrapper(repeating_timer_t *rt)
{
    PrintEncoderPosition();
    return true;
}

tEncoderDirection GetAveragedEncoderDirection(void)
{
    return averagedDirection;
}

bool GetEncoderSwitchPressed(void)
{
    return encoderSwitchPressed;
}

#include "ky-040.h"
#include "hardware/timer.h"
#include <time.h>
#include <stdio.h>

#define ENCODER_READ_INTERVAL_MS 10

typedef enum
{
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} tGpioPinState;

typedef enum
{
    KY_040_PIN_A = 6, // Pico Pin GPIO6 -> CLK on KY-040
    KY_040_PIN_B = 7, // Pico Pin GPIO7 -> DT on KY-040
    KY_040_PIN_SW = 8 // Pico Pin GPIO8 -> SW on KY-040
} tKy040Pin;

static repeating_timer_t encoderTaskInterruptTimer;
static tGpioPinState lastStateA;
static tGpioPinState lastStateB;
static tGpioPinState lastStateSW;
static tGpioPinState lastSyncedState; // Last state where both pins were the same
static tGpioPinState nextSyncedState; // Next state where both pins will be the same
static int currentEncoderPosition;
static int encoderPulses; // The encoder pulses once for every 2 positions
static tEncoderDirection lastEncoderDirection;
static tEncoderDirection currentEncoderDirection;
static void (*storedEncoderPulsedCallback)(tEncoderDirection);
static void (*storedEncoderSwitchPressedCallback)(void);
bool getEncoderReadingOnInterrupt = false;

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
    lastSyncedState = GPIO_LOW;
    nextSyncedState = GPIO_HIGH;
    currentEncoderPosition = 0;
    encoderPulses = 0;
    currentEncoderDirection = ENCODER_DIRECTION_STOPPED;
    lastEncoderDirection = ENCODER_DIRECTION_STOPPED;

    add_repeating_timer_ms(ENCODER_READ_INTERVAL_MS, Encoder_GetReadingOnNextInterrupt, NULL, &encoderTaskInterruptTimer);
}

static void UpdateEncoderPositionAndDirection()
{
    if (currentEncoderDirection == ENCODER_DIRECTION_CLOCKWISE)
    {
        currentEncoderPosition++;
        lastEncoderDirection = ENCODER_DIRECTION_CLOCKWISE;
    }
    else if (currentEncoderDirection == ENCODER_DIRECTION_COUNTERCLOCKWISE)
    {
        currentEncoderPosition--;
        lastEncoderDirection = ENCODER_DIRECTION_COUNTERCLOCKWISE;
    }
}

static void UpdateEncoderPulses()
{
    if (lastEncoderDirection == currentEncoderDirection)
    {
        encoderPulses++;
    }
    else
    {
        encoderPulses = 1;
    }

    if (encoderPulses == 2)
    {
        encoderPulses = 0;
        if (storedEncoderPulsedCallback != NULL)
        {
            storedEncoderPulsedCallback(lastEncoderDirection);
        }
    }
}

static void UpdateSyncedState()
{
    lastSyncedState = nextSyncedState;
    nextSyncedState = lastSyncedState == GPIO_LOW ? GPIO_HIGH : GPIO_LOW;
}

static void UpdateEncoderCountAndSyncState()
{
    UpdateEncoderPositionAndDirection();
    UpdateEncoderPulses();
    UpdateSyncedState();
}

void Encoder_Task(void)
{
    if (!getEncoderReadingOnInterrupt)
    {
        return;
    }

    tGpioPinState currentStateA = GetEncoderPinState(KY_040_PIN_A);
    tGpioPinState currentStateB = GetEncoderPinState(KY_040_PIN_B);
    tGpioPinState currentStateSW = GetEncoderPinState(KY_040_PIN_SW);

    bool encoderSwitchPressed = (currentStateSW == GPIO_LOW && lastStateSW == GPIO_HIGH);
    bool encoderMovingClockwise = (currentStateA == nextSyncedState && currentStateB == lastSyncedState);
    bool encoderMovingCounterClockwise = (currentStateB == nextSyncedState && currentStateA == lastSyncedState);
    bool encoderNotMoving = (currentStateA == lastSyncedState && currentStateB == lastSyncedState);
    bool bothPinsHitNextSyncState = (currentStateA == nextSyncedState && currentStateB == nextSyncedState);

    if (encoderMovingClockwise)
    {
        currentEncoderDirection = ENCODER_DIRECTION_CLOCKWISE;
    }

    else if (encoderMovingCounterClockwise)
    {
        currentEncoderDirection = ENCODER_DIRECTION_COUNTERCLOCKWISE;
    }

    else if (encoderNotMoving)
    {
        currentEncoderDirection = ENCODER_DIRECTION_STOPPED;
    }

    else if (bothPinsHitNextSyncState)
    {
        UpdateEncoderCountAndSyncState();
    }

    lastStateA = currentStateA;
    lastStateB = currentStateB;
    lastStateSW = currentStateSW;

    if (encoderSwitchPressed && storedEncoderSwitchPressedCallback != NULL)
    {
        storedEncoderSwitchPressedCallback();
    }
}

bool Encoder_GetReadingOnNextInterrupt(repeating_timer_t *rt)
{
    getEncoderReadingOnInterrupt = true;
}

void SetEncoderPulsedCallback(void (*encoderPulsedCallback)(tEncoderDirection))
{
    storedEncoderPulsedCallback = encoderPulsedCallback;
}

void SetEncoderSwitchPressedCallback(void (*encoderSwitchPressedCallback)(void))
{
    storedEncoderSwitchPressedCallback = encoderSwitchPressedCallback;
}

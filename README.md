# pico-ky-040
A C library to interface with the KY-040 rotary encoder module.

This library reads the state of the pins and determines the encoder position and direction.

The position and direction are used to detect encoder pulses, which happen with each "click" of the encoder.

# Features

- Assignable callbacks to run on encoder switch presses.
- Assignable callbacks to run each encoder pulse (click). 
- The encoder is read on a repeating timer interrupt (10ms by default).

# Setup

The default GPIO pin setup is:

Pico Pin GPIO6 -> CLK on KY-040

Pico Pin GPIO7 -> DT on KY-040

Pico Pin GPIO8 -> SW on KY-040

Pico Pin GND -> GND on KY-040

Pico Pin 3V3(OUT) -> + on KY-040

[Data sheet for KY-040](https://www.handsontec.com/dataspecs/module/Rotary%20Encoder.pdf) 

[Pinout for Pi Pico W](https://datasheets.raspberrypi.com/picow/PicoW-A4-Pinout.pdf) 

# Images

I set up the callbacks to print messages to the USB serial - This is the result.

![USB serial out](https://github.com/Bailym/pico-ky-040/blob/main/images/USB%20serial%20out.png?raw=true)

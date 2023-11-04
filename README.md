# pico-ky-040
A C library to interface with the KY-040 rotary encoder module.

This library reads the state of the pins and determines the encoder position and direction.

The position and direction are used to detect encoder pulses, which happen with each "click" of the encoder.

# Features

 - Asssignable callbacks to run on encoder switch presses.
 - Assignable callbacks to run each encoder pulse (click). 
 - The encoder is read on a repeating timer interrupt (10ms by default).

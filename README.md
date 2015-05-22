# LapTimerOSD
MinimOSD based Lap Timer for racing quadcopters

Two pins from the atmega328p need to be soldered to be used by IR Receiver and IR Emitter
http://www.hobbytronics.co.uk/image/data/tutorial/arduino-hardcore/atmega328-tqfp-arduino-pinout.jpg
Pin d9 is connected to the IR Sensor and D5 is connected to the emitter
emitter is needed only if accompanied by the Finishline recorder project

Once the IR Sensor is connected the OSD shows Batt 1 voltage on screen and whenever the quad passes over the IR Emitter at the Start/Finish line it records and displays current lap, previous lap and best lap on screen.

For Finish line IR Emitter a standard IR Remote can be used or the IR Continious project can be used accompanied by some high power IR LEDs

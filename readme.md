

Test setup using a capacitive touch sensor;

- CAP1188 sensor
    - https://learn.adafruit.com/adafruit-cap1188-breakout/python-circuitpython
    - Works for pot plants and small bushes sometimes
    - Can be driven by either a raspberry pi or an arduino.
    - Purchased here: https://core-electronics.com.au/cap1188-8-key-capacitive-touch-sensor-breakout-i2c-or-spi.html
        - Chosen since it was in stock, there are other alternatives. 
        - We can chain $i^2c$ devices so can connect many sensors to a single microcontroller.
- And WS12813 addressable RGB LED strip, can be had from kmart, bunnings etc, 
    with the controller desoldered but you have ot check the pins in the actual 
    strip. Can also buy at electronics stores but seems more expensive maybe. 
    This model needs to be driven with 12vs with 5v logic, but there are options 
    that only need 5v and 3.3v logic 
    - https://randomnerdtutorials.com/guide-for-ws2812b-addressable-rgb-led-strip-with-arduino/
    - https://www.sdiplight.com/what-is-ws2813-led-and-how-to-use-ws2813/
    - There is a limit on how many LEDs we can drive from a single board, but it 
      is proportional to the update rate somehow. They need to use an external,
      power source the limit is on the communication protocol the WS12813 chip uses. 
    - A raspberry pi _may_ be able to drive some type of LEDs but it is more iffy since it
      does not run a real-time operating system, it cannot drive _these_ leds since 
      it uses 3.3v logic and these LEDs need 5v logic.

    

An arduino is used for sensing and controlling lights and connects to a "real computer" to play audio
(since audio needs more processin gpower and we need to play multiple sounds simultaneously)
- This uses the FastLED library, and is also 
- in theory this real computer can be a mini-pc or raspberry pi
- The arduino interfaces with the pc over serial using
    - https://github.com/hideakitai/MsgPacketizer

Libraries:

```
arduino-cli compile --fqbn arduino:avr:nano lights
Sketch uses 11666 bytes (37%) of program storage space. Maximum is 30720 bytes.
Global variables use 886 bytes (43%) of dynamic memory, leaving 1162 bytes for local variables. Maximum is 2048 bytes.

Used library             Version Path                                                                        
Wire                     1.0     .../.arduino15/packages/arduino/hardware/avr/1.8.6/libraries/Wire
SPI                      1.0     .../.arduino15/packages/arduino/hardware/avr/1.8.6/libraries/SPI 
Adafruit CAP1188 Library 1.1.0   .../Arduino/libraries/Adafruit_CAP1188_Library                   
Adafruit BusIO           1.14.4  .../Arduino/libraries/Adafruit_BusIO                             
MsgPacketizer            0.4.7   .../Arduino/libraries/MsgPacketizer                              
FastLED                  3.6.0   .../Arduino/libraries/FastLED                                    

Used platform Version Path                                                         
arduino:avr   1.8.6   .../.arduino15/packages/arduino/hardware/avr/1.8.6
```


- The PC software is written using the openframeworks C++ library. 
    - https://openframeworks.cc/
    - In theory it should compile on any platform supporting openframeworks.
    - The project needs to be imported after installing OFx (untested)
    - Uses the openframeworks MsgPacketizer library which is distributed as a zipfile
      in its git repo (under `extra`) which needs to be imported.


---

Other sensing methods

If integrated capacitive touch ICs don't work it may be possible to use other 
systems for sensing that allow more manual calibration

There are also methods which allow less binary input (ie. detecting different 
types/amounts of touch), such as swept-frequency capacitive sensing "Touche"
    - https://www.nime.org/proceedings/2014/nime2014_515.pdf
    - https://fablab.ruc.dk/sensing-touch-with-arduino/
    - https://github.com/damellis/ESP/wiki/%5BExample%5D-Touch%C3%A9-swept-frequency-capacitive-sensing

- https://medium.com/@narner/talking-to-plants-touch%C3%A9-experiments-1087e1f04eb1
- capsense library
    - https://github.com/PaulStoffregen/CapacitiveSensor
    - seems surprisingly easy?

- A 555 touch sensing circuit

- It may be possible to calibrate the PlantMusic circuit to work as a switch 
  as well, e.g. increasing the threshold. 

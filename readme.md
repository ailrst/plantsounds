

Test setup using a capacitive touch sensor;

- CAP1188 sensor
    - https://learn.adafruit.com/adafruit-cap1188-breakout/python-circuitpython
    - Works for pot plants and small bushes sometimes
- And WS12813 addressable RGB LED strip, can be had from kmart, bunnings etc, 
    with the controller desoldered. Can also buy elsewhere. 
    This model needs to be driven with 12vs with 5v logic, but there are options 
    that only need 5v. 
    - https://randomnerdtutorials.com/guide-for-ws2812b-addressable-rgb-led-strip-with-arduino/
    - ttps://www.sdiplight.com/what-is-ws2813-led-and-how-to-use-ws2813/
    

An arduino is used for sensing and connects to a "real computer" to play audio
(since audio needs more processin gpower and we need to play multiple sounds simultaneously)
- in theory this real computer can be a mini-pc or raspberry pi
- The arduino interfaces with the pc over serial using
    - https://github.com/hideakitai/MsgPacketizer
- The PC software is written using the openframeworks C++ library. 
    - https://openframeworks.cc/
    - In theory it should compile on any platform supporting openframeworks.




Wiring Setup







beans: lights
	cp ../model.hpp lights/model.hpp
	arduino-cli compile --fqbn arduino:avr:nano lights
	arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano lights

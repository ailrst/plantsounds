
beans2: lights2
	arduino-cli compile --fqbn arduino:avr:nano lights2
	arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano lights2

beans: lights
	cp ../model.hpp lights/model.hpp
	arduino-cli compile --fqbn arduino:avr:nano lights
	arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano lights

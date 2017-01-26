# pi_nrf24l01_rx_mqtt
worked example of a pi that received  signals from an nrf24 network and publishes the data to mqtt.
The nrf24 libray come from http://tmrh20.github.io/RF24. Download this repository and install it on your raspberry.
You put the code (pi_nrf24_rx.cpp) into the linux example folder you will find in the rf24 library, modify the make file to just compile this file.
The key config changes you will make is the addressing. The code is set up to listern for 3 devices. You find the sender code for these devices in the arduino_nrf24_dht21 and the STM32-nRF24L01_dht21_TX repositories on this site.
You will need to install the mosquitto client on your raspberry as well as the publish is a simple shell execute command.

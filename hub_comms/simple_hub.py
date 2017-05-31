import RPi.GPIO as GPIO
from lib_nrf24 import NRF24
import time
import spidev

GPIO.setmode(GPIO.BCM)

# Addresses
pipes = [[0xE8, 0xE8, 0xF0, 0xF0, 0xE1], [0xF0, 0xF0, 0xF0, 0xF0, 0xE1]]

# Create radio object
radio = NRF24 (GPIO, spidev.SpiDev())

# (ce0 gpio8 csn pin, ce gpio17)
radio.begin(0, 17)

radio.setPayloadSize(32) # max 32
radio.setChannel(0x76)
radio.setDataRate(NRF24.BR_1MBPS)
radio.setPALevel(NRF24.PA_MIN)

radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.enableAckPayload()

radio.openReadingPipe(1,pipes[1])
radio.printDetails()
radio.startListening()

while True:

	available = radio.available()

	while not available:
		time.sleep(1.0/100.0)
		

	print(available)		
	recievedMessage = []
	radio.read(recievedMessage, radio.getDynamicPayloadSize())

	print("Recieved: {}".format(recievedMessage))

	msg = ""
	for c in recievedMessage:
		if (c >= 32) and (n <= 126):
			msg += chr(c)
		
	print("Message decodes to: {}".format(msg))

	

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

radio.setPayloadSize(32)
radio.setChannel(0x76)
radio.setDataRate(NRF24.BR_1MBPS)
radio.setPALevel(NRF24.PA_MIN)

radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.enableAckPayload()

radio.openWritingPipe(1,pipes[0])
radio.openReadingPipe(1,pipes[1])
radio.printDetails()
# radio.startListening()

cmd = list("COMMAND")
while len(cmd < 32):
	cmd.append(0) # Pad out to 32 bytes with \0

while True:

	start = time.time()
	radio.write(message)
	print("Sent: {}".format(cmd))
	radio.startListening()	
		
	while not radio.available(0):
		time.sleep(1.0/100.0)
		if time.time() - start > 1:
			print("Timeout")
			break # Make it send cmd again instead?
			
	recievedMessage = []
	radio.read(recievedMessage, radio.getDynamicPayloadSize())
	
	print("Pre-processing: {}".format(recievedMessage))
	
	msg = ""
	for c in recievedMessage:
		if c >= 32 and n <= 126:
			msg += chr(c)
			
	print("Post-processing: {}".format(msg))
	
	radio.stopListening # Stop listening to allow next cmd to be sent
	time.sleep(1)
	
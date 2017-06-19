demo = True

import sys
import bluetooth
import subprocess
import select
from copy import deepcopy

def recv(sock, length):
    data = ''
    while len(data) < length:
        packet = sock.recv(length - len(data))
        if not packet:
            return None
        data += packet
    return data

if sys.version < '3':
    input = raw_input

if len(sys.argv) < 2:
    print("usage: bt.py <address_filename>")
    sys.exit(2)

address_filename = sys.argv[1]

address_file = open(address_filename, 'r')
addresses = address_file.readlines()
address_file.close()

port = 0x1001
addr_socket={}
board_id = 0
addr_id = {}

for addr in addresses:
	a = addr.rstrip('\n\r')
	print("Trying to connect to %s on port 0x%X" % (a, port))
	addr_socket[a]=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
	addr_socket[a].connect((a, port))
	addr_id[a] = board_id
	print("Connected.")
		
	port += 1
	board_id += 1
		
socket_addr = dict((reversed(item) for item in addr_socket.items()))
socket_list = addr_socket.values()
while True:
	read_sockets, write_sockets, error_sockets = select.select(socket_list , [] , [])

	for s in read_sockets:
			# Get message length
			raw_length = recv(s, 4)
			length = int(raw_length)
			
			# Read that many chars
			data = recv(s, length)
			if data[0] == '0':
				rods = data[1:]
				s.send("ACK:" + rods + '\n\r')
			
				if demo:
					rods = rods[0:-1]
					rods += ',[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0]]'

					
sock.close()


#00:14:03:06:20:F9

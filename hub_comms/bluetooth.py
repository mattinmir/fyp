# file: l2capclient.py
# desc: Demo L2CAP client for bluetooth module.
# $Id: l2capclient.py 524 2007-08-15 04:04:52Z albert $

import sys
import bluetooth
import subprocess

if sys.version < '3':
    input = raw_input



if len(sys.argv) < 2:
    print("usage: l2capclient.py <addr>")
    sys.exit(2)


bt_addr=sys.argv[1]
port = 0x1001
#passkey = "1234"

#subprocess.call("kill `pidof bluetooth-agent`",shell=True)
#status = subprocess.call("bluetooth-agent " + passkey + " &",shell=True)

print("trying to connect to %s on PSM 0x%X" % (bt_addr, port))

sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((bt_addr, port))

print("connected.  type stuff")
while True:
    data = input()
    if(len(data) == 0): break
    sock.send(data)
    data = sock.recv(1024)
    print("Data received:", str(data))

sock.close()


#00:14:03:06:20:F9

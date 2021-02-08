# Client program

from socket import *

# Set the socket parameters
host = "192.168.50.26"
port = 20002
buf = 1024
addr = (host,port)

# Create socket
UDPSock = socket(AF_INET,SOCK_DGRAM)

def_msg = "===Enter message to send to server===";
print "\n",def_msg

# Send messages
while (1):
	data = raw_input('>> ')
	if not data:
		break
	else:
		if(UDPSock.sendto(data,addr)):
			print "Sending message '",data,"'....."

# Close socket
UDPSock.close()

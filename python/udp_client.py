import socket

addr=('192.168.4.3', 1)
s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

while True:
    msg=input()
    s.sendto(msg.encode('utf-8'),addr)
    if msg=='bye':
        break
s.close()
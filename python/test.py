import socket
import threading
import json
import struct

addr = '192.168.92.131'
dico = {}
port = 50001
dico['d2c_port'] = 54321
dico['controller_type'] = 'PC'
dico['controller_name'] = 'bebop shell'
jsonReq = json.dumps(dico, separators=(',', ':'))
print("jsonReq-------")
print(jsonReq)
print("jsonReq-------")
print(addr)
print(port)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.connect((addr, port))
sock.send(jsonReq.encode())
sock.close()
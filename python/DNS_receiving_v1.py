#from zeroconf import ServiceBrowser, Zeroconf, DNSOutgoing, ServiceInfo, DNSPointer, DNSText, DNSService,  DNSAddress
import socket
import threading
import json
import struct

# Some DNS constants

_MDNS_ADDR = '224.0.0.251'
_MDNS_PORT = 5353
_DNS_PORT = 53
_DNS_TTL = 60 * 60  # one hour default TTL

_MAX_MSG_TYPICAL = 1460  # unused
_MAX_MSG_ABSOLUTE = 8966

_FLAGS_QR_MASK = 0x8000  # query response mask
_FLAGS_QR_QUERY = 0x0000  # query
_FLAGS_QR_RESPONSE = 0x8000  # response

_FLAGS_AA = 0x0400  # Authoritative answer
_FLAGS_TC = 0x0200  # Truncated
_FLAGS_RD = 0x0100  # Recursion desired
_FLAGS_RA = 0x8000  # Recursion available

_FLAGS_Z = 0x0040  # Zero
_FLAGS_AD = 0x0020  # Authentic data
_FLAGS_CD = 0x0010  # Checking disabled

_CLASS_IN = 1
_CLASS_CS = 2
_CLASS_CH = 3
_CLASS_HS = 4
_CLASS_NONE = 254
_CLASS_ANY = 255
_CLASS_MASK = 0x7FFF
_CLASS_UNIQUE = 0x8000

_TYPE_A = 1
_TYPE_NS = 2
_TYPE_MD = 3
_TYPE_MF = 4
_TYPE_CNAME = 5
_TYPE_SOA = 6
_TYPE_MB = 7
_TYPE_MG = 8
_TYPE_MR = 9
_TYPE_NULL = 10
_TYPE_WKS = 11
_TYPE_PTR = 12
_TYPE_HINFO = 13
_TYPE_MINFO = 14
_TYPE_MX = 15
_TYPE_TXT = 16
_TYPE_AAAA = 28
_TYPE_SRV = 33
_TYPE_ANY = 255

# Mapping constants to names

_CLASSES = {_CLASS_IN: "in",
            _CLASS_CS: "cs",
            _CLASS_CH: "ch",
            _CLASS_HS: "hs",
            _CLASS_NONE: "none",
            _CLASS_ANY: "any"}

_TYPES = {_TYPE_A: "a",
          _TYPE_NS: "ns",
          _TYPE_MD: "md",
          _TYPE_MF: "mf",
          _TYPE_CNAME: "cname",
          _TYPE_SOA: "soa",
          _TYPE_MB: "mb",
          _TYPE_MG: "mg",
          _TYPE_MR: "mr",
          _TYPE_NULL: "null",
          _TYPE_WKS: "wks",
          _TYPE_PTR: "ptr",
          _TYPE_HINFO: "hinfo",
          _TYPE_MINFO: "minfo",
          _TYPE_MX: "mx",
          _TYPE_TXT: "txt",
          _TYPE_AAAA: "quada",
          _TYPE_SRV: "srv",
          _TYPE_ANY: "any"}

ACK=1
DATA=2
DATA_LOW_LATENCY=3
DATA_WITH_ACK=4

def udpconn_process():
    with thread_lock:
      # The follow print will occupy too much time
      # beyong the time offset of the TCP settings of the board
      # print ('UDP broadcast end')
      addr = "192.168.42.1"
      port = 54321
      # work as TCP server
      # socket->bind->listen->accept
      # SO_REUSEADDR unbind immediately
      send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      send_sock.settimeout(100.0)#设置超时就删了吧
      #recv_sock.bind(('0.0.0.0', port))

      takeoff_data = b'\x04\x0b\x04\x0b\x00\x00\x00\x01\x00\x01\x00'
      land_data = b'\x04\x0b\x04\x0b\x00\x00\x00\x01\x00\x03\x00'
      
      send_sock.sendto(land_data, (addr, port))#这是让他降落还是起飞啊？

      print(takeoff_data)

      send_sock.close()

def pong_process(snd_data):
    with thread_lock:
      # The follow print will occupy too much time
      # beyong the time offset of the TCP settings of the board
      # print ('UDP broadcast end')
      addr = "192.168.42.1"
      port = 54321
      # work as TCP server
      # socket->bind->listen->accept
      # SO_REUSEADDR unbind immediately
      send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      send_sock.settimeout(100.0)
      #recv_sock.bind(('0.0.0.0', port))

      send_sock.sendto(snd_data, (addr, port))

      send_sock.close()

def udpconn_listener():
    with thread_lock:
      addr = "192.168.42.1"
      port = 54321

      recv_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      recv_sock.settimeout(100.0)
      recv_sock.bind(('0.0.0.0', port))

      alive = True

      while alive:
        sock_data, _ = recv_sock.recvfrom(66000)

        the_data = sock_data
        
        while the_data:
          (type, buf, seq, size) = struct.unpack('<BBBI', the_data[0:7])#我直接不看你了，我直接从无人机C语言读数据
          recv_data = the_data[7:size]
        if buf == 0:
          type = DATA
          buf = 1
          seq = 0
          snd_data = struct.pack('<BBBI', type, buf, seq, len(recv_data) + 7)
          snd_data += recv_data
          pong_process(snd_data)

        the_data = the_data[size:]

        recv_sock.close()

if "__main__" == __name__:
  
  #TCP Process
  #addr = '192.168.42.1'
  addr = '10.181.72.254'
  dico = {}
  port = 44444
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
  jsonRet = sock.recv(4096)
  sock.close()

  retDic, _ = json.JSONDecoder().raw_decode(jsonRet)
  
  print('TCP suc')
  udp_t1 = threading.Thread(target = udpconn_process)
  udp_t2 = threading.Thread(target = udpconn_listener)
  thread_lock = threading.RLock()
  udp_t1.run()
  udp_t2.run()
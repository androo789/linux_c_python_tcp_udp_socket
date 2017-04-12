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

def tcpconn_process():
    with thread_lock:
      # The follow print will occupy too much time
      # beyong the time offset of the TCP settings of the board
      # print ('UDP broadcast end')
      port = 44444
      # work as TCP server
      # socket->bind->listen->accept
      # SO_REUSEADDR unbind immediately
      sock_server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
      sock_server.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
      sock_server.bind(("",port))
      sock_server.listen(1)
      conn,addr = sock_server.accept()
      jsonRec = conn.recv(4096)

      dico = {}
      dico['c2d_port'] = port
      dico['controller_type'] = 'Bebop2'
      dico['controller_name'] = 'Wsy'
      dico['status'] = 0
      jsonReq = json.dumps(dico, separators=(',', ':'))

      conn.send(jsonReq)
      conn.close()
      sock_server.close()

      print 'TCP socket connected!'

def udpconn_process():
    with thread_lock:
      # The follow print will occupy too much time
      # beyong the time offset of the TCP settings of the board
      # print ('UDP broadcast end')
      port = 44444
      # work as TCP server
      # socket->bind->listen->accept
      # SO_REUSEADDR unbind immediately
      recv_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      recv_sock.settimeout(100.0)
      recv_sock.bind(('0.0.0.0', port))
      while 1:
        rec_data = recv_sock.recvfrom(66000)
        print rec_data

      recv_sock.close()


if "__main__" == __name__:
 # PC_Conf = Zeroconf()
  '''
  out = DNSOutgoing(_FLAGS_QR_RESPONSE)
  out.answers = [('name','underwater')]
  '''
 # info = ServiceInfo('_arsdk-090c._udp.local.', 'Bebop2-K008852._arsdk-090c._udp.local.', socket.inet_aton('10.181.54.70'), 44444)
 # info._set_text('"{"devi]...')
 # ttl = _DNS_TTL
  '''
  out = DNSOutgoing(_FLAGS_QR_RESPONSE | _FLAGS_AA)
  out.add_answer_at_time(DNSPointer(info.type, _TYPE_PTR, _CLASS_IN, ttl, info.name), 0)
  out.add_answer_at_time(DNSText(info.name, _TYPE_TXT, _CLASS_IN, ttl, info.text), 0)
  out.add_answer_at_time(DNSService(info.name, _TYPE_SRV, _CLASS_IN, ttl, info.priority, info.weight, info.port, info.server), 0)
  out.add_answer_at_time(DNSAddress(info.server, _TYPE_A, _CLASS_IN,ttl, info.address), 0)
  '''
#  PC_Conf.register_service(info)

  #PC_Conf.send(out)
  tcp_t1 = threading.Thread(target = tcpconn_process)
  udp_t2 = threading.Thread(target = udpconn_process)

  thread_lock = threading.Lock()
  # print threading.current_thread()
#  tcp_t1.run()
  udp_t2.run()

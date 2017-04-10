#!/usr/bin/env python
# -*- coding: utf-8 -*

"""
recv_test.py

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Test the communication parameter of the ISEA modem.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
V0.1 update the broadcast thread
V0.2 update the TCP connect thread
V0.3 update the UI parameter init process

V1.0 upadate the mutli-thread process
"""
import sys
import socket
import time
import threading
from struct import *
import types
import string
import random
import os

# Command of the header message
CMD_SYS_BEEP = 0
CMD_SYS_CONFIG = 1
CMD_SYS_DASND = 2
CMD_POWER_RESET = 3
CMD_SYS_STRSND = 4
CMD_SYS_DISPSTR = 5
CMD_SYS_UDP_START = 6
CMD_SYS_UDP_STOP = 7
CMD_SYS_CALIBRATE = 8
CMD_SYS_SETPARAMS = 9
CMD_SYS_SENDLENGTH = 10
CMD_SYS_GAINPARAMS = 11
CMD_SYS_MODETYPE = 12
CMD_SYS_DECCHNUM = 13
CMD_SYS_SAVEPARA = 14

FSK = 1
QPSK = 2
QAM16 = 3
QAM64 = 4

SETLEN = 4
GUARD = 10


def broadcast_process():
    with proc_lock:
        # init the broadcast address, ID and port
        broad_addr = "192.168.42.255"
        port = 4009
        m_usSvrPort = 6490
        SYSTEM_ID = b'MWU'  # string must convert to bytes!

        m_udpSvrPort = 4900

        # pack the message to be transmitted -> struct in C program
        broad_format = '3sII'
        broad_msg = pack(broad_format, SYSTEM_ID, m_usSvrPort, m_udpSvrPort)

        # send the message by UDP socket
        # socket->sendto
        sock_clinet = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock_clinet.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

        count = 0
        while count < 100:
            sock_clinet.sendto(broad_msg, 0, (broad_addr, port))
            count = count + 1

        print('Process1 : Broad process over!\n')
        sock_clinet.close()


def command_send(conn, header, buffer, length):
    config_format = 'II'
    mod_msg = pack(config_format, length, header)
    conn.send(mod_msg)
    command_format = 'I'
    com_msg = pack(command_format, buffer)
    conn.send(com_msg)


def string_send(conn, header, strs, length):
    config_format = 'II'
    mod_msg = pack(config_format, length, header)
    conn.send(mod_msg)
    conn.send(strs)


def base_str():
    return (string.ascii_letters + string.digits)


def key_gen(test_len):
    keylist = [random.choice(base_str()) for i in range(test_len)]
    return ("".join(keylist))


def tcpconn_process():
    # The follow print will occupy too much time
    # beyong the time offset of the TCP settings of the board
    # print ('UDP broadcast end')

    m_usSvrPort = 6490
    # work as TCP server
    # socket->bind->listen->accept
    # SO_REUSEADDR unbind immediately
    sock_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock_server.bind(("", m_usSvrPort))
    sock_server.listen(1)
    conn, addr = sock_server.accept()

    with proc_lock:

        print('Process2: ' + addr[0] + 'successful connected!')

        mod_type = [FSK, QPSK, QAM16, QAM64]
        mod_name = ['FSK', 'QPSK', 'QAM16', 'QAM64']
        pack_len = [20, 118, 238, 358]
        pack_len = [2, 2, 2, 2]

        test_times = 10
        test_dur = test_times * 2


        for index in range(0, 4):
            rcv_times = 0
            start_time = time.time()
            time_inter = 0
            # set the modulation type
            command_send(conn, CMD_SYS_MODETYPE, mod_type[index], SETLEN)

            # set the four channel united
            command_send(conn, CMD_SYS_DECCHNUM, 4, SETLEN)

            file_rcv = open(mod_name[index] + '_data.txt', 'w')

            while time_inter < test_dur + GUARD:
                # Judge the header intending
                header = conn.recv(8)
                set_format = 'II'
                set_header = unpack(set_format, header)

                now_time = time.time()
                time_inter = now_time - start_time

                # Rcv the string and cal
                if set_header[1] == CMD_SYS_DISPSTR:
                    buffer = conn.recv(set_header[0])
                    strs = buffer.decode('utf-8')
                    file_rcv.write(strs)
                    print(strs)
                    rcv_times += 1

            file_rcv.write('\n')
            str_byte = str(rcv_times / test_times)
            file_rcv.write(str_byte)
            file_rcv.close()

        conn.close()
        sock_server.close()
        print("Receive success!")


if "__main__" == __name__:

    print("Receiver working")

    broad_t1 = threading.Thread(target=broadcast_process)
    tcp_t2 = threading.Thread(target=tcpconn_process)

    proc_lock = threading.Lock()

    # print threading.current_thread()
    broad_t1.run()
    tcp_t2.run()

'''
    # code above has a wrong time order 
    threads = []
    threads.append(broad_t1)
    threads.append(tcp_t2)
    for t in threads:
        #t.setDaemon(True)
        t.start()
'''
# print (addr[0] + 'disconnected!')



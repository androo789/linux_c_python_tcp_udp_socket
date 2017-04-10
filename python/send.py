#!/usr/bin/env python
# -*- coding: utf-8 -*

"""
client_pc.py

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Simulate the socket function of I-SEA underwater acoustic User Interface
(UWM.exe) model.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
V0.1 update the broadcast thread
V0.2 update the TCP connect thread
V0.3 update the UI parameter init process

"""

import socket
import time
from struct import *
import types 


class com():
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

if "__main__" == __name__:

    boot = b'0xFC\x06\x88\xFF\xBE\x0B\x04\x00\x00\x00\x01\x01\x1E\xC5'
    unlock = b'\xFC\x21\x94\xFF\xBE\x4C\x00\x00\x80\x3F\x00\x98\xA5\x46\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x90\x01\x01\x01\x00\xE4\xF2'
    target = b'\xFC\x21\x99\xFF\xBE\x4C\x00\x00\x00\x00\x00\x98\xA5\x46\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x90\x01\x01\x01\x00\x5D\xF9'
    landoff = b'\xFC\x06\x5C\xFF\xBE\x0B\x09\x00\x00\x00\x01\x01\x5F\x9B'
    fly15 = b'\xFC\x21\x9A\xFF\xBE\x4C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x70\x41\x16\x00\x01\x01\x00\x70\xAE'

    # init the broadcast address, ID and port
    broad_addr = "192.168.4.1"
    port = 14550

    # send the message by UDP socket
    # socket->sendto
    sock_clinet = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    #sock_clinet.setsockopt(socket.SOL_SOCKET,socket.SO_BROADCAST,1)

    # boot the UAV
    sock_clinet.sendto(boot,0,(broad_addr,port))
    time.sleep(0.1)
    
    # unlock the UAV
    sock_clinet.sendto(unlock,0,(broad_addr,port))
    time.sleep(0.1)

    # fly to 15 meters
    sock_clinet.sendto(fly15,0,(broad_addr,port))
    time.sleep(1)

    # landoff
    sock_clinet.sendto(landoff,0,(broad_addr,port))
    time.sleep(0.1)

    sock_clinet.close()

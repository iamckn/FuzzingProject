#!/usr/bin/python

#
# 2009 by Collin Mulliner
#
# http://www.mulliner.org/security/sms/
#
# GPLv2
#

import os
import sys
import socket
import time
import Utils
import sms
import random 
import SMSFuzzData
import fuzzutils

def randdcs():
	dcs = []
	dcs.append(0x00)
	dcs.append(0x04)
	return dcs.pop(random.randrange(0,len(dcs)))

def concat(msisdn, smsc, ts, m, c, mid, data, dcs = 0x00):
        """
        PARA msisdn: string, sms recieved number
        para smsc  : string, sms caller number
        para ts    : string timestamp
        para m,c,mid:
        para data: string sms-data
        para dcs: byte user-data coding scheme
        """
	#print "m=%d c=%d mid=%x" % (m,c,mid)
        s = sms.SMSToMS()
        s._msisdn = msisdn
        s._msisdn_type = 0x91
        s._smsc = smsc
        s._smsc_type = 0x91
        s._tppid = 0x00
        s._tpdcs = dcs
        s._timestamp = ts
        s._deliver = 0x04
        s.deliver_raw2flags()
        #s.deliver_print()
        s._deliver_udhi = 1
        s.deliver_flags2raw()
        #s.deliver_print()
        s._msg = data
        s._msg_leng = -1
        #s._msg_leng = len(data)
        u = sms.SMSUdhConcatMsg(m, c, mid)
        (s._udh_leng, s._udh) = u.getUdh()
        s._udh_leng = -1
        s.encode()
        return s._pdu

# data == None => generate data
# datalen == >0 => use num bytes, == <0 use random length length * -1 => max
def simple_multipart(start = 1, stop = 2, dcs = 0x00, mid = -1, data = None, datalen = 134):
	out = []
	if mid == -1:
		mid = random.randrange(0,0xff)
	ts = Utils.hex2bin("99309251619580",0)
	for i in xrange(start, stop+1):
		if dcs == -1:
			dcsuse = randdcs()
		else:
			dcsuse = dcs
		if data == None:
			datause = SMSFuzzData.getSMSFuzzData()
		else:
			datause = data[:datalen]
		if datalen < 0:
			datause = datause[:random.randrange(0,datalen*-1)]
		msg = concat("49177123456", "49177123456", ts, stop, i, mid, datause, dcsuse)
		line = Utils.bin2hex(msg, 1)
		leng = (len(line) / 2) - 8
		out.append((line, leng))
	return out

def random_index_multipart(count = 10, dummy = 0, dcs = 0x00, mid = -1, data = None, datalen = 134):
	out = []
	r = random.WichmannHill()
	if mid == -1:
		mid = r.randrange(0,0xff)
	ts = Utils.hex2bin("99309251619580",0)
	for i in range(0, count):
		if dcs == -1:
			dcsuse = randdcs()
		else:
			dcsuse = dcs
		if data == None:
			datause = SMSFuzzData.getSMSFuzzData()
		else:
			datause = data[:datalen]
		if datalen < 0:
			datause = datause[:r.randrange(0,datalen*-1)]
		msg = concat("49177123456", "49177123456", ts, count, r.randrange(0, 256), mid, datause, dcsuse)
		line = Utils.bin2hex(msg, 1)
		leng = (len(line) / 2) - 8
		out.append((line, leng))
	return out
	
def double_index_multipart(start = 1, stop = 2, dcs = 0x00, mid = -1, data = None, datalen = 134):
	out = []
	if mid == -1:
		mid = random.randrange(0,0xff)
	ts = Utils.hex2bin("99309251619580",0)
	for i in range(start, stop+1):
		if data == None:
			datause = SMSFuzzData.getSMSFuzzData()
		else:
			datause = data[:datalen]
		if datalen < 0:
			datause = datause[:random.randrange(0,datalen*-1)]
		msg = concat("49177123456", "49177123456", ts, stop, i, mid, datause, dcs)
		line = Utils.bin2hex(msg, 1)
		leng = (len(line) / 2) - 8
		out.append((line, leng))
		out.append((line, leng))
	return out

def double_index_multipart_length(first = 10, second = 134, dcs = 0x00, mid = -1, data = None, datalen = 134):
	out = []
	if mid == -1:
		mid = random.randrange(0,0xff)
	ts = Utils.hex2bin("99309251619580",0)
	if data == None:
		data = SMSFuzzData.getSMSFuzzData()
	msg = concat("49177123456", "49177123456", ts, 2, 1, mid, data[:first], dcs)
	line = Utils.bin2hex(msg, 1)
	leng = (len(line) / 2) - 8
	out.append((line, leng))
	msg = concat("49177123456", "49177123456", ts, 2, 1, mid, data[:second], dcs)
	line = Utils.bin2hex(msg, 1)
	leng = (len(line) / 2) - 8
	out.append((line, leng))
	return out
	
def simple_multipart_reverse(start = 1, stop = 2, dcs = 0x00, mid = -1, data = None, datalen = 134):
	out = []
	if mid == -1:
		mid = random.randrange(0,0xff)
	ts = Utils.hex2bin("99309251619580",0)
	for i in range(start, stop+1):
		if dcs == -1:
			dcsuse = randdcs()
		else:
			dcsuse = dcs
		if data == None:
			datause = SMSFuzzData.getSMSFuzzData()
		else:
			datause = data[:datalen]
		if datalen < 0:
			datause = datause[:random.randrange(0,datalen*-1)]
		msg = concat("49177123456", "49177123456", ts, stop, stop - i, mid, datause, dcsuse)
		line = Utils.bin2hex(msg, 1)
		leng = (len(line) / 2) - 8
		out.append((line, leng))
	return out
	
def random_order_multipart(start = 1, stop = 2, dcs = 0x00, mid = -1, data = None, datalen = 134):
	out = []
	if mid == -1:
		mid = random.randrange(0,0xff)
	ts = Utils.hex2bin("99309251619580",0)
	index = []
	for i in range(start, stop+1):
		index.append(int(i))
	for i in range(start, stop+1):
		pos = random.randrange(0, len(index))
		#print pos
		ppos = index.pop(int(pos))
		#print "PPos: %d" % int(ppos)
		if dcs == -1:
			dcsuse = randdcs()
		else:
			dcsuse = dcs
		if data == None:
			datause = SMSFuzzData.getSMSFuzzData()
		else:
			datause = data[:datelen]
		if datalen < 0:
			datause = datause[:random.randrange(0,datalen*-1)]
		msg = concat("4177123456", "49177123456", ts, stop, ppos, mid, datause, dcsuse)
		line = Utils.bin2hex(msg, 1)
		leng = (len(line) / 2) - 8
		out.append((line, leng))
	return out


if __name__ == "__main__":
	# random_index
	cases = simple_multipart(start = 1, stop = 2, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 10, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 50, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 100, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 255, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 255, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 2, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 10, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 50, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 100, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 255, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 255, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 2, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 10, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 50, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 100, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 255, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 255, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + simple_multipart(start = 1, stop = 2, dcs = -1, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 10, dcs = -1, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 50, dcs = -1, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 100, dcs = -1, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 255, dcs = -1, mid = -1, data = None, datalen = -134)
	cases = cases + simple_multipart(start = 1, stop = 255, dcs = -1, mid = -1, data = None, datalen = -134)
	print len(cases)
	# random index
	cases = cases + random_index_multipart(256, 0, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + random_index_multipart(256, 0, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + random_index_multipart(256, 0, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + random_index_multipart(256, 0, dcs = -1, mid = -1, data = None, datalen = -134)
	print len(cases)
	# random order index
	cases = cases + random_order_multipart(start = 1, stop = 10, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + random_order_multipart(start = 1, stop = 10, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + random_order_multipart(start = 1, stop = 10, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + random_order_multipart(start = 1, stop = 10, dcs = -1, mid = -1, data = None, datalen = -134)
	cases = cases + random_order_multipart(start = 1, stop = 50, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + random_order_multipart(start = 1, stop = 50, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + random_order_multipart(start = 1, stop = 50, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + random_order_multipart(start = 1, stop = 50, dcs = -1, mid = -1, data = None, datalen = -134)
	cases = cases + random_order_multipart(start = 1, stop = 130, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + random_order_multipart(start = 1, stop = 130, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + random_order_multipart(start = 1, stop = 130, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + random_order_multipart(start = 1, stop = 130, dcs = -1, mid = -1, data = None, datalen = -134)
	cases = cases + random_order_multipart(start = 1, stop = 256, dcs = 0x00, mid = -1, data = None, datalen = 134)
	cases = cases + random_order_multipart(start = 1, stop = 256, dcs = -1, mid = -1, data = None, datalen = 134)
	cases = cases + random_order_multipart(start = 1, stop = 256, dcs = 0x00, mid = -1, data = None, datalen = -134)
	cases = cases + random_order_multipart(start = 1, stop = 256, dcs = -1, mid = -1, data = None, datalen = -134)
	print len(cases)
	
	fuzzutils.cases2file(cases, "concat.cases")
	

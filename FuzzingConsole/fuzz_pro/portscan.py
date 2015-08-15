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
import Utils
import sms
import time
import socket
import SMSFuzzData

def toPort(msisdn, smsc, dcs, tppid, deliver, ts, src, dst, data, short = 0):
	s = sms.SMSToMS()
	s._msisdn = msisdn
	s._msisdn_type = 0x91
	s._smsc = smsc
	s._smsc_type = 0x91
	s._tppid = tppid
	s._tpdcs = dcs
	s._timestamp = ts
	s._deliver = deliver
	s.deliver_raw2flags()
	#s.deliver_print()
	s._deliver_udhi = 1
	s.deliver_flags2raw()
	s.deliver_print()
	s._msg = data
	s._msg_leng = len(data)
	if short == 0:
		u = sms.SMSUdhPorts(src, dst)
	else:
		u = sms.SMSUdh8bitPorts(src, dst)
	(s._udh_leng, s._udh) = u.getUdh()
	s.encode()
	return s._pdu

if __name__ == "__main__":
	print "change port range and dest IP in code!"

	c = 1
	for i in range(0,1000):
		s = 0
		d = 2948
		ts = Utils.hex2bin("99309251619580",0)
		data = SMSFuzzData.getSMSFuzzData()
		
		print "datalen: " + str(len(data))
		msg = toPort("49177123456", "49177123456", 0x04, 0x00, 0x04, ts, s, d, data[:130], 0)
		line = Utils.bin2hex(msg, 1)
		leng = (len(line) / 2) - 8
		
		buffer = "+CMT: ,%d\r\n%s\r\n" % (leng, line)
		#buffer = "\n+CMT: ,%d\n%s\n" % (leng, line)
		print buffer
		print "c= %d src: %d dst: %d\n" % (c,s,d)
		so = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		so.connect(("10.117.55.119", 4223))
		so.send(buffer)
		so.close()
		time.sleep(2)
		c = c + 1

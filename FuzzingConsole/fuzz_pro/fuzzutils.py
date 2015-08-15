#!/usr/bin/python

#
# 2009 by Collin Mulliner
#
# http://www.mulliner.org/security/sms/
#
# GPLv2
#

import os
import time
import socket

def sendmsg(dest_ip, msg, msg_cmt, crlftype = 1):
	error = 0
	if crlftype == 1:
		buffer = "+CMT: ,%d\r\n%s\r\n" % (msg_cmt, msg)
	elif crlftype == 2:
		buffer = "\n+CMT: ,%d\n%s\n" % (msg_cmt, msg)	
	so = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		so.connect((dest_ip, 4223))
	except:
		error = 1
	try:
		so.send(buffer)
	except:
		error = 2
	so.close()
	return error

def sendmsgbeep(dest_ip, msg, msg_cmt, crlftype = 1):
	if sendmsgto(dest_ip, msg, msg_cmt, crlftype) == 1:
		print "\a\a"

def cases2file(cases, filename):
	fp = open(filename, "w")
	for i in cases:
		(line, cmt) = i
		fp.write(line + " " + str(cmt) + "\n")
	fp.close()
		
def file2cases(filename):
	out = []
	fp = open(filename)
	line = fp.readline()
	while line:
		cr = line.split(" ")
		out.append((cr[0], int(cr[1].rstrip("\n"))))
		line = fp.readline()
	fp.close()
	return out

def sendcases(dest_ip, cases, crlftype = 1, delay = 5, status = 0, start = 0):
	count = 0
	for i in cases:
		if count >= start:
			(line, cmt) = i
			error = sendmsg(dest_ip, line, cmt, crlftype)
			if status > 0:
				print "%d) error = %d data: %s" % (count, error, line)
			time.sleep(delay)
		count = count + 1

def sendcasesfromfile(filename, dest_ip, crlftype = 1, delay = 5, status = 0, start = 0):
	cases = file2cases(filename)
	sendcases(dest_ip, cases, crlftype, delay, status, start)

if __name__ == "__main__":
	fn = os.sys.argv[1]
	dest = os.sys.argv[2]
	start = 0
	if len(os.sys.argv) > 3:
		start = int(os.sys.argv[3])
	print "Sending test cases from %s to %s" % (fn, dest)
	sendcasesfromfile(fn, dest, status = 1, start = start)

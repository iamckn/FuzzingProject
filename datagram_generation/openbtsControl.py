#!/usr/bin/python

from openbtsElements import *
from Utils import *
import socket

#===============================socket setting==========================
# socket settings of local console
rev_port=49101
send_port=49001
send_addr=("localhost",send_port)
send_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
rev_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
rev_sock.bind(('',rev_port))

##socket settings of openBTS
##OpenbtsPort=6001
##RevOpenbtsPort=6101
##OpenbtsIp='121.248.55.28'
##OpenbtsClient=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
##OpenbtsServer=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
##OpenbtsServer.bind(("",RevOpenbtsPort))
##OpenbtsServer.settimeout(60)


#=====================================class for console STREAM header============================================
def generate_msg(msg):
    log("generate message:\t"+msg)
    element_infos=GSMessage_ElementInfo_Table[msg]
    for item in element_infos:
        ele_num=item[0]
        ele=item[1]
        ele_L2FrameType=item[2]
        for string in ele.generate():
            ###add header right way
            result=add_header(msg,[ele_num,string,ele_L2FrameType])
            yield result

            
class FuzzingData_Head:
    SynData=[0x00, 0xff, 0xff, 0x00]
    f_IMSI='0'*15
    def __init__(self,wmsg_info,wElementNum=0,L2FrameType=0):
        self.FuzzingType=wmsg_info[0] #int
        self.PD=wmsg_info[1]   #int
        self.MTI=wmsg_info[2] # int
        self.ElementNum=wElementNum #int
        self.L2FrameType=L2FrameType#int 0->unitdata,1->data
        self.length=0 # unsigned int
        if not len(IMSI)==15:
            log('FuzzingdataHead:\tno imsi init')
        else:
            FuzzingData_Head.f_IMSI=IMSI

            
    def IMSI(self,imsi):
        if not len(imsi)==15:
            log('FuzzingdataHead:\terror, set IMSI')
            return
        FuzzingData_Head.f_IMSI=imsi

        
    def set_len(self,wlen):
        self.length=wlen
        
    def __str__(self):
        re=''
        for string in FuzzingData_Head.SynData:
            re+=struct.pack('B',string)
        re+=struct.pack('i',self.FuzzingType)
        re+=(FuzzingData_Head.f_IMSI+'\x00')
        re+=struct.pack('i',self.PD)
        re+=struct.pack('i',self.MTI)
        re+=struct.pack('i',self.ElementNum)
        re+=struct.pack('i',self.L2FrameType)
        re+=struct.pack('I',self.length)
        return re

def add_header(msg,element_info):
    """
    msg @type string
    msg @param the name of message to fuzz
    element_info @type list
                 @param element_num-integer,fuzz value-string
    """
    msg_info=GSMessages_info_table[msg]
    header=FuzzingData_Head(msg_info,element_info[0],element_info[2])
    header.set_len(len(element_info[1]))
    return str(header)+element_info[1]



#===============================Socket console fuctions===============================
def main_loop():
    log("=========================<<system start>>======================")
    while True:
        (rev_str,t_addr)=rev_sock.recvfrom(1024)
        rev_str=rev_str.rstrip('\x00')
        log(rev_str)
        # tar+cmd+data
        str_list=rev_str.split(" ")
        target_str=str_list[0]
        cmd_str=str_list[1]
        data_str="".join(str_list[2:])
        
        if target_str!="console":
            log("CONSOLE WR target error")
            send_sock.sendto("CONSOLE WR target error",send_addr)
            continue
        
        #console cmd process
        if cmd_str=='quit' or cmd_str=='exit':
            send_sock.sendto("CONSOLE OK shutdown",send_addr)
            log("=========================<<systemshutdown>>=======================")
            return 1
        if cmd_str=='fuzz':
            #get fuzz data
            if GSMessages_info_table.has_key(data_str):
                fuzz_loop(data_str)
                continue
            else:
                log("CONSOLE WR fuzz no message")
                send_sock.sendto("CONSOLE WR fuzz no message",send_addr)
                continue
        if cmd_str=='IMSI':
            #set imsi
            if not len(data_str)==15:
                log("CONSOLE WR imsi length error")
                send_sock.sendto("CONSOLE WR imsi length error",send_addr)
                continue
            else:
                send_sock.sendto("CONSOLE OK",send_addr)
                global IMSI
                IMSI=data_str
                updateAll()
                continue
        if cmd_str=="sulley":
            #set option
            if sulley_set(data_str):
                send_sock.sendto("CONSOLE OK ",send_addr)
            else:
                send_sock.sendto("CONSOLE WR sulley set error",send_addr)
            continue
        
        log("CONSOLE WR command noexist")
        send_sock.sendto("CONSOLE WR command noexist",send_addr);

def fuzz_loop(msg):
    log("FUZZ IMSI:\t"+str(IMSI))
    msg_pool=[]
    for string in generate_msg(msg):
        msg_pool.append(string)

    messageCount=len(msg_pool)
    log("FUZZ number of messages:\t"+str(messageCount))
    count=0
    send_sock.sendto("CONSOLE OK "+str(messageCount),send_addr)
    while True:
       (rev_str,t_addr)=rev_sock.recvfrom(1024)
       rev_str=rev_str.rstrip('\x00')
       log(rev_str)
       str_list=rev_str.split(" ")
       tar_str=str_list[0]
       cmd_str=str_list[1]
       data_str="".join(str_list[2:])

       if tar_str!="fuzz":
           send_sock.sendto("FUZZ WR target wrong",send_addr)
           return
           
       #console loop
       if cmd_str=="send" and count<len(msg_pool):
           send_sock.sendto("FUZZ OK "+str(count)+" "+str(messageCount)+" "+msg_pool[count],send_addr)
           log("\r\n"+s_hex_dump(msg_pool[count]))
           count+=1
           continue

       elif cmd_str=="send" and count==len(msg_pool):
           log("msg loop:\tFUZZ OK")
           send_sock.sendto("FUZZ OK "+str(count)+" "+str(messageCount)+" "+msg_pool[count],send_addr)
           return
           
       elif cmd_str=='abort':
           log('abort:\t'+msg)
           send_sock.sendto("FUZZ OK abort",send_addr)
           return
       else:
           send_sock.sendto("FUZZ WR command noexist",send_addr)
            #error process
           log("FUZZ WR command noexist"+rev_str)
           return
def sulley_set(set_str):
    for item in set_str.split("&"):
        [mkey,mval]=item.split("=")
        if FuzzGenerator.sulleyTable.has_key(mkey):
            if mkey!="str_value": FuzzGenerator.sulleyTable[mkey]=int(mval)
            else: FuzzGenerator.sulleyTable[mkey]=mval
        else: return False
    updateAll()
    return True
        
##def getBtsRsp():
##    (rev_str,addr)=OpenbtsServer.recvfrom(1024)
##    rev_str=rev_str.rstrip('\x00')
##    return rev_str

            
#=================================== OPENBTS console FUCS================================

        
def openbtsControlLoop(msg):
    for string in generate_msg(msg):
        while True:
            OpenbtsClient.sendto(bytes(string),(OpenbtsIp,OpenbtsPort))
            (RevStr,t_addr)=OpenbtsServer.recvfrom(1024)
            print RevStr,t_addr
            str_list=RevStr.split(' ')
            stat_str=str_list[1]
            if stat_str=="FF":
                log(s_hex_dump(string[44:]))
                log(RevStr)
                break
            if stat_str=="WR":
                return
            if stat_str=="DC":
                log(s_hex_dump(string[44:]))
                log(RevStr)
                time.sleep(5)
                continue
            break
        #bts control decide 
        
##        print RevStr,t_addr
##        strList=RevStr.split()
##        if strList[1]=='OK':
##            continue
##        if strList[1]=='exit' or strList[1]=='quit':
##            OpenbtsClient.sendto('exit',(OpenbtsIp,OpenbtsPort)) 
##            return
##        else:
##            print RevStr

    
    
if __name__=="__main__":
    while 1:
        try:
            if main_loop():
                break;
        except Exception,e:
            log(str(e))
            
    send_sock.close()
    rev_sock.close()
##    OpenbtsClient.close()
##    OpenbtsServer.close()

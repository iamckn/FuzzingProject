#!/usr/bin/python
# -*- coding: cp936 -*-
from protogen import*

#=====================================global setting=======================
#IMSI='460001883544688'
IMSI='460011580351887'
TEST_FLAG=1
                      
#======================================GSM ELEMENTS=======================================
# odd check and IMSI set of MoblieIdentity 
def MI2(res):
    if not len(IMSI)==15:
        return res
    t_len=(len(res)-2)*2-1
    odd_check=t_len%2
    #parse imsi
    t=ord(IMSI[0])-ord('0')
    t=(((t<<1)+odd_check)<<3)+1
    t_char=struct.pack('B',t)
    count=0
    for char in IMSI[1:]:
        if count==0:
            t=ord(char)-ord('0')
        else:
            t=t+((ord(char)-ord('0'))<<4)
            t_char+=struct.pack('B',t)
        count=(count+1)%2
    res=t_char+res[8:]
    return res

sms1= SMSGenerator(['./fuzz_pro/sulley.cases','./fuzz_pro/concat.cases'])

# for page
#correct->only L2Framedata change
MoblieIdentity1_1=FuzzGenerator(['bs4,'+IMSI[0],\
                                 'bs1,1','bs3,1'\
                                 ,'bs4,'+IMSI[2]\
                                 ,'bs4,'+IMSI[1]\
                                 ,'bs4,'+IMSI[4]\
                                 ,'bs4,'+IMSI[3]\
                                 ,'bs4,'+IMSI[6]\
                                 ,'bs4,'+IMSI[5]\
                                 ,'bs4,'+IMSI[8]\
                                 ,'bs4,'+IMSI[7]\
                                 ,'bs4,'+IMSI[10]\
                                 ,'bs4,'+IMSI[9]\
                                 ,'bs4,'+IMSI[12]\
                                 ,'bs4,'+IMSI[11]\
                                 ,'bs4,'+IMSI[14]\
                                 ,'bs4,'+IMSI[13]]
                              ,wtype=3,name='MoblieIdentity1_1')
#fuzz every correct field
MoblieIdentity1_2=FuzzGenerator(['b4,'+IMSI[0],\
                                 'b1,1','b3,1'\
                                 ,'b4,'+IMSI[2]\
                                 ,'b4,'+IMSI[1]\
                                 ,'b4,'+IMSI[4]\
                                 ,'b4,'+IMSI[3]\
                                 ,'b4,'+IMSI[6]\
                                 ,'b4,'+IMSI[5]\
                                 ,'b4,'+IMSI[8]\
                                 ,'b4,'+IMSI[7]\
                                 ,'b4,'+IMSI[10]\
                                 ,'b4,'+IMSI[9]\
                                 ,'b4,'+IMSI[12]\
                                 ,'b4,'+IMSI[11]\
                                 ,'b4,'+IMSI[14]\
                                 ,'b4,'+IMSI[13]]
                              ,wtype=3,name='MoblieIdentity1_2')

# length = actual len, with text behind
MoblieIdentity1_3=FuzzGenerator(['bs4,'+IMSI[0],\
                                 'bs1,1','bs3,1'\
                                 ,'bs4,'+IMSI[2]\
                                 ,'bs4,'+IMSI[1]\
                                 ,'bs4,'+IMSI[4]\
                                 ,'bs4,'+IMSI[3]\
                                 ,'bs4,'+IMSI[6]\
                                 ,'bs4,'+IMSI[5]\
                                 ,'bs4,'+IMSI[8]\
                                 ,'bs4,'+IMSI[7]\
                                 ,'bs4,'+IMSI[10]\
                                 ,'bs4,'+IMSI[9]\
                                 ,'bs4,'+IMSI[12]\
                                 ,'bs4,'+IMSI[11]\
                                 ,'bs4,'+IMSI[14]\
                                 ,'bs4,'+IMSI[13]\
                                 ,'text']
                              ,wtype=3,name='MoblieIdentity1_3')
# length = actual len, with interger behind
MoblieIdentity1_4=FuzzGenerator(['bs4,0','bs1,0','bs3,1',\
                               'bs16,0',\
                               'bs16,0',\
                               'bs16,0',\
                               'bs8,0','r10,4']\
                              ,wtype=3,name='MoblieIdentity1_4',encoder=MI2)

# 08 length behind
MoblieIdentity1_5=FuzzGenerator(['bs8,8','bs4,'+IMSI[0],\
                                 'bs1,1','bs3,1'\
                                 ,'bs4,'+IMSI[2]\
                                 ,'bs4,'+IMSI[1]\
                                 ,'bs4,'+IMSI[4]\
                                 ,'bs4,'+IMSI[3]\
                                 ,'bs4,'+IMSI[6]\
                                 ,'bs4,'+IMSI[5]\
                                 ,'bs4,'+IMSI[8]\
                                 ,'bs4,'+IMSI[7]\
                                 ,'bs4,'+IMSI[10]\
                                 ,'bs4,'+IMSI[9]\
                                 ,'bs4,'+IMSI[12]\
                                 ,'bs4,'+IMSI[11]\
                                 ,'bs4,'+IMSI[14]\
                                 ,'bs4,'+IMSI[13]\
                                 ,'r15,4']
                              ,wtype=1,name='MoblieIdentity1_5')
MoblieIdentity1_6=FuzzGenerator(['bs8,8','bs4,'+IMSI[0],\
                                 'bs1,1','bs3,1'\
                                 ,'bs4,'+IMSI[2]\
                                 ,'bs4,'+IMSI[1]\
                                 ,'bs4,'+IMSI[4]\
                                 ,'bs4,'+IMSI[3]\
                                 ,'bs4,'+IMSI[6]\
                                 ,'bs4,'+IMSI[5]\
                                 ,'bs4,'+IMSI[8]\
                                 ,'bs4,'+IMSI[7]\
                                 ,'bs4,'+IMSI[10]\
                                 ,'bs4,'+IMSI[9]\
                                 ,'bs4,'+IMSI[12]\
                                 ,'bs4,'+IMSI[11]\
                                 ,'bs4,'+IMSI[14]\
                                 ,'bs4,'+IMSI[13]\
                                 ,'text']
                              ,wtype=1,name='MoblieIdentity1_6')

#for lud

MoblieIdentity2_1=FuzzGenerator(['b4,'+IMSI[0],\
                                 'b1,1','b3,1'\
                                 ,'b4,'+IMSI[2]\
                                 ,'b4,'+IMSI[1]\
                                 ,'b4,'+IMSI[4]\
                                 ,'b4,'+IMSI[3]\
                                 ,'b4,'+IMSI[6]\
                                 ,'b4,'+IMSI[5]\
                                 ,'b4,'+IMSI[8]\
                                 ,'b4,'+IMSI[7]\
                                 ,'b4,'+IMSI[10]\
                                 ,'b4,'+IMSI[9]\
                                 ,'b4,'+IMSI[12]\
                                 ,'b4,'+IMSI[11]\
                                 ,'b4,'+IMSI[14]\
                                 ,'b4,'+IMSI[13]]
                              ,IEI=0x17,name='MoblieIdentity2_1')

MoblieIdentity2_2=FuzzGenerator(['bs4,0','bs1,0','bs3,1',\
                               'bs16,0',\
                               'bs16,0',\
                               'bs16,0',\
                               'bs8,0','text']\
                              ,IEI=0x17,name='MoblieIdentity2_2',encoder=MI2)

MoblieIdentity2_3=FuzzGenerator(['bs4,0','bs1,0','bs3,1',\
                               'bs16,0',\
                               'bs16,0',\
                               'bs16,0',\
                               'bs8,0','r15,4']\
                              ,IEI=0x17,name='MoblieIdentity2_3',encoder=MI2)
# 08 length behind
MoblieIdentity2_4=FuzzGenerator(['bs8,8','bs4,'+IMSI[0],\
                                 'bs1,1','bs3,1'\
                                 ,'bs4,'+IMSI[2]\
                                 ,'bs4,'+IMSI[1]\
                                 ,'bs4,'+IMSI[4]\
                                 ,'bs4,'+IMSI[3]\
                                 ,'bs4,'+IMSI[6]\
                                 ,'bs4,'+IMSI[5]\
                                 ,'bs4,'+IMSI[8]\
                                 ,'bs4,'+IMSI[7]\
                                 ,'bs4,'+IMSI[10]\
                                 ,'bs4,'+IMSI[9]\
                                 ,'bs4,'+IMSI[12]\
                                 ,'bs4,'+IMSI[11]\
                                 ,'bs4,'+IMSI[14]\
                                 ,'bs4,'+IMSI[13]\
                                 ,'r15,4']
                              ,wtype=1,name='MoblieIdentity2_4')
MoblieIdentity2_5=FuzzGenerator(['bs8,8','bs4,'+IMSI[0],\
                                 'bs1,1','bs3,1'\
                                 ,'bs4,'+IMSI[2]\
                                 ,'bs4,'+IMSI[1]\
                                 ,'bs4,'+IMSI[4]\
                                 ,'bs4,'+IMSI[3]\
                                 ,'bs4,'+IMSI[6]\
                                 ,'bs4,'+IMSI[5]\
                                 ,'bs4,'+IMSI[8]\
                                 ,'bs4,'+IMSI[7]\
                                 ,'bs4,'+IMSI[10]\
                                 ,'bs4,'+IMSI[9]\
                                 ,'bs4,'+IMSI[12]\
                                 ,'bs4,'+IMSI[11]\
                                 ,'bs4,'+IMSI[14]\
                                 ,'bs4,'+IMSI[13]\
                                 ,'text']
                              ,wtype=1,name='MoblieIdentity2_5')


Networkname3=FuzzGenerator(['bs1,1','bs3,1','bs1,0','bs3,0','text:value=AAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA:\"AAAAAAAAAAA;AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAA>>AAAAAAAAAAAAAAAAAAA\\\"\'AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAA@AAAA**A&AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA']\
                           ,IEI=0x45,name='Networkname3')

Networkname1=FuzzGenerator(['b1,1','b3,1','b1,0','b3,0','text']\
                          ,IEI=0x45,name='Networkname1')

#need to be fix
Networkname2=FuzzGenerator(['bs1,1','bs3,1','bs1,0','bs3,0','r15,4']\
                          ,IEI=0x45,name='Networkname2')

timeZone1=FuzzGenerator(['b8','b8','b8','b8','b8','b8','b8'],IEI=0x47,wtype=5,name='timeZone')
timeZone2=FuzzGenerator(['r10,4'],IEI=0x47,wtype=5,name='timeZone2')
timeZone3=FuzzGenerator(['text'],IEI=0x47,wtype=5,name='timeZone3')


callingPartyBCDnumber3=FuzzGenerator(['bs1,0','bs3,2','bs4,1',\
                                      'r10,4'],IEI=0x5c,name='callingPartyBCDnumber3')

callingPartyBCDnumber2=FuzzGenerator(['bs1,0','bs3,2','bs4,1',\
                                      'text:value=\x17\x95\x11\x56\x51\x87\x36\x92'],IEI=0x5c,name='callingPartyBCDnumber2')
callingPartyBCDnumber1=FuzzGenerator(['bs1,0','bs3,2','bs4,1',\
                                     'b4,0','b4,0','b4,8','b4,6',\
                                     'b4,1','b4,5','b4,6','b4,5',\
                                     'b4,1','b4,8','b4,7','b4,9',\
                                     'b4,5','b4,3','b4,3','b4,15',\
                                     'b4,9','b4,9','b4,9','b4,9',\
                                     'b4,9','b4,9'],IEI=0x5c,name='callingPartyBCDnumber1')





#============================GSM MESSAGES|ELEMENTS info TABLE==============================
# table for look up fuzzing messages
GSMessage_ElementInfo_Table={'locationUpdatingAccept':[[5,MoblieIdentity2_2,0],[5,MoblieIdentity2_3,0],[5,MoblieIdentity2_4,0],[5,MoblieIdentity2_5,0],\
                                                       [5,MoblieIdentity2_1,0]],\
              'mmInformation':[[5,Networkname3,0],[5,Networkname1,0],[5,Networkname2,0],[6,timeZone1,0],[6,timeZone2,0],[6,timeZone3,0]],\
              'pagingRequestType1':[[7,MoblieIdentity1_1,1],[7,MoblieIdentity1_5,0],[7,MoblieIdentity1_6,0]\
                                    ,[7,MoblieIdentity1_3,1],[7,MoblieIdentity1_4,1],[7,MoblieIdentity1_2,0]],\
              'setupMobileTerminated':[[10,callingPartyBCDnumber2,0],[10,callingPartyBCDnumber3,0],[10,callingPartyBCDnumber1,0]],\
                             'SMS':[[0,sms1,0]]
              }

#================================= GSM MESSAGES INFO TABLE ============================         
GSMessages_info_table={'pagingRequestType1':[0,0x6,0x21],\
                       #fuzzingtype pd mti  
                       'mmInformation':[0,0x5,0x32],\
                       'locationUpdatingAccept':[0,0x05,0x2],\
                       'setupMobileTerminated':[0,0x3,0x5],\
                       'SMS':[1,0,0]\
                       }

def updateAll():
    blocks.REQUESTS.clear()
    for mkey in GSMessage_ElementInfo_Table.iterkeys():
        if mkey is not 'SMS':
            eles=GSMessage_ElementInfo_Table[mkey]
            for item in eles:
                item[1].initialize()
        











    

# -*- coding: cp936 -*-
from sulley import *
import struct
import fuzzutils
from Utils import*

class FuzzGenerator:
    """define a generator, produce GSM elements
    @param fuzz_struct : a dict display element structure such like ['b1','b7','text']
    @param iei         : iei
    @param wtype       : TLV type bit1 means value existence; bit2 means length existence; bit3 means iei extence; bit4 means is iei in value
    @param name        : name
    @param encoder     : fuc called after rendered

    @fuzz_struct type  : 'b1','b2' ----------bitfield length 1,2
                        : 'bs1,0','bs4,0'----bitfield length 1,value 0
                        : 'text'-------------string field
                        :''r1,10,1'----------random field
    """
    sulleyTable={'bit_testall':0,\
                 'bit_minEdge':2,\
                 'bit_maxRange':3,\
                 'Lfuzz':0,\
                 'str_value':"hello",\
                 'str_maxlen':64,\
                 'rand_minreps':64,\
                 'rand_maxreps':128,\
                 'rand_maxmuns':10}
    
    def __init__(self,fuzz_struct,IEI=0,wtype=7,name='pagingRequest1',encoder=None):
        self.mblockname=name
        self.mIEI=IEI
        self.mtype=wtype
        self.mstruct=fuzz_struct
        self.mrequest=None
        self.encoder=encoder#encoder
        self.initialize()
        

    def initialize(self):
        primitives.bit_field.bit_maxRange=FuzzGenerator.sulleyTable['bit_maxRange']
        primitives.bit_field.bit_minEdge=FuzzGenerator.sulleyTable['bit_minEdge']
        s_initialize(self.mblockname)
        if self.mtype&0x2:
            s_size('value',length=1,fuzzable=FuzzGenerator.sulleyTable['Lfuzz'])
        if s_GSMblock_start('value',encoder=self.encoder):
            for items in self.mstruct:
                string=items
                if string[0]=='b' and string[1]!='s':
                    string=string[1:]
                    splitstr=string.split(',')
                    if len(splitstr)>1:
                        [lenstr,datastr]=splitstr
                    else:
                        lenstr=splitstr[0]
                        datastr='0'
                    s_bit_field(int(datastr),int(lenstr),full_range=FuzzGenerator.sulleyTable['bit_testall'])
                if string[0:4]=='text':
                    self.text_process(string[5:])
                if string[0:2]=='bs':
                    string=string[2:]
                    [lenstr,datastr]=string.split(',')
                    s_bit_field(int(datastr),int(lenstr),fuzzable=False)
                if string[0]=='i':
                    string=string[1:]
                    paralist=string.split(',')
                    min_re=FuzzGenerator.sulleyTable['rand_minreps']
                    max_re=FuzzGenerator.sulleyTable['rand_maxreps']
                    wmax_nums=FuzzGenerator.sulleyTable['rand_maxmuns']
                    if len(paralist)==5:
                        [value,width,min_re,max_re,wstep]=paralist
                        wstep=int(wstep)
                        wmax_nums=None
                    else:
                        value=paralist[0]
                        width=paralist[1]
                        wstep=None
                    print min_re,max_re,wstep,wmax_nums
                    s_GSMbit_field(int(value),int(width),int(min_re),int(max_re),step=wstep,max_num=wmax_nums)
                if string[0]=='r':
                    self.random_process(string[1:])
            s_block_end()
            self.mrequest=s_get()
        
    def text_process(self,string):
        tem=string.split(',')
        mvalue=FuzzGenerator.sulleyTable['str_value']
        mmaxlen=FuzzGenerator.sulleyTable['str_maxlen']
        for item in tem:
            if item[0:5]=='value':
                mvalue=item[6:]
            elif item[0:7]=='max_len':
                mmaxlen=int(item[8:])
        s_string(value=mvalue,max_len=mmaxlen)
    def random_process(self,string):
        min_length=FuzzGenerator.sulleyTable['rand_minreps']
        max_length=FuzzGenerator.sulleyTable['rand_maxreps']
        max_nums=FuzzGenerator.sulleyTable['rand_maxmuns']
        str_list=string.split(",")
        value=int(str_list[0])
        if len(str_list)==4:
            min_length=int(str_list[1])
            max_length=int(str_list[2])
            max_nums=int(str_list[3])
        s_random(value,min_length,max_length,max_nums)

    def get_render(self):
        rendered=self.mrequest.render()
        #iei out value
        if self.mtype&0x04:
            rendered=struct.pack('B',self.mIEI)+rendered
        #IEI in value
        if self.mtype&0x08:
            t_res=rendered[0]
            (t_res,)=struct.unpack('B',t_res)
            t_res=t_res|self.mIEI
            rendered=struct.pack('B',t_res)+rendered[1:]
        return rendered
                

    def generate(self):
        self.mrequest.reset()
        yield self.get_render()
        while self.mrequest.mutate():
            yield self.get_render()

#================================sms class================================================
class SMSGenerator:
    def __init__(self,filename):
        if isinstance(filename,list):
            self.fuzzdata=[]
            self.fn=filename
            for file_id in filename:
                self.fuzzdata+=fuzzutils.file2cases(file_id)
                
        else:
            self.fn=filename
            self.fuzzdata=fuzzutils.file2cases(self.fn)
    def generate(self):
        for line in self.fuzzdata:
            (data,length)=line
            data=hex2bin(data,0)
            yield data



    


if __name__=='__main__':
    fg=FuzzGenerator(['i8,4'],Lfuzzable=0)
    pool=[]
    for i in fg.generate():
        pool.append(i)
        print s_hex_dump(i)
    print len(pool)
    
        

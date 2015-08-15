import Utils

def transfer(fn1,fn2):
    out=[]
    f1=open(fn1,'r')
    for line in f1.readlines():
        out.append(line.rstrip('\n')+' '+str(len(line)))
    f1.close()
    f2=open(fn2,'w')
    for item in out:
        f2.write(item+'\n')
    f2.close()


if __name__=='__main__':
    transfer('sms_fuzzing.sulley','sms_sulley.cases')

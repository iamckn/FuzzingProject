from sulley import *

s_initialize("SMS")
s_string('@',max_len=140)
request=s_get()

def getSMSFuzzData():
    if s_mutate():
        return s_render()
    else:
        request.reset()
        return s_render()

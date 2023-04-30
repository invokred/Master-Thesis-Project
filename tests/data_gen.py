import numpy as np
import random
from datetime import datetime
#day month year hour minute second  //12 feb 2020 13:05:06 --> 12 2 2020 13 5 6
def generate_latlon(func):
    NUM_BOXES = 100
    NUM_RIDERS = 10
    BIN_LEN = 100
    BIN_BRE = 80
    BIN_HEI = 80
    MEAN_LEN = 40
    MEAN_BRE = 40
    MEAN_HEI = 20
    longi = random.randint(77469960, 77703250)/(1e6)
    lat = random.randint(12873190, 13065670)/(1e6)
    s=""
    s+=str(longi)
    s+=" "
    s+=str(lat)
    s+="\n"
    longit = func(longi,0.1 , size = NUM_BOXES)
    lati = func(lat, 0.1 , size = NUM_BOXES)
    lat=""
    longi=""
    s+=str(BIN_LEN)
    s+=" "
    s+=str(BIN_BRE)
    s+=" "
    s+=str(BIN_HEI)
    s+="\n"
    s+=str(NUM_RIDERS)
    s+="\n"
    s+=str(NUM_BOXES)
    s+="\n"
    leng = func(MEAN_LEN, 7.5 , size = NUM_BOXES)
    bre = func(MEAN_BRE, 7.5 , size = NUM_BOXES)
    hei = func(MEAN_HEI, 3 , size = NUM_BOXES)
    for i in range(1,NUM_BOXES+1):
        longi = str(longit[i-1])
        lat = str(lati[i-1])
        s+=longi
        s+=" "
        s+=lat
        s+=" "
        s+=str(leng[i-1])
        s+=" "
        s+=str(bre[i-1])
        s+=" "
        s+=str(hei[i-1])
        day = random.randint(12, 15)
        month = 10
        year = 2022
        hour = random.randint(10, 17)
        minute = random.randint(0,59)
        second = random.randint(0,59)
        # t = datetime(year, month, day, hour, minute, second)
        # s+=" "
        # s+=str(t.timestamp())
        s+="\n"
    return s
func = np.random.default_rng().normal
s = generate_latlon(func)
f = open("output.txt", "w")
f.write(s)




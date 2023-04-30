import numpy as np
import random

#day month year hour minute second  //12 feb 2020 13:05:06 --> 12 2 2020 13 5 6

def generate_latlon(func):
    NUM_CLUSTERS = 5
    NUM_BOXES = 120
    NUM_BOXES_PER_CLUSTER = (int)(NUM_BOXES*1.0/NUM_CLUSTERS)
    BIN_LEN = 100
    BIN_BRE = 80
    BIN_HEI = 80
    MEAN_LEN = 20
    MEAN_BRE = 20
    MEAN_HEI = 10
    NUM_RIDERS = 5

    # longi = random.randint(77469960, 77703250)/(1e6)    # warehouse longitude
    # lat = random.randint(12873190, 13065670)/(1e6)      # warehouse latitude
    lat = 12.97095
    longi = 77.59058
    s=""
    s+=str(lat)
    s+=" "
    s+=str(longi)
    s+="\n"
    clustercentrelongi = func(longi,0.05 , size = NUM_CLUSTERS)
    clustercentrelati = func(lat, 0.05 , size = NUM_CLUSTERS)
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

    for i in range(1,NUM_CLUSTERS+1):
        longit = func(clustercentrelongi[i-1], 0.01 , size = NUM_BOXES_PER_CLUSTER)
        lati = func(clustercentrelati[i-1], 0.01 , size = NUM_BOXES_PER_CLUSTER)
        leng = func(MEAN_LEN, 7.5 , size = NUM_BOXES_PER_CLUSTER)
        bre = func(MEAN_BRE, 7.5 , size = NUM_BOXES_PER_CLUSTER)
        hei = func(MEAN_HEI, 3 , size = NUM_BOXES_PER_CLUSTER)
        edd_min = unif_func.integers(480, 1020, size = NUM_BOXES_PER_CLUSTER)

        for i in range(1, NUM_BOXES_PER_CLUSTER+1):
            longi = str(longit[i-1])
            lat = str(lati[i-1])
            s+=lat
            s+=" "
            s+=longi
            s+=" "
            s+=str(leng[i-1])
            s+=" "
            s+=str(bre[i-1])
            s+=" "
            s+=str(hei[i-1])
            s+= " "
            s+= str(edd_min[i-1])
            s+= "\n"

    return s

def visualise_clusters(func):
    NUM_CLUSTERS = 5
    NUM_BOXES = 120
    NUM_BOXES_PER_CLUSTER = (int)(NUM_BOXES/NUM_CLUSTERS)
    BIN_LEN = 100
    BIN_BRE = 80
    BIN_HEI = 80
    MEAN_LEN = 20
    MEAN_BRE = 20
    MEAN_HEI = 10
    NUM_RIDERS = 5

    longi = random.randint(77469960, 77703250)/(1e6)    # warehouse longitude
    lat = random.randint(12873190, 13065670)/(1e6)      # warehouse latitude
    s=""
    s+=str(NUM_CLUSTERS)
    s+="\n"
    clustercentrelongi = func(longi,0.1 , size = NUM_CLUSTERS)
    clustercentrelati = func(lat, 0.1 , size = NUM_CLUSTERS)

    for i in range(1,NUM_CLUSTERS+1):
        s+=str(i-1)
        s+="\n"

        longit = func(clustercentrelongi[i-1], 0.01 , size = NUM_BOXES_PER_CLUSTER)
        lati = func(clustercentrelati[i-1], 0.01 , size = NUM_BOXES_PER_CLUSTER)
        leng = func(MEAN_LEN, 7.5 , size = NUM_BOXES_PER_CLUSTER)
        bre = func(MEAN_BRE, 7.5 , size = NUM_BOXES_PER_CLUSTER)
        hei = func(MEAN_HEI, 3 , size = NUM_BOXES_PER_CLUSTER)
        edd_min = unif_func.integers(480, 1020, size = NUM_BOXES_PER_CLUSTER)

        s+=str(NUM_BOXES_PER_CLUSTER+2)
        s+="\n"
        s+=str(clustercentrelati[i-1])
        s+=" "
        s+=str(clustercentrelongi[i-1])
        s+="\n"

        for j in range(1, NUM_BOXES_PER_CLUSTER+1):
            longi = str(longit[j-1])
            lat = str(lati[j-1])
            s+=lat
            s+=" "
            s+=longi
            s+= "\n"
        
        s+=str(clustercentrelati[i-1])
        s+=" "
        s+=str(clustercentrelongi[i-1])
        s+="\n"

    return s

func = np.random.default_rng().normal
unif_func = np.random.default_rng()
# s = generate_latlon(func)
s = visualise_clusters(func)
f = open("./input.txt", "w")
f.write(s)
f.close()
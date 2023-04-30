import os
import subprocess
import multiprocessing
from subprocess import Popen, PIPE
import numpy as np
import pygsheets

INPUT_DIRS = ["./VRP/A", "./VRP/B", "./VRP/E", "./VRP/F", "./VRP/G", "./VRP/M", "./VRP/P", "./VRP/V"] 

client = pygsheets.authorize(service_file='growsimplee-374918-39a3d1fb0df3.json')
sh = client.open('Optimisation Experiments')
wks = sh.sheet1

def run_each(index):
    global wks
    params = algo_combs[index]
    print("Starting process with params: {}",params)
    s_proc = subprocess.Popen(["./hgs", params["input"], "-seed 1 -t 30"],
                            stdout = PIPE, stderr = PIPE)
    stdout, stderr = s_proc.communicate()
    metrics = [-1, -1, -1]
    try:
        metrics = [float(x) for x in stdout.decode("utf-8").split('\n')[-3:-1]]

    except: 
        print("Decoding error")
        # print(stdout)
        # print(stderr,stdout)
    print(metrics)
    data = [1, params["input"], params["clustering"], params["routing"], params["binpacking"], str(metrics[1])+" , "+str(metrics[0]), metrics[2], str(metrics[3])+"/"+str(metrics[4])]
    # print(wks)
    wks.append_table(data)
    return metrics

def create_algo_combs():
    params = []
    for data_dir in INPUT_DIRS:
        filenames = os.listdir(data_dir)
        filenames = [x for x in filenames if "vrp" in x]
        for fname in filenames:
            param = {
                "input": data_dir + "/" + fname
            }
        print(param)
        params.append(param)
    return params


algo_combs = create_algo_combs()
num_combs = len(algo_combs)

num_combs = 2

def main():
    # print("HAKUNAA MATATA")
    POOL_COUNT = multiprocessing.cpu_count()
    pool = multiprocessing.Pool(POOL_COUNT)
    cost_lists = pool.map(run_each, range(num_combs))
    pool.close()
    pool.join()
    for i in range(num_combs):
        print(algo_combs[i], cost_lists[i])


main()







    


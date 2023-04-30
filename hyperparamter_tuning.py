import wandb
wandb.login()
import numpy as np 
import random
import subprocess
# 🐝 Step 1: Define training function that takes in hyperparameter 
# values from `wandb.config` and uses them to train a model and return metric

# 🐝 Step 2: Define sweep config
sweep_configuration = {
    'method': 'bayes',
    'name': 'sweep',
    'metric': {'goal': 'minimize', 'name': 'distance'},
    'parameters': 
    {
        'penaltyDuration': {'distribution': 'log_uniform','min':-2.30258,'max':4.605},# from 0.1 to 100
        'penaltyCapacity': {'distribution': 'log_uniform','min':-2.30258,'max':4.605},# from 0.1 to 100
     }
}
def main():
    # Use the wandb.init() API to generate a background process 
    # to sync and log data as a Weights and Biases run.
    # Optionally provide the name of the project. 
    run = wandb.init(project='tuneHGS')

    # note that we define values from `wandb.config` instead of 
    # defining hard values
    penaltyDuration  =  wandb.config.penaltyDuration
    penaltyCapacity  =  wandb.config.penaltyCapacity
    print('Starting ./Integrate')
    result = subprocess.run(f'./Integrate {penaltyCapacity} {penaltyDuration} < input_text.txt', shell=True,stdout=subprocess.PIPE)
    print('Exectution Done') 
    line = result.stdout.decode('utf-8').split('\n')[-2]
    distance = float(line.split(' ')[-1]) 
    wandb.log({
        'penaltyDuration': float(penaltyDuration),
        'penaltyCapacity': float(penaltyCapacity),
        'distance': distance
      })
def main2():
  print('Starting ./Integrate')
  # subprocess.run(['ls','-l'])
  input =''
  # with open('input_text.txt', 'r') as file:
    # input = file.read().encode('utf-8')

  # p = subprocess.Popen(['./Integrate','1.0','10.0','<', 'input_text.txt'],stdout = subprocess.PIPE, stderr = subprocess.PIPE)
  # result,stderr = p.communicate()

  result = subprocess.run('./Integrate 1.0 10.0 < input_text.txt', shell=True,stdout=subprocess.PIPE)
  print('Exectution Done') 
  print(result)
  line = result.stdout.decode('utf-8').split('\n')[-2]
  print(line)
  distance = float(line.split(' ')[-1])
  print(distance)

if __name__ == '__main__':
    
    # 🐝 Step 3: Initialize sweep by passing in config
  sweep_id = wandb.sweep(sweep=sweep_configuration, project='tuneHGS')
  print(sweep_id)
    # 🐝 Step 4: Call to `wandb.agent` to start a sweep
  wandb.agent(sweep_id, function=main, count=20)
  # main2()

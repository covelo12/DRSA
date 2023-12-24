import time
import random
from psgen import psgen
import matplotlib.pyplot as plt
from base64 import b64encode
import numpy as np
import sys

def generate_confusion_strings(size_confString, num_iterations):
    iterations = []
    setupTimes = []

    for i in range(num_iterations):
        buffer = bytearray(size_confString + 1)
        with open("/dev/urandom", "rb") as urandom:
            urandom.readinto(buffer)

        buf_confString = buffer[0:size_confString]
        confString = b64encode(buf_confString).decode('utf-8')
        confString= confString[0:size_confString]
        iteration = buffer[-size_confString]%10
        if (iteration == 0):
            iteration = 1

        elapsedTime, final = time_randgen("password", confString, iteration)
        setupTimes.append( elapsedTime)
        iterations.append(iteration)

    return iterations,setupTimes ,final


def time_randgen(password, confString, iterations):
    startTime = time.time()
    final = psgen(password, confString, iterations)
    endTime = time.time()
    elapsedTime = (endTime - startTime) * 1000  # Convert to milliseconds
    return elapsedTime ,final

def get_average(array):
    suma = sum(array)
    leng= len(array)
    return suma/leng

def mesure_speed():
    iterations1, setupTimes1, final = generate_confusion_strings(1, 50)
    iterations2, setupTimes2, final = generate_confusion_strings(2, 50)
    iterations3, setupTimes3, final = generate_confusion_strings(3, 50)
    iterations4, setupTimes4, final = generate_confusion_strings(4, 50)

    sizes=[1,2,3,4]
    iterations=[1,2,3,4,5,6,7,8,9,10]
    times_for_sizes=[]
    times_for_iteraitons=[]
    for i in range(0,len(iterations)):
        times_for_iteraitons.append(0)

    times_for_sizes.append(get_average(setupTimes1))
    times_for_sizes.append(get_average(setupTimes2))
    times_for_sizes.append(get_average(setupTimes3))
    times_for_sizes.append(get_average(setupTimes4))


    for i in range(0, len(iterations3)):
        times_for_iteraitons[iterations1[i]]+=setupTimes1[i]
        times_for_iteraitons[iterations2[i]]+=setupTimes2[i]
        times_for_iteraitons[iterations3[i]]+=setupTimes3[i]
        times_for_iteraitons[iterations4[i]]+=setupTimes4[i]

    for i in range(0,len(times_for_iteraitons)):
        times_for_iteraitons[i]=times_for_iteraitons[i]/len(iterations1)
        times_for_iteraitons[i]=times_for_iteraitons[i]/len(iterations1)
        times_for_iteraitons[i]=times_for_iteraitons[i]/len(iterations1)
        
    # Plotting execution time vs. size of confString
    plt.figure()
    plt.plot(sizes, times_for_sizes, marker='o')
    plt.xlabel('Size of confString')
    plt.ylabel('Average Execution Time (ms)')
    plt.title('Execution Time vs. Size of confString')
    plt.savefig("plot_confstring_size_vs_time.png")
    plt.show()
    
    plt.figure()
    plt.plot(iterations, times_for_iteraitons, marker='o')
    plt.xlabel('Number of Iterations')
    plt.ylabel('Average Execution Time (ms)')
    plt.title('Execution Time vs. Number of Iterations')
    plt.savefig("iterations_vs_time.png")
    plt.show()
 
def output_bytes():
    size_confString=random.randint(1,4)
    size_password=random.randint(1,10)
    buffer = bytearray(size_confString + 1+size_password)
    with open("/dev/urandom", "rb") as urandom:
        urandom.readinto(buffer)

    buf_confString = buffer[0:size_confString]
    confString = b64encode(buf_confString).decode('utf-8')
    confString= confString[0:size_confString]

    buf_password = buffer[size_confString:size_password]
    password = b64encode(buf_password).decode('utf-8')
    password= confString[0:size_password]

    iteration = buffer[-size_confString]%10
    if (iteration == 0):
        iteration = 1
        

    result=psgen(password,confString,iteration)
    return(result.encode())

if __name__ == "__main__":
    if len(sys.argv) !=2:
        print("Usage: python randgen.py  <mode: 1->speed_test 2->output_rand>")
        sys.exit(1)
    mode = sys.argv[1]
    if mode == "1":
        mesure_speed()
    elif mode == "2":
        result=output_bytes()
        sys.stdout.write(result.decode())

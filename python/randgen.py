import time
import random
from psgen import psgen
import matplotlib.pyplot as plt
from base64 import b64encode

def generate_confusion_strings(size_confString, num_iterations):
    confusionStrings = []
    setupTimes = []

    for i in range(num_iterations):
        buffer = bytearray(size_confString + 1)
        with open("/dev/urandom", "rb") as urandom:
            urandom.readinto(buffer)

        buf_confString = buffer[0:size_confString]
        confString = b64encode(buf_confString).decode('utf-8')
        confString= confString[0:size_confString]
        iterations = buffer[size_confString]%10
        if (iterations == 0):
            iterations = 1
        #print("Iterations: ", iterations )
        #print("confString: ", confString, "\n")

        elapsedTime, final = time_randgen("password", confString, iterations)
        setupTimes.append((confString, iterations, elapsedTime))
        confusionStrings.append(size_confString)  # Collecting the size of confusion strings

    return confusionStrings, setupTimes ,final


def time_randgen(password, confString, iterations):
    startTime = time.time()
    final = psgen(password, confString, iterations)
    endTime = time.time()
    elapsedTime = (endTime - startTime) * 1000  # Convert to milliseconds
    return elapsedTime ,final


def start():
    confusionStrings1, setupTimes1, final = generate_confusion_strings(1, 10)
    confusionStrings2, setupTimes2,final = generate_confusion_strings(2, 10)
    confusionStrings3, setupTimes3,final = generate_confusion_strings(3, 10)

    # Separate data for plotting
    iterations1 = [iteration for (_, iteration, _) in setupTimes1]
    elapsedTimes1 = [elapsedTime for (_, _, elapsedTime) in setupTimes1]

    iterations2 = [iteration for (_, iteration, _) in setupTimes2]
    elapsedTimes2 = [elapsedTime for (_, _, elapsedTime) in setupTimes2]

    iterations3 = [iteration for (_, iteration, _) in setupTimes3]
    elapsedTimes3 = [elapsedTime for (_, _, elapsedTime) in setupTimes3]

    # Plot for iterations vs. elapsed time
    plt.figure(figsize=(10, 6))
    plt.xlabel("Iterations")
    plt.ylabel("Elapsed Time (ms)")
    plt.title("Elapsed Time vs. Iterations")
    plt.scatter(iterations1, elapsedTimes1, label="1 Char ConfString")
    plt.scatter(iterations2, elapsedTimes2, label="2 Char ConfString")
    plt.scatter(iterations3, elapsedTimes3, label="3 Char ConfString")
    plt.legend()
    plt.ylim(0, 2000)
    plt.savefig("plot_iterations_vs_time.png")

    # Plot for confusion string size vs. elapsed time
    plt.figure(2)
    plt.xlabel("Size of ConfString")
    plt.ylabel("Elapsed Time (ms)")
    plt.title("Elapsed Time vs. Size of ConfString")
    plt.scatter(confusionStrings1 + confusionStrings2 + confusionStrings3, elapsedTimes1 + elapsedTimes2 +elapsedTimes3)

    plt.savefig("plot_confstring_size_vs_time.png")

if __name__ == "__main__":
    start()

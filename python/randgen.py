import time
from random import randint
from  psgen import psgen

def time_randgen(password, confString, iterations):
    startTime = time.time()

    final = psgen(password, confString, iterations)

    endTime = time.time()
    elapsedTime = (endTime - startTime) * 1000  # Convert to milliseconds

    return elapsedTime

def main():
    confusionStrings = ["aa", "d", "n", "nop"]
    numIterations = [1, 2, 3]

    setupTimes = []

    k = 0
    for confString in confusionStrings:
        for iteration in numIterations:
            k += 1
            elapsedTime = time_randgen("password", confString, iteration)
            setupTimes.append((confString, iteration, elapsedTime))
            print(f"iteration {k} done, Iterations: {iteration} and ConfString {confString}")

    # Generate and display scatter plot data
    print("Scatter plot data:")
    for confString, iteration, time in setupTimes:
        print(f"{confString} (Iterations: {iteration}): {time} ms")

    # Increment setup times by the number of iterations
    for i in range(len(setupTimes)):
        confString, iteration, time = setupTimes[i]
        setupTimes[i] = (confString, iteration, time + iteration)

    # Sort the setup times
    setupTimes.sort(key=lambda x: x[2])

    # Create and display line chart data
    print("Line chart data:")
    for confString, iteration, time in setupTimes:
        print(f"{confString} (Iterations: {iteration}): {time} ms")

if __name__ == "__main__":
    main()

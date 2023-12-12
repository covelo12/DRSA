#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>
#include <vector>
#include <openssl/des.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "psgen.cpp"
#include <iostream>
#include <chrono>
#include <vector>

using namespace std;

vector<string> confusionStrings;
vector<int> numIterations;  
vector<chrono::duration<double, milli>> setupTime;

void timeRandgen(string password, string confString, int iterations) {
  auto startTime = chrono::high_resolution_clock::now();

  string final = randgen(password, confString, iterations);

  auto endTime = chrono::high_resolution_clock::now();
  chrono::duration<double, milli> elapsedTime = endTime - startTime;

  // Add the setup time to the setupTime vector
  setupTime.push_back(elapsedTime);
}


int main() {
  // Measure the setup time for different confusion strings and number of iterations
  confusionStrings.push_back("aa");
  confusionStrings.push_back("d");
  confusionStrings.push_back("n");
  confusionStrings.push_back("nop");

  numIterations.push_back(1);
  numIterations.push_back(2);
  numIterations.push_back(3);

  int k=0;
  for (int i = 0; i < confusionStrings.size(); i++) {
    for (int j = 0; j < numIterations.size(); j++) {
      k++;
      timeRandgen("password", confusionStrings[i], numIterations[j]);
      printf("iteration %d done, Iterations: %d  and ConfString %s \n", k  , numIterations[j], confusionStrings[i].c_str());
    }
  }
  // Generate a scatter plot of the setup time vs. confusion string and number of iterations
  cout << "Generating scatter plot..." << endl;
  for (int i = 0; i < confusionStrings.size(); i++) {
    cout << confusionStrings[i] << ": " << setupTime[i].count() << endl;
  }

  // Increment the setup time by the number of iterations
for (int i = 0; i < setupTime.size(); i++) {
    setupTime[i] += chrono::duration<double, milli>(numIterations[i]);
}

  // Sort the setup time and confusionStrings vectors by their respective values
  sort(setupTime.begin(), setupTime.end());
  sort(confusionStrings.begin(), confusionStrings.end());

  // Create a line chart with the setup time vs. confusion string
  cout << "Creating line chart..." << endl;
  for (int i = 0; i < setupTime.size(); i++) {
    cout << confusionStrings[i] << ": " << setupTime[i].count() << endl;
  }

  return 0;
}

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "psgen.cpp"
#include <fstream>
// Other necessary includes

// Function declarations
void generate_confusion_strings(int size_confString, int num_iterations, std::vector<int>& iterations, std::vector<double>& setupTimes, std::string& final);
double time_randgen(std::string password,std::string confString, int iterations);
double get_average(const std::vector<double>& array);
int mesure_speed() {

    std::vector<double> iteration_time(11, 0.0);
    std::vector<int> iterations;
    std::vector<double> setupTimes;

    std::vector<int> iterations2;
    std::vector<double> setupTimes2;

    std::vector<int> iterations3;
    std::vector<double> setupTimes3;

    std::vector<int> iterations4;
    std::vector<double> setupTimes4;

    std::string final;
    generate_confusion_strings(1, 30, iterations, setupTimes, final);
    generate_confusion_strings(2, 30, iterations2, setupTimes2, final);
    generate_confusion_strings(3, 30, iterations3, setupTimes3, final);
    generate_confusion_strings(4, 30, iterations4, setupTimes4, final);

    std::cout << "Average setup time with one char ConfString: " << get_average(setupTimes) << "ms" << std::endl;
    std::cout << "Average setup time with two char ConfString: " << get_average(setupTimes2) << "ms" << std::endl;
    std::cout << "Average setup time with three char ConfString: " << get_average(setupTimes3) << "ms" << std::endl;
    std::cout << "Average setup time with four char ConfString: " << get_average(setupTimes4) << "ms" << std::endl;

    std::vector<int> aux(11, 0);
    for ( int i  =0; i<iterations.size();i++)
    {
        iteration_time[iterations[i]]= iteration_time[iterations[i]] + setupTimes[i];
        iteration_time[iterations2[i]]= iteration_time[iterations2[i]] + setupTimes2[i];
        iteration_time[iterations3[i]]= iteration_time[iterations3[i]] + setupTimes3[i];
        iteration_time[iterations4[i]]=iteration_time[iterations4[i]] + setupTimes4[i];
        aux[iterations[i]]=aux[iterations[i]]+1;
        aux[iterations2[i]]=aux[iterations2[i]]+1;
        aux[iterations3[i]]=aux[iterations3[i]]+1;
        aux[iterations4[i]]=aux[iterations4[i]]+1;
    }
    for(int i =0; i<=   10;i++)
    {
        int k;
        if(iteration_time[i] != 0){
            iteration_time[i]=iteration_time[i]/aux[i];
            k=i+1;
            std::cout << "Average time for " << k << " iterations: " << iteration_time[i] << std::endl;
        }
    }
    return 0;
}

std::vector<uint8_t> output_bytes();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./randgen <mode: 1->speed_test 2->output_rand>\n";
        return 1;
    }

    try {
        int mode = std::stoi(argv[1]);
        if (mode == 1) {
            mesure_speed();
        } else if (mode == 2) {
            auto result = output_bytes();
            // Output result
            for (auto byte : result) {
                std::cout << byte;
            }
            std::cout << std::endl;
        }
    } catch (const std::invalid_argument& ia) {
        std::cerr << "Invalid argument: " << ia.what() << '\n';
        return 1;
    } catch (const std::out_of_range& oor) {
        std::cerr << "Argument out of range: " << oor.what() << '\n';
        return 1;
    }
    return 0;
}

double time_randgen(std::string password,std::string confString, int iterations) {
    auto startTime = std::chrono::high_resolution_clock::now();

    std::string final = psgen(password,confString,iterations); 
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsedTime = endTime - startTime;

    return elapsedTime.count();
}

void generate_confusion_strings(int size_confString, int num_iterations, std::vector<int>& iterations, std::vector<double>& setupTimes, std::string& final) {
    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
    if (!urandom) {
        std::cerr << "Failed to open /dev/urandom" << std::endl;
        return;
    }

    for (int i = 0; i < num_iterations; ++i) {
        std::vector<char> buffer(size_confString + 1);
        if (!urandom.read(buffer.data(), buffer.size())) {
            std::cerr << "Failed to read from /dev/urandom" << std::endl;
            break;
        }

        std::string confString(buffer.begin(), buffer.end() - 1);
        int iteration = buffer.back() % 10;
        if (iteration == 0) iteration = 1;
        if(iteration<0) iteration=-iteration;

        double elapsedTime = time_randgen("password", confString, iteration);
        setupTimes.push_back(elapsedTime);
        iterations.push_back(iteration);
    }

    urandom.close();
    final = "Some final value"; // Replace with actual final value logic if needed
}

double get_average(const std::vector<double>& array) {
    double sum = std::accumulate(array.begin(), array.end(), 0.0);
    return sum / array.size();
}

std::vector<uint8_t> output_bytes() {
    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
    if (!urandom) {
        std::cerr << "Failed to open /dev/urandom" << std::endl;
        return {};
    }

    std::random_device rd;
    std::uniform_int_distribution<> size_dist(1, 4);
    std::uniform_int_distribution<> password_size_dist(1, 10);

    int size_confString = size_dist(rd);
    int size_password = password_size_dist(rd);

    std::vector<uint8_t> buffer(size_confString + 1 + size_password);
    if (!urandom.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
        std::cerr << "Failed to read from /dev/urandom" << std::endl;
        return {};
    }

    urandom.close();

    std::string confString(buffer.begin(), buffer.begin() + size_confString);
    std::string password(buffer.begin() + size_confString, buffer.begin() + size_confString + size_password);

    int iteration = buffer.back() % 10;
    if (iteration == 0) iteration = 1;

    printf("password: %s, String %s, Iterations: %d \n", password.c_str(),confString.c_str(),iteration);
    std::string result = psgen(password, confString, iteration);
    return std::vector<uint8_t>(result.begin(), result.end());

}



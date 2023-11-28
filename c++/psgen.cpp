#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cmath>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>

using namespace std;
namespace mp = boost::multiprecision;


std::string string_to_hex_utf8(const std::string& input) {
    std::ostringstream oss;
    for (const char& c : input) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
    }
    return oss.str();
}

string PBKDF2(string password, string salt, int keylen) {

    unsigned char key[keylen];
    PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.length(), (unsigned char *)salt.c_str(), salt.length(), 1000, keylen, key);
    
    stringstream ss;
    for(int i = 0; i < keylen; ++i)
        ss << hex << setw(2) << setfill('0') << (int)key[i];
    
    string s =ss.str();

    //printf("Primeira %s                               !!!!", s.c_str());
    return s;
}


int getIndex(string confString, string result) {
    size_t i = 0;
    while (i < result.size() && result.substr(i, confString.length()) != confString) {
        i++;
    }
    return i + confString.length();
}

string getResult(string password, string confString, string result) {
    int trys = 1000;
    string aux = string_to_hex_utf8(confString);
    while (result.find(aux) == string::npos) {
        trys += 500;
        result = PBKDF2(password, confString, trys);
    }
    return result;
}

int generate_prime_from_bytes(std::string random_bytes) {
    mp::cpp_int candidate_prime = 0;
    for (char c : random_bytes)
        candidate_prime = (candidate_prime << 8) | static_cast<int>(c);
    if (candidate_prime % 2 == 0)
        candidate_prime += 1;

    while (!mp::miller_rabin_test(candidate_prime, 25)) {
        candidate_prime += 2;
        std::cout << "A" << std::endl;
    }
    return candidate_prime.convert_to<int>();
}

void keygen(std::string keystream) {
    std::string key1 = keystream.substr(0, keystream.size()/2);
    std::string key2 = keystream.substr(keystream.size()/2);

    int p = generate_prime_from_bytes(key1);
    int q = generate_prime_from_bytes(key2);

    int n = p * q;

    int e = pow(2, 16) + 1;

    std::cout << "deu" << std::endl;
}

int main() {

    
    int iterations;
    string password, confString;
    cout << "Number of iterations ";
    cin >> iterations;
    cout << "Password ";
    cin >> password;
    cout << "Confusion String ";
    cin >> confString;

    string final = "", result = "";

    for (int k = 0; k < iterations; k++) {
        result = getResult(password, confString, result);
        int index = getIndex(confString, result);
        final += result.substr(0, index + 1);
        password = PBKDF2(password, confString, index + password.length()).substr(index);
        final += getResult(password, confString, result);
    }
    //printf("resultado , : %s ",final.c_str());
    keygen(final);

    return 0;
}


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
    //printf("password  %s \n",password.c_str());
    //printf("salt  %s \n",salt.c_str());
    //printf("leng %d",keylen);
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

string getResult(string password, string confString) {
    string result="";

    int trys = 1000;
    //string aux = string_to_hex_utf8(confString);
    while (result.find(confString) == string::npos) {
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
    std::string::size_type leng = keystream.length();
    std::string key1 = keystream.substr(0, leng);
    std::string key2 = keystream.substr(leng);


    int p = generate_prime_from_bytes(key1);
    int q = generate_prime_from_bytes(key2);

    int n = p * q;

    int e = pow(2, 16) + 1;

    std::cout << "deu" << std::endl;
}

int main() {

    
    int iterations;
    string password, confString;

    //cout << " ";
    //cin >> iterations;
    //cout << "Password ";
    //cin >> password;
    //cout << "Confusion String ";
    //cin >> confString;

    string final = "", result = "";
    password="ricardo";
    confString="H";
    iterations=4;

    
    password=string_to_hex_utf8(password);
    confString=string_to_hex_utf8(confString);
    
    // Use printf to print the string


    //printf("ConfString %s", confString.c_str());
    result=getResult(password,confString);
    
    //printf("resultado , : %s ",result.c_str()); //done1
    int index = getIndex(confString, result);
    //printf("index %d",index); done 2
    final += result.substr(0, index + 1);

    //std::cout << "resultado, : " << final << std::endl; //done3
    for (int k = 1; k < iterations; k++) {
        //printf("password %s, e confstring %s", password.c_str(), confString.c_str());
        password = PBKDF2(password, confString, index + password.length()).substr(0,password.size());
        //printf("passwrod %s \n", password.c_str()); done 4

        result=getResult(password,confString);
        index=getIndex(confString,result);
        final += result.substr(0,index);
    }
    
    int leng = final.length();
    printf("final: %s",final.c_str());
    //keygen(final);

    return 0;
}



#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bn.h>
#include <openssl/pem.h>
#include <cmath>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
using boost::multiprecision::cpp_int;
#include <boost/multiprecision/miller_rabin.hpp>



using namespace std;
namespace mp = boost::multiprecision;

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


std::string bytes_to_hex(const std::string &input) {
    std::string hex_string;
    for (unsigned char byte : input) {
        char hex_byte[3];
        snprintf(hex_byte, sizeof(hex_byte), "%02x", byte);
        hex_string += hex_byte;
    }
    return hex_string;
}
std::string encrypt_chacha20(const std::string &plaintext) {
    std::string key = plaintext.substr(0,32);
    if (plaintext.size() < 32) {
        throw std::invalid_argument("Key must be 32 bytes long");
    }

    std::vector<unsigned char> nonce = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EVP_CIPHER_CTX *ctx;
    std::string ciphertext(plaintext.size(), 0);
    int len;

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_chacha20(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), nonce.data());

    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.size());
    int total_len = len;

    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]) + len, &len);
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);

    // The size of ciphertext should already be equal to plaintext size, so no resize is needed
    return ciphertext;
}
string PBKDF2(string password, string salt) {
    password=bytes_to_hex(password);
    salt=bytes_to_hex(salt);
    int keylen= 256;
    unsigned char key[keylen];
    PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.length(), (unsigned char *)salt.c_str(), salt.length(), 1000, keylen, key);
    
    stringstream ss;
    for(int i = 0; i < keylen; ++i)
        ss << hex << setw(2) << setfill('0') << (int)key[i];
    
    string s =ss.str();
    return s;
}
string getResult(string password, string confString) {
    string result="";
    result=encrypt_chacha20(password);
    confString=bytes_to_hex(confString);
    std::string comp_result=bytes_to_hex(result);
    
    while (comp_result.find(confString) == string::npos) {
        result=encrypt_chacha20(result);
        comp_result = bytes_to_hex(result);
    }

    return result;
}
string randgen(string password, string confString, int iterations){
    string final = "", result = "";
    std::string seed=PBKDF2(password,confString);
    for (int k = 0; k < iterations; k++) {
        result = getResult(seed,confString);
        final= result;
        seed= getResult(result,confString);
    }
    return bytes_to_hex(final);
}
std::string psgen(std::string password, std::string confString, int iterations) {    
    string final  = randgen(password, confString, iterations);
    return final;
}

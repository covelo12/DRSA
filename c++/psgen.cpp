// Todo testar em VM

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
std::string to_base64(const boost::multiprecision::cpp_int& value) {
    // First, convert cpp_int to a binary representation
    std::string binary;
    export_bits(value, std::back_inserter(binary), 8);

    // Prepare OpenSSL for base64 encoding
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // Disable newline for base64 encoding
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    // Write binary data to base64
    BIO_write(bio, binary.data(), binary.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);

    // Create a std::string from the BUF_MEM structure
    std::string base64(bufferPtr->data, bufferPtr->length);

    // Clean up
    BIO_free_all(bio);
    BUF_MEM_free(bufferPtr);

    return base64;
}
unsigned char *base64_decode(const char* base64data, int* len) {
   BIO *b64, *bmem;
   size_t length = strlen(base64data);
   unsigned char *buffer = (unsigned char *)malloc(length);
   b64 = BIO_new(BIO_f_base64());
   BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
   bmem = BIO_new_mem_buf((void*)base64data, length);
   bmem = BIO_push(b64, bmem);
   *len = BIO_read(bmem, buffer, length);
   BIO_free_all(bmem);
   return buffer;
}
BIGNUM* bignum_base64_decode(const char* base64bignum) {
   BIGNUM* bn = NULL;
   int len;
   unsigned char* data = base64_decode(base64bignum, &len);
   if (len) {
       bn = BN_bin2bn(data, len, NULL);
   }
   free(data);
   return bn;
}

EVP_PKEY* RSA_fromBase64(const char* modulus_b64, const char* exp_b64) {
    BIGNUM *n = bignum_base64_decode(modulus_b64);
    BIGNUM *e = bignum_base64_decode(exp_b64);

    if (!n) printf("Invalid encoding for modulus\n");
    if (!e) printf("Invalid encoding for public exponent\n");

    if (e && n) {
        EVP_PKEY* pRsaKey = EVP_PKEY_new();
        RSA* rsa = RSA_new();
        if (rsa) {
            if (RSA_set0_key(rsa, n, e, NULL)) {
                if (EVP_PKEY_set1_RSA(pRsaKey, rsa)) {
                    return pRsaKey;
                }
            }
            RSA_free(rsa);
        }
    } else {
        if (n) BN_free(n);
        if (e) BN_free(e);
    }
    return NULL;
}
int write_RSA_PEM(const char* modulus_b64, const char* exp_b64, const char* filename) {
    EVP_PKEY* pkey = RSA_fromBase64(modulus_b64, exp_b64);

    if (pkey == NULL) {
        fprintf(stderr, "an error occurred :(\n");
        return 2;
    } else {
        printf("success decoded into RSA public key\n");
        FILE* file = fopen(filename, "w");
        if (!file) {
            fprintf(stderr, "failed to open file: %s\n", filename);
            return 3;
        }
        PEM_write_PUBKEY(file, pkey);
        fflush(file);
        fclose(file);
        printf("written to file: %s\n", filename);
    }

    return 0;
}
cpp_int modInverse(cpp_int a, cpp_int m) {
    cpp_int m0 = m, t, q;
    cpp_int x0 = 0, x1 = 1;

    if (m == 1)
      return 0;

    // Apply extended Euclid Algorithm
    while (a > 1) {
        // q is quotient
        q = a / m;
        t = m;

        // m is remainder now, process same as Euclid's algo
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    // Make x1 positive
    if (x1 < 0)
       x1 += m0;

    return x1;
}
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
    //printf("password %s \n",password.c_str() );
    //printf("confstring %s \n",confString.c_str() ); 
    while (result.find(confString) == string::npos) {
        trys += 500;
        result = PBKDF2(password, confString, trys);
    }
    return result;
}
mp::cpp_int generate_prime_from_bytes(std::string random_bytes) {

    mp::cpp_int candidate_prime = 0;
    for (char c : random_bytes)
        candidate_prime = (candidate_prime << 8) | static_cast<int>(c);
    if (candidate_prime % 2 == 0)
        candidate_prime += 1;

    while (!mp::miller_rabin_test(candidate_prime, 25)) {
        candidate_prime += 2;
    }
    return candidate_prime;
}
void keygen(std::string keystream) {
    int leng = keystream.length();
    int leng_half = leng / 2;
    std::string key1 = keystream.substr(0, leng_half);
    std::string key2 = keystream.substr(leng_half);
    mp::cpp_int p = generate_prime_from_bytes(key1); 
    mp::cpp_int q = generate_prime_from_bytes(key2);


    mp::cpp_int n = p * q;

    long e = pow(2, 16) + 1;
    

    cpp_int phi = (p - 1) * (q - 1);
    cpp_int d = modInverse(e, phi);

    std::string n_base64 = to_base64(n);
    cout << "N"<< p <<endl;
    printf("B64n %s \n", n_base64.c_str());
    std::string e_base64 = to_base64(e);
    std::string d_base64 = to_base64(d);

    // Call the function to write the PEM file
    write_RSA_PEM(n_base64.c_str(), e_base64.c_str(), "public.pem");
    write_RSA_PEM(n_base64.c_str(), d_base64.c_str(), "private.pem");
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

    //printf("final: %s",final.c_str()); done 4
    keygen(final);

    return 0;
}


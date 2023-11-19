#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

string PBKDF2(string password, string salt, int keylen) {
    unsigned char key[keylen];
    PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.length(), (unsigned char *)salt.c_str(), salt.length(), 1000, keylen, key);
    
    stringstream ss;
    for(int i = 0; i < keylen; ++i)
        ss << hex << setw(2) << setfill('0') << (int)key[i];
    
    return ss.str();
}


int getIndex(string confString, string result) {
    size_t i = 0;
    while (result.substr(i, confString.length()) != confString) {
        i++;
    }
    return i + confString.length();
}
string getResult(string password, string confString, string result) {
    int trys = 1000;
    while (result.find(confString) == string::npos) {
        trys += 500;
        result = PBKDF2(password, confString, trys);
    }
    return result;
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

    stringstream ss;
    for(int i = 0; i < final.length(); ++i)
        ss << hex << setw(2) << setfill('0') << (int)final[i];
    
    cout << ss.str() << endl;

    return 0;
}


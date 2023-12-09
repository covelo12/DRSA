#include <iostream>
#include <fstream>
#include <cryptopp/pem.h>


int main() {
    // Load a PEM-encoded private key from a file
    CryptoPP::FileSource file("private_key.pem", true);
    std::string privateKeyPEM;
    CryptoPP::StringSink privateKeySink(privateKeyPEM);

    CryptoPP::PEM_Load(file, privateKeySink);

    std::cout << "Loaded PEM-encoded private key:\n" << privateKeyPEM << std::endl;

    return 0;
}

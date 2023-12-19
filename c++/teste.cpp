#include <iostream>
#include <string>
#include <vector>
#include <openssl/evp.h>
#include <iomanip>

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

int main() {
    std::string plaintext = "12345678901234567890123456789012"; // Hex representation of "Attack at dawn. MeCe.M"
    

    std::string ciphertext = encrypt_chacha20(plaintext);
    std::string dois = encrypt_chacha20(ciphertext);
    std::string tres = encrypt_chacha20(dois);
    std::string quatro = encrypt_chacha20(tres);

    printf("cipher %s \n",bytes_to_hex(ciphertext).c_str());
    printf("cipher %s \n",bytes_to_hex(dois).c_str());
    printf("cipher %s \n",bytes_to_hex(tres).c_str());
    printf("cipher %s \n",bytes_to_hex(quatro).c_str());    

    return 0;
}
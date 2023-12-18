#include <iostream>
#include <string>
#include <vector>
#include <openssl/evp.h>
#include <iomanip>

std::string string_to_hex_utf8(const std::string& input) {
    std::ostringstream oss;
    for (const char& c : input) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
    }
    return oss.str();
}
std::vector<unsigned char> encrypt_chacha20(const std::string &plaintext, const std::string &key) {
    std::vector<unsigned char> nonce = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,13,14,15,16};
    EVP_CIPHER_CTX *ctx;
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len;

    ctx = EVP_CIPHER_CTX_new();
    // Pass the nonce to the encryption context
    EVP_EncryptInit_ex(ctx, EVP_chacha20(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), nonce.data());
    
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.size());
    int total_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    total_len += len;

    ciphertext.resize(total_len);
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

int main() {
    std::string plaintext = "ricardo"; // Hex representation of "Attack at dawn. MeCe.M"
    std::string key = "12345678901234567890123456789012";

    std::vector<unsigned char> ciphertext = encrypt_chacha20(plaintext, key);

    std::stringstream ss;
    for (size_t i = 0; i < ciphertext.size(); i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)ciphertext[i];
    }
    std::string hex_ciphertext = ss.str();

    std::cout << "hex_ciphertext: " << hex_ciphertext << std::endl;

    return 0;
}
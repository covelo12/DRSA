#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

EVP_PKEY_CTX *ctx;
EVP_PKEY *key_pair = 0;
ENGINE *engine = 0;
const OSSL_PARAM *key_pair_params;
int max_size;
BIGNUM *bn;

int main(){
// Create an RSA key pair context with the default engine
ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, engine);
if (ctx == 0) {
    // Error
}

if (EVP_PKEY_keygen_init(ctx) <= 0) {
    // Error
}

// Define required key pair parameters
if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
    // Error
}

// You can define other parameters with other
// EVP_PKEY_CTX_set_rsa_ ... functions

// Generate key pair.
// The space for it is allocated by the generator
if (EVP_PKEY_keygen(ctx, &key_pair) <= 0) {
    // Error
}

key_pair_params = EVP_PKEY_gettable_params(key_pair);
if (key_pair_params == 0) {
    // Error
}

printf("Key pair parameter list:\n");
for (int i = 0; key_pair_params[i].key != 0; i++) {
    printf("\t%s (of type %d)\n", key_pair_params[i].key, key_pair_params[i].data_type);
}

if (EVP_PKEY_get_int_param(key_pair, "max-size", &max_size) == 1) {
    printf("max-size (block size without padding) = %d\n", max_size);
}

bn = BN_new();
if (EVP_PKEY_get_bn_param(key_pair, "e", &bn) == 1) {
    printf("e = %s\n", BN_bn2hex(bn));
}
if (EVP_PKEY_get_bn_param(key_pair, "d", &bn) == 1) {
    printf("d = %s\n", BN_bn2hex(bn));
}
if (EVP_PKEY_get_bn_param(key_pair, "n", &bn) == 1) {
    printf("n = %s\n", BN_bn2hex(bn));
}

BN_free(bn);

// Write the key pair to stdout (No private key encryption)
PEM_write_PrivateKey(stdout, key_pair, 0, 0, 0, 0, 0);

// Write the key pair to stdout (The private key is encrypted with AES + CBC and a key derived from the password "senha")
PEM_write_PrivateKey(stdout, key_pair, EVP_aes_128_cbc(), &"senha", 5, 0, 0);

// Write the public key of a key pair to stdout
PEM_write_PUBKEY(stdout, key_pair);
}

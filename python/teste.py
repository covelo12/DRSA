from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC

# Create the nonce
nonce = bytes([ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,12,13,14,15,16])

# Define the plaintext and key

def encrypt(data):
    nonce = bytes([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16])
    algorithm = algorithms.ChaCha20(data[:32], nonce)
    cipher = Cipher(algorithm, mode=None)
    encryptor = cipher.encryptor()
    return encryptor.update(data)

plaintext = b"12345678901234567890123456789012"


ct = encrypt(plaintext)
dois=encrypt(ct)
tres=encrypt(dois)
quatro=encrypt(tres)

print(ct.hex(),"\n")
print(dois.hex(),"\n")
print(tres.hex(),"\n")
print(quatro.hex(),"\n")
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC

# Create the nonce
nonce = bytes([ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,12,13,14,15,16])

# Define the plaintext and key
plaintext = b"ricardo"
key = b"12345678901234567890123456789012"

algorithm = algorithms.ChaCha20(key, nonce)
cipher = Cipher(algorithm, mode=None)
encryptor = cipher.encryptor()
ct = encryptor.update(plaintext)
print(ct.hex())
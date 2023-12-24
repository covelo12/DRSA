from cryptography . hazmat . primitives . kdf . pbkdf2 import PBKDF2HMAC
from cryptography . hazmat . primitives import hashes
from Crypto.Util import number
import sympy
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
import base64
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
def encrypt(data):
    nonce = bytes([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16])
    algorithm = algorithms.ChaCha20(data[:32], nonce)
    cipher = Cipher(algorithm, mode=None)
    encryptor = cipher.encryptor()
    return encryptor.update(data)

def getResult(password, confString):
    result = b""  # Initialize as bytes
    result = encrypt(password)  # Convert password to bytes
    while confString not in result.hex():
        result = encrypt(result)
    return result

def random_gen(pwd,salt):
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA1(),
        length=256,
        salt=salt.encode("utf-8"),
        iterations=1000,
    )
    key = kdf.derive(pwd)
    return key.hex()

def psgen(password,confString,iterations):

    password = password.encode("utf-8").hex()
    confString = confString.encode("utf-8").hex()
    final=""
    result = ""
    seed= random_gen(password.encode("utf-8"),confString)
    seed=seed.encode()
    for(k) in range(0, iterations):
        result =getResult(seed,confString)
        final=result
        seed= getResult(result,confString)
    return final.hex()

def generate_prime_from_bytes(random_bytes):
    candidate_prime = 0
    for c in random_bytes:
        candidate_prime = (candidate_prime << 8) | ord(c)
    if candidate_prime % 2 == 0:
        candidate_prime += 1
    while not number.isPrime(candidate_prime):
        candidate_prime += 2

    return candidate_prime
    
def keygen(keystream):
    leng=int(len(keystream))
    leng= leng//2
    key1= keystream[:leng]
    key2= keystream[leng:]

    p= generate_prime_from_bytes(key1)
    q= generate_prime_from_bytes(key2)


    return p,q

def generate_pem_files(p, q, private_pem_filename="private.pem", public_pem_filename="public.pem"): 
    n = p * q
    e = pow(2,16) + 1
    phi = (p - 1) * (q - 1)
    d = pow(e, -1, phi)
    private_numbers = rsa.RSAPrivateNumbers(
        p=p, q=q, d=d, dmp1=d%(p-1), dmq1=d%(q-1), iqmp=rsa.rsa_crt_iqmp(p, q),
        public_numbers=rsa.RSAPublicNumbers(e=e, n=n)
    )
    integer_bytes = e.to_bytes((e.bit_length() + 7) // 8, byteorder='big')
    encoded_bytes = base64.b64encode(integer_bytes)
    encoded_string = encoded_bytes.decode('utf-8')
    private_key = private_numbers.private_key(default_backend())

    private_pem_data = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.NoEncryption()
    )

    with open(private_pem_filename, 'wb') as pem_file:
        pem_file.write(private_pem_data)

    public_key = private_key.public_key()

    public_pem_data = public_key.public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    )
    # Write the public PEM data to a file
    with open(public_pem_filename, 'wb') as pem_file:
        pem_file.write(public_pem_data)


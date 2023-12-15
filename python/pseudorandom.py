from cryptography . hazmat . primitives . kdf . pbkdf2 import PBKDF2HMAC
from cryptography . hazmat . primitives import hashes
from Crypto.Util import number
import sympy
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
import base64
from Crypto.PublicKey import RSA
#Returns the index right after the confusion string and the result are equal
class StreamCipher:
    def __init__(self, seed):
        self.state = seed

    # LCG parameters
    a = 1664525
    c = 1013904223
    m = 2**32  # 4294967296

    def next_byte(self):
        # Generate a pseudo-random byte
        self.state = (self.a * self.state + self.c) % self.m
        return self.state >> 24  # Return the top 8 bits
def psgen(password,confString,iterations):

    password = password.encode("utf-8").hex()
    confString = confString.encode("utf-8").hex()
    final=""
    result = ""
    seed= random_gen(password,confString)
    for(k) in range(0, iterations):   
        result =getResult(password,confString,iterations)
        seed= result[-len(seed)*2:-len(seed)]
    final= final + result

    return final.hex()

def getResult(password, confString,iterations):
    cipher = StreamCipher(len(password+confString)+iterations)
    final = ""

    while confString not in final:
        next_char = str(cipher.next_byte()).encode("utf-8").hex()
        final += next_char
        print(next_char)

    return final
def random_gen(pwd,salt):
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA1(),
        length=len(pwd),
        salt=salt.encode("utf-8"),
        iterations=1000,
    )
    key = kdf.derive(pwd.encode("utf-8"))

    #print("primeiro:", key.hex())
    return key.hex()

random_bytes=psgen("password","f",1)
print(random_bytes)

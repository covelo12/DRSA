from cryptography . hazmat . primitives . kdf . pbkdf2 import PBKDF2HMAC
from cryptography . hazmat . primitives import hashes
from Crypto.Util import number
import sympy
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa

#Returns the index right after the confusion string and the result are equal
def getIndex( confString, result): 
    leng= len(confString.hex())
    i = 0
    while (str(result.hex())[i:i+leng] != str(confString.hex())):
        i = i + 1
    return i+leng

def getResult(password, confString):
    result=b''
    trys = 1000
    while ( str(confString.hex()) not in  str(result.hex()) ):
        trys = trys + 500
        result = random_gen(password,confString,trys)
    return result

def random_gen(pwd,salt,len):
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA1(),
        length=len,
        salt=salt,
        iterations=1000,
    )
    key = kdf.derive( pwd )
    #print("primeiro: ", str(key.hex()))
    return key

def psgen(password,confString,iterations):

    password = password.encode("utf-8")
    confString = confString.encode("utf-8")


    final=b''
    result = b''

    result= getResult(password,confString) 
    index=  getIndex(confString, result)
    final= final + result[:index+1]
    for(k) in range(1, iterations):   
        password=random_gen(password, confString,index+len(password))[index:]
        result =getResult(password,confString)
        index= getIndex(confString, result)
        final= final + result[:index+1]
    return final


def generate_prime_from_bytes(random_bytes):
    candidate_prime = number.bytes_to_long(random_bytes)
    #print(number.size(candidate_prime))
    if candidate_prime % 2 == 0:
       candidate_prime = candidate_prime+ 1
    while not number.isPrime(candidate_prime):
       candidate_prime = candidate_prime+ 2
    return candidate_prime
    

def keygen(keystream):
    leng=int(len(keystream))
    leng= leng//2
    #print(leng)
    key1= keystream[:leng]
    key2= keystream[leng:]

    p= generate_prime_from_bytes(key1)
    q= generate_prime_from_bytes(key2)


    return p,q



def generate_pem_files(p, q, private_pem_filename="private.pem", public_pem_filename="public.pem"):
    # Calculate n (modulus)
    n = p * q

    # Public exponent
    e = 2**16 + 1

    # Calculate phi(n)
    phi = (p - 1) * (q - 1)

    # Calculate the private exponent
    d = pow(e, -1, phi)

    # Generate the private key
    private_numbers = rsa.RSAPrivateNumbers(
        p=p, q=q, d=d, dmp1=d%(p-1), dmq1=d%(q-1), iqmp=rsa.rsa_crt_iqmp(p, q),
        public_numbers=rsa.RSAPublicNumbers(e=e, n=n)
    )
    private_key = private_numbers.private_key(default_backend())

    # Serialize the private key to PEM format
    private_pem_data = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.NoEncryption()
    )

    # Write the private PEM data to a file
    with open(private_pem_filename, 'wb') as pem_file:
        pem_file.write(private_pem_data)

    # Get the public key from the private key
    public_key = private_key.public_key()

    # Serialize the public key to PEM format
    public_pem_data = public_key.public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    )

    # Write the public PEM data to a file
    with open(public_pem_filename, 'wb') as pem_file:
        pem_file.write(public_pem_data)



random_bytes=psgen("asda","l",2)
print(len(random_bytes))
p,q=keygen(random_bytes)
generate_pem_files(p,q)


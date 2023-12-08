from cryptography . hazmat . primitives . kdf . pbkdf2 import PBKDF2HMAC
from cryptography . hazmat . primitives import hashes
from Crypto.Util import number
import sympy
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa

#Returns the index right after the confusion string and the result are equal
def getIndex( confString, result): 

    leng= len(confString)
    i = 0
    while (result[i:i+leng] != confString):
        i = i + 1
    return i+leng

def getResult(password, confString):
    result=""
    trys = 1000
    while (confString not in  result ):
        trys = trys + 500
        result = random_gen(password,confString,trys)
    return result

def random_gen(pwd,salt,len):
    #print("password"+ pwd)
    #print("salt"+ salt)
    #print(len)
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA1(),
        length=len,
        salt=salt.encode("utf-8"),
        iterations=1000,
    )
    key = kdf.derive(pwd.encode("utf-"))

    #print("primeiro:", key.hex())
    return key.hex()

def psgen(password,confString,iterations):

    password = password.encode("utf-8").hex()
    confString = confString.encode("utf-8").hex()

    final=""
    result = ""

    result= getResult(password,confString)
    #print(result) #done 1
    index=  getIndex(confString, result)
    #print(index) #done 2
    final+= result[:index+1]
    #print(final) #done 3
    for(k) in range(1, iterations):   
        password=random_gen(password, confString,index+len(password))[:int(len(password))]
        #print(password) done 4
        result =getResult(password,confString)
        index= getIndex(confString, result)
        final= final + result[:index]

    return final

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
    #print(leng)
    key1= keystream[:leng]
    key2= keystream[leng:]
    #print("key 1" + key1 +"\n") #done 1
    #print("key 2" + key2 + "\n") #done 2

    p= generate_prime_from_bytes(key1)
    q= generate_prime_from_bytes(key2)
    print("q "+str(q))
    print("p "+str(p))


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

random_bytes=psgen("ricardo","H",4)
p,q=keygen(random_bytes)
generate_pem_files(p,q)


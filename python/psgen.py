from cryptography . hazmat . primitives . kdf . pbkdf2 import PBKDF2HMAC
from Crypto.Hash import SHA512
from Crypto.Random import get_random_bytes
from cryptography . hazmat . primitives import hashes




#Returns the index right after the confusion string and the result are equal
def getIndex( confString, result): 
    leng= len(confString.hex())
    i = 0
    while (str(result.hex())[i:i+leng] != str(confString.hex())):
        i = i + 1
    return i+leng

def getResult(password, confString, result):
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

iterations = int(input("Number of iterations "))
password = input("Password ").encode("utf-8")
confString = input("Confusion String ").encode("utf-8")


final=b''
result = b''

for(k) in range(0, iterations):   
    result= getResult(password,confString,result) 
    index=  getIndex(confString, result)
    final+= result[:index+1]
    password=random_gen(password, confString,index+len(password))[index:]
    final+= getResult(password,confString,result)
print(final.hex())
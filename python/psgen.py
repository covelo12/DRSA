from Crypto.Protocol.KDF import PBKDF2
from Crypto.Hash import SHA512
from Crypto.Random import get_random_bytes


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
        result = PBKDF2(password, confString, trys, count=iterations, hmac_hash_module=SHA512)
    return result

iterations = int(input("Number of iterations "))
password = input("Password ")
confString = input("Confusion String ").encode()
#confString = "13".encode()

final=b''
result = b''

for(k) in range(0, iterations):   
    result= getResult(password,confString,result) 
    index=  getIndex(confString, result)
    final+= result[:index+1]
    password = PBKDF2(password, confString, index+len(password), count=iterations, hmac_hash_module=SHA512)[index:]
    final+= getResult(password,confString,result)
print(final.hex())
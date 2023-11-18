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

def getResult(confString, result):
    while ( str(confString.hex()) not in  str(result.hex()) ):
        trys = trys + 500
        result = PBKDF2(password, confString, trys, count=iterations, hmac_hash_module=SHA512)
    return result

iterations = 1000
password = "teste"
confString = b'12'
trys = 1000


result = PBKDF2(password, confString, trys, count=iterations, hmac_hash_module=SHA512)
result= getResult(confString,result)
index=  getIndex(confString, result)

print(index)
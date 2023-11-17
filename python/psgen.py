from Crypto.Protocol.KDF import PBKDF2
from Crypto.Hash import SHA512
from Crypto.Random import get_random_bytes

iterations = 1000
password = "teste"
confString = b'12'
trys = 1000


result = PBKDF2(password, confString, trys, count=iterations, hmac_hash_module=SHA512)

while ( str(confString.hex()) not in  str(result.hex()) ):
    trys = trys + 500
    result = PBKDF2(password, confString, trys, count=iterations, hmac_hash_module=SHA512)

len= len(confString.hex())
i = 0
while (str(result.hex())[i:i+len] != str(confString.hex())):
    i = i + 1
print(i)
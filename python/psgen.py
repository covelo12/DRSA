from Crypto.Protocol.KDF import PBKDF2
from Crypto.Hash import SHA512
from Crypto.Random import get_random_bytes

iterations = 1000
password = "teste"
confString = b'12'


result = PBKDF2(password, confString, iterations, count=iterations, hmac_hash_module=SHA512)

while ( str(confString.hex()) not in  str(result.hex()) ):
    iterations = iterations + 1000
    result = PBKDF2(password, confString, iterations, count=iterations, hmac_hash_module=SHA512)

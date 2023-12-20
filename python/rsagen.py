from psgen import keygen,generate_pem_files
import sys
import random

def start(bytes):
    p,q=keygen(bytes.hex())
    generate_pem_files(p,q)

if __name__ == "__main__":
    if len(sys.argv) >2 :
        print("Usage: python rsagen.py  optional <bytestring>")
        sys.exit(1)
    elif len(sys.argv) == 2:
        start(sys.argv[1].encode())
    else: 
        start(random.randbytes(256))


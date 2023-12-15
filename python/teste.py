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

# Example usage
cipher = StreamCipher(12345)  # Seed for the generator

# Generate 10 bytes of keystream
keystream = [cipher.next_byte() for _ in range(10)]
print(keystream)

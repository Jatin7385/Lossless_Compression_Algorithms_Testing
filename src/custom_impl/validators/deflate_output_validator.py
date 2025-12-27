import zlib

# Read your raw deflate-compressed data
with open("../output.deflate", "rb") as f:
    compressed = f.read()

try:
    # wbits = -15 means: RAW DEFLATE stream (no zlib / gzip header)
    decompressed = zlib.decompress(compressed, wbits=-15)
    print("Decompression successful!")
    print(decompressed)
except zlib.error as e:
    print("Decompression failed:", e)

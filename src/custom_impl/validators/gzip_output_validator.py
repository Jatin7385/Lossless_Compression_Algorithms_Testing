import gzip

# Read your raw deflate-compressed data
with open("./gzip_output.gz", "rb") as f:
    compressed = f.read()

try:
    # wbits = -15 means: RAW DEFLATE stream (no zlib / gzip header)
    decompressed = gzip.decompress(compressed)
    print("Decompression successful!")
    print(decompressed.decode('utf-8')) 
except zlib.error as e:
    print("Decompression failed:", e)

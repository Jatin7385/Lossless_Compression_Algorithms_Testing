import lz4.frame
import gzip
import zstandard as zstd
import snappy
import brotli

def lz4Processing(text: str) -> bytes:
    # ------------------- Compression/Decompression -------------------
    print("Original size:", len(text.encode('utf-8')), "bytes")

    # Compress
    compressed = lz4.frame.compress(text.encode('utf-8'))
    print("Compressed size:", len(compressed), "bytes")

    # Decompress
    decompressed = lz4.frame.decompress(compressed)
    print("Decompressed size:", len(decompressed), "bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    print("Percentage of compression reduced:", percentage_reduced, "%")

    # Verify
    assert decompressed == text.encode('utf-8')
    print("Decompression verified ✅")
    
    return compressed

def gzipProcessing(text: str) -> bytes:
    # ------------------- Compression/Decompression -------------------
    print("Original size:", len(text.encode('utf-8')), "bytes")

    # Compress
    compressed = gzip.compress(text.encode('utf-8'))
    print("Compressed size:", len(compressed), "bytes")

    # Decompress
    decompressed = gzip.decompress(compressed)
    print("Decompressed size:", len(decompressed), "bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    print("Percentage of compression reduced:", percentage_reduced, "%")

    # Verify
    assert decompressed == text.encode('utf-8')
    print("Decompression verified ✅")
    
    return compressed

def zstdProcessing(text: str, level: int = 3) -> bytes:
    """
    Compress and decompress text using Zstandard.
    
    Args:
        text (str): The text to compress.
        level (int): Compression level (1-22). Default is 3.
    
    Returns:
        bytes: Compressed data
    """
    print("Original size:", len(text.encode('utf-8')), "bytes")

    # Compress
    cctx = zstd.ZstdCompressor(level=level)
    compressed = cctx.compress(text.encode('utf-8'))
    print("Compressed size:", len(compressed), "bytes")

    # Decompress
    dctx = zstd.ZstdDecompressor()
    decompressed = dctx.decompress(compressed)
    print("Decompressed size:", len(decompressed), "bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    print("Percentage of compression reduced:", percentage_reduced, "%")

    # Verify
    assert decompressed == text.encode('utf-8')
    print("Decompression verified ✅")
    
    return compressed



def brotliProcessing(text: str, printFlag = True, quality: int = 11, mode: int = 0, lgwin: int = 22) -> dict:
    # ------------------- Compression/Decompression -------------------
    if printFlag:
        print("Original size:", len(text.encode('utf-8')), "bytes")

    mode_dict = {
        0: brotli.MODE_GENERIC,
        1: brotli.MODE_TEXT,
        2: brotli.MODE_FONT
    }

    # Compress
    compressed = brotli.compress(text.encode('utf-8'), quality=quality, mode=mode_dict[mode], lgwin=lgwin)
    
    if printFlag:
        print("Compressed size:", len(compressed), "bytes")

    # Decompress
    decompressed = brotli.decompress(compressed)
    
    if printFlag:
        print("Decompressed size:", len(decompressed), "bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    
    if printFlag:
        print("Percentage of compression reduced:", percentage_reduced, "%")

    # Verify
    assert decompressed == text.encode('utf-8')
    if printFlag:
        print("Decompression verified ✅")
    if printFlag:
        print("Compressed size:", len(compressed), "bytes")

    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(compressed) / len(text.encode('utf-8'))
    }

def snappyProcessing(text: str) -> bytes:
    """
    Compress and decompress text using Snappy.

    Args:
        text (str): The text to compress.

    Returns:
        bytes: Compressed data
    """
    print("Original size:", len(text.encode('utf-8')), "bytes")

    # Compress
    compressed = snappy.compress(text.encode('utf-8'))

    # Decompress
    decompressed = snappy.decompress(compressed)
    print("Decompressed size:", len(decompressed), "bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    print("Percentage of compression reduced:", percentage_reduced, "%")

    # Verify
    assert decompressed == text.encode('utf-8')
    print("Decompression verified ✅")

    return compressed
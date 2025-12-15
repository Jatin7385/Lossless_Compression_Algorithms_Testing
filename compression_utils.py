import lz4.frame
import gzip
import zstandard as zstd
import snappy
import brotli

def conditionalPrint(printFlag: bool, message: str):
    if printFlag:
        print(message)

'''
LZ4 Compression : 
    - Additional parameters(Not used for now) : 
        ->  block_linked=True,       # True=better ratio, False=faster
            content_checksum=True,   # CRC32 validation
            store_size=True,         # Include original size in header
            auto_flush=True          # Flush buffers immediately

    - We're going with minimal parameters for now.
)
'''
def lz4Processing(text: str, compression_level: int = 1, block_size: int = 0, printFlag: bool = True) -> bytes:
    # ------------------- Compression/Decompression -------------------
    conditionalPrint(printFlag, "Original size: " + str(len(text.encode('utf-8'))) + " bytes")

    # Compress
    compressed = lz4.frame.compress(text.encode('utf-8'),
                                    compression_level=compression_level,
                                    block_size=block_size)
    conditionalPrint(printFlag, "Compressed size: " + str(len(compressed)) + " bytes")

    # Decompress
    decompressed = lz4.frame.decompress(compressed)
    conditionalPrint(printFlag, "Decompressed size: " + str(len(decompressed)) + " bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    conditionalPrint(printFlag, "Percentage of compression reduced: " + str(percentage_reduced) + "%")

    # Verify
    assert decompressed == text.encode('utf-8')
    conditionalPrint(printFlag, "Decompression verified ✅")
    
    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(compressed) / len(text.encode('utf-8'))
    }

def gzipProcessing(text: str, custom_compress_level: int = 6, printFlag: bool = True) -> bytes: # Default compress level is 6.
    # ------------------- Compression/Decompression -------------------
    conditionalPrint(printFlag, "Original size: " + str(len(text.encode('utf-8'))) + " bytes")

    # Compress
    compressed = gzip.compress(text.encode('utf-8'), compresslevel=custom_compress_level)
    conditionalPrint(printFlag, "Compressed size: " + str(len(compressed)) + " bytes")

    # Decompress
    decompressed = gzip.decompress(compressed)
    conditionalPrint(printFlag, "Decompressed size: " + str(len(decompressed)) + " bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    conditionalPrint(printFlag, "Percentage of compression reduced: " + str(percentage_reduced) + "%")

    # Verify
    conditionalPrint(printFlag, "Decompression verified ✅")
    assert decompressed == text.encode('utf-8')
    
    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(compressed) / len(text.encode('utf-8'))
    }

def zstdProcessing(text: str, level: int = 3) -> bytes:
    """
    Compress and decompress text using Zstandard.
    
    Args:
        text (str): The text to compress.
        level (int): Compression level (1-22). Default is 3.
    
    Returns:
        bytes: Compressed data
    """
    conditionalPrint(printFlag, "Original size: " + str(len(text.encode('utf-8'))) + " bytes")

    # Compress
    cctx = zstd.ZstdCompressor(level=level)
    compressed = cctx.compress(text.encode('utf-8'))
    conditionalPrint(printFlag, "Compressed size: " + str(len(compressed)) + " bytes")

    # Decompress
    dctx = zstd.ZstdDecompressor()
    decompressed = dctx.decompress(compressed)
    conditionalPrint(printFlag, "Decompressed size: " + str(len(decompressed)) + " bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    conditionalPrint(printFlag, "Percentage of compression reduced: " + str(percentage_reduced) + "%")

    # Verify
    assert decompressed == text.encode('utf-8')
    print("Decompression verified ✅")
    
    return compressed



def brotliProcessing(text: str, printFlag = True, quality: int = 11, mode: int = 0, lgwin: int = 22) -> dict:
    # ------------------- Compression/Decompression -------------------
    conditionalPrint(printFlag, "Original size: " + str(len(text.encode('utf-8'))) + " bytes")

    mode_dict = {
        0: brotli.MODE_GENERIC,
        1: brotli.MODE_TEXT,
        2: brotli.MODE_FONT
    }

    # Compress
    compressed = brotli.compress(text.encode('utf-8'), quality=quality, mode=mode_dict[mode], lgwin=lgwin)
    
    conditionalPrint(printFlag, "Compressed size: " + str(len(compressed)) + " bytes")

    # Decompress
    decompressed = brotli.decompress(compressed)
    
    conditionalPrint(printFlag, "Decompressed size: " + str(len(decompressed)) + " bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    
    conditionalPrint(printFlag, "Percentage of compression reduced: " + str(percentage_reduced) + "%")

    # Verify
    assert decompressed == text.encode('utf-8')
    conditionalPrint(printFlag, "Decompression verified ✅")
    conditionalPrint(printFlag, "Compressed size: " + str(len(compressed)) + " bytes")

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

    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(compressed) / len(text.encode('utf-8'))
    }
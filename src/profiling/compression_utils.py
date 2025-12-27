"""
Compression utilities for testing various lossless compression algorithms.

This module provides wrapper functions for Brotli, Gzip, LZ4, Zstandard, and Snappy
compression algorithms with built-in benchmarking and verification.
"""

import lz4.frame
import gzip
import zstandard as zstd
import snappy
import brotli
import logging
from typing import Dict, Union

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

def conditionalPrint(printFlag: bool, message: str) -> None:
    """
    Conditionally print a message based on the print flag.
    
    Args:
        printFlag (bool): Whether to print the message
        message (str): The message to print
    """
    if printFlag:
        print(message)

def lz4Processing(text: str, compression_level: int = 1, block_size: int = 0, 
                  printFlag: bool = True) -> Dict[str, Union[float, int]]:
    """
    Compress and decompress text using LZ4 algorithm.
    
    LZ4 is a fast compression algorithm that prioritizes speed over compression ratio.
    
    Additional parameters available (not used for simplicity):
        - block_linked=True      # True=better ratio, False=faster
        - content_checksum=True  # CRC32 validation
        - store_size=True        # Include original size in header
        - auto_flush=True        # Flush buffers immediately
    
    Args:
        text (str): The input text to compress
        compression_level (int): Compression level (-5 to 16, default=1)
                                -5 is fastest, 16 is maximum compression
        block_size (int): Block size for compression (0=auto, 4=64KB, 5=256KB, 
                         6=1MB, 7=4MB)
        printFlag (bool): Whether to print compression statistics
        
    Returns:
        dict: Dictionary containing:
            - compression_percentage (float): Percentage of size reduction
            - compressed_size (int): Size of compressed data in bytes
            - compression_ratio (float): Ratio of original to compressed size
            
    Raises:
        AssertionError: If decompression doesn't match original data
    """
    logger.debug(f"Starting LZ4 compression with level={compression_level}, block_size={block_size}")
    
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
    
    logger.debug(f"LZ4 compression complete: {percentage_reduced:.2f}% reduction")
    
    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(text.encode('utf-8')) / len(compressed)
    }

def gzipProcessing(text: str, custom_compress_level: int = 6, printFlag: bool = True) -> Dict[str, Union[float, int]]:
    """
    Compress and decompress text using Gzip algorithm.
    
    Gzip is the standard compression for web servers and APIs, offering a good
    balance between compression ratio and speed.
    
    Args:
        text (str): The input text to compress
        custom_compress_level (int): Compression level (1-9, default=6)
                                     1 is fastest, 9 is maximum compression
        printFlag (bool): Whether to print compression statistics
        
    Returns:
        dict: Dictionary containing:
            - compression_percentage (float): Percentage of size reduction
            - compressed_size (int): Size of compressed data in bytes
            - compression_ratio (float): Ratio of original to compressed size
            
    Raises:
        AssertionError: If decompression doesn't match original data
    """
    logger.debug(f"Starting Gzip compression with level={custom_compress_level}")
    
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
    
    logger.debug(f"Gzip compression complete: {percentage_reduced:.2f}% reduction")
    
    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(text.encode('utf-8')) / len(compressed)
    }

def zstdProcessing(text: str, level: int = 3, printFlag: bool = True) -> Dict[str, Union[float, int]]:
    """
    Compress and decompress text using Zstandard (Zstd) algorithm.
    
    Zstandard offers excellent compression ratios with fast decompression speeds,
    making it ideal for modern applications. It's increasingly supported by browsers
    but not yet as standardized as Gzip.
    
    Args:
        text (str): The text to compress
        level (int): Compression level (1-22, default=3)
                    Lower is faster, higher gives better compression
                    1-3: Fast compression
                    4-19: Balanced
                    20-22: Ultra compression (slow)
        printFlag (bool): Whether to print compression statistics
    
    Returns:
        dict: Dictionary containing:
            - compression_percentage (float): Percentage of size reduction
            - compressed_size (int): Size of compressed data in bytes
            - compression_ratio (float): Ratio of original to compressed size
            
    Raises:
        AssertionError: If decompression doesn't match original data
    """
    logger.debug(f"Starting Zstd compression with level={level}")
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
    conditionalPrint(printFlag, "Decompression verified ✅")
    
    logger.debug(f"Zstd compression complete: {percentage_reduced:.2f}% reduction")
    
    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(text.encode('utf-8')) / len(compressed)
    }

def brotliProcessing(text: str, printFlag: bool = True, quality: int = 11, 
                    mode: int = 0, lgwin: int = 22) -> Dict[str, Union[float, int]]:
    """
    Compress and decompress text using Brotli algorithm.
    
    Brotli is Google's compression algorithm that achieves excellent compression
    ratios, especially for static web content. It's widely supported by modern browsers.
    
    Args:
        text (str): The input text to compress
        printFlag (bool): Whether to print compression statistics
        quality (int): Compression quality (0-11, default=11)
                      0-4: Fast, lower compression
                      5-8: Balanced
                      9-11: Maximum compression, slower
        mode (int): Compression mode (default=0)
                   0: MODE_GENERIC - arbitrary data
                   1: MODE_TEXT - UTF-8 text (optimized for language patterns)
                   2: MODE_FONT - WOFF/WOFF2 fonts
        lgwin (int): Log window size (10-24, default=22)
                    Window size = 2^lgwin bytes
                    10: 1KB (fast, less compression)
                    16: 64KB (similar to Gzip)
                    22: 4MB (excellent for large files)
                    24: 16MB (maximum)
        
    Returns:
        dict: Dictionary containing:
            - compression_percentage (float): Percentage of size reduction
            - compressed_size (int): Size of compressed data in bytes
            - compression_ratio (float): Ratio of original to compressed size
            
    Raises:
        AssertionError: If decompression doesn't match original data
    """
    logger.debug(f"Starting Brotli compression with quality={quality}, mode={mode}, lgwin={lgwin}")
    
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

    logger.debug(f"Brotli compression complete: {percentage_reduced:.2f}% reduction")

    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(text.encode('utf-8')) / len(compressed)
    }

def snappyProcessing(text: str, printFlag: bool = True) -> Dict[str, Union[float, int]]:
    """
    Compress and decompress text using Snappy algorithm.
    
    Snappy prioritizes speed and is designed for scenarios where decompression
    speed is critical (databases, RPCs). It has the lowest compression ratio
    but is extremely fast.
    
    Note: Snappy has no tunable parameters - it's optimized for consistent
    performance across different data types.

    Args:
        text (str): The text to compress
        printFlag (bool): Whether to print compression statistics

    Returns:
        dict: Dictionary containing:
            - compression_percentage (float): Percentage of size reduction
            - compressed_size (int): Size of compressed data in bytes
            - compression_ratio (float): Ratio of original to compressed size
            
    Raises:
        AssertionError: If decompression doesn't match original data
    """
    logger.debug("Starting Snappy compression")
    
    conditionalPrint(printFlag, f"Original size: {len(text.encode('utf-8'))} bytes")

    # Compress
    compressed = snappy.compress(text.encode('utf-8'))

    # Decompress
    decompressed = snappy.decompress(compressed)
    conditionalPrint(printFlag, f"Decompressed size: {len(decompressed)} bytes")

    # Percentage of compression reduced
    percentage_reduced = (len(text.encode('utf-8')) - len(compressed)) / len(text.encode('utf-8')) * 100
    conditionalPrint(printFlag, f"Percentage of compression reduced: {percentage_reduced}%")

    # Verify
    assert decompressed == text.encode('utf-8')
    conditionalPrint(printFlag, "Decompression verified ✅")

    logger.debug(f"Snappy compression complete: {percentage_reduced:.2f}% reduction")

    return {
        'compression_percentage': percentage_reduced,
        'compressed_size': len(compressed),
        'compression_ratio': len(text.encode('utf-8')) / len(compressed)
    }
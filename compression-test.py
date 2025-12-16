"""
Compression testing script for benchmarking various algorithms.

This script tests compression algorithms on data from data.txt and provides
detailed profiling information including time, memory usage, and CPU usage.
"""

import time
import tracemalloc
import cProfile
import pstats
import logging
import sys
from compression_utils import *

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# Read input data with error handling
text = ""
try:
    with open('data.txt', 'r', encoding='utf-8') as file:
        text = file.read()
    logger.info(f"Successfully loaded data.txt ({len(text)} characters)")
except FileNotFoundError:
    logger.error("Error: data.txt not found in current directory")
    sys.exit(1)
except Exception as e:
    logger.error(f"Error reading file: {e}")
    sys.exit(1)

# ------------------- Start Profiling -------------------
# Time profiling
start_time = time.time()

# Memory profiling
tracemalloc.start()

# CPU profiling
profiler = cProfile.Profile()
profiler.enable()

# Uncomment the algorithm you want to test:
# result = brotliProcessing(text)
# result = gzipProcessing(text)
# result = lz4Processing(text)
# result = zstdProcessing(text)
result = snappyProcessing(text, printFlag=True)

# ------------------- End Profiling -------------------
profiler.disable()
end_time = time.time()
current, peak = tracemalloc.get_traced_memory()
tracemalloc.stop()

logger.info(f"\n{'='*60}")
logger.info("BENCHMARK RESULTS")
logger.info(f"{'='*60}")
print(f"\nTime taken: {end_time - start_time:.6f} seconds")
print(f"Current memory usage: {current/1e6:.6f} MB")
print(f"Peak memory usage: {peak/1e6:.6f} MB")

# Print CPU profiling stats (top 10 functions)
print("\nTop 10 CPU-intensive functions:")
stats = pstats.Stats(profiler)
stats.sort_stats("cumulative").print_stats(10)

# Print summary table
print(f"\n{'='*60}")
print(f"| Metric | Value |")
print(f"|--------|-------|")
print(f"| Compression Percentage | {result['compression_percentage']:>7.2f}% |")
print(f"| Time Taken | {end_time - start_time:.6f}s |")
print(f"| Peak Memory Usage | {peak/1e6:.6f} MB |")
print(f"| Compression Ratio | {result['compression_ratio']:>8.4f} |")
print(f"{'='*60}")
            

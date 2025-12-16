import time
import tracemalloc
import cProfile
import pstats
from compression_utils import *

text = ""
with open('data.txt', 'r') as file:
    text = file.read()

# ------------------- Start Profiling -------------------
# Time profiling
start_time = time.time()

# Memory profiling
tracemalloc.start()

# CPU profiling
profiler = cProfile.Profile()
profiler.enable()

# brotliProcessing(text)
# gzipProcessing(text)
# lz4Processing(text)
# zstdProcessing(text)
result = snappyProcessing(text)

# ------------------- End Profiling -------------------
profiler.disable()
end_time = time.time()
current, peak = tracemalloc.get_traced_memory()
tracemalloc.stop()

print(f"\nTime taken: {end_time - start_time:.6f} seconds")
print(f"Current memory usage: {current/1e6:.6f} MB")
print(f"Peak memory usage: {peak/1e6:.6f} MB")

# Print CPU profiling stats (top 10 functions)
stats = pstats.Stats(profiler)
stats.sort_stats("cumulative").print_stats(10)


print(f"| Optimzed For | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |")
print(f"|--------------|---------|------|-------|------------------------|")
print(f"| Optimized Snappy | {result['compression_percentage']:>7.2f}% | {end_time - start_time:.6f}s | {current/1e6:.6f} MB | {result['compression_ratio']:>8.4f}")
            

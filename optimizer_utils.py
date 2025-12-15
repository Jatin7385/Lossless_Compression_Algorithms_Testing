from compression_utils import *
from dataclasses import dataclass
import time
import tracemalloc
import cProfile
import pstats

@dataclass
class CompressionResult:
    """
    Results from a compression algorithm.
    """

    quality: int
    mode: int
    lgwin: int
    compressLevel: int
    compressionLevel: int
    blockSize: int
    timeTaken: float
    peakMemoryUsage: float
    compressionPercentage: float

    def __str__(self):
        return f"| {self.quality} | {self.mode} | {self.lgwin} | {self.timeTaken:.6f}s | {self.peakMemoryUsage:.6f} MB | {self.compressionPercentage} |"

class Optimizer:
    """
    Find optimal compression parameters through grid search.
    """

    # Constants
    BYTES_TO_MB = 1e6

    def __init__(self, text: str, compression_algorithm: str, printFlag: bool = False):
        self.text = text
        self.compression_algorithm = compression_algorithm
        self.printFlag = printFlag

    def profile(self, compressionLevel = None, blockSize = None, compressLevel = None, quality = None, mode = None, lgwin = None) -> CompressionResult:
        """
        Profiling the parameters passed and returning CompressionResult object.
        """

        # ------------------- Start Profiling -------------------
        # Time profiling
        start_time = time.time()

        # Memory profiling
        tracemalloc.start()

        # CPU profiling
        profiler = cProfile.Profile()
        profiler.enable()

        # ---------------------- Compression ---------------------
        if self.compression_algorithm == 'brotli':
            result = brotliProcessing(self.text, self.printFlag, quality, mode, lgwin)
        elif self.compression_algorithm == 'gzip':
            result = gzipProcessing(self.text, custom_compress_level=compressLevel, printFlag=self.printFlag)
        elif self.compression_algorithm == 'lz4':
            result = lz4Processing(self.text, compression_level=compressionLevel, block_size=blockSize, printFlag=self.printFlag)
        # elif self.compression_algorithm == 'zstd':
        #     result = zstdProcessing(self.text, self.printFlag, quality, mode, lgwin)
        # elif self.compression_algorithm == 'snappy':
        #     result = snappyProcessing(self.text, self.printFlag, quality, mode, lgwin)

        # ------------------- End Profiling -------------------
        profiler.disable()
        end_time = time.time()
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()

        timeTaken = end_time - start_time
        peakMemoryUsage = current/1e6

        return CompressionResult(compressionLevel=compressionLevel, 
                                 blockSize=blockSize,
                                 compressLevel=compressLevel, 
                                 quality=quality, 
                                 mode=mode, 
                                 lgwin=lgwin, 
                                 timeTaken=timeTaken, 
                                 peakMemoryUsage=peakMemoryUsage, 
                                 compressionPercentage=result['compression_percentage'])

    def run_lz4_grid_search(self, 
                        logFlag: bool = False,
                        compressionLevelRange: range = range(-5, 17),
                        blockSizeRange: range = range(0, 8)) -> dict:
        """
        Run the grid search and find the optimal parameters.
        """

        # Dictionary to store the optimal results
        optimalResults = {
            'optimizedTime': None,
            'optimizedPeakMemory': None,
            'optimizedCompressionPercentage': None
        }

        # Initialize the minimum and maximum values for each metric.
        minimumTimeTaken = float('inf')
        minimumPeakMemoryUsage = float('inf')
        maximumCompressionPercentage = float('-inf')

        # Grid Search - O(n)
        for compressionLevel in compressionLevelRange:
            for blockSize in blockSizeRange:
                result = self.profile(
                                      compressionLevel=compressionLevel,
                                      blockSize=blockSize,
                                      compressLevel=None,
                                      quality=None, 
                                      mode=None, 
                                      lgwin=None) # Call profile method to get CompressionResult object


                if result.timeTaken < minimumTimeTaken:
                    minimumTimeTaken = result.timeTaken
                    optimalResults['optimizedTime'] = result
                if result.peakMemoryUsage < minimumPeakMemoryUsage:
                    minimumPeakMemoryUsage = result.peakMemoryUsage
                    optimalResults['optimizedPeakMemory'] = result
                if result.compressionPercentage > maximumCompressionPercentage:
                    maximumCompressionPercentage = result.compressionPercentage
                    optimalResults['optimizedCompressionPercentage'] = result
                
                if logFlag:
                    print(f"{quality:>7} | {mode:>4} | {lgwin:>5} | "
                        f"{result.compressionPercentage:>7.2f}% | "
                        f"{result.timeTaken:>10.6f} | "
                        f"{result.peakMemoryUsage:>12.6f} | "
                        f"{result.compressionPercentage:>8.4f}")
    

        return optimalResults


    def run_gzip_grid_search(self, 
                        logFlag: bool = False,
                        compressLevelRange: range = range(1, 10)) -> dict:
        """
        Run the grid search and find the optimal parameters.
        """

        # Dictionary to store the optimal results
        optimalResults = {
            'optimizedTime': None,
            'optimizedPeakMemory': None,
            'optimizedCompressionPercentage': None
        }

        # Initialize the minimum and maximum values for each metric.
        minimumTimeTaken = float('inf')
        minimumPeakMemoryUsage = float('inf')
        maximumCompressionPercentage = float('-inf')

        # Grid Search - O(n)
        for compressLevel in compressLevelRange:
            result = self.profile(compressLevel=compressLevel, quality=None, mode=None, lgwin=None) # Call profile method to get CompressionResult object


            if result.timeTaken < minimumTimeTaken:
                minimumTimeTaken = result.timeTaken
                optimalResults['optimizedTime'] = result
            if result.peakMemoryUsage < minimumPeakMemoryUsage:
                minimumPeakMemoryUsage = result.peakMemoryUsage
                optimalResults['optimizedPeakMemory'] = result
            if result.compressionPercentage > maximumCompressionPercentage:
                maximumCompressionPercentage = result.compressionPercentage
                optimalResults['optimizedCompressionPercentage'] = result
            
            if logFlag:
                print(f"{quality:>7} | {mode:>4} | {lgwin:>5} | "
                    f"{result.compressionPercentage:>7.2f}% | "
                    f"{result.timeTaken:>10.6f} | "
                    f"{result.peakMemoryUsage:>12.6f} | "
                    f"{result.compressionPercentage:>8.4f}")
    

        return optimalResults


    def run_brotli_grid_search(self, 
                        logFlag: bool = False,
                        qualityRange: range = range(0, 12),
                        modeRange: range = range(0, 3),
                        lgwinRange: range = range(10,25)) -> dict:
        """
        Run the grid search and find the optimal parameters.
        """

        # Dictionary to store the optimal results
        optimalResults = {
            'optimizedTime': None,
            'optimizedPeakMemory': None,
            'optimizedCompressionPercentage': None
        }

        # Initialize the minimum and maximum values for each metric.
        minimumTimeTaken = float('inf')
        minimumPeakMemoryUsage = float('inf')
        maximumCompressionPercentage = float('-inf')

        # Grid Search - O(n^3)
        for quality in qualityRange:
            for mode in modeRange:
                for lgwin in lgwinRange:
                    result = self.profile(compressLevel=None, quality=quality, mode=mode, lgwin=lgwin) # Call profile method to get CompressionResult object


                    if result.timeTaken < minimumTimeTaken:
                        minimumTimeTaken = result.timeTaken
                        optimalResults['optimizedTime'] = result
                    if result.peakMemoryUsage < minimumPeakMemoryUsage:
                        minimumPeakMemoryUsage = result.peakMemoryUsage
                        optimalResults['optimizedPeakMemory'] = result
                    if result.compressionPercentage > maximumCompressionPercentage:
                        maximumCompressionPercentage = result.compressionPercentage
                        optimalResults['optimizedCompressionPercentage'] = result
                    
                    if logFlag:
                        print(f"{quality:>7} | {mode:>4} | {lgwin:>5} | "
                          f"{result.compressionPercentage:>7.2f}% | "
                          f"{result.timeTaken:>10.6f} | "
                          f"{result.peakMemoryUsage:>12.6f} | "
                          f"{result.compressionPercentage:>8.4f}")
    

        return optimalResults

    def print_optimal_results(self, optimalResults: dict):
        if self.compression_algorithm == 'lz4':
            '''
            Print the optimal results.
            '''
            print(f"| Optimzed For | Compression Level | Block Size | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |")
            print(f"|--------------|---------|------|-------|------------------------|------------|-------------------|-------------------|")
            print(f"| Optimized Time | {optimalResults['optimizedTime'].compressionLevel} | {optimalResults['optimizedTime'].blockSize} | {optimalResults['optimizedTime'].compressionPercentage:>7.2f}% | {optimalResults['optimizedTime'].timeTaken:>10.6f}s | {optimalResults['optimizedTime'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedTime'].compressionPercentage:>8.4f}")
            print(f"| Optimized Peak Memory | {optimalResults['optimizedPeakMemory'].compressionLevel} | {optimalResults['optimizedPeakMemory'].blockSize} | {optimalResults['optimizedPeakMemory'].compressionPercentage:>7.2f}% | {optimalResults['optimizedPeakMemory'].timeTaken:>10.6f}s | {optimalResults['optimizedPeakMemory'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedPeakMemory'].compressionPercentage:>8.4f}")
            print(f"| Optimized Compression Percentage | {optimalResults['optimizedCompressionPercentage'].compressionLevel} | {optimalResults['optimizedCompressionPercentage'].blockSize} | {optimalResults['optimizedCompressionPercentage'].compressionPercentage:>7.2f}% | {optimalResults['optimizedCompressionPercentage'].timeTaken:>10.6f}s | {optimalResults['optimizedCompressionPercentage'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedCompressionPercentage'].compressionPercentage:>8.4f}")
        elif self.compression_algorithm == 'gzip':
            '''
            Print the optimal results.
            '''
            print(f"| Optimzed For | Compress Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |")
            print(f"|--------------|----------------|------------------------|------------|-------------------|-------------------|")
            print(f"| Optimized Time | {optimalResults['optimizedTime'].compressLevel} | {optimalResults['optimizedTime'].compressionPercentage:>7.2f}% | {optimalResults['optimizedTime'].timeTaken:>10.6f}s | {optimalResults['optimizedTime'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedTime'].compressionPercentage:>8.4f}")
            print(f"| Optimized Peak Memory | {optimalResults['optimizedPeakMemory'].compressLevel} | {optimalResults['optimizedPeakMemory'].compressionPercentage:>7.2f}% | {optimalResults['optimizedPeakMemory'].timeTaken:>10.6f}s | {optimalResults['optimizedPeakMemory'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedPeakMemory'].compressionPercentage:>8.4f}")
            print(f"| Optimized Compression Percentage | {optimalResults['optimizedCompressionPercentage'].compressLevel} | {optimalResults['optimizedCompressionPercentage'].compressionPercentage:>7.2f}% | {optimalResults['optimizedCompressionPercentage'].timeTaken:>10.6f}s | {optimalResults['optimizedCompressionPercentage'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedCompressionPercentage'].compressionPercentage:>8.4f}")
        elif self.compression_algorithm == 'brotli':
            '''
            Print the optimal results.
            '''
            print(f"| Optimzed For | Quality | Mode | Lgwin | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |")
            print(f"|--------------|---------|------|-------|------------------------|------------|-------------------|-------------------|")
            print(f"| Optimized Time | {optimalResults['optimizedTime'].quality} | {optimalResults['optimizedTime'].mode} | {optimalResults['optimizedTime'].lgwin} | {optimalResults['optimizedTime'].compressionPercentage:>7.2f}% | {optimalResults['optimizedTime'].timeTaken:>10.6f}s | {optimalResults['optimizedTime'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedTime'].compressionPercentage:>8.4f}")
            print(f"| Optimized Peak Memory | {optimalResults['optimizedPeakMemory'].quality} | {optimalResults['optimizedPeakMemory'].mode} | {optimalResults['optimizedPeakMemory'].lgwin} | {optimalResults['optimizedPeakMemory'].compressionPercentage:>7.2f}% | {optimalResults['optimizedPeakMemory'].timeTaken:>10.6f}s | {optimalResults['optimizedPeakMemory'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedPeakMemory'].compressionPercentage:>8.4f}")
            print(f"| Optimized Compression Percentage | {optimalResults['optimizedCompressionPercentage'].quality} | {optimalResults['optimizedCompressionPercentage'].mode} | {optimalResults['optimizedCompressionPercentage'].lgwin} | {optimalResults['optimizedCompressionPercentage'].compressionPercentage:>7.2f}% | {optimalResults['optimizedCompressionPercentage'].timeTaken:>10.6f}s | {optimalResults['optimizedCompressionPercentage'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedCompressionPercentage'].compressionPercentage:>8.4f}")
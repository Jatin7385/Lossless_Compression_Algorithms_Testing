from compression_utils import brotliProcessing
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
    timeTaken: float
    peakMemoryUsage: float
    compressionPercentage: float

    def __str__(self):
        return f"| {self.quality} | {self.mode} | {self.lgwin} | {self.timeTaken:.6f}s | {self.peakMemoryUsage:.6f} MB | {self.compressionPercentage} |"

class BrotliOptimizer:
    """
    Find optimal Brotli compression parameters through grid search.
    """

    # Constants
    BYTES_TO_MB = 1e6

    def __init__(self, text: str, printFlag: bool = False):
        self.text = text
        self.printFlag = printFlag

    def profile(self, quality: int, mode: int, lgwin: int) -> CompressionResult:
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
        result = brotliProcessing(self.text, self.printFlag, quality, mode, lgwin)

        # ------------------- End Profiling -------------------
        profiler.disable()
        end_time = time.time()
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()

        timeTaken = end_time - start_time
        peakMemoryUsage = current/1e6

        return CompressionResult(quality=quality, mode=mode, lgwin=lgwin, timeTaken=timeTaken, peakMemoryUsage=peakMemoryUsage, compressionPercentage=result['compression_percentage'])

    def run_grid_search(self, 
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
                    result = self.profile(quality, mode, lgwin) # Call profile method to get CompressionResult object


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
        """
        Print the optimal results.
        """
        print(f"| Quality | Mode | Lgwin | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |")
        print(f"|---------|------|-------|------------------------|------------|-------------------|-------------------|")
        print(f"| {optimalResults['optimizedTime'].quality} | {optimalResults['optimizedTime'].mode} | {optimalResults['optimizedTime'].lgwin} | {optimalResults['optimizedTime'].compressionPercentage:>7.2f}% | {optimalResults['optimizedTime'].timeTaken:>10.6f}s | {optimalResults['optimizedTime'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedTime'].compressionPercentage:>8.4f}")
        print(f"| {optimalResults['optimizedPeakMemory'].quality} | {optimalResults['optimizedPeakMemory'].mode} | {optimalResults['optimizedPeakMemory'].lgwin} | {optimalResults['optimizedPeakMemory'].compressionPercentage:>7.2f}% | {optimalResults['optimizedPeakMemory'].timeTaken:>10.6f}s | {optimalResults['optimizedPeakMemory'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedPeakMemory'].compressionPercentage:>8.4f}")
        print(f"| {optimalResults['optimizedCompressionPercentage'].quality} | {optimalResults['optimizedCompressionPercentage'].mode} | {optimalResults['optimizedCompressionPercentage'].lgwin} | {optimalResults['optimizedCompressionPercentage'].compressionPercentage:>7.2f}% | {optimalResults['optimizedCompressionPercentage'].timeTaken:>10.6f}s | {optimalResults['optimizedCompressionPercentage'].peakMemoryUsage:>12.6f} MB | {optimalResults['optimizedCompressionPercentage'].compressionPercentage:>8.4f}")

def main():
    # Read the data from the file
    with open('data.txt', 'r') as file:
        text = file.read()

    # Define the range of parameters to search
    qualityRange = range(0, 12)
    modeRange = range(0, 3)
    lgwinRange = range(10,25)

    # Create the BrotliOptimizer object
    optimizer = BrotliOptimizer(text, printFlag=False)

    # Run grid search
    optimalResults = optimizer.run_grid_search(logFlag=False, qualityRange=qualityRange, modeRange=modeRange, lgwinRange=lgwinRange)

    optimizer.print_optimal_results(optimalResults)

if __name__ == "__main__":
    main()
    
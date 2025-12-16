from compression_utils import *
from dataclasses import dataclass
import time
import tracemalloc
import cProfile
import pstats
import logging
from typing import Dict, Optional, Any

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

@dataclass
class CompressionResult:
    """
    Results from a compression algorithm.
    
    Attributes:
        level (int): Compression level for zstd
        quality (int): Quality level for brotli (0-11)
        mode (int): Mode for brotli (0=generic, 1=text, 2=font)
        lgwin (int): Log window size for brotli (10-24)
        compressLevel (int): Compression level for gzip (1-9)
        compressionLevel (int): Compression level for lz4 (-5 to 16)
        blockSize (int): Block size for lz4
        timeTaken (float): Time taken for compression in seconds
        peakMemoryUsage (float): Peak memory usage in MB
        compressionPercentage (float): Percentage of size reduction
        compressionRatio (float): Ratio of original to compressed size
    """

    level: Optional[int] = None
    quality: Optional[int] = None
    mode: Optional[int] = None
    lgwin: Optional[int] = None
    compressLevel: Optional[int] = None
    compressionLevel: Optional[int] = None
    blockSize: Optional[int] = None
    timeTaken: float = 0.0
    peakMemoryUsage: float = 0.0
    compressionPercentage: float = 0.0
    compressionRatio: float = 0.0

    def __str__(self):
        return f"| {self.quality} | {self.mode} | {self.lgwin} | {self.timeTaken:.6f}s | {self.peakMemoryUsage:.6f} MB | {self.compressionPercentage} |"

class Optimizer:
    """
    Find optimal compression parameters through grid search.
    
    This class performs grid search optimization to find the best parameters
    for various compression algorithms based on different optimization goals:
    - Minimum time taken
    - Minimum peak memory usage
    - Maximum compression percentage
    - Maximum compression ratio
    
    Attributes:
        text (str): The input text to be compressed
        compression_algorithm (str): Name of the algorithm ('brotli', 'gzip', 'lz4', 'zstd')
        printFlag (bool): Whether to print debug information
    """

    # Constants
    BYTES_TO_MB = 1e6

    def __init__(self, text: str, compression_algorithm: str, printFlag: bool = False):
        """
        Initialize the Optimizer.
        
        Args:
            text (str): The input text to be compressed
            compression_algorithm (str): Algorithm name ('brotli', 'gzip', 'lz4', 'zstd')
            printFlag (bool): Whether to print debug information during compression
        """
        self.text = text
        self.compression_algorithm = compression_algorithm
        self.printFlag = printFlag
        logger.info(f"Initialized Optimizer for {compression_algorithm} algorithm")

    def profile(self, level: Optional[int] = None, compressionLevel: Optional[int] = None, 
                blockSize: Optional[int] = None, compressLevel: Optional[int] = None, 
                quality: Optional[int] = None, mode: Optional[int] = None, 
                lgwin: Optional[int] = None) -> CompressionResult:
        """
        Profile compression with the specified parameters.
        
        This method runs the compression algorithm with the given parameters and
        measures time taken, memory usage, compression percentage, and compression ratio.
        
        Args:
            level (int, optional): Compression level for zstd
            compressionLevel (int, optional): Compression level for lz4
            blockSize (int, optional): Block size for lz4
            compressLevel (int, optional): Compression level for gzip
            quality (int, optional): Quality level for brotli
            mode (int, optional): Mode for brotli
            lgwin (int, optional): Log window size for brotli
            
        Returns:
            CompressionResult: Object containing all compression metrics
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
        elif self.compression_algorithm == 'zstd':
            result = zstdProcessing(self.text, level=level, printFlag=self.printFlag)
        else:
            logger.error(f"Unknown compression algorithm: {self.compression_algorithm}")
            raise ValueError(f"Unsupported algorithm: {self.compression_algorithm}")

        # ------------------- End Profiling -------------------
        profiler.disable()
        end_time = time.time()
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()

        timeTaken = end_time - start_time
        peakMemoryUsage = peak/1e6  # FIXED: Was using 'current' instead of 'peak'

        return CompressionResult(level=level,
                                 compressionLevel=compressionLevel, 
                                 blockSize=blockSize,
                                 compressLevel=compressLevel, 
                                 quality=quality, 
                                 mode=mode, 
                                 lgwin=lgwin, 
                                 timeTaken=timeTaken, 
                                 peakMemoryUsage=peakMemoryUsage, 
                                 compressionPercentage=result['compression_percentage'],
                                 compressionRatio=result['compression_ratio'])

    def run_zstd_grid_search(self, 
                        logFlag: bool = False,
                        levelRange: range = range(1, 23)) -> Dict[str, Optional[CompressionResult]]:
        """
        Run grid search to find optimal Zstandard compression parameters.
        
        Tests different compression levels and identifies optimal parameters
        for different optimization goals.
        
        Args:
            logFlag (bool): Whether to log each iteration's results
            levelRange (range): Range of compression levels to test (1-22)
            
        Returns:
            dict: Dictionary with keys 'optimizedTime', 'optimizedPeakMemory',
                  'optimizedCompressionPercentage', 'optimizedCompressionRatio',
                  each containing the optimal CompressionResult for that metric
        """
        logger.info(f"Starting ZSTD grid search with level range {levelRange}")

        # Dictionary to store the optimal results
        optimalResults = {
            'optimizedTime': None,
            'optimizedPeakMemory': None,
            'optimizedCompressionPercentage': None,
            'optimizedCompressionRatio': None
        }

        # Initialize the minimum and maximum values for each metric.
        minimumTimeTaken = float('inf')
        minimumPeakMemoryUsage = float('inf')
        maximumCompressionPercentage = float('-inf')
        maximumCompressionRatio = float('-inf')

        # Grid Search - O(n)
        for level in levelRange:
            result = self.profile(level=level)

            if result.timeTaken < minimumTimeTaken:
                minimumTimeTaken = result.timeTaken
                optimalResults['optimizedTime'] = result
            if result.peakMemoryUsage < minimumPeakMemoryUsage:
                minimumPeakMemoryUsage = result.peakMemoryUsage
                optimalResults['optimizedPeakMemory'] = result
            if result.compressionPercentage > maximumCompressionPercentage:
                maximumCompressionPercentage = result.compressionPercentage
                optimalResults['optimizedCompressionPercentage'] = result
            if result.compressionRatio > maximumCompressionRatio:
                maximumCompressionRatio = result.compressionRatio
                optimalResults['optimizedCompressionRatio'] = result
            if logFlag:
                logger.info(f"Level {level:>7} | "
                    f"Compression: {result.compressionPercentage:>7.2f}% | "
                    f"Time: {result.timeTaken:>10.6f}s | "
                    f"Memory: {result.peakMemoryUsage:>12.6f} MB | "
                    f"Ratio: {result.compressionRatio:>8.4f}")
    
        logger.info(f"ZSTD grid search complete")
        return optimalResults



    def run_lz4_grid_search(self, 
                        logFlag: bool = False,
                        compressionLevelRange: range = range(-5, 17),
                        blockSizeRange: range = range(0, 8)) -> Dict[str, Optional[CompressionResult]]:
        """
        Run grid search to find optimal LZ4 compression parameters.
        
        Tests different compression levels and block sizes to identify
        optimal parameters for different optimization goals.
        
        Args:
            logFlag (bool): Whether to log each iteration's results
            compressionLevelRange (range): Range of compression levels (-5 to 16)
            blockSizeRange (range): Range of block sizes to test
            
        Returns:
            dict: Dictionary with keys 'optimizedTime', 'optimizedPeakMemory',
                  'optimizedCompressionPercentage', each containing the optimal
                  CompressionResult for that metric
        """
        logger.info(f"Starting LZ4 grid search with compression level {compressionLevelRange} and block size {blockSizeRange}")

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

        # Grid Search - O(n*m)
        for compressionLevel in compressionLevelRange:
            for blockSize in blockSizeRange:
                result = self.profile(compressionLevel=compressionLevel, blockSize=blockSize)

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
                    logger.info(f"Level {compressionLevel:>7} | Block {blockSize:>4} | "
                        f"Compression: {result.compressionPercentage:>7.2f}% | "
                        f"Time: {result.timeTaken:>10.6f}s | "
                        f"Memory: {result.peakMemoryUsage:>12.6f} MB | "
                        f"Ratio: {result.compressionRatio:>8.4f}")
    
        logger.info(f"LZ4 grid search complete")
        return optimalResults


    def run_gzip_grid_search(self, 
                        logFlag: bool = False,
                        compressLevelRange: range = range(1, 10)) -> Dict[str, Optional[CompressionResult]]:
        """
        Run grid search to find optimal Gzip compression parameters.
        
        Tests different compression levels to identify optimal parameters
        for different optimization goals.
        
        Args:
            logFlag (bool): Whether to log each iteration's results
            compressLevelRange (range): Range of compression levels (1-9)
            
        Returns:
            dict: Dictionary with keys 'optimizedTime', 'optimizedPeakMemory',
                  'optimizedCompressionPercentage', each containing the optimal
                  CompressionResult for that metric
        """
        logger.info(f"Starting GZIP grid search with compression level {compressLevelRange}")

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
            result = self.profile(compressLevel=compressLevel)

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
                logger.info(f"Level {compressLevel:>7} | "
                    f"Compression: {result.compressionPercentage:>7.2f}% | "
                    f"Time: {result.timeTaken:>10.6f}s | "
                    f"Memory: {result.peakMemoryUsage:>12.6f} MB | "
                    f"Ratio: {result.compressionRatio:>8.4f}")
    
        logger.info(f"GZIP grid search complete")
        return optimalResults


    def run_brotli_grid_search(self, 
                        logFlag: bool = False,
                        qualityRange: range = range(0, 12),
                        modeRange: range = range(0, 3),
                        lgwinRange: range = range(10,25)) -> Dict[str, Optional[CompressionResult]]:
        """
        Run grid search to find optimal Brotli compression parameters.
        
        Tests different quality levels, modes, and window sizes to identify
        optimal parameters for different optimization goals.
        
        Args:
            logFlag (bool): Whether to log each iteration's results
            qualityRange (range): Range of quality levels (0-11)
            modeRange (range): Range of modes (0=generic, 1=text, 2=font)
            lgwinRange (range): Range of log window sizes (10-24)
            
        Returns:
            dict: Dictionary with keys 'optimizedTime', 'optimizedPeakMemory',
                  'optimizedCompressionPercentage', 'optimizedCompressionRatio',
                  each containing the optimal CompressionResult for that metric
        """
        logger.info(f"Starting Brotli grid search with quality {qualityRange}, mode {modeRange}, lgwin {lgwinRange}")

        # Dictionary to store the optimal results
        optimalResults = {
            'optimizedTime': None,
            'optimizedPeakMemory': None,
            'optimizedCompressionPercentage': None,
            'optimizedCompressionRatio': None
        }

        # Initialize the minimum and maximum values for each metric.
        minimumTimeTaken = float('inf')
        minimumPeakMemoryUsage = float('inf')
        maximumCompressionPercentage = float('-inf')
        maximumCompressionRatio = float('-inf')

        # Grid Search - O(n^3)
        for quality in qualityRange:
            for mode in modeRange:
                for lgwin in lgwinRange:
                    result = self.profile(quality=quality, mode=mode, lgwin=lgwin)

                    if result.timeTaken < minimumTimeTaken:
                        minimumTimeTaken = result.timeTaken
                        optimalResults['optimizedTime'] = result
                    if result.peakMemoryUsage < minimumPeakMemoryUsage:
                        minimumPeakMemoryUsage = result.peakMemoryUsage
                        optimalResults['optimizedPeakMemory'] = result
                    if result.compressionPercentage > maximumCompressionPercentage:
                        maximumCompressionPercentage = result.compressionPercentage
                        optimalResults['optimizedCompressionPercentage'] = result
                    if result.compressionRatio > maximumCompressionRatio:
                        maximumCompressionRatio = result.compressionRatio
                        optimalResults['optimizedCompressionRatio'] = result
                    if logFlag:
                        logger.info(f"Quality {quality:>7} | Mode {mode:>4} | Lgwin {lgwin:>5} | "
                          f"Compression: {result.compressionPercentage:>7.2f}% | "
                          f"Time: {result.timeTaken:>10.6f}s | "
                          f"Memory: {result.peakMemoryUsage:>12.6f} MB | "
                          f"Ratio: {result.compressionRatio:>8.4f}")
    
        logger.info(f"Brotli grid search complete")
        return optimalResults

    def _get_param_columns(self, result: CompressionResult) -> str:
        """
        Get algorithm-specific parameter columns for table output.
        
        Args:
            result (CompressionResult): The compression result to format
            
        Returns:
            str: Formatted parameter columns
        """
        if self.compression_algorithm == 'lz4':
            return f"{result.compressionLevel} | {result.blockSize}"
        elif self.compression_algorithm == 'gzip':
            return f"{result.compressLevel}"
        elif self.compression_algorithm == 'brotli':
            return f"{result.quality} | {result.mode} | {result.lgwin}"
        elif self.compression_algorithm == 'zstd':
            return f"{result.level}"
        return ""

    def _get_header_row(self) -> tuple:
        """
        Get the header row for the results table.
        
        Returns:
            tuple: (header_line, separator_line)
        """
        common_cols = "Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio"
        
        headers = {
            'lz4': (
                f"| Optimized For | Compression Level | Block Size | {common_cols} |",
                "|--------------|-------------------|------------|------------------------|------------|-------------------|-------------------|"
            ),
            'gzip': (
                f"| Optimized For | Compress Level | {common_cols} |",
                "|--------------|----------------|------------------------|------------|-------------------|-------------------|"
            ),
            'brotli': (
                f"| Optimized For | Quality | Mode | Lgwin | {common_cols} |",
                "|--------------|---------|------|-------|------------------------|------------|-------------------|-------------------|"
            ),
            'zstd': (
                f"| Optimized For | Level | {common_cols} |",
                "|--------------|-------|------------------------|------------|-------------------|-------------------|"
            )
        }
        
        return headers.get(self.compression_algorithm, ("", ""))

    def _format_result_row(self, optimization_type: str, result: CompressionResult) -> str:
        """
        Format a single result row for the table.
        
        Args:
            optimization_type (str): Type of optimization (e.g., "Optimized Time")
            result (CompressionResult): The compression result to format
            
        Returns:
            str: Formatted table row
        """
        params = self._get_param_columns(result)
        return (f"| {optimization_type} | {params} | "
                f"{result.compressionPercentage:>7.2f}% | "
                f"{result.timeTaken:>10.6f}s | "
                f"{result.peakMemoryUsage:>12.6f} MB | "
                f"{result.compressionRatio:>8.4f}")

    def print_optimal_results(self, optimalResults: Dict[str, Optional[CompressionResult]]) -> None:
        """
        Print optimal compression results in a formatted table.
        
        This method displays the optimal parameters found for different
        optimization goals (time, memory, compression) in a markdown table format.
        
        Args:
            optimalResults (dict): Dictionary containing optimal results for
                                   different optimization metrics
        """
        logger.info(f"Printing optimal results for {self.compression_algorithm}")
        
        # Get and print headers
        header, separator = self._get_header_row()
        print(header)
        print(separator)
        
        # Print each optimization type
        optimization_types = [
            ('optimizedTime', 'Optimized Time'),
            ('optimizedPeakMemory', 'Optimized Peak Memory'),
            ('optimizedCompressionPercentage', 'Optimized Compression Percentage')
        ]
        
        for key, label in optimization_types:
            if key in optimalResults and optimalResults[key] is not None:
                print(self._format_result_row(label, optimalResults[key]))
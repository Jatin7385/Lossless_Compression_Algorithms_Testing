"""
LZ4 compression parameter optimization script.

This script performs a grid search to find optimal LZ4 compression parameters
for different optimization goals (time, memory, compression ratio).
"""

from compression_utils import lz4Processing
from optimizer_utils import Optimizer
import lz4.frame
import logging
import sys

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

def main():
    """
    Main function to run LZ4 parameter optimization.
    
    Performs grid search over compression levels and block sizes to find
    optimal settings for different use cases.
    """
    # Read the data from the file with error handling
    try:
        with open('data.txt', 'r', encoding='utf-8') as file:
            text = file.read()
        logger.info(f"Loaded data.txt successfully ({len(text)} characters)")
    except FileNotFoundError:
        logger.error("Error: data.txt not found")
        sys.exit(1)
    except Exception as e:
        logger.error(f"Error reading file: {e}")
        sys.exit(1)

    # Define the range of parameters to search
    # -5 is fastest (negative = fast mode), 16 is maximum compression
    compressionLevelRange = range(-5, 17)
    
    # LZ4 block sizes: must use lz4.frame constants
    blockSizeRange = [
        lz4.frame.BLOCKSIZE_DEFAULT,    # 0 - Auto (default)
        lz4.frame.BLOCKSIZE_MAX64KB,    # 4 - 64KB blocks
        lz4.frame.BLOCKSIZE_MAX256KB,   # 5 - 256KB blocks
        lz4.frame.BLOCKSIZE_MAX1MB,     # 6 - 1MB blocks
        lz4.frame.BLOCKSIZE_MAX4MB      # 7 - 4MB blocks
    ]

    logger.info(f"Starting grid search: compression levels {compressionLevelRange}, block sizes {len(blockSizeRange)} options")
    logger.info(f"Total combinations: {len(compressionLevelRange) * len(blockSizeRange)}")

    # Create the Optimizer object
    optimizer = Optimizer(text, compression_algorithm='lz4', printFlag=False)

    # Run grid search
    optimalResults = optimizer.run_lz4_grid_search(
        logFlag=False, 
        compressionLevelRange=compressionLevelRange, 
        blockSizeRange=blockSizeRange
    )

    # Print results
    optimizer.print_optimal_results(optimalResults)
    logger.info("Optimization complete!")

if __name__ == "__main__":
    main()
    
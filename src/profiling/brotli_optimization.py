"""
Brotli compression parameter optimization script.

This script performs a grid search to find optimal Brotli compression parameters
for different optimization goals (time, memory, compression ratio).
"""

from compression_utils import brotliProcessing
from optimizer_utils import Optimizer
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
    Main function to run Brotli parameter optimization.
    
    Performs grid search over quality, mode, and window size parameters
    to find optimal settings for different use cases.
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
    qualityRange = range(0, 12)  # 0-11
    modeRange = range(0, 3)      # 0=generic, 1=text, 2=font
    lgwinRange = range(10, 25)   # Window size: 2^lgwin bytes

    logger.info(f"Starting grid search: quality={qualityRange}, mode={modeRange}, lgwin={lgwinRange}")
    logger.info(f"Total combinations: {len(qualityRange) * len(modeRange) * len(lgwinRange)}")

    # Create the Optimizer object
    optimizer = Optimizer(text, compression_algorithm='brotli', printFlag=False)

    # Run grid search
    optimalResults = optimizer.run_brotli_grid_search(
        logFlag=False, 
        qualityRange=qualityRange, 
        modeRange=modeRange, 
        lgwinRange=lgwinRange
    )

    # Print results
    optimizer.print_optimal_results(optimalResults)
    logger.info("Optimization complete!")

if __name__ == "__main__":
    main()
    
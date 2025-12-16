"""
Gzip compression parameter optimization script.

This script performs a grid search to find optimal Gzip compression levels
for different optimization goals (time, memory, compression ratio).
"""

from compression_utils import gzipProcessing
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
    Main function to run Gzip parameter optimization.
    
    Performs grid search over compression levels to find optimal settings
    for different use cases.
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
    # 1-9: 1 is fastest, 9 is maximum compression (0 means no compression)
    compressLevelRange = range(1, 10)

    logger.info(f"Starting grid search with compression levels {compressLevelRange}")

    # Create the Optimizer object
    optimizer = Optimizer(text, compression_algorithm='gzip', printFlag=False)

    # Run grid search
    optimalResults = optimizer.run_gzip_grid_search(
        logFlag=False, 
        compressLevelRange=compressLevelRange
    )

    # Print results
    optimizer.print_optimal_results(optimalResults)
    logger.info("Optimization complete!")

if __name__ == "__main__":
    main()
    
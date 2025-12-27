"""
Zstandard (Zstd) compression parameter optimization script.

This script performs a grid search to find optimal Zstd compression levels
for different optimization goals (time, memory, compression ratio).
"""

from compression_utils import zstdProcessing
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
    Main function to run Zstd parameter optimization.
    
    Performs grid search over compression levels to find optimal settings
    for different use cases.
    """
    # Read the data from the file with error handling
    try:
        with open('data.txt', 'r', encoding='utf-8') as file:
            text = file.read()
        logger.info(f"Loaded data.txt successfully ({len(text)} characters)")
        logger.info(f"Original size: {len(text.encode('utf-8'))} bytes")
    except FileNotFoundError:
        logger.error("Error: data.txt not found")
        sys.exit(1)
    except Exception as e:
        logger.error(f"Error reading file: {e}")
        sys.exit(1)

    # Define the range of parameters to search
    # 1-22: 1 is fastest, 22 is maximum compression
    level = range(1, 23)

    logger.info(f"Starting grid search with compression levels {level}")

    # Create the Optimizer object
    optimizer = Optimizer(text, compression_algorithm='zstd', printFlag=False)

    # Run grid search
    optimalResults = optimizer.run_zstd_grid_search(
        logFlag=False, 
        levelRange=level
    )

    # Print results
    optimizer.print_optimal_results(optimalResults)
    logger.info("Optimization complete!")

if __name__ == "__main__":
    main()
    
from compression_utils import brotliProcessing
from optimizer_utils import Optimizer
import lz4.frame

def main():
    # Read the data from the file
    with open('data.txt', 'r') as file:
        text = file.read()

    # ------------------- Start Profiling -------------------
    print(f"Original size: {len(text.encode('utf-8'))} bytes")

    # Define the range of parameters to search
    level = range(1, 23) # 1 to 22, but 1 is the fastest and 22 is the slowest.

    # Create the ZstdOptimizer object
    optimizer = Optimizer(text, compression_algorithm='zstd', printFlag=False)

    # Run grid search
    optimalResults = optimizer.run_zstd_grid_search(logFlag=False, levelRange=level)

    optimizer.print_optimal_results(optimalResults)

if __name__ == "__main__":
    main()
    
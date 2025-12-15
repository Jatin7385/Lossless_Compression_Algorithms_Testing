from compression_utils import brotliProcessing
from optimizer_utils import Optimizer

def main():
    # Read the data from the file
    with open('data.txt', 'r') as file:
        text = file.read()

    # Define the range of parameters to search
    compressLevelRange = range(1, 10) # 1 - 9, but 0 is no compression. 1 is the fastest and 9 is the slowest.

    # Create the BrotliOptimizer object
    optimizer = Optimizer(text, compression_algorithm='gzip', printFlag=False)

    # Run grid search
    optimalResults = optimizer.run_gzip_grid_search(logFlag=False, compressLevelRange=compressLevelRange)

    optimizer.print_optimal_results(optimalResults)

if __name__ == "__main__":
    main()
    
from compression_utils import brotliProcessing
from optimizer_utils import Optimizer
import lz4.frame

def main():
    # Read the data from the file
    with open('data.txt', 'r') as file:
        text = file.read()

    # Define the range of parameters to search
    compressionLevelRange = range(-5, 17) # -5 to 16, but 0 is no compression. -5 is the fastest and 16 is the slowest.
    # LZ4 block sizes: must use lz4.frame constants, not raw integers
    blockSizeRange = [
        lz4.frame.BLOCKSIZE_DEFAULT,    # 0 - Auto (default)
        lz4.frame.BLOCKSIZE_MAX64KB,    # 4 - 64KB blocks
        lz4.frame.BLOCKSIZE_MAX256KB,   # 5 - 256KB blocks
        lz4.frame.BLOCKSIZE_MAX1MB,     # 6 - 1MB blocks
        lz4.frame.BLOCKSIZE_MAX4MB      # 7 - 4MB blocks
    ]

    # Create the LZ4Optimizer object
    optimizer = Optimizer(text, compression_algorithm='lz4', printFlag=False)

    # Run grid search
    optimalResults = optimizer.run_lz4_grid_search(logFlag=False, compressionLevelRange=compressionLevelRange, blockSizeRange=blockSizeRange)

    optimizer.print_optimal_results(optimalResults)

if __name__ == "__main__":
    main()
    
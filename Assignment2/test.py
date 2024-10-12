import numpy as np
import ctypes
from numpy.ctypeslib import ndpointer
import time

# Load your shared library
mySortLib = ctypes.CDLL('./libmysort.so')

# Define the C function signature using ndpointer
mySortLib.insertionSort.argtypes = [ndpointer(ctypes.c_int, flags="C_CONTIGUOUS"), ctypes.c_int]
mySortLib.insertionSort.restype = None

# Create a large array
arr = np.random.randint(-1000000, 1000000, size=500000, dtype=np.int32)

# Copy the array to avoid sorting in place (optional)
arr_copy = np.copy(arr)

n = len(arr_copy)

# Measure time and call the C function
start = time.time()
mySortLib.insertionSort(arr_copy, n)
end = time.time()

print("Sorted array using Insertion Sort:", arr_copy)
print(f"Time to sort: {end - start} seconds")
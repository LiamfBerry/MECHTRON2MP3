import numpy as np
from scipy.optimize import differential_evolution

# Define the Michalewicz function
def michalewicz(x, m=10):
    d = len(x)
    return -sum(np.sin(x[i]) * (np.sin((i + 1) * x[i]**2 / np.pi))**(2 * m) for i in range(d))

# Set bounds for each dimension [0, π]
def get_bounds(d):
    return [(0, np.pi) for _ in range(d)]

# Optimize the Michalewicz function using Differential Evolution
def find_global_minimum(d):
    bounds = get_bounds(d)
    result = differential_evolution(michalewicz, bounds, strategy='best1bin', popsize=15, tol=1e-6)
    return result.fun

print("Started")
# Find the global minimum for d=50 and d=100
min_50 = find_global_minimum(50)
min_100 = find_global_minimum(100)

print(f"Global minimum for d=50: {min_50:.6f}")
print(f"Global minimum for d=100: {min_100:.6f}")

# Create and enter build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
make

# Run tests (if any)
make test

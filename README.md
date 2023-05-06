# SYCL-financial-derivatives
By team Breakfast Banana for Intel oneapi codemaven hackathon. We developed this project to make GBM based stock prediction much faster using SYCL form intel OneApi.

# Requirements
 - Intel oneApi HPC toolkit
 - Gnuplot for plotting
 - Proper extensions to run on GPU
 
# Building
Clone with --recursive flag
git clone https://github.com/Surya99941/SYCL-financial-derivatives.git --recursive

### setup environment
replace path with your oneapi installation directory
$ . /opt/intel/oneapi/setvars.sh --include-intel-llvm
$ make cpusycl # Sycl with CPU
$ make gpu # Sycl with GPU

Respective biaries will be at the project directory

# Using
[Your binary file] -i [path to input csv file] -o [path to output file] -p [true/false] -samp [number of samples] -days [number of days]
 -i [string] is mandatory
 -o [string] optional with default as ./res.pdf
 -p [true/false] true by default
 -samp [integer] optional with default 1000
 -days [integer] optional with default 252

# Benchmarking
For benchmarking Run the code with -p as false, since plotting is pretty expensive and done on host.

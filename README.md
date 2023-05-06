# SYCL financial derivatives Monte Carlo Simulation
By team Breakfast Banana for Intel oneapi codemaven hackathon. We developed this project to make GBM based stock prediction much faster using SYCL form intel OneApi.

Github link of the project : https://github.com/Surya99941/SYCL-financial-derivatives

# Requirements
 - Intel oneApi HPC toolkit
 - Gnuplot installed for plotting
 - Proper extensions to run on GPU
 
# Building
Clone with --recursive flag

```git clone https://github.com/Surya99941/SYCL-financial-derivatives.git --recursive```

### setup environment
replace path with your oneapi installation directory

```$ . /opt/intel/oneapi/setvars.sh --include-intel-llvm
$ make cpusycl # Sycl with CPU
$ make gpu # Sycl with GPU
$ make cpu # without sycl for benchmarking
```

Respective bianries will be at the project directory

# Using
```[Your binary file] -i [path to input csv file] -o [path to output file] -p [true/false] -samp [number of samples] -days [number of days]```
 - -i [string] is mandatory : Input file (look at training_data.csv in this repo for format)
 - -o [string] optional with default as ./res.pdf : output file PDF is reccomended
 - -p [true/false] true by default : Wheter or not show the plots, keep it false while benchmarking
 - -samp [integer] optional with default 1000 : How many samples has to be taken
 - -days [integer] optional with default 252 : How many days in future to look

# Benchmarking
For benchmarking Run the code with -p as false, since plotting is pretty expensive and done on host.
Few benchmarks I ran on my system just taken with time command in linux ( so not very accurate, just the basic idea)

CPU: Ryzen 5 5600H (laptop cpu)
GPU: Nvidia RTX 3060 Laptop GPU
| Sample | Days | SYCL-GPU(s) | SYCL-CPU(s) | CPU(s) |
|--------|------|-------------|-------------|--------|
| 1000   | 252  | 1.2         | 1.38        | 3.55   |
| 10000  | 252  | 1.3         | 1.43        | 40     |
| 50000  | 252  | 1.46        | 4.19        | 177.19 |
| 100000 | 252  | 1.67        | 8.44        | 355    |

# Other Links 
 - Presentation : https://docs.google.com/presentation/d/18E6k8AlqdnkcgE88PwGWv_S7leb1-_5HQZGlz8ZDV5o/edit?usp=sharing
 - Video : https://drive.google.com/file/d/1B52u-0Nte6kxQoro_3A_MJbyiKGwU9KF/view?usp=sharing
 - Github : https://github.com/Surya99941/SYCL-financial-derivatives

These files are also available in this repo

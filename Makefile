CC = clang++
CCS = icpx
CFLAGS_NV = -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda
CFLAGS = "-I./vendor/sciplot"

EXECUTABLE = ./GBM


clean:
	rm -f $(EXECUTABLE)

cpu:
	$(CC) $(CFLAGS) -O3 src/main_cpu.cpp -o $(EXECUTABLE)_cpu

cpusycl:
	$(CCS) $(CFLAGS) -fsycl -O3 src/main_cpu_sycl.cpp -o $(EXECUTABLE)_cpusycl

gpu:
	$(CC) $(CFLAGS_NV) $(CFLAGS) -O3 src/main.cpp -o $(EXECUTABLE)_gpu

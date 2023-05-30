#include "GBM.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <string.h>
#include <stdio.h>

#include "stockdata.hpp"

#include <CL/sycl.hpp>
#include <oneapi/dpl/random>


// days = No of days to be calculated + 1 is to store initial value for calculation
// samples = No of samples
double* GBM(int& days, int& samples, std::vector<StockData>& data) {
    days++;
    double dt = (double)1/days;          //Scaling factor for drift
    int size = days*samples;             //Size of buffer

    //Calculate Mean and SD
    auto temp = Mesd(data);
    const double mean =temp.first, stdDev = temp.second;
    const double s0 = data[data.size()-1].close;

    //Buffer for storing paths
    sycl::buffer<double,1> buf((sycl::range<1>(size)));

    sycl::queue simulate{sycl::gpu_selector_v};

    {   //Sycl Section
        simulate.submit([&] (sycl::handler& h) {
            auto dbuf = buf.get_access<sycl::access::mode::write>(h);

            h.parallel_for<class Simulate>
                (
                    sycl::range<1> {static_cast<unsigned int>(size)},
            [=](sycl::item<1> index) {

                //Generate Random normal number epx
                std::uint64_t offset = index.get_linear_id();
                oneapi::dpl::minstd_rand engine(1, offset);
                oneapi::dpl::normal_distribution<float> z(0,stdDev);
                //Actual Calculation of GBM
                double drift = (mean- (0.5*(stdDev*stdDev)));
                double epx = z(engine) * (1/cbrt(dt));
                dbuf[index] = std::exp( drift + epx );
            }
            );
        }).wait();
    }

    sycl::queue calculate_path{sycl::gpu_selector_v};
    {
        calculate_path.submit([&] (sycl::handler& h) {
            auto dbuf = buf.get_access<sycl::access::mode::write>(h);

            h.parallel_for<class CalculatePath>(
                    sycl::range<1> {static_cast<unsigned int>(samples)},
            [=](sycl::item<1> index) {
                const int i = index*days;
                dbuf[i] = s0;
                for(int j = 1; j < days; j++) {
                    dbuf[i + j] = dbuf[ i + j-1] * dbuf[i+j];
                }
            });
        }).wait();
    }



    auto buf_cpu = buf.get_access<sycl::access::mode::read_write>();
    double* res = buf_cpu.get_pointer();
    double* ret = (double*)malloc(size*sizeof(double));
    memcpy(ret,res,size*sizeof(double));
    return ret;
}

std::pair<double,double> Mesd(std::vector<StockData>& v){
  double mean = 0;
  double lt   = 0;
  double mt   = 0;
  double st   = 0;
  int size    = v.size();
  int mid     = size/2;
  int sht     = size - size/4;
  for(int i = 0; i < size; i++) {
    mean += v[i].log_return;

    if(i >= mid) mt += v[i].log_return;
    if(i >= sht) st += v[i].log_return;
  }
  mean = mean/v.size();
  st   = st/sht;
  mt   = mt/mid;

  mean = (mean + st*2 + mt)/4;
  
  double sd = 0;
  for( StockData& i : v) sd += pow((i.log_return - mean),2);
  sd = sd/v.size();

  return std::pair(mean,sd);
}
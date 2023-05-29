#include "main.hpp"
#include "stockdata.hpp"
#include "readdata.hpp"
#include <cmath>
// days = No of days to be calculated + 1 is to store initial value for calculation
// samples = No of samples
int GBM(int& days, int& samples, bool is_plot) {
    days++;
    double dt = (double)1/days;          //Scaling factor for drift
    int size = days*samples;     //Size of buffer

    //Read data
    std::vector<StockData> data = readdata(1000+days);

    {
        std::reverse(data.begin(),data.end());
        data = std::vector<StockData>(data.begin(), data.begin()+(data.size()-days));
    }
    //Sorting based on date
    //std::sort(data.begin(),data.end(),DateSort);

    //Calculate Mean and SD
    auto temp = Mesd(data);
    const double mean =temp.first, stdDev = temp.second;
    const double s0 = data[data.size()-1].close;

    printf("DT = %lf\n",dt);

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
                oneapi::dpl::normal_distribution<float> z(0,1);
                //Actual Calculation of GBM
                double drift = (mean- (0.5*(stdDev*stdDev))) * dt;
                double epx = z(engine) * stdDev * sqrt(dt);
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


    //Copy to host buffer
    auto buf_cpu = buf.get_access<sycl::access::mode::read_write>();
    double* res = buf_cpu.get_pointer();

    GLplot myplot(1280,720);
    myplot.add_data(res, samples, days, data);
    myplot.draw();
    return 0;
}

void call_error() {
    std::cout<<"Usage"<<std::endl;
    std::cout<<"GBM -i [path to csv file] -o [path to output file] -p [true/false] -samp [number of samples] -days [number of days]"<<std::endl;
    exit(0);
}

#include <string.h>
int main(int argc, char* argv[]) {
    bool is_plot = true;
    int samp = 1000;
    int days = 252;
    for( int i = 1; i < argc; i++) {
        if(strcmp("-p",argv[i]) == 0) {
            if(i+1 < argc) {
                if(strcmp(argv[i+1],"true") == 0) is_plot = true;
                else if(strcmp(argv[i+1],"false") == 0) is_plot = false;
                else {
                    printf("Possible values of -p is true/false but %s provided so falling back to default (true)",argv[i+1]);
                }
            }
            else call_error();
        }
        if(strcmp("-samp",argv[i]) == 0) {
            if(i+1 < argc) {
                samp = atoi(argv[i+1]);
            }
            else call_error();
        }
        if(strcmp("-days",argv[i]) == 0) {
            if(i+1 < argc) {
                days = atoi(argv[i+1]);
            }
            else call_error();
        }
    }
    GBM(days,samp,is_plot);
    return 0;
}

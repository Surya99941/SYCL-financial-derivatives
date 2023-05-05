#include "main.hpp"



class VecAdd;

int main() {
    //Initialize
    sycl::queue q{sycl::gpu_selector_v};
    std::cout
            << "Running on "
            << q.get_device().get_info<sycl::info::device::name>()
            << std::endl;


    //Initialize Constants
    const int T = 252;                //No of days to be calculated
    const int samples = 3000;          //No of samples
    const int N = T;               //No of calculations per day (obsolete :>)
    const double dt = 1/T; 
    const int size = N*samples;       //Size of buffer

    //Read data
    std::unordered_map<std::string, std::vector<StockData>> stocklists = ReadFile("training_data.csv");
    std::vector<StockData> data = stocklists["MSFT"];
    //Sorting based on date
    std::sort(data.begin(),data.end(),DateSort);
    {
        int t = data.size();
        int b = t -500;
        data = std::vector(data.begin()+b,data.end());
    }
    
    //Calculate Mean and SD
    auto temp = Mesd(data);
    const double mean =temp.first, stdDev = temp.second;


    const double s0 = data[data.size()-1].close;
    std::cout<<mean<<" | "<<stdDev<<std::endl;
    
    //Buffer for storing paths
    sycl::buffer<double,1> buf((sycl::range<1>(size)));
    sycl::buffer<double,1> norm((sycl::range<1>(size)));

    {   //Sycl Section
        q.submit([&] (sycl::handler& h) {
            auto dbuf = buf.get_access<sycl::access::mode::write>(h);
            auto z = norm.get_access<sycl::access::mode::write>(h);

            h.parallel_for<class VecAdd>
                (
                sycl::range<1> {static_cast<unsigned int>(size)},
            [=](sycl::item<1> index) {

                    //Generate Random normal number epx
                    std::uint64_t offset = index.get_linear_id();
                    oneapi::dpl::minstd_rand engine(1, offset);
                    oneapi::dpl::normal_distribution<float> distr(0,1);
                    z[index] = distr(engine);
                    //Actual Calculation of GBM
                    double drift = (mean-(0.5*pow(stdDev,2)))*dt;
                    double epx = stdDev * z[index];
                    dbuf[index] = std::exp( drift + epx );
            }
            );
        }).wait();
    }

    //Copy to host buffer
    auto buf_cpu = buf.get_access<sycl::access::mode::read_write>();
    auto norm_cpu = norm.get_access<sycl::access::mode::read_write>();
    double* res = buf_cpu.get_pointer();

    
    //for (int i = 0; i < size; i++){ std::cout<<res[i]<<" , ";}
    std::cout<<std::endl;
    //for (int i = 0; i < size; i++){ std::cout<<norm_cpu[i]<<" , ";}
    std::cout<<std::endl;

    res[0] = s0;
    for (int i = 1; i < size; i++){
        //if its first element of the sample make it S0
        if(i%N == 0){
            res[i] = s0;
           // float risk = sharpe_ratio(&res[i],N,0.8);
        }
        else{ //Si = S(i-1)* calculated value which is in res btw
            res[i] = (res[i]*res[i-1]);
        }
        
    }
    //Writing data to file
    write_file("out.m",size,N, res, data);
    return 0;
}
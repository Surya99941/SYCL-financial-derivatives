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
    const float dt = 1;               //time difference between concecutive stock prices
    const int T = 252;                //No of days to be calculated
    const int samples = 100;          //No of samples
    const int N = T/dt;               //No of calculations per day (obsolete :>)
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

    //Host side buffer
    double res[size];

    const double s0 = data[data.size()-1].close;

    {   //Sycl Section

        //Intialize Buffer for single sample
        //Buffer in GPU written back once scope ends
        sycl::buffer<double,1> buf(res, size);

        q.submit([&] (sycl::handler& h) {
            auto dbuf = buf.get_access<sycl::access::mode::write>(h);

            h.parallel_for<class VecAdd>
                (
                sycl::range<1> {static_cast<unsigned int>(size)},
            [=](sycl::item<1> index) {

                    //Generate Random normal number epx
                    std::uint64_t offset = index.get_linear_id();
                    oneapi::dpl::minstd_rand engine(777, offset);
                    oneapi::dpl::normal_distribution<float> distr(0,1);

                    //Actual Calculation of GBM
                    double drift = mean-(0.2*pow(stdDev,2));
                    double epx = distr(engine)*stdDev*0.01;
                    dbuf[index+1] = std::exp( drift + epx );
            }
            );
        });
    }

    for (int i = 1; i < size; i++){
        //if its first element of the sample make it S0
        bool pit = false;
        if(i%N == 0){
            if(pit) std::cout<<"\n"<<std::endl;
            res[i] = s0;
            float risk = sharpe_ratio(&res[i],N,0.8);
            if(risk < 1){
                pit = true;
                printf("%d:%f:",i,risk);    
            }
            else pit = false;
        }
        else{ //Si = S(i-1)* calculated value which is in res btw
            res[i] = res[i]*res[i-1];
            if(pit) std::cout<<res[i]<<",";
        }
        
    }
    //Writing data to file
    write_file("out.m",size,N, res, data);
    return 0;
}
#include "stockdata.hpp"
#include "window.h"




class GLplot {
private:
    int points;

    //Left side
    double* stock_data;
    double* stock_days;
    int ndays;
    int nsamples;
    double* old_data;
    double* old_days;
    int ndays_old;

    //Right side
    double* hist_data;
    double* hist_steps;
    int bin_size;

    //Metrics Data
    double expected_return;
    double return_percent;
    double probability;
    
    Window* m_window;
    
public:
    GLplot(Window&);
    void add_data(double* dataptr, long int samples, long int days, std::vector<StockData>& old);
    void draw();
    ~GLplot();
};
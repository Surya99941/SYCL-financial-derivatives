#include "stockdata.hpp"
#include "window.h"




class GLplot {
private:
    int points;
    double* stock_data;
    double* stock_days;
    int ndays;
    int nsamples;
    double* old_data;
    double* old_days;
    int ndays_old;
    double* hist_data;
    double* hist_steps;
    int bin_size;

    Window* m_window;
    
public:
    GLplot(Window&);
    void add_data(double* dataptr, long int samples, long int days, std::vector<StockData>& old);
    void draw();
    ~GLplot();
};
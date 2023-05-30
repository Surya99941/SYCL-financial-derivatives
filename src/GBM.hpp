#pragma once

#include <vector>
#include <string>
#include "stockdata.hpp"

double* GBM(int& days, int& samples, std::vector<StockData>& old_data);
std::pair<double,double> Mesd(std::vector<StockData>& v);

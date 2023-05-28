#pragma once


#include <cstddef>
#include "rapidcsv.h"
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <string>
#include <stdio.h>
#include "glplot.hpp"
#include "stockdata.hpp"
#include <oneapi/dpl/random>
#include <CL/sycl.hpp>


std::pair<double,double> Mesd(std::vector<StockData>& v){
  double mean = 0;
  for( StockData& i : v) mean += i.log_return;
  mean = mean/v.size();
  
  double sd = 0;
  for( StockData& i : v) sd += pow((i.log_return - mean),2);
  sd = sd/v.size();

  return std::pair(mean,sd);
}

bool DateSort(StockData& A, StockData& B){
  Date& a = A.date, b = B.date;
  if(a.year < b.year){
    return true;
  }
  else if( a.year == b.year){
    if(a.month < b.month){
      return true;
    }
    else if(a.month == b.month){
      if(a.day < b.day){
        return true;
      }
    }
  }
  return false;
}

double sharpe_ratio(double* returns,int size, double risk_free_rate) {
    double excess_return = 0.0;
    double mean_return = 0.0;
    double std_dev = 0.0;

    // Calculate mean return
    for (int ret = 0; ret < size; ret++) {
        mean_return += returns[ret];
    }
    mean_return /= size;

    // Calculate excess return and standard deviation
    for (int i = 0; i < size; i++) {
        excess_return += returns[i] - risk_free_rate;
        std_dev += pow(returns[i] - mean_return, 2);
    }
    std_dev = sqrt(std_dev / (size - 1));

    // Calculate Sharpe Ratio
    return excess_return / std_dev;
}
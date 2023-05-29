#pragma once


#include <cstddef>
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
  for(auto i = v.begin(); i != v.end(); i++) {
    mean += i->log_return;
  }
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


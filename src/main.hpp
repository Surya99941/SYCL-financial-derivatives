#pragma once


#include <cstddef>
#include <iostream>
#include <cmath>
#include <array>
#include "rapidcsv.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include "glplot.hpp"
#include <oneapi/dpl/random>
#include <CL/sycl.hpp>

struct Date{
  int day;
  int month;
  int year;
  std::string st_date;
  Date(std::string st){
    st_date = st;
    //"2023-02-22";
    day   = std::stoi(st.substr(0,4));
    month = std::stoi(st.substr(5,2));
    year  = std::stoi(st.substr(8,2));
  }
};

struct StockData{
  StockData(std::string date, std::string name, double ret,double cls)
  : date(date)
  , name(name)
  , stock_return(ret)
  , close(cls)
  {
    log_return = std::log(1+ret);
  }
  Date date;
  std::string name;
  double stock_return;
  double log_return;
  double close;
  double open;
};

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


std::vector<StockData> ReadFile(const char* file){
    //Read CSV
    rapidcsv::Document doc(file);
    std::string Stock = "AMZN";
    std::vector<double> close = doc.GetColumn<double>("Close");
    std::vector<std::string> date = doc.GetColumn<std::string>("Date");
    double prev_close = 0;
    bool has_prev = false;
    std::vector<StockData> stocks;

    for(int i = 0; i < close.size(); i++) {
        if(has_prev == false) {
          prev_close = close[i];
          has_prev = true;
        }
        //StockData(std::string date, std::string name, double ret,double cls)
        else stocks.emplace_back(date[i],Stock,(prev_close - close[i])/close[i],close[i]);
    }

    return stocks;
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
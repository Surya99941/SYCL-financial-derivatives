#pragma once

#include <iostream>
#include <vector>
#include <string>

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
  StockData(std::string date, double ret,double cls)
  : date(date)
  , stock_return(ret)
  , close(cls)
  {
    log_return = std::log(1+ret);
  }
  Date date;
  double stock_return;
  double log_return;
  double close;
};

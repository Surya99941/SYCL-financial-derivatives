#pragma once


#include <cstddef>
#include <iostream>
#include <cmath>
#include <array>
#include "csv.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include <oneapi/dpl/random>
#include <CL/sycl.hpp>


struct Date{
  int day;
  int month;
  int year;
  std::string st_date;
  Date(std::string st){
    st_date = st;
    day   = std::stoi(st.substr(0,2));
    month = std::stoi(st.substr(3,2));
    year  = std::stoi(st.substr(6,4));
  }
};

struct StockData{
  StockData(std::string date, std::string name, double ret,double cls,double opn)
  : date(date)
  , name(name)
  , stock_return(ret)
  , close(cls)
  , open(opn)
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


std::unordered_map<std::string, std::vector<StockData>> ReadFile(const char* file){
    //Read CSV
    io::CSVReader<7> mycsv(file);
    mycsv.read_header(io::ignore_extra_column, "Date",	"Open",	"High",	"Low", "Close",	"Volume",	"Stock");
    std::string Date, Stock;
    double Open, High, Low, Close, Volume,prev_close=0;
    bool has_prev = false;
    std::vector<StockData> stocks;

    while(mycsv.read_row(Date,Open,High,Low,Close,Volume,Stock)) {
        if(has_prev == false) {
          prev_close = Close;
          has_prev = true;
        }
        else stocks.emplace_back(Date,Stock,(prev_close - Close)/Close,Close,Open);
    }

    //Seperate Stocks
    std::unordered_map<std::string, std::vector<StockData>> stocklists;

    for( StockData i : stocks) {
        stocklists[i.name].push_back(i);
    }
    return stocklists;
}

void write_file(const char* filepath,const int size, const int days, double* preds, std::vector<StockData>& st){
  FILE* file = fopen(filepath,"w");
  if (!file) {
    printf("Error opening file %s\n",filepath);
    return;
  }
  fprintf(file, "fig = figure();\nsubplot(2,2,1);");
  fprintf(file, "x = [");
  for(StockData& data: st){
    fprintf(file,"\"%s\" ;",data.date.st_date.c_str());
  }
  fprintf(file,"]\n");

  
  fprintf(file, "y = [");
  for(StockData& data: st){
    fprintf(file,"%f ;",data.stock_return);
  }
  fprintf(file,"]\n");
  fprintf(file,
    "x_datenum = datenum(x, \"dd-mm-yyyy\"); \n\
plot(x_datenum,y)\n\
datetick(\"x\", \"dd mmm yyyy\"); \n\
xlabel(\"Date\"); \n\
ylabel(\"Stock Return\");\n\
title(\"Stock return over time\"); \
    "
    );

  //Plot preds
  fprintf(file,"\n\nsubplot(2,2,2)\n");

  std::string px= "xp=[",py="yp=[";
  int o = 0;
  for( o = 0; o < st.size(); o++){
      px = px + std::to_string(o) + ",";
      py = py + std::to_string(st[o].close) + ",";
  }
  fprintf(file,"%s]\n%s]\nplot(xp,yp)\n\n",px.c_str(),py.c_str());
  
  fprintf(file,"\n\nsubplot(2,2,3)\n");

  int n = 1;
  std::string xs= "x0=[",ys="y0=[";
  //Preds X
  for(int i = 1; i < size; i++){
    if(i%days == 0){
      fprintf(file,"%s]\n%s]\nplot(x%d,y%d)\nhold on\n\n",xs.c_str(),ys.c_str(),n-1,n-1);
      xs = std::string("x")+  std::to_string(n) +std::string("=[") + std::to_string(i%days) + ",";
      ys = std::string("y")+  std::to_string(n) +std::string("=[") + std::to_string(preds[i]) + ",";
      n++;
    }
    else{
      xs = xs + std::to_string(i%days) + ",";
      ys = ys + std::to_string(preds[i]) + ",";
    }
  }
    
  fprintf(file,"%s]\n%s]\nplot(x%d,y%d)\n\n\n",xs.c_str(),ys.c_str(),n-1,n-1);
  fprintf(file,
"xlabel(\"Day\"); \n\
ylabel(\"Stock Return\");\n\
title(\"Predicted value\"); \
"
    ); 
  fclose(file);
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
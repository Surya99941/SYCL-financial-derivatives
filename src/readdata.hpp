#pragma once

#include "stockdata.hpp"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* data) {
    size_t totalSize = size * nmemb;
    data->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

void related(std::string key,std::vector<std::string>* search_result, bool* done){
    //https://www.alphavantage.co/query?function=SYMBOL_SEARCH&keywords=BA&apikey=demo&datatype=csv
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string apiKey = "8W0PNXMO5O5G4JD6";  // Replace with your Alpha Vantage API key
        std::string function = "SYMBOL_SEARCH";
        std::string url = "https://www.alphavantage.co/query?function=" + function +
                           "&keywords=" + key + "&apikey=" + apiKey + "&datatype=csv";

        std::string responseData;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        
        CURLcode chk = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (chk != CURLE_OK) {
            std::cerr << "Failed to retrieve data from the API: " << curl_easy_strerror(chk) << std::endl;
            *done = true;
            return;
        }

        const char* response = responseData.c_str();
        const char* read = response;
        while(*read != '\0'){
            while(*read != '\n') read++;
            read++;
            if(*read == '\0') break;
            char temp[20];
            sscanf(read,"%[^,]",temp);
            search_result->emplace_back(temp);
        }
    }
    *done = true;
}

std::vector<StockData> readdata(int days,std::string stock_name) {
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string apiKey = "8W0PNXMO5O5G4JD6";  // Replace with your Alpha Vantage API key
        std::string function = "TIME_SERIES_DAILY_ADJUSTED";
        std::string url = "https://www.alphavantage.co/query?function=" + function +
                           "&symbol=" + stock_name + "&apikey=" + apiKey + "&outputsize=full&datatype=csv";

        std::string responseData;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to retrieve data from the API: " << curl_easy_strerror(res) << std::endl;
            exit(EXIT_FAILURE);
        }

        std::istringstream iss(responseData);
        
        bool has_prev = false;
        double prev_close = 1;
        double  open, close,high,low,adjustedClose;
        int volume;
        char date[11];
        std::vector<StockData> stockDataList;
        stockDataList.reserve(days);

        const char* response =  responseData.c_str();
        const char* strptr = response;
        for(int o = 0; o < days;o++) {
            while(*strptr != '\n') strptr++;
            strptr++;
            int numMatches = sscanf(strptr, "%10[^,],%lf,%lf,%lf,%lf,%d,%lf,\n",
                                    date, &open, &high, &low,
                                    &close, &volume, &adjustedClose);
            auto strdate = std::string(date);
            if(has_prev == false) {
                has_prev = true;
                prev_close = close;
            }
            else{
                double ret = (close - prev_close)/prev_close;
                prev_close = close;
                stockDataList.emplace_back(strdate,ret,close);
            }
        }

        return stockDataList;
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
        exit(EXIT_FAILURE);
    }


}
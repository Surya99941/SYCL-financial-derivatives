#pragma once

#include <iostream>

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
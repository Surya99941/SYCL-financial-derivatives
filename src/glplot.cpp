#include "glplot.hpp"

#include <cstdio>
#include <vector>
#include <cmath>
#include <cstdlib>

#include <iostream>
#include <algorithm>


template <class T>
T map_range(T value, T inMin, T inMax, T outMin, T outMax){
    value = std::max(value, inMin);
    value = std::min(value, inMax);

    // Map the value to the output range
    T inRange = inMax - inMin;
    T outRange = outMax - outMin;
    T scaledValue = (value - inMin) / inRange;
    T mappedValue = outMin + (scaledValue * outRange);

    return mappedValue;
}

double probablity_of_breakeven(std::vector<double>& last_days, double base_line,double& min, double& max){
    int high = 0;
    for(auto i:last_days){
        double o = map_range<double>(i,min,max,0,100);
        if(o > base_line){
            high++;
        }
    }
    return ((double)high/last_days.size())*100;   
}


GLplot::GLplot(Window& win)
: m_window(&win)
{
    
}

void
GLplot::add_data(double* dataptr, long int samples, long int days, std::vector<StockData>& old) {
    old_data = (double*)malloc(old.size()*sizeof(double));
    old_days = (double*)malloc(old.size()*sizeof(double));
    for(int o = 0; o < old.size(); o++) {
        old_data[o] = old[o].close;
        old_days[o] = o;
    }
    ndays_old = old.size();
    this->points = samples*days;
    ndays = days;
    nsamples = samples;
    this->stock_data = dataptr;
    this->stock_days = (double *)malloc(sizeof(double) * points);
    for(int i = 0; i < samples*days; i++) {
        this->stock_days[i] = (i%days)+old.size();
    }

    expected_return = 0;
    return_percent = 0;
    std::vector<double> last_day(nsamples);
    for( int i = 0; i < samples; i++) {
        last_day[i] = dataptr[i*days + days - 1];
        expected_return += last_day[i];
        return_percent += dataptr[i*days];
    }
    expected_return /= nsamples;
    return_percent /= nsamples;
    return_percent =((expected_return - return_percent)/expected_return)*100;

    double min = *std::min_element(last_day.begin(),last_day.end());
    double max = *std::max_element(last_day.begin(),last_day.end());

    //Calculate Probablity
    this->probability = probablity_of_breakeven(last_day,50,min,max);
    
    //Allocations
    bin_size = 100;
    hist_data = (double*)malloc(sizeof(double)*bin_size);
    hist_steps = (double*)malloc(sizeof(double)*bin_size);
    for(int i = 0; i < bin_size; i++) {
        hist_data[i] = hist_steps[i] = 0;
    }

    //Fill Buckets
    for(double& i : last_day) {
        int bucket = std::floor(map_range<double>( i, min, max, 0, bin_size));
        hist_data[bucket]+=1;
    }
    //Calculate steps
    double step = (max-min)/bin_size;
    for(int i = 0; i < bin_size; i++) {
        hist_steps[i] = (i)*step + min;
    }
}

void
GLplot::draw() {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(m_window->getWidth()/2,m_window->getHeight()*0.9));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        // Create your ImGui UI elements here
        ImGui::Begin("My Window",NULL,(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove));  // Begin a new ImGui window

        // Add UI elements
        if (ImPlot::BeginPlot("Stock Price Paths", ImVec2(m_window->getWidth()/2,m_window->getHeight()*0.9))) {
            ImPlot::PlotLine("Price Paths History", old_days, old_data, ndays_old);
            for(int i = 0; i < nsamples; i++) {
                char line_name[20];
                sprintf(line_name,"Price Path:%d",i);
                ImPlot::PlotLine(line_name, &stock_days[i*ndays], &stock_data[i*ndays], ndays);
            }
        }
        ImPlot::EndPlot();
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2(m_window->getWidth()/2,m_window->getHeight()*0.9));
        ImGui::SetNextWindowPos(ImVec2(m_window->getWidth()/2, 0));

        // Create your ImGui UI elements here
        ImGui::Begin("My Window2",NULL,(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove));  // Begin a new ImGui window

        // Add UI elements
        if (ImPlot::BeginPlot("Price Histogram", ImVec2(m_window->getWidth()/2,m_window->getHeight()*0.9))) {
            for(int i = 0; i < nsamples; i++) {
                ImPlot::PlotLine("Histogram",hist_steps,hist_data,bin_size);
            }
        }
        ImPlot::EndPlot();
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2(m_window->getWidth()/2,m_window->getHeight()*0.1));
        ImGui::SetNextWindowPos(ImVec2(0, m_window->getHeight()*0.9));

        ImGui::Begin("Metrics",NULL,(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove));
        ImGui::Text("Expected Return: $ %f",expected_return);
        ImGui::Text("Return Percent : %f percentage",return_percent);
        ImGui::Text("Probability of Breakeven: %f percentage",probability);

        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


GLplot::~GLplot() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    free(hist_data);
    free(hist_steps);
    free(old_data);
    free(old_days);
}

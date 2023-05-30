#include "glplot.hpp"

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

    std::vector<double> last_day(nsamples);
    for( int i = 0; i < samples; i++) {
        last_day[i] = dataptr[i*days + days - 1];
    }

    double min = *std::min_element(last_day.begin(),last_day.end());
    double max = *std::max_element(last_day.begin(),last_day.end());

    //Allocations
    bin_size = (nsamples > 100)? nsamples/10:nsamples;
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
        ImGui::SetNextWindowSize(ImVec2(m_window->getWidth()/2,m_window->getHeight()));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        // Create your ImGui UI elements here
        ImGui::Begin("My Window",NULL,(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove));  // Begin a new ImGui window

        // Add UI elements
        if (ImPlot::BeginPlot("Stock Price Paths", ImVec2(m_window->getWidth()/2,m_window->getHeight()))) {
            ImPlot::PlotLine("Price Paths History", old_days, old_data, ndays_old);
            for(int i = 0; i < nsamples; i++) {
                ImPlot::PlotLine("Price Paths", &stock_days[i*ndays], &stock_data[i*ndays], ndays);
            }
        }
        ImPlot::EndPlot();
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2(m_window->getWidth()/2,m_window->getHeight()));
        ImGui::SetNextWindowPos(ImVec2(m_window->getWidth()/2, 0));

        // Create your ImGui UI elements here
        ImGui::Begin("My Window2",NULL,(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove));  // Begin a new ImGui window

        // Add UI elements
        if (ImPlot::BeginPlot("Price Histogram", ImVec2(m_window->getWidth()/2,m_window->getHeight()))) {
            for(int i = 0; i < nsamples; i++) {
                ImPlot::PlotLine("Histogram",hist_steps,hist_data,bin_size);
            }
        }
        ImPlot::EndPlot();
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

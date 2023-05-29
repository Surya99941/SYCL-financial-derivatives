#include <cmath>
#include <cstdlib>
#include <imgui.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <iostream>
#include <algorithm>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "stockdata.hpp"

void processInput(GLFWwindow *window);


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


void error_callback(int error, const char* description);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

class GLplot {
private:
    int points;
    GLFWwindow* m_window;
    double* stock_data;
    double* stock_days;
    int ndays;
    int nsamples;
    std::vector<double> old_data;
    std::vector<std::string> old_days;
    double* hist_data;
    double* hist_steps;
    int bin_size;
    
public:
    int m_width;
    int m_height;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        GLplot* glplot_window = static_cast<GLplot*>(glfwGetWindowUserPointer(window));
        glplot_window->m_height = height;
        glplot_window->m_width  = width; 
        glViewport(0, 0, width, height);
    }
    GLplot(const int width, const int height);
    void add_data(double* dataptr, long int samples, long int days, std::vector<StockData> old);
    void draw();
    ~GLplot();
};
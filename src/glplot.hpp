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
T map_range(T value, T fmin, T fmax, T tmin, T tmax){
    T normalized_value = (value - fmin) / (tmax - fmin);
    // Map the normalized value to the new range
    return tmin + normalized_value * (tmax - tmin);
}


void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        printf("Escape pressed\n");
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
    double* last_day;
    
public:
    int m_width;
    int m_height;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        GLplot* glplot_window = static_cast<GLplot*>(glfwGetWindowUserPointer(window));
        glplot_window->m_height = height;
        glplot_window->m_width  = width; 
        glViewport(0, 0, width, height);
    }
    GLplot(const int width, const int height)
    :m_width(width)
    ,m_height(height)
    {
        glfwInit();

        // Configure GLFW to use OpenGL 3.3 core profile
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create a window object
        m_window = glfwCreateWindow(width, height, "Plot Window", NULL, NULL);
        glfwSetWindowUserPointer(m_window, this);
        
        if (m_window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }

        glfwMakeContextCurrent(m_window);
        // Initialize Glad
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        //Initialize IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard controls
        ImGui::StyleColorsDark();  // Apply a dark theme

        // Set up platform/renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // Set up the viewport
        glViewport(0, 0, m_width, m_height);
        // Set the callback function for resizing the window
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetErrorCallback(error_callback);
        glfwSetKeyCallback(m_window, key_callback);
        // Set up the OpenGL context
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void add_data(double* dataptr, long int samples, long int days, std::vector<StockData> old){
        for(auto o: old){
            old_data.push_back(o.close);
            old_days.push_back(o.date.st_date);
        }
        this->points = samples*days;
        ndays = days;
        nsamples = samples;
        this->stock_data = (double *)malloc(sizeof(double) * points);
        this->stock_days = (double *)malloc(sizeof(double) * points);
        for(int i = 0; i < samples*days; i++){
            this->stock_data[i] = dataptr[i];
            this->stock_days[i] = i%days;
        }
        
        last_day = (double*)malloc(nsamples*sizeof(double));
        for( int i = 1; i <= samples; i++){
            last_day[i] = dataptr[i*days-1];
        }
    }

    void draw(){
        while(!glfwWindowShouldClose(m_window)){
            glClear(GL_COLOR_BUFFER_BIT);
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SetNextWindowSize(ImVec2(m_width/2,m_height));
            ImGui::SetNextWindowPos(ImVec2(0, 0));

            // Create your ImGui UI elements here
            ImGui::Begin("My Window",NULL,(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove));  // Begin a new ImGui window
    
            // Add UI elements
            if (ImPlot::BeginPlot("Stock Price Paths", ImVec2(m_width/2,m_height))){
                for(int i = 0; i < nsamples; i++){
                    ImPlot::PlotLine("Price Paths", &stock_days[i*ndays], &stock_data[i*ndays], ndays);
                }
            }
            ImPlot::EndPlot();
            ImGui::End();

            ImGui::SetNextWindowSize(ImVec2(m_width/2,m_height));
            ImGui::SetNextWindowPos(ImVec2(m_width/2, 0));

            // Create your ImGui UI elements here
            ImGui::Begin("My Window2",NULL,(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove));  // Begin a new ImGui window
    
            // Add UI elements
            if (ImPlot::BeginPlot("Price Histogram", ImVec2(m_width/2,m_height))){
                for(int i = 0; i < nsamples; i++){
                    ImPlot::PlotHistogram("Histogram", last_day, nsamples, 100);
                }
            }
            ImPlot::EndPlot();
            ImGui::End();

            
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


            glfwSwapBuffers(m_window);
        }
    }


    ~GLplot(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
};


// Function for processing input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

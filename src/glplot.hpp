#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "stockdata.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
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
    int m_width;
    int m_height;
    int points;
    GLFWwindow* m_window;
    double* data;
    double* days;
    int ndays;
    int nsamples;
    std::vector<double> old_data;
    std::vector<std::string> old_days;
    
public:
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
        this->data = (double *)malloc(sizeof(double) * points);
        this->days = (double *)malloc(sizeof(double) * points);
        for(int i = 0; i < samples*days; i++){
            this->data[i] = dataptr[i];
            this->days[i] = i%days;
        }
    }

    void draw(){
        while(!glfwWindowShouldClose(m_window)){
            glClear(GL_COLOR_BUFFER_BIT);
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Create your ImGui UI elements here
            ImGui::Begin("My Window");  // Begin a new ImGui window
    
            // Add UI elements
            if (ImPlot::BeginPlot("Dual Axis Plot", "X", "Y1")){
                for(int i = 0; i < nsamples; i++){
                    ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL);
                    ImPlot::PlotLine("Y1 Data", &days[i*ndays], &data[i*ndays], ndays);
                }
            }
            ImPlot::EndPlot();

            
            ImGui::End();  // End the ImGui window
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

// Function called when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Function for processing input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

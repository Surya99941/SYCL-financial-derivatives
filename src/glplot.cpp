#include "glplot.hpp"

#include <CL/sycl.hpp>

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLplot::GLplot(const int width, const int height)
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

void
GLplot::add_data(double* dataptr, long int samples, long int days, std::vector<StockData> old) {
    for(auto o: old) {
        old_data.push_back(o.close);
        old_days.push_back(o.date.st_date);
    }
    this->points = samples*days;
    ndays = days;
    nsamples = samples;
    this->stock_data = dataptr;
    this->stock_days = (double *)malloc(sizeof(double) * points);
    for(int i = 0; i < samples*days; i++) {
        this->stock_days[i] = i%days;
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
    while(!glfwWindowShouldClose(m_window)) {
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
        if (ImPlot::BeginPlot("Stock Price Paths", ImVec2(m_width/2,m_height))) {
            for(int i = 0; i < nsamples; i++) {
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
        if (ImPlot::BeginPlot("Price Histogram", ImVec2(m_width/2,m_height))) {
            for(int i = 0; i < nsamples; i++) {
                ImPlot::PlotLine("Histogram",hist_steps,hist_data,bin_size);
            }
        }
        ImPlot::EndPlot();
        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(m_window);
    }
}


GLplot::~GLplot() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();

    free(hist_data);
    free(hist_steps);
}

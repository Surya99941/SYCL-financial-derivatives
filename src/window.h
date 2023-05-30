#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>




static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
void processInput(GLFWwindow *window);
void error_callback(int error, const char* description);




class Window{	
private:
    GLFWwindow* m_window;

public:
    int m_width;
    int m_height;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        Window* mwindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        mwindow->m_height = height;
      	mwindow->m_width  = width; 
        glViewport(0, 0, width, height);
    }
    Window(const int width, const int height);
    ~Window();
		bool isopen();
    void swapbuffers(int interval);
		void before_draw();
    int  getWidth();
    int  getHeight();
};
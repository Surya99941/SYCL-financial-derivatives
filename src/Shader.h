#ifndef SHADER_H
#define SHADER_H
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <time.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <errno.h>



extern int errno;

class Shader
{
    public:
        Shader(const char* path);
        Shader(){}
        ~Shader();
        void bind_shader();
        void unbind_shader();
        void remove();
        void compile();
        GLuint ID;
        bool isRecompiled;
        inline static Shader* CurrentlyBound;

        //Uniforms
        void setFloat1(GLfloat value , const   char    name[]);
        void setFloat2(GLfloat value1, GLfloat value2 , const   char    name[]);
        void setFloat3(GLfloat value1, GLfloat value2 , GLfloat value3, const   char    name[]);
        void setFloat4(GLfloat value1, GLfloat value2 , GLfloat value3, GLfloat value4 , const  char  name[]);
        void setInt1  (GLint   value , const   char     name[]);
        void setInt2  (GLint   value1, GLint   value2 , const   char    name[]);
        void setInt3  (GLint   value1, GLint   value2 , GLint   value3, const   char     name[]);
        void setInt4  (GLint   value1, GLint   value2 , GLint   value3, GLint   value4 , const  char  name[]);
        void setMatrix2(GLfloat *value , const char    name[]);
        void setMatrix3(GLfloat *value , const char    name[]);
        void setMatrix4(GLfloat *value , const char    name[]);

    protected:
    	enum shadertype{
	    	VERTEX = 1,
		    FRAGMENT
	    };

        int m_bound = 0;
        FILE* shaderfile;
        void m_read();
    private:

        void m_count_size();
	      shadertype getshadertype();
        void m_compile_shader(unsigned int shadertype,char* source);
        void m_add_program();
        time_t m_readOn;

        const char* m_ShaderCodePath;
        char* vertexsource;
        char* fragmentsource;
        int vertexsize;
        int fragsize;
        unsigned int m_vertexshader;
        unsigned int m_fragmentshader;
};

#endif // SHADER_H

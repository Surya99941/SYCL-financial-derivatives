#include "Shader.h"

Shader::Shader(const char filepath[])
:isRecompiled(false)
,m_bound(0)
,m_ShaderCodePath(filepath)
,vertexsize(0)
,fragsize(0)
{
    m_readOn = 0;
    shaderfile = fopen(m_ShaderCodePath,"r");
    if(shaderfile == NULL){
        printf("Error opening shader file %s %d",m_ShaderCodePath,errno);
        perror("Error string from perror:");
        exit(0);
    }
    m_vertexshader   = glCreateShader(GL_VERTEX_SHADER);
    m_fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    compile();
}

void
Shader::compile(){

    //Check if file is modified before compiling 0 not compiled
    struct stat filestats;
    stat(m_ShaderCodePath,&filestats);

    if(m_readOn < filestats.st_mtime){

        //reopen shader file if recompiling else read on will be 0
        if(m_readOn !=0 ) shaderfile = freopen(m_ShaderCodePath,"r",shaderfile);
        m_readOn = filestats.st_mtime;
        isRecompiled = true;

        //Count size and read file
        m_count_size();
        vertexsource   = new char[vertexsize];
        fragmentsource = new char[fragsize];
        m_read();


        //Compile shader
        m_compile_shader(m_vertexshader,vertexsource);
        m_compile_shader(m_fragmentshader,fragmentsource);
        m_add_program();

        //check if shader is already bound
        if(m_bound ==1){
            this->bind_shader();
        }

        //cleanup
        delete[] vertexsource;
        delete[] fragmentsource;
    }
    else isRecompiled = false;
}

Shader::~Shader()
{
    fclose(shaderfile);
    glDeleteProgram(ID);
}

void
Shader::m_compile_shader(unsigned int shadertype,char* source){
	glShaderSource(shadertype,1,&source,NULL);
  glCompileShader(shadertype);
	int success;
	char infolog[512];
	glGetShaderiv(shadertype,GL_COMPILE_STATUS,&success);
	if(!success){
        printf("%s",source);
        glGetShaderInfoLog(shadertype,512,NULL,infolog);
        printf("%s",infolog);
	}
}
void
Shader::m_add_program(){
    int success;
    char infolog[512];
    ID  = glCreateProgram();
    glAttachShader(ID,m_vertexshader);
    glAttachShader(ID,m_fragmentshader);
    glLinkProgram(ID);
    glGetProgramiv(ID,GL_LINK_STATUS,&success);
    if(!success){
        glGetProgramInfoLog(ID,512,NULL,infolog);
        printf("%s",infolog);
    }
    glDeleteShader(m_vertexshader);
}

void
Shader::bind_shader(){
    Shader* sh = Shader::CurrentlyBound;
    if(Shader::CurrentlyBound != nullptr){
        Shader::CurrentlyBound->unbind_shader();
    }
    Shader::CurrentlyBound = this;
    glUseProgram(ID);
    m_bound = 1;
}

void
Shader::unbind_shader(){
    m_bound = 0;
}

Shader::shadertype
Shader::getshadertype (){
	char ch;
	char vertex[] = "vertex\0";
	char fragment[] = "fragment\0";
	shadertype result;
	int current = 0;
	while((ch = getc(shaderfile)) != EOF && ch != '\n'){
		if      (ch == vertex[current]   && current == 0) result = shadertype::VERTEX;
		else if (ch == fragment[current] && current == 0) result = shadertype::FRAGMENT;
		current++;
	}
	return result;
}

void
Shader::m_read(){
    char letter;
    int current = 0;
    int index[] = {0,0};
    while((letter = getc(shaderfile))!= EOF){
        if(letter == '$'){
            current = getshadertype();
        }
        else{
            switch(current){
                case(1):
                    vertexsource[index[current - 1]] = letter;
                    index[current - 1]++;
                    break;
                case(2):
                    fragmentsource[index[current - 1]] = letter;
                    index[current - 1]++;
                    break;
                default:
                    printf("Default condition reached\n");
            }
        }
    }
    vertexsource[vertexsize] = '\0';
    fragmentsource[fragsize] = '\0';
    fragsize = vertexsize = 0;
}

void
Shader::m_count_size(){
    char ch;
    shadertype current;
    while((ch = getc(shaderfile)) != EOF){
        if(ch == '$'){
            current =  getshadertype();
        }
        else{
            switch(current){
              case shadertype::VERTEX:
                vertexsize++;
                break;
              case shadertype::FRAGMENT:
                fragsize++;
                break;
            default:
                printf("Default condition reached at counting");
            }
        }
    }
    fseek(shaderfile,0,SEEK_SET);
}
//Uniforms
void
Shader::setFloat1(GLfloat value , const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniform1f(glGetUniformLocation(ID,name),value);
    glUseProgram(id);
}

void
Shader::setFloat2(GLfloat value1, GLfloat value2 ,const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniform2f(glGetUniformLocation(ID,name),value1,value2);
    glUseProgram(id);
}
void
Shader::setFloat3(GLfloat value1, GLfloat value2 , GLfloat value3,const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniform3f(glGetUniformLocation(ID,name),value1,value2,value3);
    glUseProgram(id);
}
void
Shader::setFloat4(GLfloat value1, GLfloat value2 , GLfloat value3, GLfloat value4 ,const  char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniform4f(glGetUniformLocation(ID,name),value1,value2,value3,value4);
    glUseProgram(id);
}
void
Shader::setInt1  (GLint   value ,const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniform1i(glGetUniformLocation(ID,name),value);
    glUseProgram(id);
}
void
Shader::setInt2  (GLint   value1, GLint   value2 ,const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    (glUniform2i(glGetUniformLocation(ID,name),value1,value2));
    glUseProgram(id);
}
void
Shader::setInt3  (GLint   value1, GLint   value2 , GLint   value3, const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniform3i(glGetUniformLocation(ID,name),value1,value2,value3);
    glUseProgram(id);
}
void
Shader::setInt4  (GLint   value1, GLint   value2 , GLint   value3, GLint   value4 , const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniform4i(glGetUniformLocation(ID,name),value1,value2,value3,value4);
    glUseProgram(id);
}

void
Shader::remove(){
    glDeleteProgram(ID);
}
void
Shader::setMatrix2(GLfloat *value , const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniformMatrix2fv(glGetUniformLocation(ID,name),1,GL_FALSE,value);
    glUseProgram(id);
}
void
Shader::setMatrix3(GLfloat *value ,const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniformMatrix3fv(glGetUniformLocation(ID,name),1,GL_FALSE,value);
    glUseProgram(id);
}
void
Shader::setMatrix4(GLfloat *value , const char name[]){
    GLint id;
    glGetIntegerv(GL_CURRENT_PROGRAM,&id);
    this->bind_shader();
    glUniformMatrix4fv(glGetUniformLocation(ID,name),1,GL_FALSE,value);
    glUseProgram(id);
}

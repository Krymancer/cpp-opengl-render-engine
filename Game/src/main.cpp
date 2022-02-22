#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearErrors();\
    x;\
    ASSERT(GLLogCheck(#x,__FILE__,__LINE__))


static void GLClearErrors() {
    while (glGetError() != GL_NO_ERROR);

}

static bool GLLogCheck(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL] [Error] (" << error << ") function: " 
            << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource parseShader(const std::string& filepath) {
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream shaders[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                // set mode to vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                // set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            shaders[(int)type] << line << '\n';
        }
    }

    return {
        shaders[(int)ShaderType::VERTEX].str(), shaders[(int)ShaderType::FRAGMENT].str()
    };
}

static int compileShader(uint32_t type, const std::string& source) {
    GLCall(uint32_t id = glCreateShader(type));
    const char* src = source.c_str(); // source need to exits CAREFULL WITH BREAK
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    //  TODO: Error Handling
    int32_t result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    
    if (result == GL_FALSE) {
        int32_t length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)_malloca(length * sizeof(char)); // Allocate memory for error string
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Falied to compile"
            << ((type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment")
            << "shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return GL_FALSE; // Return 0 or GL_FALSE
    }

    return id;   
}

static int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLCall(uint32_t program = glCreateProgram();)
    uint32_t vs = compileShader(GL_VERTEX_SHADER,vertexShader);
    uint32_t fs = compileShader(GL_FRAGMENT_SHADER,fragmentShader);
    
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize GLFW */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Janela Foda", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Use Vsync */
    glfwSwapInterval(1);

    /* Initialize GLEW */
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW error!" << std::endl;

    /* Print the version of OpenGL */
    std::cout << glGetString(GL_VERSION) << std::endl;
   
    /* Positions */
    float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f  // 3
    };

    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    /* Make vertex array object */
    uint32_t vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    /* Make buffer */ 
    uint32_t buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float),positions,GL_STATIC_DRAW));    
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

    /* Make index */
    uint32_t ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW));

    ShaderProgramSource programSource = parseShader("res/shaders/basic.shader");
    uint32_t shader = createShader(programSource.VertexSource, programSource.FragmentSource);
    GLCall(glUseProgram(shader));
    
    GLCall(uint32_t location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);

    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
    float alpha = 1.0f;
    float increment = 0.05f;
    GLCall(glUniform4f(location, red, green, blue, alpha));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, red, green, blue, alpha));

        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    
        if (red > 1.0f || red < 0.0f)
            increment *= -1;        

        red += increment;

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    GLCall(glDeleteProgram(shader));
    glfwTerminate();
    return 0;
}
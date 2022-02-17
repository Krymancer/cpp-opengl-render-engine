#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static int compileShader(uint32_t type, const std::string& source) {
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str(); // source need to exits CAREFULL WITH BREAK
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //  TODO: Error Handling
    int32_t result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    
    if (result == GL_FALSE) {
        int32_t length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); // Allocate memory for error string
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Falied to compile"
            << ((type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment")
            << "shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return GL_FALSE; // Return 0 or GL_FALSE
    }

    return id;   
}

static int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    uint32_t program = glCreateProgram();
    uint32_t vs = compileShader(GL_VERTEX_SHADER,vertexShader);
    uint32_t fs = compileShader(GL_FRAGMENT_SHADER,fragmentShader);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize GLFW */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Janela Foda", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLEW */
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    /* Print the version of OpenGL */
    std::cout << glGetString(GL_VERSION) << std::endl;
   
    /* Positions */
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    /* Make buffer */
    uint32_t buffer;
    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float),positions,GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    std::string vertexShaderSource = 
        "#version 330 core\n"
        "\n"
        "layout(location=0) in vec4 position;\n"
        "\n"
        "void main(){\n"
        "   glPosition = position;\n"
        ""
        "}\n";

    std::string fragmentShaderSource =
        "#version 330 core\n"
        "\n"
        "layout(location=0) out vec4 color;\n"
        "\n"
        "void main(){\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        ""
        "}\n";

    uint32_t shader = createShader(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shader);
   
    /* Loop until the user closes the window */

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}
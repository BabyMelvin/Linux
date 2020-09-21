#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(1.0f,0.5f,0.2f,1.0f);\n"
    "}\0";

//输入控制，检查用户是否按下了返回键(Esc)
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // 渲染窗口尺寸
    glViewport(0, 0, width, height);
}

int main()
{
    GLFWwindow *window;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口对象
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // build and compile our shader program
    // ------------------------------------
    // 顶点着色器 vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    //片段着色器 fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // 着色器程序：着色器程序是多个着色器合并最终链接完成版本 link shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    //在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f, -0.5f, 0.0f,  // right
        0.0f, 0.5f, 0.0f    // top
    };
    /**
         * 顶点数据作为输入发送给图形渲染管线的第一个处理阶段：顶点着色器。
         * 顶点着色器会在GPU上创建内存用于储存我们的顶点数据，还要配置OpenGL如何解释这些内存，并且指定其如何发送给显卡。
         * 顶点着色器接着会处理我们在内存中指定数量的顶点。
         * 
         * 通过顶点缓冲对象(Vertex Buffer Objects, VBO)管理这个内存，它会在GPU内存（通常被称为显存）中储存大量顶点。
         * 
         * 使用这些缓冲对象的好处是我们可以一次性的发送一大批数据到显卡上，而不是每个顶点发送一次。从CPU把数据发送到显卡相对较慢，所以只要可能我们都要尝试尽量一次性发送尽可能多的数据。当数据发送至显卡的内存中后，
         * 顶点着色器几乎能立即访问顶点，这是个非常快的过程。
         * */
    unsigned int VBO;
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); //OpenGL对象,有一个独一无二的ID
    // bind the Vertex Array Object first, 
    // then bind and set vertex buffer(s), and 
    // then configure vertex attributes(s).
    glBindVertexArray(VAO);

    // 我们同时绑定多个缓冲，只要它们是不同的缓冲类型
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /**
     * 第四个参数指定了我们希望显卡如何管理给定的数据
     *  GL_STATIC_DRAW ：数据不会或几乎不会改变。
     *  GL_DYNAMIC_DRAW：数据会被改变很多。
     *  GL_STREAM_DRAW ：数据每次绘制时都会改变
     * */
    //调用glBufferData函数，它会把之前定义的顶点数据复制到缓冲的内存中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // note that this is allowed, the call to glVertexAttribPointer
    // registered VBO as the vertex attribute's bound vertex buffer object 
    // so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // you can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO,
    // but this rarely happens.
    // Modifying other VAOs requires a call to glBindVertexArray anyways so we generally no't unbind VAOs(nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // 渲染循环, render loop
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);

        //渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 0.1f);
        //GL_DEPTH_BUFFER_BIT,(模板，蜡纸)GL_STENCIL_BUFFER_BIT
        glClear(GL_COLOR_BUFFER_BIT);

        //激活程序
        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time,
                                //but we'll do so to keep things a bit more organized

        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time

        // 检查并调用事件，交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

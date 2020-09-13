/**
 * 顶点数组对象：Vertex Array Object，VAO
 * 顶点缓冲对象：Vertex Buffer Object，VBO
 * 索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object，IBO
 * 
 * 
 * 3D坐标转为2D坐标的处理过程是由OpenGL的图形渲染管线（Graphics Pipeline，
 * 大多译为管线，实际上指的是一堆原始图形数据途经一个输送管道，期间经过各种变化处理最终出现在屏幕的过程）管理的。
 *  图形渲染管线可以被划分为两个主要部分：
 *          第一部分把你的3D坐标转换为2D坐标
 *          第二部分是把2D坐标转变为实际的有颜色的像素。
 * 
 * 2D坐标和像素也是不同的，2D坐标精确表示一个点在2D空间中的位置，而2D像素是这个点的近似值，2D像素受到你的屏幕/窗口分辨率的限制。
 * 
 * 正是由于它们具有并行执行的特性，当今大多数显卡都有成千上万的小处理核心，
 * 它们在GPU上为每一个（渲染管线）阶段运行各自的小程序，从而在图形渲染管线中快速处理你的数据。这些小程序叫做着色器(Shader)。
 * 
 * OpenGL着色器是用OpenGL着色器语言(OpenGL Shading Language, GLSL)写成的
 * 
 * OpenGL需要你去指定这些数据所表示的渲染类型
 *  做出的这些提示叫做图元(Primitive)
 *      GL_POINTS
 *      GL_TRIANGLES
 *      GL_LINE_STRIP (长长的线)
 * 
 * 图形渲染管线几个部分：
 *  第一个部分是顶点着色器(Vertex Shader)
 *  第二个部分是图元装配(Primitive Assembly)阶段将顶点着色器输出的所有顶点作为输入
 *      并所有的点装配成指定图元的形状；
 *  第三部分图元装配阶段的输出会传递给几何着色器(Geometry Shader)。
 *         几何着色器把图元形式的一系列顶点的集合作为输入，它可以通过产生新顶点构造出新的（或是其它的）图元来生成其他形状。
 *  第四部分是几何着色器的输出会被传入光栅化阶段(Rasterization Stage)，这里它会把图元映射为最终屏幕上相应的像素
 *      生成供片段着色器(Fragment Shader)使用的片段(Fragment)
 * 
 * 在片段着色器运行之前会执行裁切(Clipping)。裁切会丢弃超出你的视图以外的所有像素，用来提升执行效率。
 * 
 * OpenGL中的一个片段是OpenGL渲染一个像素所需的所有数据
 * 
 *   第五部分片段着色器的主要目的是计算一个像素的最终颜色，这也是所有OpenGL高级效果产生的地方。通常，片段着色器包含3D场景的数据（比如光照、阴影、光的颜色等等），这些数据可以被用来计算最终像素的颜色。
 *   第六部分在所有对应颜色值确定以后，最终的对象将会被传到最后一个阶段，我们叫做Alpha测试和混合(Blending)阶段。这个阶段检测片段的对应的深度（和模板(Stencil)）值（后面会讲），用它们来判断这个像素是其它物体的前面还是后面，决定是否应该丢弃。
 * 
 * 对于大多数场合，我们只需要配置顶点和片段着色器就行了
 * 
 * 在现代OpenGL中，我们必须定义至少一个顶点着色器和一个片段着色器（因为GPU中没有默认的顶点/片段着色器）。
 * */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

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
    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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
    // max vertex attribute
    int nAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nAttributes << std::endl;
    // 顶点着色器
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    //片段着色器
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // 着色器程序：着色器程序是多个着色器合并最终链接完成版本
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
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};
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
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
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
    //registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(VAO);
    // 渲染循环
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
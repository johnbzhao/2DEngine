#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

const unsigned int SCR_WIDTH=800;
const unsigned int SCR_HEIGHT=600;

float quadVertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  
};
unsigned int indices[]={
    0,1,2,
    2,3,0
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void window_close_callback(GLFWwindow* window){
    std:: cout<<"end......"<<std::endl;
}

int main() {
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 配置OpenGL上下文
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "2D Engine", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置视口和回调
    glViewport(0, 0, 400, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowCloseCallback(window,window_close_callback);

    Shader shader("texture.vert","texture.frag");
    
    unsigned int VBO, VAO,EBO,texture;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //加载纹理
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    //纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // OpenGL5纹理上下翻转
    unsigned char *data = stbi_load("texture.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    // 全局变量：精灵的动态属性
    glm::vec3 spritePosition(0.3f, 0.3f, 0.0f); // 初始位置
    float spriteRotation = 0.0f;                // 初始旋转角度
    glm::vec3 spriteScale(0.5f, 0.5f, 1.0f);    // 初始缩放比例

    // 渲染循环
    Shader colorShader("color.vert", "color.frag");
    while (!glfwWindowShouldClose(window)) {
        // 清屏
        glClearColor(1.0, 0.1, 1.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
           // 处理键盘输入
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) spritePosition.y += 0.0001f; // 向上移动
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) spritePosition.y -= 0.0001f; // 向下移动
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) spritePosition.x -= 0.0001f; // 向左移动
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) spritePosition.x += 0.0001f; // 向右移动
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) spriteRotation += 0.05f;  // 顺时针旋转
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) spriteRotation -= 0.05f;  // 逆时针旋转
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) spriteScale *= 1.01f;     // 放大
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) spriteScale *= 0.99f;     // 缩小

        // 摄像机：正交投影
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f * SCR_HEIGHT / SCR_WIDTH, 1.0f * SCR_HEIGHT / SCR_WIDTH, -1.0f, 1.0f);
        shader.setMat4("projection", glm::value_ptr(projection));

        // 模型矩阵：旋转缩放
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, spritePosition); // 平移
        model = glm::rotate(model, spriteRotation, glm::vec3(0.0f, 0.0f, 1.0f)); // 旋转
        model = glm::scale(model, spriteScale); // 缩放
        shader.setMat4("model", glm::value_ptr(model));

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // 清理资源
    glfwTerminate();
    return 0;
}

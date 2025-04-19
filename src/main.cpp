#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include "Camera.h"

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

    // 调整摄像头缩放逻辑，确保动态适应小精灵的排列范围
    float spriteWidth = static_cast<float>(width) / SCR_WIDTH; // 纹理宽度相对于屏幕宽度的比例
    float spriteHeight = static_cast<float>(height) / SCR_HEIGHT; // 纹理高度相对于屏幕高度的比例

    const int numSprites = 100; // 渲染100个小精灵
    glm::mat4 instanceModels[numSprites];
    for (int i = 0; i < numSprites; i++) {
        float x = (i % 10) * spriteWidth * 0.5f - (spriteWidth * 2.5f); // 横向排列，间隔为纹理宽度的0.5倍
        float y = (i / 10) * spriteHeight * 0.5f - (spriteHeight * 2.5f); // 纵向排列，间隔为纹理高度的0.5倍
        instanceModels[i] = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    }

    // 动态调整摄像头的初始缩放值
    float maxSpriteWidth = spriteWidth * 0.5f * 10; // 横向排列的总宽度
    float maxSpriteHeight = spriteHeight * 0.5f * 10; // 纵向排列的总高度
    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f); // 初始化摄像机
    camera.adjustZoomToFit(maxSpriteWidth, maxSpriteHeight); // 调整摄像头缩放以适应小精灵范围

    // 创建实例化缓冲区
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instanceModels), &instanceModels[0], GL_STATIC_DRAW);

    // 设置实例化属性
    glBindVertexArray(VAO);
    for (int i = 0; i < 4; i++) { // mat4 占用4个顶点属性位置
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(2 + i, 1); // 每个实例更新一次
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 清屏
        glClearColor(0.6, 0.8, 1.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        // 摄像机：处理键盘输入
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.move(glm::vec3(0.0f, 0.01f, 0.0f)); // 向上移动
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.move(glm::vec3(0.0f, -0.01f, 0.0f)); // 向下移动
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.move(glm::vec3(-0.01f, 0.0f, 0.0f)); // 向左移动
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.move(glm::vec3(0.01f, 0.0f, 0.0f)); // 向右移动
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) camera.setZoom(camera.zoom * 0.99f); // 缩小视图
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) camera.setZoom(camera.zoom * 1.01f); // 放大视图

        // 摄像机：正交投影
        glm::mat4 projection = camera.getProjectionMatrix((float)SCR_WIDTH / SCR_HEIGHT);
        shader.setMat4("projection", glm::value_ptr(projection));

        // 绘制实例化小精灵
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, numSprites);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    glfwTerminate();
    return 0;
}
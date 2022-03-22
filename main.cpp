#pragma region Includes

#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Sprite.h"

#pragma endregion

#pragma region Globals
const GLint WIDTH = 800, HEIGHT = 600;

float yaw = 0.0f, pitch = 0.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
std::vector<Sprite*> spriteList;
//Vertex Shader
static const char* vShader = "Shaders/shader.vert";
//Fragment Shader
static const char* fShader = "Shaders/shader.frag";
#pragma endregion

#pragma region DefineFunctions
void CreateShape();
void CreateShaders();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

#pragma endregion

#pragma region DefineShapes
struct{
    Mesh* xyzStar;
} Shapes;
#pragma endregion

int main()
{
    #pragma region Initialize & Setup variables
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateShape();
    CreateShaders();

    GLuint uniformModel = 0;
    GLuint uniformProjection = 0;
    GLuint uniformView = 0;
    #pragma endregion
    
    #pragma region CreateSprites

    Sprite* centralStar = new Sprite(Shapes.xyzStar);
    centralStar->position = glm::vec3(0.0f, 0.0f, 0.0f);
    spriteList.push_back(centralStar);

    #pragma endregion

    #pragma region Camera Setup
    glm::mat4 projection = glm::perspective(
        45.0f, 
        (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight() , 
        0.1f, 
        100.0f
        );

    // glm::mat4 projection = glm::ortho(
    //     -4.0f, 4.0f, // left, right 
    //     -3.0f, 3.0f, // bottom, top
    //     -0.1f, 100.0f // near, far
    //     );

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);    
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget-cameraPos);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

    #pragma endregion


    glfwSetCursorPosCallback(mainWindow.getWindow() , mouse_callback);
    //Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        #pragma region EventHandling
        glfwPollEvents();
        
        //glm::vec3 direction = cameraDirection;
        //printf("%f %f %f\n", pitch, yaw, 0.0f);
        // direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw)) * direction.x;
        // direction.y = cos(glm::radians(yaw)) * direction.y;
        // direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw)) * direction.z;
        //printf("%f %f %f\n", direction.x, direction.y, direction.z);
        

        // cameraDirection = glm::normalize(direction);
        // cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        // cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));
        //Get + Handle user input events

        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_W) == GLFW_PRESS )
            cameraPos += cameraDirection * 0.01f;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_S) == GLFW_PRESS )
            cameraPos -= cameraDirection * 0.01f;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_D) == GLFW_PRESS )
            cameraPos += cameraRight * 0.01f;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_A) == GLFW_PRESS )
            cameraPos -= cameraRight * 0.01f;

        #pragma endregion
    
        //Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw here
        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetUniformLocation("model");
        uniformView = shaderList[0].GetUniformLocation("view");
        uniformProjection = shaderList[0].GetUniformLocation("projection");

        #pragma region Camera
        
        glm::mat4 view (1.0f);

        glm::mat4 cameraPosMat (1.0f);
        cameraPosMat[0][3] = -cameraPos.x;
        cameraPosMat[1][3] = -cameraPos.y;
        cameraPosMat[2][3] = -cameraPos.z;

        glm::mat4 cameraRotateMat (1.0f);
        cameraRotateMat[0] = glm::vec4(cameraRight.x, cameraUp.x, cameraDirection.x, 0.0f);
        cameraRotateMat[1] = glm::vec4(cameraRight.y, cameraUp.y, cameraDirection.y, 0.0f);
        cameraRotateMat[2] = glm::vec4(-cameraRight.z, -cameraUp.z, -cameraDirection.z, 0.0f);

        //view= cameraRotateMat * cameraPosMat;
        view = glm::lookAt(cameraPos, cameraDirection + cameraPos, up); 

        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        #pragma endregion
        
        for (int i = 0; i < 10; i++)
        {
            spriteList[i]->Draw(uniformModel);
        }

        glUseProgram(0);
        //end draw

        mainWindow.swapBuffers();
    }

    return 0;
}

void CreateShape()
{
    GLfloat vertices[] =
    {
        1.0f, 0.0f, 0.0f,  // 0
        0.0f, 1.0f, 0.0f,  // 1
        0.0f, 0.0f, 1.0f,  // 2
        -1.0f, 0.0f, 0.0f, // 3
        0.0f, -1.0f, 0.0f, // 4
        0.0f, 0.0f, -1.0f, // 5
        
         0.25f,  0.25f, 0.0f,  // 6 x y
         0.25f, -0.25f, 0.0f, // 7 x -y
        -0.25f,  0.25f, 0.0f, // 8 -x y
        -0.25f, -0.25f, 0.0f, // 9 -x -y
         0.0f, 0.25f, 0.25f,  // 10 y z
         0.0f, 0.25f,-0.25f, // 11 y -z
         0.0f,-0.25f, 0.25f, // 12 -y z
         0.0f,-0.25f,-0.25f,// 13 -y -z
         0.25f, 0.0f,  0.25f,  // 14 x z
         0.25f, 0.0f, -0.25f, // 15 x -z
        -0.25f, 0.0f,  0.25f, // 16 -x z
        -0.25f, 0.0f, -0.25f,// 17 -x -z

    };

    unsigned int indices[] = 
    {
        8, 1, 10,
        10, 1, 6,
        6, 1, 11,
        11, 1, 8,
        8, 3, 16,
        16, 8, 10,
        10, 2, 16,
        10, 2, 14,
        14, 10, 6,
        6, 0 , 14,
        6, 0 , 15,
        15, 6, 11,
        11, 5, 15,
        11, 5, 17,
        17, 11, 8,
        8, 3, 17,
        9, 3, 16,
        16, 9, 12,
        12, 2, 16,
        12, 2, 14,
        14, 12, 7,
        7, 0, 14,
        7, 0, 15,
        15, 7, 13,
        13, 5, 15,
        13, 5, 17,
        17, 13, 9,
        9, 3, 17,
        9, 4, 12,
        12, 4, 7,
        7, 4, 13,
        13, 4, 9,
    };

    Shapes.xyzStar = new Mesh();
    Shapes.xyzStar->CreateMesh(vertices, indices, 54, 96);

}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static float lastX = mainWindow.getBufferWidth() / 2.0f;
    static float lastY = mainWindow.getBufferHeight() / 2.0f;
     
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;  
    
    yaw += xoffset;
    pitch += yoffset;

}


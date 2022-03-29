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
    Mesh 
        *xyzStar,
        *square1,
        *light;
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
    GLuint uniformLightColor = 0;
    GLuint uniformLightPos = 0;

    //time
    GLfloat currentTime = glfwGetTime();
    GLfloat elapsedTime = 0.0f;
    GLfloat lastTime = currentTime;
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
    glm::vec3 targetDirection = glm::normalize(cameraTarget-cameraPos);
    glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
    
    GLfloat cameraSpeed = 10.0f;

    // rotate direction to targetDirection
    float angle = acos(glm::dot(direction, targetDirection));
    glm::vec3 axis = glm::cross(direction, targetDirection);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, axis);

    glm::vec3 cameraDirection;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;

    #pragma endregion
    
    #pragma region light
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(4.0f, 6.0f, 5.0f);
    Sprite *lightSprite = new Sprite(Shapes.light);
    lightSprite->position = lightPos;
    lightSprite->scale = glm::vec3(0.1f, 0.1f, 0.1f);
    #pragma endregion

    GLint grid[40][40];
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 40; j++)
        {
            grid[i][j] = 1;
        }
    }

    #pragma region CreateSprites

    // Sprite* center = new Sprite(Shapes.square1);
    // center->position = glm::vec3(0.0f, 0.0f, 0.0f);
    // spriteList.push_back(center);

    float sqrSize = 2.0f;
    
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 40; j++)
        {
            if(grid[i][j] == 1)
            {
                Sprite* newSprite = new Sprite(Shapes.square1);
                newSprite->position = glm::vec3(i*(sqrSize), 0.0f, j*(sqrSize));
                newSprite->scale = glm::vec3(0.98f, 0.98f, 0.98f);
                spriteList.push_back(newSprite);
            }
        }
    }

    

    #pragma endregion

    glfwSetInputMode(mainWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(mainWindow.getWindow() , mouse_callback);
    //Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        #pragma region CalculateTime
        currentTime = glfwGetTime();
        elapsedTime = currentTime - lastTime;
        lastTime = currentTime;
        #pragma endregion

        #pragma region EventHandling
        glfwPollEvents();

        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

        // tranform direction with rotation matrix
        direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));

        cameraDirection = glm::normalize(direction);
        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));
        //Get + Handle user input events

        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_W) == GLFW_PRESS )
            cameraPos += cameraDirection * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_S) == GLFW_PRESS )
            cameraPos -= cameraDirection * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_D) == GLFW_PRESS )
            cameraPos += cameraRight * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_A) == GLFW_PRESS )
            cameraPos -= cameraRight * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_SPACE) == GLFW_PRESS )
            cameraPos += cameraUp * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS )
            cameraPos -= cameraUp * cameraSpeed * elapsedTime;


        #pragma endregion
    
        //Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetUniformLocation("model");
        uniformView = shaderList[0].GetUniformLocation("view");
        uniformProjection = shaderList[0].GetUniformLocation("projection");
        uniformLightColor = shaderList[0].GetUniformLocation("lightColor");
        uniformLightPos = shaderList[0].GetUniformLocation("lightPosition");

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


        #pragma endregion
        
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4f(uniformLightColor, lightColor.x, lightColor.y, lightColor.z, 1.0f);
        glUniform3f(uniformLightPos, lightPos.x, lightPos.y, lightPos.z);

        lightSprite->Draw(uniformModel);
        
        for (int i = 0; i < spriteList.size(); i++)
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
    GLfloat xyzStar_v[] =
    {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        
         0.25f,  0.25f, 0.0f,  1.0f, 0.0f, 1.0f,
         0.25f, -0.25f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.25f,  0.25f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.25f, -0.25f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.0f, 0.25f, 0.25f,  1.0f, 0.0f, 1.0f,
         0.0f, 0.25f,-0.25f, 0.0f, 1.0f, 1.0f,
         0.0f,-0.25f,-0.25f, 1.0f, 1.0f, 0.0f,
         0.0f,-0.25f, 0.25f, 1.0f, 0.0f, 1.0f,
         0.25f, 0.0f,  0.25f,  1.0f, 1.0f, 0.0f,
         0.25f, 0.0f, -0.25f, 0.0f, 1.0f, 1.0f,
        -0.25f, 0.0f,  0.25f, 1.0f, 1.0f, 0.0f,
        -0.25f, 0.0f, -0.25f, 1.0f, 0.0f, 1.0f,

    };

    unsigned int xyzStar_i[] = 
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
    Shapes.xyzStar->CreateMesh(xyzStar_v, xyzStar_i, 18*6, 96);

    GLfloat square1_v[] =
    {
        /*Posiion  */           /*Color*/               /*Normal*/
        // up
        1.0f, 1.0f, 1.0f,       0.4f, 0.85f, 1.0f,      0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,      0.4f, 0.85f, 1.0f,      0.0f, 1.0f,-0.0f,
        -1.0f, 1.0f, -1.0f,     0.4f, 0.85f, 1.0f,     -0.0f, 1.0f,-0.0f,
        -1.0f, 1.0f, 1.0f,      0.4f, 0.85f, 1.0f,     -0.0f, 1.0f, 0.0f,
        
        // down
        1.0f, -1.0f, 1.0f,      0.4f, 0.85f, 1.0f,      0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,     0.4f, 0.85f, 1.0f,      0.0f, -1.0f,-0.0f,     
        -1.0f, -1.0f, -1.0f,    0.4f, 0.85f, 1.0f,     -0.0f, -1.0f,-0.0f,   
        -1.0f, -1.0f, 1.0f,     0.4f, 0.85f, 1.0f,     -0.0f, -1.0f, 0.0f,   

        // left
        -1.0f, 1.0f, 1.0f,      0.4f, 0.85f, 1.0f,      -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,     0.4f, 0.85f, 1.0f,      -1.0f, 0.0f,-0.0f,
        -1.0f, -1.0f, -1.0f,    0.4f, 0.85f, 1.0f,      -1.0f,-0.0f,-0.0f,
        -1.0f, -1.0f, 1.0f,     0.4f, 0.85f, 1.0f,      -1.0f,-0.0f, 0.0f,

        // right
        1.0f, 1.0f, 1.0f,       0.4f, 0.85f, 1.0f,      1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,      0.4f, 0.85f, 1.0f,      1.0f, 0.0f, 0.0f,   
        1.0f, -1.0f, -1.0f,     0.4f, 0.85f, 1.0f,      1.0f, 0.0f, 0.0f,   
        1.0f, -1.0f, 1.0f,      0.4f, 0.85f, 1.0f,      1.0f, 0.0f, 0.0f,   

        // front
        1.0f, 1.0f, 1.0f,       0.4f, 0.85f, 1.0f,      0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f,      0.4f, 0.85f, 1.0f,      0.0f,-0.0f, 1.0f,   
        -1.0f, -1.0f, 1.0f,     0.4f, 0.85f, 1.0f,     -0.0f,-0.0f, 1.0f,   
        -1.0f, 1.0f, 1.0f,      0.4f, 0.85f, 1.0f,     -0.0f, 0.0f, 1.0f,   

        // back
        1.0f, 1.0f, -1.0f,      0.4f, 0.85f, 1.0f,      0.0f, 0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,     0.4f, 0.85f, 1.0f,      0.0f,-0.0f, -1.0f,       
        -1.0f, -1.0f, -1.0f,    0.4f, 0.85f, 1.0f,     -0.0f,-0.0f, -1.0f,       
        -1.0f, 1.0f, -1.0f,     0.4f, 0.85f, 1.0f,     -0.0f, 0.0f, -1.0f,       
    };

    unsigned int square1_i[]=
    {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        8, 9, 10,
        10, 11, 8,
        12, 13, 14,
        14, 15, 12,
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 20,   
    };
    
    Shapes.square1 = new Mesh();
    Shapes.square1->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    GLfloat light_v[] =
    {
        // up
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,      
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,         
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,         
        
        // down
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,  
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,     
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,     
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,     

        // left
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       -1.0f, 0.0f, 0.0f,    
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       -1.0f, 0.0f, 0.0f,    
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       -1.0f, 0.0f, 0.0f,    

        // right
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f, 
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,    
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,    
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,    

        // front
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,  
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,     
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,     
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,     

        // back
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f, 
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,   
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,   
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,   
    };

    unsigned int light_i[] =
    {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        8, 9, 10,
        10, 11, 8,
        12, 13, 14,
        14, 15, 12,
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 20,   
    };

    Shapes.light = new Mesh();
    Shapes.light->CreateMesh(light_v, light_i, 24*9, 12*3);
    
}

void CreateShaders()
{
    Shader* basicShader = new Shader();
    basicShader->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*basicShader);
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

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
}


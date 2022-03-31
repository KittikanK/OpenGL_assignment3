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

#include "Block.h"

#pragma endregion

#pragma region Defines
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#pragma endregion 

#pragma region Globals
const GLint WIDTH = 800, HEIGHT = 600;

float yaw = 0.0f, pitch = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
std::vector<Block*> spriteList;
//Vertex Shader
static const char* vShader = "Shaders/shader.vert";
//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

#pragma endregion

#pragma region DefineFunctions
void CreateShape();
void CreateShaders();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
glm::vec3 HexColor2RGB(int hexColor);
template<std::size_t L, std::size_t R,std::size_t C>
void createBlock(GLint (&myArray)[L][R][C],glm::vec3 position, glm::vec3 scale);
template<std::size_t L, std::size_t R,std::size_t C>
void createBlock(GLint (&myArray)[L][R][C], glm::vec3 position);
template<std::size_t L, std::size_t R,std::size_t C>
void createBlock(GLint (&myArray)[L][R][C], glm::vec3 position, glm::vec3 scale, int Axis, bool flipX, bool flipY, bool flipZ);
#pragma endregion

#pragma region DefineShapes
struct{
    Mesh 
        *xyzStar,
        *square1_cyan,
        *square1_floor,
        *square1_wall,
        *square1_woodwall,
        *square1_woodceiling,

        *square1_blue,
        *square1_yellow,
        *square1_red,
        *square1_white,
        *square1_black,
        *square1_navy,
        *square1_flesh,
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
    GLuint uniformCameraPos = 0;

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
    glm::vec4 lightColor = glm::vec4(HexColor2RGB(0xFFFFFF), 1.0f);
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 35.0f);
    Block *lightSprite = new Block(Shapes.light);
    lightSprite->position = lightPos;
    lightSprite->scale = glm::vec3(0.1f, 0.1f, 0.1f);
    #pragma endregion

    #pragma region character
    GLint nobita[15][6][7]={
        { //1
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,0,6,0,6,0,0},
        {0,0,6,0,6,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0}
        },
        { //2
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,0,4,0,4,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0}
        },
        { //3
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,0,7,0,7,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0}
        },
        { //4
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,0,7,0,7,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0}
        },
        { //5
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,7,6,6,6,7,0},
        {0,0,6,6,6,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0}
        },
        { //6
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,7,2,2,2,7,0},
        {0,0,2,2,2,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0}
        },
        { //7
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,2,2,2,2,2,0},
        {0,0,2,2,2,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0}
        },
        { //8 
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,0,4,4,4,0,0},
        {0,0,4,4,4,0,0},
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0}
        },
        { //9
        {0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0},
        {0,7,7,7,7,7,0},
        {0,7,7,7,7,7,0},
        {0,7,7,7,7,7,0},
        {0,0,0,0,0,0,0}
        },
        { //10
        {0,0,0,0,0,0,0},
        {0,5,5,5,5,5,0},
        {0,7,7,7,7,7,0},
        {0,7,7,7,7,7,0},
        {0,7,7,7,7,7,0},
        {0,0,0,7,0,0,0}
        },
        { //11
        {0,0,5,5,5,0,0},
        {0,5,5,5,5,5,0},
        {7,7,7,7,7,7,7},
        {0,4,7,7,7,4,0},
        {0,0,5,4,5,0,0},
        {0,0,0,0,0,0,0}
        },
        { //12
        {0,5,5,5,5,5,0},
        {0,5,5,5,5,5,0},
        {0,5,7,7,7,5,0},
        {0,4,7,7,7,4,0},
        {0,0,5,4,5,0,0},
        {0,0,0,0,0,0,0}
        },
        { //13
        {0,5,5,5,5,5,0},
        {0,5,5,5,5,5,0},
        {0,7,7,7,7,7,0},
        {0,4,7,7,7,4,0},
        {0,0,4,4,4,0,0},
        {0,0,0,0,0,0,0}
        },
        { //14
        {0,5,5,5,5,5,0},
        {0,5,5,5,5,5,0},
        {0,5,5,5,5,5,0},
        {0,5,5,5,5,5,0},
        {0,5,5,5,5,5,0},
        {0,0,0,0,0,0,0}
        },
        { //15
        {0,0,5,5,5,0,0},
        {0,5,5,5,5,5,0},
        {0,5,5,5,5,5,0},
        {0,5,5,5,5,5,0},
        {0,0,5,5,5,0,0},
        {0,0,0,0,0,0,0}
        }
    };

    GLint doraemon[12][6][9] = {
        {// 1
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,4,4,0,4,4,0,0},
        {0,0,4,4,0,4,4,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //2
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //3
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,4,1,1,1,1,1,4,0},
        {0,0,0,4,4,4,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //4
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,0},
        {0,0,4,4,4,4,4,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //5
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,3,3,3,3,3,3,3,0},
        {0,0,3,3,3,3,3,0,0},
        {0,0,0,0,2,0,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //6
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,1,1,1,0,0,0},
        {0,1,1,0,0,0,1,1,0},
        {0,0,4,0,0,0,4,0,0},
        {0,0,0,4,4,4,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //7
        {0,0,1,1,1,1,1,0,0},
        {1,1,1,0,0,0,1,1,1},
        {1,0,0,0,0,0,0,0,1},
        {0,4,0,0,0,0,0,4,0},
        {0,0,4,4,4,4,4,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //8
        {0,1,1,1,1,1,1,1,0},
        {1,1,0,0,0,0,0,1,1},
        {1,0,0,0,0,0,0,0,1},
        {0,4,0,0,0,0,0,4,0},
        {0,0,4,4,4,4,4,0,0},
        {0,0,0,0,3,0,0,0,0}
        },
        { //9
        {0,1,1,1,1,1,1,1,0},
        {1,1,0,0,0,0,0,1,1},
        {1,1,0,0,0,0,0,1,1},
        {0,1,4,0,0,0,4,1,0},
        {0,0,0,5,4,5,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //10
        {0,1,1,1,1,1,1,1,0},
        {0,1,0,0,0,0,0,1,0},
        {0,1,0,0,0,0,0,1,0},
        {0,1,4,0,0,0,4,1,0},
        {0,0,0,5,4,5,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //11
        {0,0,1,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,1,0},
        {0,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,4,4,4,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        },
        { //12
        {0,0,0,1,1,1,0,0,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,1,1,1,1,1,0,0},
        {0,0,0,1,1,1,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0}
        }
    };
    #pragma endregion
    
    #pragma region CreateCharecter
    createBlock(doraemon,glm::vec3(0.0f,1.0f,-5.0f),glm::vec3(1.0f));
    createBlock(nobita,glm::vec3(20.0f,1.0f,0.0f),glm::vec3(1.0f),AXIS_Z,false,false,true);
    
    int room[30][40][56];
    for (size_t l = 0; l < 30; l++)
    {
        for (size_t r = 0; r < 40; r++)
        {
            for (size_t c = 0; c < 56; c++)
            {
                room[l][r][c] = 0;
                //foor
                if(l<2)
                {
                    room[l][r][c] = 98;
                }
                
                //wall
                else if(r<3 || c>55-3)
                {
                    //wood
                    if
                    (
                        (l>=2&&l<6) ||
                        (l>=10&&l<11) ||
                        (l>=25&&l<26) 
                    ) 
                        room[l][r][c] = 96;

                    else if(l>=29&&l<30) room[l][r][c] = 95;
                    //white
                    else room[l][r][c] = 97;
                }

            }
        }
    };
    

    createBlock(room,glm::vec3(0.0f,-1.0f,0.0f));
     

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
        glm::vec3 bgcolor = HexColor2RGB(0x5678B5);
        glClearColor(bgcolor.x,bgcolor.y,bgcolor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetUniformLocation("model");
        uniformView = shaderList[0].GetUniformLocation("view");
        uniformProjection = shaderList[0].GetUniformLocation("projection");
        uniformLightColor = shaderList[0].GetUniformLocation("lightColor");
        uniformLightPos = shaderList[0].GetUniformLocation("lightPosition");
        uniformCameraPos = shaderList[0].GetUniformLocation("cameraPosition");

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
        lightPos = glm::vec3(cameraPos.x,cameraPos.y,cameraPos.z);

        #pragma endregion
        
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4f(uniformLightColor, lightColor.x, lightColor.y, lightColor.z, 1.0f);
        glUniform3f(uniformLightPos, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(uniformCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);

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

#pragma region Square1
GLfloat square1_v[] =
    {
        /*Posiion  */           /*Color*/           /*Normal*/
        // up
        1.0f, 1.0f, 1.0f,       1.0f,1.0f,1.0f,     0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,      1.0f,1.0f,1.0f,     0.0f, 1.0f,-0.0f,
        -1.0f, 1.0f, -1.0f,     1.0f,1.0f,1.0f,    -0.0f, 1.0f,-0.0f,
        -1.0f, 1.0f, 1.0f,      1.0f,1.0f,1.0f,    -0.0f, 1.0f, 0.0f,
        
        // down
        1.0f, -1.0f, 1.0f,      1.0f,1.0f,1.0f,     0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,     0.0f, -1.0f,-0.0f,     
        -1.0f, -1.0f, -1.0f,    1.0f,1.0f,1.0f,    -0.0f, -1.0f,-0.0f,   
        -1.0f, -1.0f, 1.0f,     1.0f,1.0f,1.0f,    -0.0f, -1.0f, 0.0f,   

        // left
        -1.0f, 1.0f, 1.0f,      1.0f,1.0f,1.0f,    -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,     1.0f,1.0f,1.0f,    -1.0f, 0.0f,-0.0f,
        -1.0f, -1.0f, -1.0f,    1.0f,1.0f,1.0f,    -1.0f,-0.0f,-0.0f,
        -1.0f, -1.0f, 1.0f,     1.0f,1.0f,1.0f,    -1.0f,-0.0f, 0.0f,

        // right
        1.0f, 1.0f, 1.0f,       1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,      1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,   
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,   
        1.0f, -1.0f, 1.0f,      1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,   

        // front
        1.0f, 1.0f, 1.0f,       1.0f,1.0f,1.0f,      0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f,      1.0f,1.0f,1.0f,      0.0f,-0.0f, 1.0f,   
        -1.0f, -1.0f, 1.0f,     1.0f,1.0f,1.0f,     -0.0f,-0.0f, 1.0f,   
        -1.0f, 1.0f, 1.0f,      1.0f,1.0f,1.0f,     -0.0f, 0.0f, 1.0f,   

        // back
        1.0f, 1.0f, -1.0f,      1.0f,1.0f,1.0f,      0.0f, 0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,      0.0f,-0.0f, -1.0f,       
        -1.0f, -1.0f, -1.0f,    1.0f,1.0f,1.0f,     -0.0f,-0.0f, -1.0f,       
        -1.0f, 1.0f, -1.0f,     1.0f,1.0f,1.0f,     -0.0f, 0.0f, -1.0f,       
    };

void setSquare1_Color(glm::vec3 color)
{
    for (int i = 3; i < sizeof(square1_v) / sizeof(GLfloat); i += 9)
    {
        square1_v[i + 0] = color.x;
        square1_v[i + 1] = color.y;
        square1_v[i + 2] = color.z;
    }
}
#pragma endregion

glm::vec3 HexColor2RGB(int hexColor)
{
    glm::vec3 color;
    color.x = ((hexColor >> 16) & 0xFF) / 255.0f;
    color.y = ((hexColor >> 8) & 0xFF) / 255.0f;
    color.z = ((hexColor >> 0) & 0xFF) / 255.0f;
    
    return color;
}

void CreateShape()
{   
    unsigned int square1_i[]=
    {
        //up
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

    glm::vec3 cyan = HexColor2RGB(0x2ae6f7);
    glm::vec3 green_floor = HexColor2RGB(0xABCA87);
    glm::vec3 white_wall = HexColor2RGB(0xE7E9DA);
    glm::vec3 wood_wall = HexColor2RGB(0xB08D60);
    glm::vec3 wood_ceiling = HexColor2RGB(0xD07634);

    glm::vec3 blue = HexColor2RGB(0x0176C3);
    glm::vec3 yellow = HexColor2RGB(0xE1BD42);
    glm::vec3 red = HexColor2RGB(0x9A1F22);
    glm::vec3 white = HexColor2RGB(0xEAEAEA);
    glm::vec3 black = HexColor2RGB(0x262B2E);
    glm::vec3 navy = HexColor2RGB(0x31496F);
    glm::vec3 flesh = HexColor2RGB(0xF1C27D);
    
    
    setSquare1_Color(cyan);
    Shapes.square1_cyan = new Mesh();
    Shapes.square1_cyan->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    setSquare1_Color(green_floor);
    Shapes.square1_floor = new Mesh();
    Shapes.square1_floor->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    setSquare1_Color(wood_wall);
    Shapes.square1_woodwall = new Mesh();
    Shapes.square1_woodwall->CreateMesh(square1_v, square1_i, 24*9, 12*3); 

    setSquare1_Color(wood_ceiling);
    Shapes.square1_woodceiling = new Mesh();
    Shapes.square1_woodceiling->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    
    setSquare1_Color(white_wall);
    Shapes.square1_wall = new Mesh();
    Shapes.square1_wall->CreateMesh(square1_v, square1_i, 24*9, 12*3);
    
    setSquare1_Color(blue);
    Shapes.square1_blue = new Mesh();
    Shapes.square1_blue->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    setSquare1_Color(yellow);
    Shapes.square1_yellow = new Mesh();
    Shapes.square1_yellow->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    setSquare1_Color(red);
    Shapes.square1_red = new Mesh();
    Shapes.square1_red->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    setSquare1_Color(white);
    Shapes.square1_white = new Mesh();
    Shapes.square1_white->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    setSquare1_Color(black);
    Shapes.square1_black = new Mesh();
    Shapes.square1_black->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    setSquare1_Color(navy);
    Shapes.square1_navy = new Mesh();
    Shapes.square1_navy->CreateMesh(square1_v, square1_i, 24*9, 12*3);

    setSquare1_Color(flesh);
    Shapes.square1_flesh = new Mesh();
    Shapes.square1_flesh->CreateMesh(square1_v, square1_i, 24*9, 12*3);



    GLfloat light_v[] =
    {
        // up
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,      
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,         
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,         
        
        // down
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,  
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,     
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,     
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,     

        // left
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,    
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,    
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,    

        // right
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,      -1.0f, 0.0f, 0.0f, 
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f, 1.0f,      -1.0f, 0.0f, 0.0f,    
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,      -1.0f, 0.0f, 0.0f,    
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,      -1.0f, 0.0f, 0.0f,    

        // front
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,  
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,     
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,     
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,     

        // back
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f, 
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,   
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,   
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,   
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

#pragma region createBlock function
template<std::size_t L, std::size_t R,std::size_t C>
void createBlock(GLint (&myArray)[L][R][C], glm::vec3 position, glm::vec3 scale)
{
    GLfloat length = 1.0f*scale.x;
    GLfloat width = 1.0f*scale.z;
    GLfloat height = 1.0f*scale.y;
    for (int l = 0; l < L; l++)
    {
        for (int i = 0; i < R; i++)
        {
            for (int j = 0; j < C; j++)
            {
                if(myArray[l][i][j] == 0) continue;
                Block* newSprite = new Block(Shapes.square1_cyan);
                switch(myArray[l][i][j])
                {
                    case 1:
                        newSprite = new Block(Shapes.square1_blue);
                        break;
                    case 2:
                        newSprite = new Block(Shapes.square1_yellow);
                        break;
                    case 3:
                        newSprite = new Block(Shapes.square1_red);
                        break;
                    case 4:
                        newSprite = new Block(Shapes.square1_white);
                        break;
                    case 5:
                        newSprite = new Block(Shapes.square1_black);
                        break;
                    case 6:
                        newSprite = new Block(Shapes.square1_navy);
                        break;
                    case 7:
                        newSprite = new Block(Shapes.square1_flesh);
                        break;

                    case 95:
                        newSprite = new Block(Shapes.square1_woodceiling);
                        break;
                    case 96:
                        newSprite = new Block(Shapes.square1_woodwall);
                        break;
                    case 97:
                        newSprite = new Block(Shapes.square1_wall);
                        break;
                    case 98:
                        newSprite = new Block(Shapes.square1_floor);
                        break;
                }
                newSprite->position = glm::vec3(j*(width)+position.x, l*(height)+position.y, i*(length)+position.z-R*(length));
                newSprite->scale = glm::vec3(0.5f*scale.x, 0.5f*scale.y, 0.5f*scale.z);
                spriteList.push_back(newSprite);
            }
        }
    }
}

template<std::size_t L, std::size_t R,std::size_t C>
void createBlock(GLint (&myArray)[L][R][C], glm::vec3 position)
{
    createBlock(myArray, position, glm::vec3(1.0f, 1.0f, 1.0f));
}

template<std::size_t L, std::size_t R,std::size_t C>
void flipCol(GLint (&myArray)[L][R][C])
{
    for(int i = 0; i < L; i++)
    {
        for(int j = 0; j < R; j++)
        {
            int foo = 0, bar = C - 1;
            while(foo < bar)
            {
                std::swap(myArray[i][j][foo], myArray[i][j][bar]);
                foo++;
                bar--;
            }
        }
    }
}
template<std::size_t L, std::size_t R,std::size_t C>
void flipRow(GLint (&myArray)[L][R][C])
{
    for(int l = 0; l < L; l++)
    {
        int foo = 0, bar = R - 1;
        while(foo < bar)
        {
            std::swap(myArray[l][foo], myArray[l][bar]);
            foo++;
            bar--;
        }
    }
}

template<std::size_t L, std::size_t R,std::size_t C>
void flipLayer(GLint (&myArray)[L][R][C])
{
    int foo = 0, bar = L - 1;
    while(foo < bar)
    {
        std::swap(myArray[foo], myArray[bar]);
        foo++;
        bar--;
    }

}

template<std::size_t L, std::size_t R,std::size_t C>
void createBlock(GLint (&myArray)[L][R][C], glm::vec3 position, glm::vec3 scale, int Axis, bool flipX, bool flipY, bool flipZ)
{
    switch(Axis)
    {
        case 0:
            GLint newArray0[L][R][C];
            for(int i = 0; i < L; i++)
            {
                for(int j = 0; j < R; j++)
                {
                    for(int k = 0; k < C; k++)
                    {
                        newArray0[i][j][k] = myArray[i][j][k];
                    }
                }
            }
            if(flipZ) flipCol(newArray0);
            if(flipX) flipRow(newArray0);
            if(flipY) flipLayer(newArray0);
            createBlock(newArray0, position, scale);
            break;
        case 1:
            GLint newArray1[R][L][C];
            for(int i = 0; i < R; i++)
            {
                for(int j = 0; j < L; j++)
                {
                    for(int k = 0; k < C; k++)
                    {
                        newArray1[i][j][k] = myArray[j][i][k];
                    }
                }
            }
            if(flipZ) flipCol(newArray1);
            if(flipX) flipRow(newArray1);
            if(flipY) flipLayer(newArray1);
            createBlock(newArray1, position, scale);
            break;
        case 2:   
            GLint newArray2[L][C][R];
            for(int i = 0; i < L; i++)
            {
                for(int j = 0; j < C; j++)
                {
                    for(int k = 0; k < R; k++)
                    {
                        newArray2[i][j][k] = myArray[i][k][j];
                    }
                }
            }
            if(flipZ) flipCol(newArray2);
            if(flipX) flipRow(newArray2);
            if(flipY) flipLayer(newArray2);
            createBlock(newArray2, position, scale);
            break;
    }
}

#pragma endregion createBlock
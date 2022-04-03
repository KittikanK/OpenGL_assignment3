#pragma region Includes

#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include "Libs/stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Block.h"
#include "util/Cylinder.h"
#include "util/Sphere.h"

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
void LoadTextures();
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
unsigned int loadTexture(const char *directory);
#pragma endregion

#pragma region DefineShapes
struct{
    Mesh 
        *xyzStar,
        *square1_cyan,      //99
        *square1_floor,     // 98
        *square1_wall,      //97
        *square1_woodwall,  //96
        *square1_table,     //95
        *square1_drawer,    //94

        *square1_blue,      //1
        *square1_yellow,    //2
        *square1_red,       //3
        *square1_white,     //4
        *square1_black,     //5
        *square1_navy,      //6
        *square1_flesh,     //7
        *square1_magenta,   //8
        *square1_pink,      //9
        *square1_brown,     //10
        *square1_green,    //11

        *square_purewhite, 
        *light,
        *bookcase,
        *smallBookcase,
        *trashcan,
        *ball,
        
        *flatXZ;
} Shapes;

struct{
    unsigned int
        defaultTexture,
        LegoTop,
        Bookcase,
        Container,
        Wood,
        Ball,
        NobiHouse,
        SmallBookcase,
        DoorRoomR,
        DoorRoomL,
        outsideWindow1,
        outsideWindow2,
        Where;
} Textures;
#pragma endregion

int main()
{
    #pragma region Initialize & Setup variables
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateShape();
    CreateShaders();
    LoadTextures();

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
        150.0f
        );

    // glm::mat4 projection = glm::ortho(
    //     -4.0f, 4.0f, // left, right 
    //     -3.0f, 3.0f, // bottom, top
    //     -0.1f, 100.0f // near, far
    //     );

    glm::vec3 cameraPos = glm::vec3(-10.0f, 20.0f, -10.0f);    
    glm::vec3 cameraTarget = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 targetDirection = glm::normalize(cameraTarget-cameraPos);
    glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
    
    GLfloat cameraSpeed = 10.0f;

    // rotate direction to targetDirection
    // float angle = acos(glm::dot(direction, targetDirection));
    // glm::vec3 axis = glm::cross(direction, targetDirection);
    // glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, axis);

    glm::vec3 cameraDirection;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;

    #pragma endregion
    
    #pragma region light
    glm::vec4 lightColor = glm::vec4(HexColor2RGB(0xffefc2), 1.0f);
    //c7e5ff
    //ffefc2
    glm::vec3 lightPos = glm::vec3(25.0f, 25.0f, -25.0f);
    Block *lightSprite = new Block(Shapes.light,Textures.defaultTexture);
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
        {0,7,0,0,0,7,0},
        {0,7,0,0,0,7,0},
        {0,7,7,7,7,7,0},
        {0,0,0,7,0,0,0}
        },
        { //11
        {0,0,5,5,5,0,0},
        {0,5,0,0,0,5,0},
        {7,7,0,0,0,7,7},
        {0,4,0,0,0,4,0},
        {0,0,5,4,5,0,0},
        {0,0,0,0,0,0,0}
        },
        { //12
        {0,5,5,5,5,5,0},
        {0,5,0,0,0,5,0},
        {0,5,0,0,0,5,0},
        {0,4,0,0,0,4,0},
        {0,0,5,4,5,0,0},
        {0,0,0,0,0,0,0}
        },
        { //13
        {0,5,5,5,5,5,0},
        {0,5,0,0,0,5,0},
        {0,7,0,0,0,7,0},
        {0,4,0,0,0,4,0},
        {0,0,4,4,4,0,0},
        {0,0,0,0,0,0,0}
        },
        { //14
        {0,5,5,5,5,5,0},
        {0,5,0,0,0,5,0},
        {0,5,0,0,0,5,0},
        {0,5,0,0,0,5,0},
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

    int room[40][52][56];
    for (size_t l = 0; l < 40; l++)
    {
        for (size_t r = 0; r < 52; r++)
        {
            for (size_t c = 0; c < 56; c++)
            {
                room[l][r][c] = 0;
                //foor
                if(l<2)
                {
                    room[l][r][c] = 98;
                    if (r == 13 || r == 37 || 
                    (c == 30 && r > 13 && r < 37) || 
                    (c == 6 && r > 13 && r < 37) ||
                    (c == 16 && r > 37))
                    {
                        room[l][r][c] = 10;
                    }
                }
                
                //wall
                else if(r<3 || c>55-3 || r>50)
                {
                    room[l][r][c] = 97;
                    //wood frame
                    if
                    (
                        (l>=2&&l<6) ||
                        (l>=10&&l<11) ||
                        (l>=29&&l<31) 
                    ) 
                        room[l][r][c] = 96;


                    //right window
                    float rw_p = 27, rw_w = 15;
                    if(r > (rw_p-rw_w) && r < (rw_p+rw_w))
                    {
                        room[l][r][c] = 96;
                        if(r > (rw_p-rw_w)+3 && r < (rw_p+rw_w)-3)
                        {
                            //under
                            if(l>=6&&l<13) room[l][r][c] = 97;
                            //window
                            if(l>13&&l<29)
                            { 
                                room[l][r][c] = 0;
                                if(l == 14 || l == 28 || r == roundf(rw_p-rw_w)+4 || r == roundf(rw_p+rw_w)-4|| r == roundf(rw_p))
                                {
                                    room[l][r][c] = 99;
                                }
                            }
                            //above
                            if(l>=31) room[l][r][c] = 97;
                        }
                    }

                    // left window
                    float lw_p = 28, lw_w = 15;
                    if(c > (lw_p-lw_w) && c < (lw_p+lw_w) && r<3)
                    {
                        room[l][r][c] = 96;
                        if(c > (lw_p-lw_w)+3 && c < (lw_p+lw_w)-3)
                        {
                            //under
                            if(l>=6&&l<13) room[l][r][c] = 97;
                            //window
                            if(l>13&&l<29)
                            { 
                                room[l][r][c] = 0;
                                if(l == 14 || l == 28 || c == roundf(lw_p-lw_w)+4 || c == roundf(lw_p+lw_w)-4|| c == roundf(lw_p))
                                {
                                    room[l][r][c] = 99;
                                }
                            }
                            //above
                            if(l>=31) room[l][r][c] = 97;
                        }
                    }
                    
                    //door
                    float door_p = 22, door_w = 18;
                    if(r == 51 && c > (door_p-door_w) && c < (door_p+door_w))
                    {
                        room[l][r][c] = 96;
                        if(c > (door_p-door_w)+3 && c < (door_p+door_w)-3)
                        {
                            if(l<29)
                            { 
                                room[l][r][c] = 0;
                                
                            }
                            // above
                            if(l>=31) room[l][r][c] = 97;
                        }
                    }

                    if(l>=38) room[l][r][c] = 96;
                }

            }
        }
    };

    int roomExtended[40][52][10];
    for (size_t l = 0; l < 40; l++)
    {
        for (size_t r = 0; r < 52; r++)
        {
            for (size_t c = 0; c < 10; c++)
            {
                roomExtended[l][r][c] = 0;
                //foor
                if(l<2)
                {
                    roomExtended[l][r][c] = 98;
                    if (r == 13 || r == 37)
                    {
                        roomExtended[l][r][c] = 10;
                    }
                }
            }
        }
    }
    
    int anywhereDoor[20][12][14];
    for (size_t l = 0; l < 20; l++)
    {
        for (size_t r = 0; r < 12; r++)
        {
            for (size_t c = 0; c < 14; c++)
            {
                anywhereDoor[l][r][c] = 0;
                //door frame
                if(r < 2)
                {
                    anywhereDoor[l][r][c] = 8;
                    if(
                        (c > 1 && c <12) &&
                        (l == 0 || (l>2 && l<19))
                    )
                    {
                        anywhereDoor[l][r][c] = 0;
                    }

                    if(
                        (c == 0 || c == 13) &&
                        (l>1 && l<19)
                    )
                    {
                        anywhereDoor[l][r][c] = 0;
                    }
                }
                // // door 
                // if(c == 12 && r >= 2 && l > 2 && l < 19)
                // {
                //     anywhereDoor[l][r][c] = 8;
                //     if(r == 2 || r == 11 || l == 3 || l == 18)
                //     {
                //         anywhereDoor[l][r][c] = 9;
                //     }
                // }
            }
        }
    }

    int table[12][18][11];
    for (size_t l = 0; l < 12; l++)
    {
        for (size_t r = 0; r < 18; r++)
        {
            for (size_t c = 0; c < 11; c++)
            {
                if(c > 0)
                {
                    if(table[l][r][c] == 94) continue;
                    table[l][r][c] = 95;
                    // space
                    if(l< 9 && r<12)
                    {
                        if(r>0 && c<10) table[l][r][c] = 0;
                        if(r == 0 && (c != 1 && c != 10))  table[l][r][c] = 0;
                        if(r == 0 && l == 2) table[l][r][c] = 95 ;
                        
                        if(c == 10 && (r != 0 && r != 12))  table[l][r][c] = 0;
                        if(c == 10 && l == 2) table[l][r][c] = 95 ;
                    }
                }
                else
                {
                    table[l][r][c] = 0;
                    if(l<11 && l>8&& r>0 && r<12)
                    {
                        table[l][r][c+1] = 94;
                        table[l][r][c] = 94;
                    }
                    if
                    ((r>12 && r<=16) &&
                        (
                            (l<11 && l>8) ||
                            (l<8 && l>5) ||
                            (l<5 && l>0) 
                        )
                    )
                    {
                    table[l][r][c+1] = 94;
                    }
                }
            }
        }
    }


    #pragma endregion
    
    #pragma region CreateCharecter
    //glm::vec3(10.0f,0.0f,-32.0f)
    createBlock(doraemon,glm::vec3(6.0f,0.0f,-26.0f),glm::vec3(1.0f),AXIS_Z,false,false,true);
    createBlock(nobita,glm::vec3(32.0f,0.0f,-10.0f),glm::vec3(1.0f),AXIS_Z,false,false,true);
    createBlock(room,glm::vec3(0.0f,-2.0f,0.0f));
    createBlock(anywhereDoor,glm::vec3(20.0f,0.0f,-34.0f),glm::vec3(1.0f),AXIS_X,false,false,false);
    createBlock(table,glm::vec3(42.0f,0.0f,-10.0f));
    createBlock(roomExtended,glm::vec3(-10.0f,-2.0f,0.0f));
    
    Block *bookcase = new Block(Shapes.bookcase, Textures.Bookcase);
    bookcase->position = glm::vec3(0.0f,-0.5f,-49.0f)+glm::vec3(8,12,3);
    spriteList.push_back(bookcase);

    Block *smallBookcase = new Block(Shapes.smallBookcase, Textures.SmallBookcase);
    smallBookcase->scale = glm::vec3(7.0f);
    smallBookcase->position = glm::vec3(45.0f, 6.0f,-42.0f);
    spriteList.push_back(smallBookcase);

    Block *trashcan = new Block(Shapes.trashcan, Textures.defaultTexture);
    trashcan->scale = glm::vec3(2.0f);
    trashcan->rotation = glm::vec3(glm::radians(-90.0f),0.0f,0.0f);
    trashcan->position = glm::vec3(44.0f,1.5f,-7.5f);
    spriteList.push_back(trashcan);

    Block *ball = new Block(Shapes.ball, Textures.Ball);
    ball->scale = glm::vec3(2.0f);
    ball->rotation = glm::vec3(0.0f,glm::radians(30.0f),glm::radians(10.0f));
    ball->position = glm::vec3(13.0f,1.0f,-14.0f);
    spriteList.push_back(ball);

    Block *ceiling = new Block(Shapes.flatXZ, Textures.Container);
    ceiling->scale = glm::vec3(28.0f,1.0f,26.0f);
    ceiling->position = glm::vec3(28.0f,36.5f,-26.0f);
    spriteList.push_back(ceiling);

    Block *Light = new Block(Shapes.square_purewhite, Textures.defaultTexture);
    Light->scale = glm::vec3(3.0f,0.5f,3.0f);
    Light->position = glm::vec3(25.0f,36.0f,-25.0f);
    spriteList.push_back(Light);

    Block *RDoor = new Block(Shapes.square_purewhite, Textures.DoorRoomR);
    RDoor->scale = glm::vec3(7.5f,13.5f,0.2f);
    RDoor->position = glm::vec3(15.0f,13.0f,-1.2f);
    spriteList.push_back(RDoor);

    Block *LDoor = new Block(Shapes.square_purewhite, Textures.DoorRoomL);
    LDoor->scale = glm::vec3(7.5f,13.5f,0.2f);
    LDoor->position = glm::vec3(29.0f,13.0f,-0.75f);
    spriteList.push_back(LDoor);

    Block *outsiteTable = new Block(Shapes.square_purewhite, Textures.outsideWindow1);
    outsiteTable->scale = glm::vec3(0.2f,8.0f,13.0f);
    outsiteTable->position = glm::vec3(58.0f,19.5f,-25.0f);
    spriteList.push_back(outsiteTable);

    Block *outsiteS = new Block(Shapes.square_purewhite, Textures.outsideWindow2);
    outsiteS->scale = glm::vec3(13.0f,8.0f,0.2f);
    outsiteS->position = glm::vec3(29.0f,19.5f,-54.0f);
    spriteList.push_back(outsiteS);
    
    Block *Where = new Block(Shapes.square_purewhite, Textures.Where);
    Where->scale = glm::vec3(5.0f,8.0f,0.2f);
    Where->position = glm::vec3(26.5f,10.5f,-46.0f);
    spriteList.push_back(Where);

    

    // red doraemon
    for(int l = 0; l<12; l++)
    {
        for (int r = 0; r < 6; r++)
        {
            for (int c = 0; c < 9; c++)
            {   
                // body color
                if(doraemon[l][r][c] == 1)
                {
                    doraemon[l][r][c] = 3;
                }
                // nose color
                else if(doraemon[l][r][c] == 3)
                {
                    doraemon[l][r][c] = 1;
                }
                // bell color
            }   
        }
    }
    createBlock(doraemon,glm::vec3(47.0f,11.5f,-20.0f),glm::vec3(0.5f,0.4f,0.4f),AXIS_Z,false,false,true);

    // green doraemon
    for(int l = 0; l<12; l++)
    {
        for (int r = 0; r < 6; r++)
        {
            for (int c = 0; c < 9; c++)
            {
                // body color
                if(doraemon[l][r][c] == 3)
                {
                    doraemon[l][r][c] = 11;
                }
                // nose color
                else if(doraemon[l][r][c] == 1)
                {
                    doraemon[l][r][c] = 3;
                }
                // bell color
            }   
        }
    }
    createBlock(doraemon,glm::vec3(47.0f,11.5f,-24.0f),glm::vec3(0.5f,0.4f,0.4f),AXIS_Z,false,false,true);


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
        // direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));

        cameraDirection = glm::normalize(direction);
        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));
        //Get + Handle user input events

        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_UP) == GLFW_PRESS )
            cameraPos += cameraDirection * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_DOWN) == GLFW_PRESS )
            cameraPos -= cameraDirection * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_RIGHT) == GLFW_PRESS )
            cameraPos += cameraRight * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_LEFT) == GLFW_PRESS )
            cameraPos -= cameraRight * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_O) == GLFW_PRESS )
            cameraPos += cameraUp * cameraSpeed * elapsedTime;
        if(glfwGetKey(mainWindow.getWindow(),GLFW_KEY_P) == GLFW_PRESS )
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
        // lightPos = glm::vec3(cameraPos.x,cameraPos.y,cameraPos.z);

        #pragma endregion
        
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4f(uniformLightColor, lightColor.x, lightColor.y, lightColor.z, 1.0f);
        glUniform3f(uniformLightPos, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(uniformCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);

        //lightSprite->Draw(uniformModel);
        
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
        /*Posiion  */           /*Color*/           /*Normal*/              /*TexCoord*/
        // up
        1.0f, 1.0f, 1.0f,       1.0f,1.0f,1.0f,     0.0f, 1.0f, 0.0f,       1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,      1.0f,1.0f,1.0f,     0.0f, 1.0f,-0.0f,       1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,     1.0f,1.0f,1.0f,    -0.0f, 1.0f,-0.0f,       0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,      1.0f,1.0f,1.0f,    -0.0f, 1.0f, 0.0f,       0.0f, 1.0f,
        
        // down
        1.0f, -1.0f, 1.0f,      1.0f,1.0f,1.0f,     0.0f, -1.0f, 0.0f,      1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,     0.0f, -1.0f,-0.0f,      1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    1.0f,1.0f,1.0f,    -0.0f, -1.0f,-0.0f,      0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,     1.0f,1.0f,1.0f,    -0.0f, -1.0f, 0.0f,      0.0f, 1.0f,

        // left
        -1.0f, -1.0f,-1.0f,    1.0f,1.0f,1.0f,    -1.0f,-0.0f,-0.0f,       0.0f, 1.0f,
        -1.0f,  1.0f,-1.0f,     1.0f,1.0f,1.0f,    -1.0f,-0.0f, 0.0f,      0.0f, 0.0f,
        -1.0f,  1.0f, 1.0f,      1.0f,1.0f,1.0f,    -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,     1.0f,1.0f,1.0f,    -1.0f, 0.0f,-0.0f,      1.0f, 1.0f,

        // right
        1.0f, 1.0f, 1.0f,       1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,      1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,      1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,      1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,      0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,      1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,      0.0f, 1.0f,

        // front
        -1.0f, -1.0f, 1.0f,       1.0f,1.0f,1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
        1.0f, -1.0f, 1.0f,      1.0f,1.0f,1.0f,      0.0f,-0.0f, 1.0f,      1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,     1.0f,1.0f,1.0f,       -0.0f,-0.0f, 1.0f,        1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,      1.0f,1.0f,1.0f,     -0.0f, 0.0f, 1.0f,      0.0f, 0.0f,

        // back
        -1.0f, -1.0f, -1.0f,      1.0f,1.0f,1.0f,    0.0f, 0.0f, -1.0f,     1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,     1.0f,1.0f,1.0f,      0.0f,-0.0f, -1.0f,     1.0f, 0.0f,  
        1.0f, 1.0f, -1.0f,    1.0f,1.0f,1.0f,     -0.0f,-0.0f, -1.0f,     0.0f, 0.0f,  
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,     -0.0f, 0.0f, -1.0f,     0.0f, 1.0f,  
    };

void setSquare1_Color(glm::vec3 color)
{
    for (int i = 3; i < sizeof(square1_v) / sizeof(GLfloat); i += 11)
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
    glm::vec3 table = HexColor2RGB(0xab7337);
    glm::vec3 drawer = HexColor2RGB(0x805930);

    glm::vec3 blue = HexColor2RGB(0x0176C3);
    glm::vec3 yellow = HexColor2RGB(0xE1BD42);
    glm::vec3 red = HexColor2RGB(0x9A1F22);
    glm::vec3 white = HexColor2RGB(0xEAEAEA);
    glm::vec3 black = HexColor2RGB(0x262B2E);
    glm::vec3 navy = HexColor2RGB(0x31496F);
    glm::vec3 flesh = HexColor2RGB(0xF1C27D);
    glm::vec3 magenta = HexColor2RGB(0xed4aa6);
    glm::vec3 pink = HexColor2RGB(0xd193af);
    glm::vec3 brown = HexColor2RGB(0x74592C);
    glm::vec3 green = HexColor2RGB(0x379769);
    glm::vec3 purewhite = HexColor2RGB(0xFFFFFF);
    
    int squareVertices = 24*11;
    int squareIndices = 12*3;
    setSquare1_Color(cyan);
    Shapes.square1_cyan = new Mesh();
    Shapes.square1_cyan->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(green_floor);
    Shapes.square1_floor = new Mesh();
    Shapes.square1_floor->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(wood_wall);
    Shapes.square1_woodwall = new Mesh();
    Shapes.square1_woodwall->CreateMesh(square1_v, square1_i, squareVertices, squareIndices); 

    setSquare1_Color(table);
    Shapes.square1_table = new Mesh();
    Shapes.square1_table->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(drawer);
    Shapes.square1_drawer = new Mesh();
    Shapes.square1_drawer->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(purewhite);
    Shapes.square_purewhite = new Mesh();
    Shapes.square_purewhite->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    
    setSquare1_Color(white_wall);
    Shapes.square1_wall = new Mesh();
    Shapes.square1_wall->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);
    
    setSquare1_Color(blue);
    Shapes.square1_blue = new Mesh();
    Shapes.square1_blue->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(yellow);
    Shapes.square1_yellow = new Mesh();
    Shapes.square1_yellow->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(red);
    Shapes.square1_red = new Mesh();
    Shapes.square1_red->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(white);
    Shapes.square1_white = new Mesh();
    Shapes.square1_white->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(black);
    Shapes.square1_black = new Mesh();
    Shapes.square1_black->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(navy);
    Shapes.square1_navy = new Mesh();
    Shapes.square1_navy->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(flesh);
    Shapes.square1_flesh = new Mesh();
    Shapes.square1_flesh->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(magenta);
    Shapes.square1_magenta = new Mesh();
    Shapes.square1_magenta->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(pink);
    Shapes.square1_pink = new Mesh();
    Shapes.square1_pink->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(brown);
    Shapes.square1_brown = new Mesh();
    Shapes.square1_brown->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);

    setSquare1_Color(green);
    Shapes.square1_green = new Mesh();
    Shapes.square1_green->CreateMesh(square1_v, square1_i, squareVertices, squareIndices);


    GLfloat light_v[] =
    {
        // up
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,      1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,      1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,      0.0f, 0.0f,   
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, -1.0f, 0.0f,      0.0f, 1.0f,   
        
        // down
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,       1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,       0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f,

        // left
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,       0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 0.0f,       0.0f, 1.0f,

        // right
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,      -1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f, 1.0f,      -1.0f, 0.0f, 0.0f,       1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,      -1.0f, 0.0f, 0.0f,       0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,      -1.0f, 0.0f, 0.0f,       0.0f, 1.0f,

        // front
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,      1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,      0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, -1.0f,      0.0f, 1.0f,

        // back
        -1.0f, -1.0f, -1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,       1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 1.0f,
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
    Shapes.light->CreateMesh(light_v, light_i, squareVertices, squareIndices);
    
    float bc_h = 24.0f, bc_w = 6.0f, bc_l = 16.0f;
    bc_h = bc_h / 2.0f;
    bc_w = bc_w / 2.0f;
    bc_l = bc_l / 2.0f;
    GLfloat bookcase_v[] = {
        /*Posiion  */           /*Color*/           /*Normal*/
        // up
        bc_l,  bc_h,-bc_w,       1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,      0.01f, 0.01f,
        bc_l,  bc_h, bc_w,       1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,      0.01f, 0.01f,
        -bc_l, bc_h, bc_w,       1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,      0.01f, 0.01f,
        -bc_l, bc_h,-bc_w,       1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,      0.01f, 0.01f,

        // down
        bc_l , -bc_h, -bc_w,       1.0f, 1.0f, 1.0f,    0.0f, -1.0f, 0.0f,  0.01f, 0.01f,
        bc_l , -bc_h, bc_w,       1.0f, 1.0f, 1.0f,    0.0f, -1.0f, 0.0f,   0.01f, 0.01f,
        -bc_l, -bc_h, bc_w,       1.0f, 1.0f, 1.0f,    0.0f, -1.0f, 0.0f,   0.01f, 0.01f,
        -bc_l, -bc_h, -bc_w,       1.0f, 1.0f, 1.0f,    0.0f, -1.0f, 0.0f,  0.01f, 0.01f,

        // back
        -bc_l, -bc_h, -bc_w,       1.0f, 1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,   0.01f, 0.01f,
        bc_l, -bc_h, -bc_w,       1.0f, 1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,    0.01f, 0.01f,
        bc_l, bc_h, -bc_w,       1.0f, 1.0f, 1.0f,   -1.0f,-0.0f, 0.0f,     0.01f, 0.01f,
        -bc_l, bc_h, -bc_w,       1.0f, 1.0f, 1.0f,   -1.0f,-0.0f, 0.0f,    0.01f, 0.01f,

        // font
        bc_l, -bc_h, bc_w,       1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f,      1.0f, 1.0f,
        -bc_l, -bc_h, bc_w,       1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
        -bc_l, bc_h, bc_w,       1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f,      0.0f, 0.0f,
        bc_l, bc_h, bc_w,       1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f,       1.0f, 0.0f,

        // right
        bc_l, -bc_h, -bc_w,       1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.01f, 0.01f,
        bc_l, -bc_h, bc_w,       1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,     0.01f, 0.01f,
        bc_l, bc_h, bc_w,       1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,      0.01f, 0.01f,
        bc_l, bc_h, -bc_w,       1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,     0.01f, 0.01f,

        // left
        -bc_l, -bc_h, bc_w,       1.0f, 1.0f, 1.0f,    0.0f, 0.0f, -1.0f,   0.01f, 0.01f,
        -bc_l, -bc_h, -bc_w,       1.0f, 1.0f, 1.0f,    0.0f,-0.0f, -1.0f,  0.01f, 0.01f,
        -bc_l, bc_h, -bc_w,       1.0f, 1.0f, 1.0f,   -0.0f,-0.0f, -1.0f,   0.01f, 0.01f,
        -bc_l, bc_h, bc_w,       1.0f, 1.0f, 1.0f,   -0.0f, 0.0f, -1.0f,    0.01f, 0.01f,

    };

    unsigned int bookcase_i[] =
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

    Shapes.bookcase = new Mesh();
    Shapes.bookcase->CreateMesh(bookcase_v, bookcase_i, squareVertices, squareIndices);

    GLfloat smallBookcase_v[] =
    {
        /*Posiion  */           /*Color*/           /*Normal*/
        // up
        1.0f, 1.0f, 0.0f,       1.0f,1.0f,1.0f,     0.0f, 1.0f, 0.0f,       0.02f, 0.02f,
        1.0f, 1.0f, -1.0f,      1.0f,1.0f,1.0f,     0.0f, 1.0f,-0.0f,       0.02f, 0.02f,    
        -1.0f, 1.0f, -1.0f,     1.0f,1.0f,1.0f,    -0.0f, 1.0f,-0.0f,       0.02f, 0.02f,    
        -1.0f, 1.0f, 0.0f,      1.0f,1.0f,1.0f,    -0.0f, 1.0f, 0.0f,       0.02f, 0.02f,
        
        // down
        1.0f, -1.0f, 0.0f,      1.0f,1.0f,1.0f,     0.0f, -1.0f, 0.0f,      0.02f, 0.02f,
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,     0.0f, -1.0f,-0.0f,      0.02f, 0.02f,   
        -1.0f, -1.0f, -1.0f,    1.0f,1.0f,1.0f,    -0.0f, -1.0f,-0.0f,      0.02f, 0.02f, 
        -1.0f, -1.0f, 0.0f,     1.0f,1.0f,1.0f,    -0.0f, -1.0f, 0.0f,      0.02f, 0.02f, 

        // left
        -1.0f, 1.0f, 0.0f,      1.0f,1.0f,1.0f,    -1.0f, 0.0f, 0.0f,       0.02f, 0.02f,
        -1.0f, 1.0f, -1.0f,     1.0f,1.0f,1.0f,    -1.0f, 0.0f,-0.0f,       0.02f, 0.02f,
        -1.0f, -1.0f, -1.0f,    1.0f,1.0f,1.0f,    -1.0f,-0.0f,-0.0f,       0.02f, 0.02f,
        -1.0f, -1.0f, 0.0f,     1.0f,1.0f,1.0f,    -1.0f,-0.0f, 0.0f,       0.02f, 0.02f,

        // right
        1.0f, 1.0f, 0.0f,       1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,      0.02f, 0.02f,
        1.0f, 1.0f, -1.0f,      1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,      0.02f, 0.02f,    
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,      0.02f, 0.02f,    
        1.0f, -1.0f, 0.0f,      1.0f,1.0f,1.0f,      1.0f, 0.0f, 0.0f,      0.02f, 0.02f,    

        // front
        -1.0f, -1.0f, 0.0f,     1.0f,1.0f,1.0f,     -0.0f,-0.0f, 1.0f,      0.0f, 1.0f,    
        1.0f, -1.0f, 0.0f,      1.0f,1.0f,1.0f,      0.0f,-0.0f, 1.0f,     1.0f, 1.0f,     
        1.0f, 1.0f, 0.0f,       1.0f,1.0f,1.0f,      0.0f, 0.0f, 1.0f,      1.0f, 0.0f,    
        -1.0f, 1.0f, 0.0f,      1.0f,1.0f,1.0f,     -0.0f, 0.0f, 1.0f,      0.0f, 0.0f,    

        // back
        1.0f, 1.0f, -1.0f,      1.0f,1.0f,1.0f,      0.0f, 0.0f, -1.0f,     0.02f, 0.02f,
        1.0f, -1.0f, -1.0f,     1.0f,1.0f,1.0f,      0.0f,-0.0f, -1.0f,     0.02f, 0.02f,    
        -1.0f, -1.0f, -1.0f,    1.0f,1.0f,1.0f,     -0.0f,-0.0f, -1.0f,     0.02f, 0.02f,    
        -1.0f, 1.0f, -1.0f,     1.0f,1.0f,1.0f,     -0.0f, 0.0f, -1.0f,     0.02f, 0.02f,    
    };

    unsigned int smallBookcase_i[] =
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

    Shapes.smallBookcase = new Mesh();
    Shapes.smallBookcase->CreateMesh(smallBookcase_v, smallBookcase_i, squareVertices, squareIndices);

    Cylinder *cylinder = new Cylinder(0.8, 1, 2, 16, 2, false);

    const float * cln_v = cylinder->getVertices();
    int cln_nv = cylinder->getVertexCount();
    const float * cln_n = cylinder->getNormals();
    const float * cln_t = cylinder->getTexCoords();
    
    std::vector<GLfloat> trashcan_v;
    glm::vec3 trashcan_color = HexColor2RGB(0xed6a24);
    for (int i = 0, j = 0; i < cln_nv*3; i+=3, j+=2)
    {
        trashcan_v.push_back(cln_v[i]);
        trashcan_v.push_back(cln_v[i+1]);
        trashcan_v.push_back(cln_v[i+2]);
        trashcan_v.push_back(trashcan_color.x);
        trashcan_v.push_back(trashcan_color.y);
        trashcan_v.push_back(trashcan_color.z);
        trashcan_v.push_back(cln_n[i]);
        trashcan_v.push_back(cln_n[i+1]);
        trashcan_v.push_back(cln_n[i+2]);
        trashcan_v.push_back(cln_t[j]);
        trashcan_v.push_back(cln_t[j+1]);
    }
    
    const unsigned int * cln_i = cylinder->getIndices();
    int cln_nl = cylinder->getIndexCount();
    std::vector<GLuint> trashcan_i;
    for (int i = 0; i < cln_nl; i++)
    {
        trashcan_i.push_back(cln_i[i]);
    }

    Shapes.trashcan = new Mesh();
    Shapes.trashcan->CreateMesh(&trashcan_v[0], &trashcan_i[0], cln_nv*11, cln_nl);

    Sphere *sphere = new Sphere(1, 36, 18, true);
    const float * sph_v = sphere->getVertices();
    int sph_nv = sphere->getVertexCount();
    const float * sph_n = sphere->getNormals();
    const float * sph_t = sphere->getTexCoords();

    std::vector<GLfloat> ball_v;
    glm::vec3 ball_color = HexColor2RGB(0xFFFFFF);
    for (int i = 0, j = 0 ; i < sph_nv*3; i+=3, j+=2)
    {
        ball_v.push_back(sph_v[i]);
        ball_v.push_back(sph_v[i+1]);
        ball_v.push_back(sph_v[i+2]);
        ball_v.push_back(ball_color.x);
        ball_v.push_back(ball_color.y);
        ball_v.push_back(ball_color.z);
        ball_v.push_back(sph_n[i]);
        ball_v.push_back(sph_n[i+1]);
        ball_v.push_back(sph_n[i+2]);
        ball_v.push_back(sph_t[j]);
        ball_v.push_back(sph_t[j+1]);
    }

    const unsigned int * sph_i = sphere->getIndices();
    int sph_nl = sphere->getIndexCount();
    std::vector<GLuint> ball_i;
    for (int i = 0; i < sph_nl; i++)
    {
        ball_i.push_back(sph_i[i]);
    }

    Shapes.ball = new Mesh();
    Shapes.ball->CreateMesh(&ball_v[0], &ball_i[0], sph_nv*11, sph_nl);

    GLfloat flatXZ_v[] = {
        -1.0f, 0.0f, -1.0f,      1.0f,1.0f,1.0f,       -0.5f, -1.0f, -0.5f,      0.0f, 1.0f,
        -1.0f, 0.0f, 1.0f,       1.0f,1.0f,1.0f,       -0.5f, -1.0f, 0.5f,      0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,        1.0f,1.0f,1.0f,       0.5f,  -1.0f, 0.5f,      0.0f, 1.0f,
        1.0f, 0.0f, -1.0f,       1.0f,1.0f,1.0f,       0.5f,  -1.0f, -0.5f,      0.0f, 1.0f,
    };

    unsigned int flatXZ_i[] =
    {
        0, 1, 2,
        2, 3, 0,
    };

    Shapes.flatXZ = new Mesh();
    Shapes.flatXZ->CreateMesh(flatXZ_v, flatXZ_i, 4*11, 3*2);

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
                glm::vec3 addition = glm::vec3(0.0f,0.0f,0.0f);
                if(myArray[l][i][j] == 0) continue;
                Block* newSprite = new Block(Shapes.square1_cyan, Textures.defaultTexture);
                switch(myArray[l][i][j])
                {
                    case 1:
                        newSprite = new Block(Shapes.square1_blue, Textures.defaultTexture);
                        break;
                    case 2:
                        newSprite = new Block(Shapes.square1_yellow, Textures.defaultTexture);
                        break;
                    case 3:
                        newSprite = new Block(Shapes.square1_red, Textures.defaultTexture);
                        break;
                    case 4:
                        newSprite = new Block(Shapes.square1_white, Textures.defaultTexture);
                        break;
                    case 5:
                        newSprite = new Block(Shapes.square1_black, Textures.defaultTexture);
                        break;
                    case 6:
                        newSprite = new Block(Shapes.square1_navy, Textures.defaultTexture);
                        break;
                    case 7:
                        newSprite = new Block(Shapes.square1_flesh, Textures.defaultTexture);
                        break;
                    case 8:
                        newSprite = new Block(Shapes.square1_magenta, Textures.defaultTexture);
                        break;
                    case 9:
                        newSprite = new Block(Shapes.square1_pink, Textures.defaultTexture);
                        break;
                    case 10:
                        newSprite = new Block(Shapes.square1_brown, Textures.Wood);
                        break;
                    case 11:
                        newSprite = new Block(Shapes.square1_green, Textures.Wood);
                        break;

                    case 94:
                        newSprite = new Block(Shapes.square1_drawer, Textures.defaultTexture);
                        addition.x = -0.3f;
                        break;
                    case 95:
                        newSprite = new Block(Shapes.square1_table, Textures.defaultTexture);
                        break;
                    case 96:
                        newSprite = new Block(Shapes.square1_woodwall, Textures.defaultTexture);
                        break;
                    case 97:
                        newSprite = new Block(Shapes.square1_wall, Textures.defaultTexture);
                        break;
                    case 98:
                        newSprite = new Block(Shapes.square1_floor, Textures.defaultTexture);
                        break;
                }
                newSprite->position = glm::vec3(j*(length)+position.x+addition.x, l*(height)+position.y+addition.y, i*(width)+position.z-R*(length)+addition.z);
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


unsigned int loadTexture(const char *directory)
{
    //file extension
    std::string fileExtension = directory;
    fileExtension = fileExtension.substr(fileExtension.find_last_of(".") + 1);
    //texture;
    int width, height, nrChannels;
    
    unsigned char* data = stbi_load(directory, &width, &height, &nrChannels, 0);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // Texture binding in shader frag
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST
    
    if (data)
    {
        unsigned int colorFormat;
        // if(fileExtension == "jpg" || fileExtension == "jpeg")
            colorFormat = GL_RGB;
        // else
        //     colorFormat = GL_RGBA;
        //bind image to texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); //generate mipmaps
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

void LoadTextures()
{
    Textures.defaultTexture = loadTexture("Textures/defaultTexture.jpg");
    Textures.LegoTop = loadTexture("Textures/LegoTop.jpg");
    Textures.Bookcase = loadTexture("Textures/Bookcase.jpg");
    Textures.Container = loadTexture("Textures/container.jpg");
    Textures.Wood = loadTexture("Textures/Wood.jpg");
    Textures.Ball = loadTexture("Textures/ball.jpg");
    Textures.NobiHouse = loadTexture("Textures/NobiHouse.jpg");
    Textures.SmallBookcase = loadTexture("Textures/smallBookCase.jpg");
    Textures.DoorRoomR = loadTexture("Textures/DoorRoomR.jpg");
    Textures.DoorRoomL = loadTexture("Textures/DoorRoomL.jpg");
    Textures.outsideWindow1 = loadTexture("Textures/OutsideWindow1.jpg");
    Textures.outsideWindow2 = loadTexture("Textures/OutsideWindow2.jpg");
    Textures.Where = loadTexture("Textures/DoorAnywhere2.jpg");
}
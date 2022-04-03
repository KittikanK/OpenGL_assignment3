#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 texCoord;
out vec4 vColor; 
out vec2 TexCoord;
out vec3 vNormal;
out vec3 vPosition;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 

void main()
{
    vPosition = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * vec4(vPosition, 1.0);
    vColor = vec4(color, 1.0);
    vNormal = normal;
    TexCoord = texCoord;
}
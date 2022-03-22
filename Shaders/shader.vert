#version 330

layout (location = 0) in vec3 pos;
out vec4 vColor; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;  

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    vColor = vec4(abs(pos.x),abs(pos.y),abs(pos.z), 1.0);
}
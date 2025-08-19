#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;  
out vec3 Normal;
out vec2 TexCoord0;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    vec4 worldPos = u_Model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    TexCoord0 = aTexCoord0;
    Normal = vec3(u_Model * vec4(aNormal, 1.0));
    
    gl_Position = u_Projection * u_View * worldPos;
}
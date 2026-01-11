#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord0;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    TexCoord0 = aTexCoord0;

    // Use mat3 to ignore translation and handle non-uniform scaling correctly
    mat3 normalMatrix = mat3(transpose(inverse(_Model)));
    Normal = normalize(normalMatrix * aNormal);

    gl_Position = _Projection * _View * worldPos;
}
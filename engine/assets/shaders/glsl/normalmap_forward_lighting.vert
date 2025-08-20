#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 4) in vec3 aTangent;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;
out vec2 TexCoord0;
// Tangent-Bitangent-Normal matrix.
out mat3 TBN;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    TexCoord0 = aTexCoord0;

    // Transform TBN vectors to the same space as our lighting vectors (world space)
    vec3 T = normalize(vec3(_Model * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(_Model * vec4(aNormal, 0.0)));
    // Recalculate Bitangent to ensure it's orthonormal
    vec3 B = cross(N, T);

    // Create the TBN matrix. Each column is a basis vector.
    TBN = mat3(T, B, N);

    gl_Position = _Projection * _View * worldPos;
}
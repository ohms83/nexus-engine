@glsl_version 330 core

@program vertex
layout (location = 0) in vec3 aPos;

invariant gl_Position;
out vec3 TexCoord0;

uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    TexCoord0 = aPos;
    gl_Position = vec4(_Projection * _View * vec4(aPos, 1.0)).xyww;
}
@endprogram vertex

@program fragment
out vec4 FragColor;

in vec3 TexCoord0;

uniform samplerCube _Skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
}
@endprogram fragment
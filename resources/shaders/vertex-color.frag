// Output intepolated colors from the previous stage as-is.
#version 410

layout(location = 0)  in vec3 vertexColor;
layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = vec4(vertexColor, 1.0);
}

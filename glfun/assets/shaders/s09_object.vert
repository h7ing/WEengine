#version 330 core

uniform mat4 mvp;
uniform mat4 modelview;
uniform mat3 normalmatrix;

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 fragPos;
out vec3 fragNormal;

void main() {
    gl_Position = mvp * vec4(aPos, 1.0);

    fragPos = vec3(modelview * vec4(aPos, 1.0));
    fragNormal = normalmatrix * aNormal;
    texCoord = aTexCoord;
}
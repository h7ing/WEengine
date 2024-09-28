#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vColor;

void main() {
    vec3 offset = vec3(0, -0.5, 0);
	gl_Position = vec4(aPos + offset, 1.0);
    vColor = gl_Position.xyz;
}
#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 mvp;
uniform mat4 modelview;
uniform mat3 norm;

uniform vec3 lightPos;
uniform vec3 camPos;

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;

out vec3 vNormal;
out vec3 fragPos;
out vec3 gouraud;

void main() {
	gl_Position = mvp * vec4(aPos, 1.0f);
	vNormal = norm * aNormal; // mark: the view space way
	fragPos = vec3(modelview * vec4(aPos, 1.0f));
}
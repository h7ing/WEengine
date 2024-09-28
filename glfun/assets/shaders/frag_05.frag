#version 330 core

uniform sampler2D tex;


in vec3 vertColor;
in vec2 vertCord;

out vec4 fragColor;

void main() {
	fragColor = texture(tex, vertCord);
}
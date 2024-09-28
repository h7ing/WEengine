#version 330 core

uniform sampler2D tex1;
uniform sampler2D tex2;

in vec3 vertColor;
in vec2 vertCord;

out vec4 fragColor;

void main() {
	fragColor = mix(texture(tex1, vertCord), texture(tex2, vertCord), 0.5);
}
#version 330 core

uniform float uColorRatio;

in vec3 vertColor;

out vec4 fragColor;

void main() {
	fragColor = vec4(vertColor.rgb * uColorRatio, 1.0);
}
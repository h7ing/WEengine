#version 330 core

out vec4 fragColor;
uniform float greenVal;

void main() {
	fragColor = vec4(1.0, greenVal, 0.44, 1.0);
}
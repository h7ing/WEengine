#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 mvp;
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
	vNormal = vec3(model * vec4(aNormal, 0)); // 法线也需要转换到世界坐标，但不需要齐次坐标，w分量为0
	fragPos = vec3(model * vec4(aPos, 1.0f));

	// mark: the view space way
	// vNormal = mat3(transpose(inverse(view * model))) * aNormal;
	// fragPos = vec3(view * model * vec4(aPos, 1.0f));


	// mark: per vertex shading way
	// float ambientFactor = 0.1f;
	// vec3 lightColor = vec3(1.0f);
	// vec3 ambient = ambientFactor * lightColor;

	// float diffuseStrength = 0.5f;
	// vec3 fragNormal = normalize(vNormal);
	// vec3 lightDir = normalize(lightPos - fragPos);
	// float diffuseFactor = max(dot(lightDir, fragNormal), 0);
	// vec3 diffuse = diffuseStrength * diffuseFactor * lightColor;

	// vec3 camDir = normalize(camPos - fragPos);
	// float specularStrenth = 1.0f;
	// float shininess = 128.0f;
	// vec3 reflectDir = reflect(-lightDir, fragNormal);
	// float specularFactor = pow(max(0, dot(reflectDir, camDir)), shininess);
	// vec3 specular = specularStrenth * specularFactor * lightColor;

	// gouraud = ambient + diffuse + specular;
}
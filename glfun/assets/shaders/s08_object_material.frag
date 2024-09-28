#version 330 core

// mark: the Blinn-Phong shading model

struct Light {
	vec3 color;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform mat4 view;
uniform Light light;
uniform Material material;

in vec3 vNormal;
in vec3 fragPos;
out vec4 FragColor;

void main() {
	vec3 ambient = material.ambient * light.ambient;

	vec3 fragNormal = normalize(vNormal);
	vec4 lightPos = view * vec4(light.position, 1.0f);
	vec3 lightDir = normalize(lightPos.xyz - fragPos);
	float diffuseFactor = max(dot(lightDir, fragNormal), 0);
	vec3 diffuse = material.diffuse * diffuseFactor * light.diffuse;

	vec3 camDir = normalize(-fragPos); // mark: = camPos - fragPos, view space中相机位置为原点
	vec3 halfVec = normalize(camDir + lightDir);
	float specularFactor = pow(max(0, dot(fragNormal, halfVec)), material.shininess);
	vec3 specular = material.specular * specularFactor * light.specular;


	vec3 result = (ambient + diffuse + specular) * material.color;

	FragColor = vec4(result, 1.0);
}
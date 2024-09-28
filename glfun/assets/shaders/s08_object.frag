#version 330 core

// mark: the Blinn-Phong shading model

uniform vec3 lightPos;
uniform vec3 camPos;

in vec3 vNormal;
in vec3 fragPos;
in vec3 gouraud;
out vec4 FragColor;

void main() {
	float ambientFactor = 0.1f;
	vec3 lightColor = vec3(1.0f);
	vec3 ambient = ambientFactor * lightColor;

	float diffuseStrength = 0.5f;
	vec3 fragNormal = normalize(vNormal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffuseFactor = max(dot(lightDir, fragNormal), 0);
	vec3 diffuse = diffuseStrength * diffuseFactor * lightColor;

	vec3 camDir = normalize(camPos - fragPos);
	// vec3 camDir = normalize(-fragPos); // mark: the view space way
	float specularStrenth = 0.5f;
	float shininess = 256.0f;
	// mark: Phong model use the reflect way from learnopengl
	// vec3 reflectDir = reflect(-lightDir, fragNormal);
	// float specularFactor = pow(max(0, dot(reflectDir, camDir)), shininess); // mark: cos(alpha)
	// vec3 specular = specularStrenth * specularFactor * lightColor;

	// mark: or Blinn-Phong model use the halfvec way from tiger book
	vec3 halfVec = normalize(camDir + lightDir);
	float specularFactor = pow(max(0, dot(fragNormal, halfVec)), shininess); // mark: cos(beta), here alpha = 2beta, we can prove it
	vec3 specular = specularStrenth * specularFactor * lightColor;


	vec3 objColor = vec3(1.0, 0,0);
	vec3 result = (ambient + diffuse + specular) * objColor;
	// vec3 result = gouraud * objColor;
	FragColor = vec4(result, 1.0);
}
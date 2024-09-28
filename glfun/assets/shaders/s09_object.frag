#version 330 core

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
	sampler2D diffuse;
	sampler2D specular;
    sampler2D emission;
	float shininess;
};

uniform mat4 viewmatrix;
uniform Light light;
uniform Material material;
uniform float delta;

in vec2 texCoord;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragColor;

void main() {
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(fragNormal);

    vec4 lightPos = viewmatrix * vec4(light.position, 1.0f);

    vec3 lightDir = normalize(lightPos.xyz - fragPos);
    float diffuseFactor = max(dot(lightDir, norm), 0);
    vec3 diffuse = texture(material.diffuse, texCoord).rgb * diffuseFactor * light.diffuse;

    vec3 camDir = normalize(-fragPos);
    vec3 halfVec = normalize(camDir + lightDir);
    float specularFactor = pow(max(0, dot(norm, halfVec)), material.shininess);
    vec3 specular = texture(material.specular, texCoord).rgb * specularFactor * light.specular;

    vec3 emission = texture(material.emission, vec2(texCoord.x, texCoord.y + delta)).rgb;

    fragColor = vec4((ambient + diffuse + specular + emission) * material.color, 1);
}
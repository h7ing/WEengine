#version 330 core

struct DirectionalLight {
	float enable;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	float enable;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// attenuation
	float kc;
	float kl;
	float kq;
};

struct SpotLight {
	float enable;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// attenuation
	float kc;
	float kl;
	float kq;

	vec3 direction;
	float inner;
	float outer;
};

uniform mat4 modelview;
uniform mat4 viewmatrix;
uniform mat4 normalmatrix;
uniform mat4 projmatrix;
uniform DirectionalLight light0;
uniform PointLight light1;
uniform SpotLight light2;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 uv;
out vec3 norm;
out vec3 fragposmv;

// 方向光
out vec3 halfdir;
out vec3 lightdir;

// 点光
out vec3 halfdir1;
out vec3 lightdir1;
// out float lightdist1;
out vec3 lightpos1;

// 聚光
out vec3 lightdir2;
out vec3 halfdir2;
out vec3 lightpos2;
out vec3 lightspotdirection2;


void main() {
	uv = aTexCoord;
	norm = vec3(normalmatrix * vec4(aNormal, 0));
	vec4 fragPos = modelview * vec4(aPos, 1.0);
	gl_Position = projmatrix * fragPos;

	fragposmv = fragPos.xyz;

	// directional light
	// mark: 光源方向=光源位置-被照对象位置，与光照模型公式相反
	lightdir = normalize(vec3(viewmatrix * vec4(-light0.direction, 0)));
	vec3 eyedir = normalize(-fragPos.xyz);
	halfdir = normalize(lightdir + eyedir);


	// point light
	vec4 light1Pos = viewmatrix * vec4(light1.position, 1.0f);
	// lightdist1 = length(light1Pos.xyz - fragPos.xyz); // 距离的插值结果不正确
	lightpos1 = light1Pos.xyz;

	lightdir1 = normalize( vec3( light1Pos.xyz - fragPos.xyz ) );
	halfdir1 = normalize(lightdir1 + eyedir);


	// spot light
	vec4 light2Pos = viewmatrix * vec4(light2.position, 1.0);
	lightdir2 = normalize(vec3(light2Pos.xyz - fragPos.xyz));
	halfdir2 = normalize(lightdir2 + eyedir);
	lightpos2 = light2Pos.xyz;

	lightspotdirection2 = vec3(viewmatrix * vec4(light2.direction, 0));
}
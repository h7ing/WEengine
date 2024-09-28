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

struct Material {
	vec3 color;
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform mat4 modelview;
uniform mat4 viewmatrix;

uniform DirectionalLight light0;
uniform PointLight light1;
uniform SpotLight light2;
uniform Material material;

in vec2 uv;
in vec3 norm;
in vec3 fragposmv;

// 方向光
in vec3 halfdir;
in vec3 lightdir;

// 点光
in vec3 halfdir1;
in vec3 lightdir1;
// in float lightdist1;
in vec3 lightpos1;


// 聚光
in vec3 lightdir2;
in vec3 halfdir2;
in vec3 lightpos2;
in vec3 lightspotdirection2;

out vec4 fragColor;

vec3 calc_directional_lit(DirectionalLight light, vec3 n, vec3 h, vec3 l) {
	vec3 ambient = material.ambient * light.ambient;

	vec3 diffuse = texture(material.diffuse, uv).rgb * light.diffuse * max(0, dot(n, l));

	vec3 specular = texture(material.specular, uv).rgb * light.specular * pow(max(0, dot(n, h)), material.shininess);

	return (ambient + diffuse + specular);
}

vec3 calc_point_lit(PointLight light, vec3 n, vec3 h, vec3 l, float att) {
	vec3 ambient = material.ambient * light.ambient;

	vec3 diffuse = texture(material.diffuse, uv).rgb * light.diffuse * max(0, dot(n, l));

	vec3 specular = texture(material.specular, uv).rgb * light.specular * pow( max(0, dot(n, h)), material.shininess);

	return (ambient + diffuse + specular) * att;
}

vec3 calc_spot_lit(SpotLight light, vec3 n, vec3 h, vec3 l, float att, vec3 spotdir) {
	vec3 ambient = material.ambient * light.ambient;

	vec3 diffuse = texture(material.diffuse, uv).rgb * light.diffuse * max(0, dot(n, l));

	vec3 specular = texture(material.specular, uv).rgb * light.specular * pow( max(0, dot(n, h)), material.shininess);


	float costheta = dot(spotdir, l);

	// mark: 可以不用判断
	// if (costheta < light.outer) {
	// 	diffuse = vec3(0);
	// 	specular = vec3(0);
	// } else if (costheta < light.inner) {
	// 	float intensity = (costheta - light.outer) / (light.inner - light.outer);
	// 	intensity = clamp(intensity, 0, 1.0);
	// 	diffuse *= intensity;
	// 	specular *= intensity;
	// }

	float intensity = (costheta - light.outer) / (light.inner - light.outer);
	intensity = clamp(intensity, 0, 1.0);
	// intensity = 1.0 - intensity; // 灯下黑 :)
	diffuse *= intensity;
	specular *= intensity;


	return (ambient + diffuse + specular) * att;
}

void main() {
	vec3 n = normalize(norm.xyz);
	vec3 l = normalize(lightdir);
	vec3 h = normalize(halfdir);

	vec3 directional = calc_directional_lit(light0, n, h, l) * light0.enable;


	vec3 l1 = normalize(lightdir1);
	vec3 h1 = normalize(halfdir1);

	float lightdist1 = length(lightpos1 - fragposmv);

	float attenuation = 1.0 / (light1.kc + light1.kl * lightdist1 + light1.kq * lightdist1 * lightdist1);
	vec3 pointlight = calc_point_lit(light1, n, h1, l1, attenuation) * light1.enable;


	vec4 light2Pos = viewmatrix * vec4(light2.position, 1.0);
	// vec3 l2 = normalize(lightdir2);
	vec3 l2 = normalize(vec3(light2Pos.xyz - fragposmv));
	vec3 eyedir = normalize(-fragposmv);
	vec3 h2 = normalize(l2 + eyedir);
	// vec3 h2 = normalize(halfdir2);
	vec3 l2dir = normalize(-lightspotdirection2); // 注意光源计算的时候是从被照点到光源的方向
	float lightdist2 = length(lightpos2 - fragposmv);
	float att2 = 1.0 / (light2.kc + light2.kl * lightdist2 + light2.kq * lightdist2 * lightdist2);
	vec3 spotlight = calc_spot_lit(light2, n, h2, l2, att2, l2dir) * light2.enable;

	vec3 intensity = directional + pointlight + spotlight;

	fragColor = vec4(intensity * material.color, 1.0);
}
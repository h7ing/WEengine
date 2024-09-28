#include "play_05_texture.h"
#include <GL/glew.h>
#include "hegl_util.h"
#include "hengine.h"


typedef struct HETexComp {
	GLuint vao;
	GLuint shader;
	GLuint texture;
} HETexComp;

static HETexComp texcomp;


static void init_05_texture() {
	int width, height, channels;
	unsigned char *data = he_asset_load_texture("textures/texture.jpg", &width, &height, &channels);

	glGenTextures(1, &texcomp.texture);
	glBindTexture(GL_TEXTURE_2D, texcomp.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	he_asset_free_image(data);
}

static void init_05_vertex() {
	glGenVertexArrays(1, &texcomp.vao);
	glBindVertexArray(texcomp.vao);

	float vertices[] = {
		// vert, color, texcoord
		-0.5f, 0.5f, 0, 1.0f, 1.0f, 1.0f, 0, 1.0f,    // top left
		0.5f, 0.5f, 0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  // top right
		0.5f, -0.5f, 0, 1.0f, 1.0f, 1.0f, 1.0f, 0,    // bottom right
		-0.5f, -0.5f, 0, 1.0f, 1.0f, 1.0f, 0, 0       // bottom left
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 0-vec3 aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// 1-vec3 aColor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 2-vec2 aTexCoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	GLuint indices[] = {
		0,1,3,
		1,2,3
	};

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

static void init_05_elements() {
	texcomp.shader = hegl_create_shader_program("shaders/vert_05.vert", "shaders/frag_05.frag");

	init_05_texture();

	init_05_vertex();
}


void play_05_draw_with_texture() {
	static int inited = 0;
	if (!inited) {
		inited = 1;

		init_05_elements();
	}

	glUseProgram(texcomp.shader);
	glBindVertexArray(texcomp.vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texcomp.texture);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



typedef struct HETwoTex {
	GLuint vao;
	GLuint shader;
	GLuint tex1;
	GLuint tex2;
} HETwoTex;

static HETwoTex textwo;

static void init_05_two_textures() {
	int width, height, channels;
	unsigned char *data = he_asset_load_texture("textures/texture.png", &width, &height, &channels);
	glGenTextures(1, &textwo.tex1);
	glBindTexture(GL_TEXTURE_2D, textwo.tex1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	he_asset_free_image(data);


	unsigned char *img2 = he_asset_load_texture("textures/awesomeface.png", &width, &height, &channels);
	glGenTextures(1, &textwo.tex2);
	glBindTexture(GL_TEXTURE_2D, textwo.tex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img2);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	he_asset_free_image(img2);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textwo.tex1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textwo.tex2);

	glUseProgram(textwo.shader);
	
	glUniform1i(glGetUniformLocation(textwo.shader, "tex1"), 0);
	glUniform1i(glGetUniformLocation(textwo.shader, "tex2"), 1);
}

static void init_05_two_vertex() {
	glGenVertexArrays(1, &textwo.vao);
	glBindVertexArray(textwo.vao);

	float vertices[] = {
		// vert, color, texcoord
		-0.5f, 0.5f, 0, 1.0f, 1.0f, 1.0f, 0, 1.0f,    // top left
		0.5f, 0.5f, 0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  // top right
		0.5f, -0.5f, 0, 1.0f, 1.0f, 1.0f, 1.0f, 0,    // bottom right
		-0.5f, -0.5f, 0, 1.0f, 1.0f, 1.0f, 0, 0       // bottom left
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 0-vec3 aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// 1-vec3 aColor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 2-vec2 aTexCoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	GLuint indices[] = {
		0,1,3,
		1,2,3
	};

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

static void init_05_two_elements() {

	textwo.shader = hegl_create_shader_program("shaders/vert_05.vert", "shaders/frag_05_02.frag");

	init_05_two_textures();

	init_05_two_vertex();
}

void play_05_draw_two_textures() {
	static int inited = 0;
	if (!inited) {
		inited = 1;
		init_05_two_elements();
	}

	glUseProgram(textwo.shader);
	glBindVertexArray(textwo.vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
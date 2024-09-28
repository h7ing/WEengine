#include "play_09_lightmap.h"
#include <math.h>
#include <cglm/cglm.h>
#include <GL/glew.h>
#include "hengine.h"
#include "hegl_util.h"


static float TORAD = M_PI / 180.0f;

typedef struct Transform {
    vec3 position;
    vec3 rotation; // rad
    vec3 scale;
    mat4 matrix;
} Transform;

static void transform_set_position(Transform *t, float x, float y, float z) {
    t->position[0] = x;
    t->position[1] = y;
    t->position[2] = z;
}

static void transform_set_rotation_euler(Transform *t, float rx, float ry, float rz) {
    t->rotation[0] = rx * TORAD;
    t->rotation[1] = ry * TORAD;
    t->rotation[2] = rz * TORAD;
}

static void transform_set_scale(Transform *t, vec3 scale) { glm_vec3_copy(scale, t->scale); }

static void transform_update_matrix(Transform *t) {
    glm_mat4_identity(t->matrix);

    glm_translate(t->matrix, t->position);

    glm_scale(t->matrix, t->scale);

    glm_rotate_y(t->matrix, t->rotation[1], t->matrix);
    glm_rotate_x(t->matrix, t->rotation[0], t->matrix);
    glm_rotate_z(t->matrix, t->rotation[1], t->matrix);
}


typedef struct ObjectMaterial {
    vec3 color;
    vec3 ambient;
    GLuint diffusemap;
    GLuint specularmap;
    GLuint emissionmap;
    float shininess;
    mat4 modelviewinv;
    mat3 normalmatrix;
    GLuint shader;
    GLuint vao;
    GLuint vbo;
} ObjectMaterial;

typedef struct LightMaterial {
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    GLuint shader;
    GLuint vao;
    GLuint vbo;
} LightMaterial;

typedef struct Camera {
    Transform transform;
    vec3 direction;
    float fovy; // rad
    mat4 view;
    mat4 projection;
    mat4 modelview; // view * model
    mat4 vp; // projection * view
    mat4 mvp; // projection * view * model
} Camera;

typedef struct Light {
    Transform transform;
    LightMaterial material;
} Light;

typedef struct RenderObject {
    Transform transform;
    ObjectMaterial material;
} RenderObject;

typedef struct Scene {
    Camera camera;
    Light light;
    RenderObject object;
} Scene;


// data

static float vertices_light[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

static float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

static Scene scene09;



static void __init_scene_09(Scene *scene) {
    // camera
    transform_set_position(&scene->camera.transform, 1.5f, 1.5f, 5.0f);
    glm_vec3_negate_to(scene->camera.transform.position, scene->camera.direction);
    glm_normalize(scene->camera.direction);
    glm_look(scene->camera.transform.position, scene->camera.direction, GLM_YUP, scene->camera.view);

    scene->camera.fovy = M_PI_4;
    glm_perspective(scene->camera.fovy, he_get_window_aspect(), 0.1f, 100.0f, scene->camera.projection);


    // light
    transform_set_position(&scene->light.transform, 1.2f, 1.0f, 2.0f);
    transform_set_rotation_euler(&scene->light.transform, 0, 0, 0);
    transform_set_scale(&scene->light.transform, (vec3){0.2f,0.2f,0.2f});
    transform_update_matrix(&scene->light.transform);
    glm_vec3_copy((vec3){0,1.0f,1.0f}, scene->light.material.color);
    glm_vec3_copy(GLM_VEC3_ONE, scene->light.material.ambient);
    glm_vec3_copy(GLM_VEC3_ONE, scene->light.material.diffuse);
    glm_vec3_copy(GLM_VEC3_ONE, scene->light.material.specular);

    glGenVertexArrays(1, &scene->light.material.vao);
    glBindVertexArray(scene->light.material.vao);
    glGenBuffers(1, &scene->light.material.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, scene->light.material.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_light), vertices_light, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    scene->light.material.shader = hegl_create_shader_program("shaders/s08_light.vert", "shaders/s08_light.frag");
    glUseProgram(scene->light.material.shader);
    glUniform3fv(glGetUniformLocation(scene->light.material.shader, "lightColor"), 1, scene->light.material.color);


    // object
    transform_set_position(&scene->object.transform, 0, 0, 0);
    transform_set_rotation_euler(&scene->object.transform, 0, 0, 0);
    transform_set_scale(&scene->object.transform, (vec3){1,1,1});
    transform_update_matrix(&scene->object.transform);

    glGenVertexArrays(1, &scene->object.material.vao);
    glBindVertexArray(scene->object.material.vao);
    glGenBuffers(1, &scene->object.material.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, scene->object.material.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // aNormal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // aTexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    scene->object.material.shader = hegl_create_shader_program("shaders/s09_object.vert", "shaders/s09_object.frag");

    glm_vec3_copy((vec3){0.1f, 0.1f, 0.1f}, scene->object.material.ambient);
    glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, scene->object.material.color);
    scene->object.material.shininess = 128.0f * 2.0f;
    scene->object.material.diffusemap = hegl_create_texture("textures/container2.png");
    scene->object.material.specularmap = hegl_create_texture("textures/container2_specular.png");
    // scene->object.material.specularmap = hegl_create_texture("textures/lighting_maps_specular_color.png");
    scene->object.material.emissionmap = hegl_create_texture("textures/matrix.jpg");

    glUseProgram(scene->object.material.shader);
    glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.color"), 1, scene->light.material.color);
    glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.ambient"), 1, scene->light.material.ambient);
    glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.diffuse"), 1, scene->light.material.diffuse);
    glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.specular"), 1, scene->light.material.specular);

    glUniform3fv(glGetUniformLocation(scene->object.material.shader, "material.color"), 1, scene->object.material.color);
    glUniform3fv(glGetUniformLocation(scene->object.material.shader, "material.ambient"), 1, scene->object.material.ambient);
    glUniform1f(glGetUniformLocation(scene->object.material.shader, "material.shininess"), scene->object.material.shininess);

    int diffuse = 0;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene->object.material.diffusemap);

    int specular = 1;
    glActiveTexture(GL_TEXTURE0 + specular);
    glBindTexture(GL_TEXTURE_2D, scene->object.material.specularmap);

    int emission = 2;
    glActiveTexture(GL_TEXTURE0 + emission);
    glBindTexture(GL_TEXTURE_2D, scene->object.material.emissionmap);

    glUniform1i(glGetUniformLocation(scene->object.material.shader, "material.diffuse"), diffuse);
    glUniform1i(glGetUniformLocation(scene->object.material.shader, "material.specular"), specular);
    glUniform1i(glGetUniformLocation(scene->object.material.shader, "material.emission"), emission);
}


static void __draw_scene_09(Scene *scene) {
    glm_look(scene->camera.transform.position, scene->camera.direction, GLM_YUP, scene->camera.view);
    glm_mat4_mul(scene->camera.projection, scene->camera.view, scene->camera.vp);

    // light
    glm_mat4_mul(scene->camera.vp, scene->light.transform.matrix, scene->camera.mvp);
    GLuint shader = scene->light.material.shader;
    glUseProgram(shader);
    glBindVertexArray(scene->light.material.vao);
    glUniformMatrix4fv(glGetUniformLocation(shader, "mvp"), 1, GL_FALSE, &scene->camera.mvp[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // object
    shader = scene->object.material.shader;
    glUseProgram(shader);
    glBindVertexArray(scene->object.material.vao);
    glm_mat4_mul(scene->camera.vp, scene->object.transform.matrix, scene->camera.mvp);
    glm_mat4_mul(scene->camera.view, scene->object.transform.matrix, scene->camera.modelview);
    glm_mat4_inv(scene->camera.modelview, scene->object.material.modelviewinv);
    glm_mat4_pick3t(scene->object.material.modelviewinv, scene->object.material.normalmatrix);

    glUniformMatrix4fv(glGetUniformLocation(shader, "mvp"), 1, GL_FALSE, &scene->camera.mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelview"), 1, GL_FALSE, &scene->camera.modelview[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(shader, "normalmatrix"), 1, GL_FALSE, &scene->object.material.normalmatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewmatrix"), 1, GL_FALSE, &scene->camera.view[0][0]);

    static float delta = 0;
    delta += he_get_delta() * 0.5f;
    if (delta > 1.0f) delta = 0;
    glUniform1f(glGetUniformLocation(shader, "delta"), delta);

    glUniform3fv(glGetUniformLocation(shader, "light.position"), 1, scene->light.transform.position);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void play_09_lightmap(int op_translate_x, int op_translate_y, int op_translate_z) {
    static int inited = 0;
    Scene *scene = &scene09;
    if (!inited) {
        inited = 1;
        __init_scene_09(scene);
    }

    if (op_translate_x || op_translate_y || op_translate_z) {
		glm_vec3_muladd((vec3){0.25f,0.25f,0.25f}, (vec3){op_translate_x, op_translate_y, op_translate_z} ,scene->light.transform.position);

		transform_update_matrix(&scene->light.transform);
	}

    __draw_scene_09(scene);
}
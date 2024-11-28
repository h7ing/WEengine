#include "wetransform.h"
#include "cglm.h"

void WEtransform_compose_matrix(struct WEtransform *t) {
    // compose matrix: in T*R*S order (scale first)
    // see: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#coordinate-system-and-units

    glm_scale(t->matrix, t->scale);
    glm_quat_rotate(t->matrix, t->rotation, t->matrix);
    glm_translate(t->matrix, t->position);
}

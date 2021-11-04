#pragma once
#include "entity_view.h"

float smooth_val(float cur, float tgt, float dt);
float smooth_val_spherical(float cur, float tgt, float dt);
void predict_receive_update(entity_view *d, entity_view *data);

void do_entity_fadeinout(uint64_t key, entity_view * data);
void lerp_entity_positions(uint64_t key, entity_view *data);

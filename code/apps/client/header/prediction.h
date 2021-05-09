#pragma once
#include "entity_view.h"

float smooth_val(float cur, float tgt, float dt);
void predict_receive_update(entity_view *d, entity_view *data);
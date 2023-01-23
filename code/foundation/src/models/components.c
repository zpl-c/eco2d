#include "models/components.h"



#define X(comp) ECS_COMPONENT_DECLARE(comp);
_COMPS
#undef X

void ComponentsImport(ecs_world_t *ecs) {
    ECS_MODULE(ecs, Components);

#define X(comp) ECS_COMPONENT_DEFINE(ecs, comp);
	_COMPS
#undef X
}

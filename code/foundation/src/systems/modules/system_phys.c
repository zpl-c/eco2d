
void PhysOnCreateBody(ecs_iter_t *it) {
	PhysicsBody *pb = ecs_field(it, PhysicsBody, 1);
	Position *p = ecs_field(it, Position, 2);

	for (int i = 0; i < it->count; i++) {
		if (pb[i].body_ptr > 0) continue; 
		const frMaterial mat = {
			.density = pb[i].density,
			.staticFriction = pb[i].static_friction,
			.dynamicFriction = pb[i].dynamic_friction,
		};

		frShape *shape = 0;
		if (pb[i].kind == PHYS_CIRCLE) {
			shape = frCreateCircle(mat, pb[i].circle.r);
		} else {
			shape = frCreateRectangle(mat, pb[i].rect.w, pb[i].rect.h);
		}

		frBodyFlags flags = 0x0;
		if (pb[i].inf_inertia) flags |= FR_FLAG_INFINITE_INERTIA;
		if (pb[i].inf_mass) flags |= FR_FLAG_INFINITE_MASS;
		frBody *body = frCreateBodyFromShape(FR_BODY_DYNAMIC, flags, frVec2PixelsToMeters((Vector2){p[i].x, p[i].y}), shape);
		frAddToWorld(phys_world, body);
		pb[i].body_ptr = (uintptr_t)body;
	}
}

void PhysOnRemoveBody(ecs_iter_t *it) {
	PhysicsBody *pb = ecs_field(it, PhysicsBody, 1);

	for (int i = 0; i < it->count; i++) {
		frBody *body = (frBody*)pb[i].body_ptr;
		frRemoveFromWorld(phys_world, body);
		frShape *shape = frGetBodyShape(body);
		frReleaseBody(body);
		frReleaseShape(shape);
	}
}

void PhysSetVelocity(ecs_iter_t *it) {
	PhysicsBody *pb = ecs_field(it, PhysicsBody, 1);
	Velocity *v = ecs_field(it, Velocity, 2);

	for (int i = 0; i < it->count; i++) {
		frBody *body = (frBody*)pb[i].body_ptr;
		frSetBodyVelocity(body, frVec2PixelsToMeters((Vector2) { v[i].x , v[i].y  }));
	}
}

void PhysUpdatePosition(ecs_iter_t *it) {
	PhysicsBody *pb = ecs_field(it, PhysicsBody, 1);
	Position *p = ecs_field(it, Position, 2);
	Velocity *v = ecs_field(it, Velocity, 3);

	for (int i = 0; i < it->count; i++) {
		frBody *body  = (frBody*)pb[i].body_ptr;
		Vector2 pos = frVec2MetersToPixels(frGetBodyPosition(body));
		entity_set_position(it->entities[i], pos.x, pos.y);
		Vector2 vel = frVec2MetersToPixels(frGetBodyVelocity(body));
		v[i].x = vel.x;
		v[i].y = vel.y;
	}
}

void PhysResetPosition(ecs_iter_t *it) {
	Position *p = ecs_field(it, Position, 1);

	for (int i = 0; i < it->count; i++) {
		const PhysicsBody *pb = ecs_get(it->world, it->entities[i], PhysicsBody);
		if (!pb) continue; 
		frBody *body = (frBody*)pb->body_ptr;
		frSetBodyPosition(body, frVec2PixelsToMeters((Vector2){p[i].x, p[i].y}));
	}
}

void PhysSimulateWorld(ecs_iter_t *it) {
	frSimulateWorld(phys_world, 1.0f/60.0f);
}

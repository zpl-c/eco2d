#include "zpl.h"
#include "librg.h"
#include "components/general.h"
#include "world/world.h"

typedef struct {
    uint8_t *data;
    uint32_t seed;
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t block_size;
    uint16_t chunk_size;
    uint16_t world_size;
    ecs_world_t *ecs;
    librg_world *tracker;
} world_data;


static world_data world = {0};

int32_t world_gen();

int32_t world_write_update(librg_world *w, librg_event *e) {
    int64_t owner_id = librg_event_owner_get(w, e);
    int64_t entity_id = librg_event_entity_get(w, e);

    return 0;

    // /* prevent sending updates to users who own that entity */
    // /* since they will be responsible on telling where that entity is supposed to be */
    // if (librg_entity_owner_get(w, entity_id) == owner_id) {
    //     return LIBRG_WRITE_REJECT;
    // }

    // /* read our current position */
    // ENetPeer *peer = (ENetPeer *)librg_entity_userdata_get(w, entity_id);

    // char *buffer = librg_event_buffer_get(w, e);
    // size_t max_length = librg_event_size_get(w, e);

    // /* check if we have enough space to write and valid position */
    // if (sizeof(vec3) > max_length || !peer->data) {
    //     return LIBRG_WRITE_REJECT;
    // }

    // /* write data and return how much we've written */
    // memcpy(buffer, peer->data, sizeof(vec3));
    // return sizeof(vec3);
}

int32_t world_init(int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t world_size) {
    if (world.data) {
        world_destroy();
    }

    world.seed = seed;
    world.block_size = block_size;
    world.chunk_size = chunk_size;
    world.world_size = world_size;

    world.width = chunk_size * world_size;
    world.height = chunk_size * world_size;
    world.size = world.width * world.height;
    world.data = zpl_malloc(sizeof(uint8_t)*world.size);

    if (!world.data) {
        return WORLD_ERROR_OUTOFMEM;
    }

    world.ecs = ecs_init();
    ecs_set_entity_range(world.ecs, 0, UINT32_MAX);

    world.tracker = librg_world_create();

    if (world.tracker == NULL) {
        zpl_printf("[ERROR] An error occurred while trying to create a server world.\n");
        return WORLD_ERROR_TRACKER_FAILED;
    }

    zpl_printf("[INFO] Created a new server world\n");

    /* config our world grid */
    librg_config_chunksize_set(world.tracker, block_size * chunk_size, block_size * chunk_size, 1);
    librg_config_chunkamount_set(world.tracker, world_size, world_size, 1);
    librg_config_chunkoffset_set(world.tracker, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID);

    ECS_IMPORT(world.ecs, General);

    for (int i = 0; i < chunk_size * chunk_size; ++i) {
        ecs_entity_t e = ecs_new(world.ecs, 0);
        ecs_set(world.ecs, e, Chunk, {
            .x = i % chunk_size,
            .y = i / chunk_size,
        });

        librg_entity_track(world.tracker, e);
        librg_entity_chunk_set(world.tracker, e, i);
    }

    // librg_event_set(world.tracker, LIBRG_WRITE_UPDATE, world_write_update);
    // librg_event_set(world.tracker, LIBRG_READ_UPDATE, server_read_update);

    return world_gen();
}

int32_t world_destroy(void) {
    librg_world_destroy(world.tracker);
    ecs_fini(world.ecs);
    zpl_mfree(world.data);
    zpl_memset(&world, 0, sizeof(world));
    zpl_printf("[INFO] World was destroyed.\n");
    return WORLD_ERROR_NONE;
}

int32_t world_update() {
    ecs_progress(world.ecs, 0);
    return 0;
}

uint32_t world_buf(uint8_t const **ptr, uint32_t *width) {
    ZPL_ASSERT_NOT_NULL(world.data);
    ZPL_ASSERT_NOT_NULL(ptr);
    *ptr = world.data;
    if (width) *width = world.width;
    return world.size;
}

ecs_world_t * world_ecs() {
    return world.ecs;
}

librg_world * world_tracker() {
    return world.tracker;
}

#include "world_gen.c"

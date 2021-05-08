#include "zpl.h"
#include "librg.h"
#include "modules/general.h"
#include "modules/net.h"
#include "world/world.h"
#include "entity_view.h"

#include "packets/pkt_send_librg_update.h"

typedef struct {
    uint8_t *data;
    uint32_t seed;
    uint32_t size;
    uint16_t block_size;
    uint16_t chunk_size;
    uint16_t chunk_amount;
    uint16_t dim;
    uint64_t tracker_update[3];
    ecs_world_t *ecs;
    librg_world *tracker;
    world_pkt_reader_proc *reader_proc;
    world_pkt_writer_proc *writer_proc;
} world_data;

static world_data world = {0};

#define WORLD_TRACKER_UPDATE_FAST_MS 10
#define WORLD_TRACKER_UPDATE_NORMAL_MS 100
#define WORLD_TRACKER_UPDATE_SLOW_MS 800

int32_t world_gen();

entity_view world_build_entity_view(int64_t e) {
    ECS_IMPORT(world_ecs(), General);
    ECS_IMPORT(world_ecs(), Net);
    entity_view view = {0};
    
    // TODO(zaklaus): branch out based on ECS tags
    const Position *pos = ecs_get(world_ecs(), e, Position);
    if (pos) {
        view.kind = ecs_has(world_ecs(), e, EcsClient) ? EKIND_PLAYER : EKIND_THING;
        view.x = pos->x;
        view.y = pos->y;
        return view;
    }
    
    const Chunk *chpos = ecs_get(world_ecs(), e, Chunk);
    if (chpos) {
        view.kind = EKIND_CHUNK;
        view.x = chpos->x;
        view.y = chpos->y;
        return view;
    }
    
    return view;
}

int32_t tracker_write_create(librg_world *w, librg_event *e) {
    int64_t owner_id = librg_event_owner_get(w, e);
    int64_t entity_id = librg_event_entity_get(w, e);
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);
    
    return entity_view_pack_struct(buffer, actual_length, world_build_entity_view(entity_id));
}

int32_t tracker_write_remove(librg_world *w, librg_event *e) {
    return 0;
}

int32_t tracker_write_update(librg_world *w, librg_event *e) {
    int64_t entity_id = librg_event_entity_get(w, e);
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);
    
    return entity_view_pack_struct(buffer, actual_length, world_build_entity_view(entity_id));
}

void world_setup_pkt_handlers(world_pkt_reader_proc *reader_proc, world_pkt_writer_proc *writer_proc) {
    world.reader_proc = reader_proc;
    world.writer_proc = writer_proc;
}

int32_t world_init(int32_t seed, uint16_t block_size, uint16_t chunk_size, uint16_t chunk_amount) {
    if (world.data) {
        return 0;
    }
    
    world.seed = seed;
    world.chunk_size = chunk_size;
    world.chunk_amount = chunk_amount;
    
    world.block_size = block_size;
    world.dim = (world.chunk_size * world.chunk_amount);;
    world.size = world.dim * world.dim;
    
    if (world.tracker == NULL) {
        world.tracker = librg_world_create();
    }
    
    if (world.tracker == NULL) {
        zpl_printf("[ERROR] An error occurred while trying to create a server world.\n");
        return WORLD_ERROR_TRACKER_FAILED;
    }
    
    /* config our world grid */
    librg_config_chunksize_set(world.tracker, block_size * chunk_size, block_size * chunk_size, 0);
    librg_config_chunkamount_set(world.tracker, chunk_amount, chunk_amount, 0);
    librg_config_chunkoffset_set(world.tracker, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID, 0);
    
    librg_event_set(world.tracker, LIBRG_WRITE_CREATE, tracker_write_create);
    librg_event_set(world.tracker, LIBRG_WRITE_REMOVE, tracker_write_remove);
    librg_event_set(world.tracker, LIBRG_WRITE_UPDATE, tracker_write_update);
    
    world.data = zpl_malloc(sizeof(uint8_t)*world.size);
    
    if (!world.data) {
        return WORLD_ERROR_OUTOFMEM;
    }
    
    world.ecs = ecs_init();
    ecs_set_entity_range(world.ecs, 0, UINT32_MAX);
    
    ECS_IMPORT(world.ecs, General);

    for (int i = 0; i < chunk_amount * chunk_amount; ++i) {
        ecs_entity_t e = ecs_new(world.ecs, 0);
        Chunk *chunk = ecs_get_mut(world.ecs, e, Chunk, NULL);
        chunk->x = i % chunk_amount - chunk_amount/2;
        chunk->y = i / chunk_amount - chunk_amount/2;
        
        librg_chunk chid = librg_chunk_from_chunkpos(world.tracker, chunk->x, chunk->y, 0);
        librg_entity_track(world.tracker, e);
        librg_entity_chunk_set(world.tracker, e, chid);
    }

    zpl_printf("[INFO] Created a new server world\n");
    
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

#define WORLD_LIBRG_BUFSIZ 8192

static void world_tracker_update(uint8_t ticker, uint8_t freq, uint8_t radius) {
    if (world.tracker_update[ticker] > zpl_time_rel_ms()) return;
        world.tracker_update[ticker] = zpl_time_rel_ms() + freq;
    
    ECS_IMPORT(world.ecs, Net);
    ecs_query_t *query = ecs_query_new(world.ecs, "Net.ClientInfo, general.Position");
    
    ecs_iter_t it = ecs_query_iter(query);
    static char buffer[WORLD_LIBRG_BUFSIZ] = {0};
    
    while (ecs_query_next(&it)) {
        ClientInfo *p = ecs_column(&it, ClientInfo, 1);
        Position *pos = ecs_column(&it, Position, 2);
        
        for (int i = 0; i < it.count; i++) {
            size_t datalen = WORLD_LIBRG_BUFSIZ;
            
            // TODO(zaklaus): push radius once librg patch comes in
            int32_t result = librg_world_write(world_tracker(), p[i].peer, buffer, &datalen, NULL);
            
            if (result > 0) {
                zpl_printf("[info] buffer size was not enough, please increase it by at least: %d\n", result);
            } else if (result < 0) {
                zpl_printf("[error] an error happened writing the world %d\n", result);
            }
            
            pkt_world_write(MSG_ID_LIBRG_UPDATE, pkt_send_librg_update_encode(buffer, datalen), 1, p[i].view_id, p[i].peer);
        }
    }
}

int32_t world_update() {
    ecs_progress(world.ecs, 0);

    world_tracker_update(0, WORLD_TRACKER_UPDATE_FAST_MS, 2);
    //world_tracker_update(1, WORLD_TRACKER_UPDATE_NORMAL_MS, 4);
    //world_tracker_update(2, WORLD_TRACKER_UPDATE_SLOW_MS, 6);
    return 0;
}

int32_t world_read(void* data, uint32_t datalen, void *udata) {
    if (world.reader_proc) {
        return world.reader_proc(data, datalen, udata);
    }
    return -1;
}

int32_t world_write(pkt_header *pkt, void *udata) {
    if (world.writer_proc) {
        return world.writer_proc(pkt, udata);
    }
    return -1;
}

uint32_t world_buf(uint8_t const **ptr, uint32_t *width) {
    ZPL_ASSERT_NOT_NULL(world.data);
    ZPL_ASSERT_NOT_NULL(ptr);
    *ptr = world.data;
    if (width) *width = world.dim;
    return world.size;
}

ecs_world_t * world_ecs() {
    return world.ecs;
}

librg_world * world_tracker() {
    return world.tracker;
}

uint16_t world_block_size(void) {
    return world.block_size;
}

uint16_t world_chunk_size(void) {
    return world.chunk_size;
}

uint16_t world_chunk_amount(void) {
    return world.chunk_amount;
}

uint16_t world_dim(void) {
    return world.block_size * world.chunk_size * world.chunk_amount;
}

#include "world_gen.c"

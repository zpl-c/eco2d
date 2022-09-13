#include "zpl.h"
#include "librg.h"
#include "modules/components.h"
#include "modules/systems.h"
#include "world/world.h"
#include "entity_view.h"
#include "debug_replay.h"
#include "items.h"
#include "world/worldgen/worldgen.h"
#include "platform.h"
#include "profiler.h"
#include "game.h"
#include "entity.h"

#include "packets/pkt_send_librg_update.h"

ZPL_TABLE(static, world_snapshot, world_snapshot_, entity_view);

static world_data world = {0};
static world_snapshot streamer_snapshot;

entity_view world_build_entity_view(int64_t e) {
    entity_view *cached_ev = world_snapshot_get(&streamer_snapshot, e);
    if (cached_ev) return *cached_ev;

    entity_view view = {0};

    const Classify *classify = ecs_get(world_ecs(), e, Classify);
    ZPL_ASSERT(classify);

    view.kind = classify->id;

    const Position *pos = ecs_get(world_ecs(), e, Position);
    if (pos) {
        view.x = pos->x;
        view.y = pos->y;
    }

    const Velocity *vel = ecs_get(world_ecs(), e, Velocity);
    if (vel) {
        view.flag |= EFLAG_INTERP;
        view.vx = vel->x;
        view.vy = vel->y;
    }

    const Health *health = ecs_get(world_ecs(), e, Health);
    if (health) {
        view.hp = health->hp;
        view.max_hp = health->max_hp;
    }

    if (ecs_get(world_ecs(), e, Vehicle)) {
        Vehicle const* veh = ecs_get(world_ecs(), e, Vehicle);
        view.heading = veh->heading;
    }

    if (ecs_get(world_ecs(), e, ItemDrop)) {
        ItemDrop const* dr = ecs_get(world_ecs(), e, ItemDrop);
        view.asset = dr->kind;
        view.quantity = dr->quantity;
    }

    if (ecs_get(world_ecs(), e, Device)) {
        Device const* dev = ecs_get(world_ecs(), e, Device);
        view.asset = dev->asset;
    }

    view.inside_vehicle = ecs_get(world_ecs(), e, IsInVehicle) != 0 ? true : false;

    Inventory *inv = 0;
    if ((inv = ecs_get_mut_if(world_ecs(), e, Inventory))) {
        view.has_items = true;

        for (int i = 0; i < ITEMS_INVENTORY_SIZE; i += 1) {
            view.items[i] = inv->items[i];
        }

        const Input *in = ecs_get(world_ecs(), e, Input);
        if (in){
            view.selected_item = in->selected_item;
            view.pick_ent = (uint64_t)in->pick_ent;
            view.sel_ent = (uint64_t)in->sel_ent;

            if (world_entity_valid(in->storage_ent)){
                ItemContainer *ic = 0;
                if ((ic = ecs_get_mut_if(world_ecs(), in->storage_ent, ItemContainer))){
                    view.has_storage_items = true;

                    for (int i = 0; i < ITEMS_CONTAINER_SIZE; i += 1) {
                        view.storage_items[i] = ic->items[i];
                    }

                    view.storage_selected_item = in->storage_selected_item;
                }
            }
        }
    }

    Chunk *chunk = 0;
    if ((chunk = ecs_get_mut_if(world_ecs(), e, Chunk))) {
        view.x = chunk->x;
        view.y = chunk->y;
        view.blocks_used = 1;
        view.is_dirty = chunk->is_dirty;
        chunk->is_dirty = false;

        for (int i = 0; i < world.chunk_size*world.chunk_size; i += 1) {
            view.blocks[i] = world.block_mapping[chunk->id][i];
        }

        for (int i = 0; i < world.chunk_size*world.chunk_size; i += 1) {
            view.outer_blocks[i] = world.outer_block_mapping[chunk->id][i];
        }
    }

    world_snapshot_set(&streamer_snapshot, e, view);
    return view;
}

int32_t tracker_write_create(librg_world *w, librg_event *e) {
    int64_t entity_id = librg_event_entity_get(w, e);
#ifdef WORLD_LAYERING
    if (world.active_layer_id != WORLD_TRACKER_LAYERS-1) {
        // NOTE(zaklaus): reject updates from smaller layers
        return LIBRG_WRITE_REJECT;
    }
#endif
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);

    return (int32_t)entity_view_pack_struct(buffer, actual_length, world_build_entity_view(entity_id));
}

int32_t tracker_write_remove(librg_world *w, librg_event *e) {
    (void)e;
    (void)w;
#ifdef WORLD_LAYERING
    if (world.active_layer_id != WORLD_TRACKER_LAYERS-1) {
        // NOTE(zaklaus): reject updates from smaller layers
        return LIBRG_WRITE_REJECT;
    }
#endif
    return 0;
}

int32_t tracker_write_update(librg_world *w, librg_event *e) {
    int64_t entity_id = librg_event_entity_get(w, e);
    size_t actual_length = librg_event_size_get(w, e);
    char *buffer = librg_event_buffer_get(w, e);
    entity_view view = world_build_entity_view(entity_id);

    // NOTE(zaklaus): exclude chunks from updates as they never move
    {
        if (view.kind == EKIND_CHUNK && !view.is_dirty) {
            return LIBRG_WRITE_REJECT;
        }
    }

    // NOTE(zaklaus): action-based updates
#if ECO2D_STREAM_ACTIONFILTER
    {
        if (view.kind != EKIND_CHUNK && !entity_can_stream(entity_id)) {
            return LIBRG_WRITE_REJECT;
        }
    }
#endif

    return (int32_t)entity_view_pack_struct(buffer, actual_length, view);
}

void world_setup_pkt_handlers(world_pkt_reader_proc *reader_proc, world_pkt_writer_proc *writer_proc) {
    world.reader_proc = reader_proc;
    world.writer_proc = writer_proc;
}

static inline
void world_chunk_setup_grid(void) {
    for (int i = 0; i < zpl_square(world.chunk_amount); ++i) {
        ecs_entity_t e = ecs_new(world.ecs, 0);
        ecs_set(world.ecs, e, Classify, {.id = EKIND_CHUNK });
        Chunk *chunk = ecs_get_mut(world.ecs, e, Chunk);
        librg_entity_track(world.tracker, e);
        librg_entity_chunk_set(world.tracker, e, i);
        librg_chunk_to_chunkpos(world.tracker, i, &chunk->x, &chunk->y, NULL);
        world.chunk_mapping[i] = e;
        world.block_mapping[i] = zpl_malloc(sizeof(block_id)*zpl_square(world.chunk_size));
        world.outer_block_mapping[i] = zpl_malloc(sizeof(block_id)*zpl_square(world.chunk_size));
        chunk->id = i;
        chunk->is_dirty = false;

        for (int y = 0; y < world.chunk_size; y += 1) {
            for (int x = 0; x < world.chunk_size; x += 1) {
                int chk_x = chunk->x * world.chunk_size;
                int chk_y = chunk->y * world.chunk_size;

                block_id *c = &world.block_mapping[i][(y*world.chunk_size)+x];
                *c = world.data[(chk_y+y)*world.dim + (chk_x+x)];

                c = &world.outer_block_mapping[i][(y*world.chunk_size)+x];
                *c = world.outer_data[(chk_y+y)*world.dim + (chk_x+x)];
            }
        }
    }
}

static inline
void world_configure_tracker(void) {
    world.tracker = librg_world_create();

    ZPL_ASSERT_MSG(world.tracker, "[ERROR] An error occurred while trying to create a server world.");

    /* config our world grid */
    librg_config_chunksize_set(world.tracker, WORLD_BLOCK_SIZE * world.chunk_size, WORLD_BLOCK_SIZE * world.chunk_size, 1);
    librg_config_chunkamount_set(world.tracker, world.chunk_amount, world.chunk_amount, 0);
    librg_config_chunkoffset_set(world.tracker, LIBRG_OFFSET_BEG, LIBRG_OFFSET_BEG, LIBRG_OFFSET_BEG);

    librg_event_set(world.tracker, LIBRG_WRITE_CREATE, tracker_write_create);
    librg_event_set(world.tracker, LIBRG_WRITE_REMOVE, tracker_write_remove);
    librg_event_set(world.tracker, LIBRG_WRITE_UPDATE, tracker_write_update);
}

static inline
void world_init_worldgen_data(void) {
    world.data = zpl_malloc(sizeof(block_id)*world.size);
    world.outer_data = zpl_malloc(sizeof(block_id)*world.size);

    ZPL_ASSERT(world.data && world.outer_data);
}

static inline
void world_setup_ecs(void) {
    world.ecs = ecs_init();

    ECS_IMPORT(world.ecs, Components);
    ECS_IMPORT(world.ecs, Systems);
    world.ecs_update = ecs_query_new(world.ecs, "components.ClientInfo, components.Position");
    world.ecs_clientinfo = ecs_query_new(world.ecs, "components.ClientInfo");
}

static inline
void world_init_mapping(void) {
    world.chunk_mapping = zpl_malloc(sizeof(ecs_entity_t)*zpl_square(world.chunk_amount));
    world.block_mapping = zpl_malloc(sizeof(block_id*)*zpl_square(world.chunk_amount));
    world.outer_block_mapping = zpl_malloc(sizeof(block_id*)*zpl_square(world.chunk_amount));
    world_snapshot_init(&streamer_snapshot, zpl_heap());
}

static inline
void world_generate_instance(void) {
    int32_t world_build_status = worldgen_test(&world);
    ZPL_ASSERT(world_build_status >= 0);

    for (int i = 0; i < zpl_square(world.dim); ++i) {
        if (world.data[i] == 0) {
            ZPL_PANIC("Worldgen failure! Block %d is unset!\n", i);
            return;
        }
    }
}

static inline
void world_free_worldgen_data(void) {
    zpl_mfree(world.data);
    zpl_mfree(world.outer_data);
    world.data = NULL;
    world.outer_data = NULL;
}

int32_t world_init(int32_t seed, uint16_t chunk_size, uint16_t chunk_amount) {
    world.is_paused = false;
    world.seed = seed;
    world.chunk_size = chunk_size;
    world.chunk_amount = chunk_amount;

    world.dim = (world.chunk_size * world.chunk_amount);
    world.size = world.dim * world.dim;

    world_configure_tracker();
    world_setup_ecs();
    world_init_worldgen_data();
    world_generate_instance();
    world_init_mapping();
    world_chunk_setup_grid();
    world_free_worldgen_data();

    zpl_printf("[INFO] Created a new server world\n");

    return WORLD_ERROR_NONE;
}

int32_t world_destroy(void) {
    librg_world_destroy(world.tracker);
    ecs_fini(world.ecs);
    zpl_mfree(world.chunk_mapping);
    for (int i = 0; i < zpl_square(world.chunk_amount); i+=1) {
        zpl_mfree(world.block_mapping[i]);
        zpl_mfree(world.outer_block_mapping[i]);
    }
    zpl_mfree(world.block_mapping);
    zpl_mfree(world.outer_block_mapping);
    world_snapshot_destroy(&streamer_snapshot);
    zpl_memset(&world, 0, sizeof(world));
    zpl_printf("[INFO] World was destroyed.\n");
    return WORLD_ERROR_NONE;
}

#define WORLD_LIBRG_BUFSIZ 2000000

static void world_tracker_update(uint8_t ticker, float freq, uint8_t radius) {
    if (world.tracker_update[ticker] > (float)zpl_time_rel()) return;
    world.tracker_update[ticker] = (float)zpl_time_rel() + freq;

    profile(PROF_WORLD_WRITE) {
        ecs_iter_t it = ecs_query_iter(world_ecs(), world.ecs_update);
        static char buffer[WORLD_LIBRG_BUFSIZ] = {0};
        world.active_layer_id = ticker;

        while (ecs_query_next(&it)) {
            ClientInfo *p = ecs_field(&it, ClientInfo, 1);

            for (int i = 0; i < it.count; i++) {
                size_t datalen = WORLD_LIBRG_BUFSIZ;

                if (!p[i].active)
                    continue;

                int32_t result = librg_world_write(world_tracker(), it.entities[i], radius, buffer, &datalen, NULL);

                if (result > 0) {
                    zpl_printf("[info] buffer size was not enough, please increase it by at least: %d\n", result);
                } else if (result < 0) {
                    zpl_printf("[error] an error happened writing the world %d\n", result);
                }

                pkt_send_librg_update((uint64_t)p[i].peer, p[i].view_id, ticker, buffer, datalen);
            }
        }

        // NOTE(zaklaus): clear out our streaming snapshot
        // TODO(zaklaus): move this to zpl
        {
            zpl_array_clear(streamer_snapshot.hashes);
            zpl_array_clear(streamer_snapshot.entries);
        }
    }
}

int32_t world_update() {
    profile (PROF_UPDATE_SYSTEMS) {
        ecs_progress(world.ecs, 0.0f);
    }

    float fast_ms = WORLD_TRACKER_UPDATE_FAST_MS;
    float normal_ms = WORLD_TRACKER_UPDATE_NORMAL_MS;
    float slow_ms = WORLD_TRACKER_UPDATE_SLOW_MS;

    if (game_get_kind() != GAMEKIND_SINGLE) {
        fast_ms = WORLD_TRACKER_UPDATE_MP_FAST_MS;
        normal_ms = WORLD_TRACKER_UPDATE_MP_NORMAL_MS;
        slow_ms = WORLD_TRACKER_UPDATE_MP_SLOW_MS;
    }

    world_tracker_update(0, fast_ms, 1);
    world_tracker_update(1, normal_ms, 2);
    world_tracker_update(2, slow_ms, 3);

    entity_update_action_timers();
    debug_replay_update();
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

uint32_t world_buf(block_id const **ptr, uint32_t *width) {
    ZPL_ASSERT_NOT_NULL(world.data);
    ZPL_ASSERT_NOT_NULL(ptr);
    *ptr = world.data;
    if (width) *width = world.dim;
    return world.size;
}

uint32_t world_seed(void) {
    return world.seed;
}

ecs_world_t * world_ecs() {
    if (world.ecs_stage != NULL) {
        return world.ecs_stage;
    }
    return world.ecs;
}

ecs_query_t *world_ecs_clientinfo(void) {
    return world.ecs_clientinfo;
}

void world_set_stage(ecs_world_t *ecs) {
    world.ecs_stage = ecs;
}

librg_world *world_tracker() {
    return world.tracker;
}

void world_pause(void) {
    ecs_set_time_scale(world.ecs, 0.0f);
    world.is_paused = true;
}

void world_resume(void) {
    ecs_set_time_scale(world.ecs, 1.0f);
    world.is_paused = false;
}

bool world_is_paused(void) {
    return world.is_paused;
}

void world_step(float step_size) {
    world_resume();
    ecs_progress(world.ecs, step_size);
    world_pause();
}

uint16_t world_chunk_size(void) {
    return world.chunk_size;
}

uint16_t world_chunk_amount(void) {
    return world.chunk_amount;
}

uint16_t world_dim(void) {
    return WORLD_BLOCK_SIZE * world.chunk_size * world.chunk_amount;
}

ecs_entity_t world_chunk_mapping(librg_chunk id) {
    ZPL_ASSERT(id >= 0 && id < zpl_square(world.chunk_amount));
    return world.chunk_mapping[id];
}

world_block_lookup world_block_from_realpos(float x, float y) {
    x = zpl_clamp(x, 0, world_dim()-1);
    y = zpl_clamp(y, 0, world_dim()-1);
    librg_chunk chunk_id = librg_chunk_from_realpos(world.tracker, x, y, 0);
    ecs_entity_t e = world.chunk_mapping[chunk_id];
    int32_t size = world.chunk_size * WORLD_BLOCK_SIZE;
    int16_t chunk_x, chunk_y;
    librg_chunk_to_chunkpos(world.tracker, chunk_id, &chunk_x, &chunk_y, NULL);

    // NOTE(zaklaus): pos relative to chunk
    float chx = x - chunk_x * size;
    float chy = y - chunk_y * size;

    uint16_t bx = (uint16_t)chx / WORLD_BLOCK_SIZE;
    uint16_t by = (uint16_t)chy / WORLD_BLOCK_SIZE;
    uint16_t block_idx = (by*world.chunk_size)+bx;
    block_id bid = world.outer_block_mapping[chunk_id][block_idx];
    bool is_outer = true;
    if (bid == 0) {
        bid = world.block_mapping[chunk_id][block_idx];
        is_outer = false;
    }

    // NOTE(zaklaus): pos relative to block's center
    float box = chx - bx * WORLD_BLOCK_SIZE - WORLD_BLOCK_SIZE/2.0f;
    float boy = chy - by * WORLD_BLOCK_SIZE - WORLD_BLOCK_SIZE/2.0f;

    world_block_lookup lookup = {
        .id = block_idx,
        .bid = bid,
        .chunk_id = chunk_id,
        .chunk_e = e,
        .ox = box,
        .oy = boy,
        .is_outer = is_outer,
    };

    return lookup;
}

void world_chunk_destroy_block(float x, float y, bool drop_item) {
    world_block_lookup l = world_block_from_realpos(x, y);
    if (blocks_get_flags(l.bid) & BLOCK_FLAG_ESSENTIAL) return;
    world_chunk_replace_block(l.chunk_id, l.id, 0);

    if (l.is_outer && l.bid > 0 && drop_item) {
        asset_id item_asset = blocks_get_asset(l.bid);
        if (item_find(item_asset) == ASSET_INVALID) return;
        uint64_t e = item_spawn(item_asset, 1);

        Position *dest = ecs_get_mut(world_ecs(), e, Position);
        dest->x = x;
        dest->y = y;
    }
}

world_block_lookup world_block_from_index(int64_t id, uint16_t block_idx) {
    block_id bid = world.outer_block_mapping[id][block_idx];
    if (bid == 0) {
        bid = world.block_mapping[id][block_idx];
    }

    world_block_lookup lookup = {
        .id = block_idx,
        .bid = bid,
        .chunk_id = id,
        .chunk_e = world.chunk_mapping[id],
    };

    return lookup;
}

int64_t world_chunk_from_realpos(float x, float y) {
    librg_chunk chunk_id = librg_chunk_from_realpos(world.tracker, x, y, 0);
    return world.chunk_mapping[chunk_id];
}

int64_t world_chunk_from_entity(ecs_entity_t id) {
    return librg_entity_chunk_get(world.tracker, id);
}

void world_chunk_replace_worldgen_block(int64_t id, uint16_t block_idx, block_id bid) {
    ZPL_ASSERT(block_idx >= 0 && block_idx < zpl_square(world.chunk_size));
    world.block_mapping[id][block_idx] = bid;
    world_chunk_mark_dirty(world.chunk_mapping[id]);
}

void world_chunk_replace_block(int64_t id, uint16_t block_idx, block_id bid) {
    ZPL_ASSERT(block_idx >= 0 && block_idx < zpl_square(world.chunk_size));
    world.outer_block_mapping[id][block_idx] = bid;
    world_chunk_mark_dirty(world.chunk_mapping[id]);
}

bool world_chunk_place_block(int64_t id, uint16_t block_idx, block_id bid) {
    ZPL_ASSERT(block_idx >= 0 && block_idx < zpl_square(world.chunk_size));
    if (world.outer_block_mapping[id][block_idx] != 0 && bid != 0) return false;
    world.outer_block_mapping[id][block_idx] = bid;
    world_chunk_mark_dirty(world.chunk_mapping[id]);
    return true;
}

block_id *world_chunk_get_blocks(int64_t id) {
    return world.block_mapping[id];
}

void world_chunk_mark_dirty(ecs_entity_t e) {
    bool was_added=false;
    Chunk *chunk = ecs_get_mut(world_ecs(), e, Chunk);
    if (chunk) chunk->is_dirty = true;
}

bool world_chunk_is_dirty(ecs_entity_t e) {
    bool was_added=false;
    Chunk *chunk = ecs_get_mut(world_ecs(), e, Chunk);
    if (chunk) return chunk->is_dirty;
    return false;
}

int64_t *world_chunk_fetch_entities(librg_chunk chunk_id, size_t *ents_len) {
    ZPL_ASSERT_NOT_NULL(ents_len);
    static int64_t ents[UINT16_MAX];
    *ents_len = UINT16_MAX;
    librg_world_fetch_chunk(world.tracker, chunk_id, ents, ents_len);
    return ents;
}

int64_t *world_chunk_fetch_entities_realpos(float x, float y, size_t *ents_len) {
    return world_chunk_fetch_entities(librg_chunk_from_realpos(world.tracker, x, y, 0), ents_len);
}

int64_t *world_chunk_query_entities(int64_t e, size_t *ents_len, int8_t radius) {
    ZPL_ASSERT_NOT_NULL(ents_len);
    static int64_t ents[UINT16_MAX];
    *ents_len = UINT16_MAX;
    librg_entity_radius_set(world.tracker, e, radius);
    librg_world_query(world.tracker, e, radius, ents, ents_len);
    return ents;
}

bool world_entity_valid(ecs_entity_t e) {
    if (!e) return false;
    return ecs_is_alive(world_ecs(), e);
}

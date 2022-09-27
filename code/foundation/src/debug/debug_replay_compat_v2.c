typedef struct {
    float x;
    float y;
    uint8_t use;
    uint8_t sprint;
} pkt_send_keystate_v2;

typedef struct {
    replay_kind kind;
    pkt_send_keystate_v2 pkt;
    uint64_t delay;
} replay_record_v2;

void debug_replay_load_record_v2(replay_record *rec, void const *buf) {
    replay_record_v2 v2_rec;
    zpl_memcopy(&v2_rec, buf, sizeof(replay_record_v2));
    
    pkt_send_keystate pkt = {
        .x = v2_rec.pkt.x,
        .y = v2_rec.pkt.y,
        .sprint = v2_rec.pkt.sprint,
        .use = v2_rec.pkt.use,
    };
    
    rec->kind = v2_rec.kind;
    rec->pkt = pkt;
    rec->delay = (double)v2_rec.delay;
}
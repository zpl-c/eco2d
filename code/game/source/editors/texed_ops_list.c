
#define PARAM(k,n,v) { .kind = k, .name = n, .str = v }

static td_op default_ops[] = {
    {
        OP(TOP_CLEAR),
        .num_params = 1,
        .params = (td_param[]) {
            PARAM(TPARAM_COLOR, "color", "ffffffff"),
        }
    },
    {
        OP(TOP_DRAW_RECT),
        .num_params = 5,
        .params = (td_param[]) {
            PARAM(TPARAM_INT, "x", "0"),
            PARAM(TPARAM_INT, "y", "0"),
            PARAM(TPARAM_INT, "w", "10"),
            PARAM(TPARAM_INT, "h", "10"),
            PARAM(TPARAM_COLOR, "color", "0"),
        }
    },
    {
        OP(TOP_DRAW_LINE),
        .num_params = 5,
        .params = (td_param[]) {
            PARAM(TPARAM_INT, "x1", "0"),
            PARAM(TPARAM_INT, "y1", "0"),
            PARAM(TPARAM_INT, "x2", "64"),
            PARAM(TPARAM_INT, "y2", "64"),
            PARAM(TPARAM_COLOR, "color", "0"),
        }
    },
    {
        OP(TOP_DITHER),
        .num_params = 4,
        .params = (td_param[]) {
            PARAM(TPARAM_INT, "r_bpp", "8"),
            PARAM(TPARAM_INT, "g_bpp", "8"),
            PARAM(TPARAM_INT, "b_bpp", "8"),
            PARAM(TPARAM_INT, "a_bpp", "8"),
        }
    },
    {
        OP(TOP_LOAD_IMAGE),
        .num_params = 6,
        .params = (td_param[]) {
            PARAM(TPARAM_STRING, "src", "art/natives/test.png"),
            PARAM(TPARAM_INT, "x", "0"),
            PARAM(TPARAM_INT, "y", "0"),
            PARAM(TPARAM_INT, "w", "-1"),
            PARAM(TPARAM_INT, "h", "-1"),
            PARAM(TPARAM_COLOR, "tint", "ffffffff"),
        }
    }
};

// NOTE(zaklaus): IMPORTANT !! keep these in sync
static char const *add_op_list = "CLEAR SOLID;DRAW RECTANGLE;PLOT LINE;DITHER;LOAD IMAGE";

#define DEF_OPS_LEN (int)(sizeof(default_ops) / (sizeof(default_ops[0])))

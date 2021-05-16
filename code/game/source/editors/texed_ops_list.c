
#define PARAM(k,n,v) { .kind = k, .name = n, .str = v }
#define PARAM_DEF_COLOR "000000ff"

static td_op default_ops[] = {
    {
        OP(TOP_NEW_IMAGE),
        .num_params = 3,
        .params = (td_param[]) {
            PARAM(TPARAM_COORD, "w", "64"),
            PARAM(TPARAM_COORD, "h", "64"),
            PARAM(TPARAM_COLOR, "color", "ffffffff"),
        }
    },
    {
        OP(TOP_DRAW_RECT),
        .num_params = 5,
        .params = (td_param[]) {
            PARAM(TPARAM_COORD, "x", "0"),
            PARAM(TPARAM_COORD, "y", "0"),
            PARAM(TPARAM_COORD, "w", "10"),
            PARAM(TPARAM_COORD, "h", "10"),
            PARAM(TPARAM_COLOR, "color", PARAM_DEF_COLOR),
        }
    },
    {
        OP(TOP_DRAW_LINE),
        .num_params = 5,
        .params = (td_param[]) {
            PARAM(TPARAM_COORD, "x1", "0"),
            PARAM(TPARAM_COORD, "y1", "0"),
            PARAM(TPARAM_COORD, "x2", "64"),
            PARAM(TPARAM_COORD, "y2", "64"),
            PARAM(TPARAM_COLOR, "color", PARAM_DEF_COLOR),
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
            PARAM(TPARAM_STRING, "src", "samples/test.png"),
            PARAM(TPARAM_COORD, "x", "0"),
            PARAM(TPARAM_COORD, "y", "0"),
            PARAM(TPARAM_COORD, "w", "-1"),
            PARAM(TPARAM_COORD, "h", "-1"),
            PARAM(TPARAM_COLOR, "tint", "ffffffff"),
        }
    },
    {
        OP(TOP_DRAW_TEXT),
        .num_params = 5,
        .params = (td_param[]) {
            PARAM(TPARAM_STRING, "text", "hello world"),
            PARAM(TPARAM_COORD, "x", "0"),
            PARAM(TPARAM_COORD, "y", "0"),
            PARAM(TPARAM_COORD, "size", "16"),
            PARAM(TPARAM_COLOR, "color", PARAM_DEF_COLOR),
        }
    },
    {
        OP(TOP_RESIZE_IMAGE),
        .num_params = 3,
        .params = (td_param[]) {
            PARAM(TPARAM_COORD, "w", "64"),
            PARAM(TPARAM_COORD, "h", "64"),
            PARAM(TPARAM_COORD, "mode (0=nearest,1=bicubic)", "0"),
        }
    }
};

#define DEF_OPS_LEN (int)(sizeof(default_ops) / (sizeof(default_ops[0])))

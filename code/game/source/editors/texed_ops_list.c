
static td_op default_ops[] = {
    {
        OP(TOP_CLEAR),
        .num_params = 1,
        .params = (td_param[]) {
            {
                .kind = TPARAM_COLOR,
                .name = "color",
                .str = "ffffffff"
            }
        }
    },
    {
        OP(TOP_DRAW_RECT),
        .num_params = 5,
        .params = (td_param[]) {
            {
                .kind = TPARAM_INT,
                .name = "x",
                .str = "0"
            },
            {
                .kind = TPARAM_INT,
                .name = "y",
                .str = "0"
            },
            {
                .kind = TPARAM_INT,
                .name = "w",
                .str = "10"
            },
            {
                .kind = TPARAM_INT,
                .name = "h",
                .str = "10"
            },
            {
                .kind = TPARAM_COLOR,
                .name = "color",
                .str = "ff0000ff"
            },
        }
    },
    {
        OP(TOP_DRAW_LINE),
        .num_params = 5,
        .params = (td_param[]) {
            {
                .kind = TPARAM_INT,
                .name = "x1",
                .str = "0"
            },
            {
                .kind = TPARAM_INT,
                .name = "y1",
                .str = "0"
            },
            {
                .kind = TPARAM_INT,
                .name = "x2",
                .str = "64"
            },
            {
                .kind = TPARAM_INT,
                .name = "y2",
                .str = "64"
            },
            {
                .kind = TPARAM_COLOR,
                .name = "color",
                .str = "ffff00ff"
            },
        }
    },
    {
        OP(TOP_DITHER),
        .num_params = 4,
        .params = (td_param[]) {
            {
                .kind = TPARAM_INT,
                .name = "r_bpp",
                .str = "8",
            },
            {
                .kind = TPARAM_INT,
                .name = "g_bpp",
                .str = "8",
            },
            {
                .kind = TPARAM_INT,
                .name = "b_bpp",
                .str = "8",
            },
            {
                .kind = TPARAM_INT,
                .name = "a_bpp",
                .str = "8",
            },
        }
    },
    {
        OP(TOP_LOAD_IMAGE),
        .num_params = 6,
        .params = (td_param[]) {
            {
                .kind = TPARAM_STRING,
                .name = "src",
                .str = ""
            },
            {
                .kind = TPARAM_INT,
                .name = "x",
                .str = "0"
            },
            {
                .kind = TPARAM_INT,
                .name = "y",
                .str = "0"
            },
            {
                .kind = TPARAM_INT,
                .name = "w",
                .str = "-1"
            },
            {
                .kind = TPARAM_INT,
                .name = "h",
                .str = "-1"
            },
            {
                .kind = TPARAM_COLOR,
                .name = "tint",
                .str = "ffffffff",
            },
        }
    }
};

// NOTE(zaklaus): IMPORTANT !! keep these in sync
static char const *add_op_list = "CLEAR SOLID;DRAW RECTANGLE;PLOT LINE;DITHER;LOAD IMAGE";

#define DEF_OPS_LEN (int)(sizeof(default_ops) / (sizeof(default_ops[0])))

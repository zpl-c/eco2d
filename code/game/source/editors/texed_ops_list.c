
#define PARAM(k,n,v) { .kind = k, .name = n, .str = v }
#define PARAMS(n) .num_params = n, .params = (td_param[])
#define PARAM_DEF_COLOR "000000ff"

static td_op default_ops[] = {
    {
        OP(TOP_NEW_IMAGE),
        .is_locked = true,
        PARAMS(3) {
            PARAM(TPARAM_COORD, "w", "64"),
            PARAM(TPARAM_COORD, "h", "64"),
            PARAM(TPARAM_COLOR, "color", "ffffffff"),
        }
    },{
        OP(TOP_PUSH_IMAGE),
        PARAMS(3) {
            PARAM(TPARAM_COORD, "w", "64"),
            PARAM(TPARAM_COORD, "h", "64"),
            PARAM(TPARAM_COLOR, "color", "ffffffff"),
        }
    },{
        OP(TOP_POP_IMAGE),
        PARAMS(5) {
            PARAM(TPARAM_COORD, "x", "0"),
            PARAM(TPARAM_COORD, "y", "0"),
            PARAM(TPARAM_COORD, "w", "0"),
            PARAM(TPARAM_COORD, "h", "0"),
            PARAM(TPARAM_COLOR, "tint", "ffffffff"),
        }
    },{
        OP(TOP_IMAGE_ALPHA_MASK),
    },{
        OP(TOP_IMAGE_ALPHA_MASK_CLEAR),
        PARAMS(2) {
            PARAM(TPARAM_COLOR, "color", "ffffffff"),
            PARAM(TPARAM_FLOAT, "threshold", "1.0"),
        }
    },{
        OP(TOP_DRAW_RECT),
        PARAMS(5) {
            PARAM(TPARAM_COORD, "x", "0"),
            PARAM(TPARAM_COORD, "y", "0"),
            PARAM(TPARAM_COORD, "w", "10"),
            PARAM(TPARAM_COORD, "h", "10"),
            PARAM(TPARAM_COLOR, "color", PARAM_DEF_COLOR),
        }
    },{
        OP(TOP_DRAW_LINE),
        PARAMS(5) {
            PARAM(TPARAM_COORD, "x1", "0"),
            PARAM(TPARAM_COORD, "y1", "0"),
            PARAM(TPARAM_COORD, "x2", "64"),
            PARAM(TPARAM_COORD, "y2", "64"),
            PARAM(TPARAM_COLOR, "color", PARAM_DEF_COLOR),
        }
    },{
        OP(TOP_DRAW_IMAGE),
        PARAMS(8) {
            PARAM(TPARAM_STRING, "src", "samples/test.png"),
            PARAM(TPARAM_COORD, "x", "0"),
            PARAM(TPARAM_COORD, "y", "0"),
            PARAM(TPARAM_COORD, "w", "0"),
            PARAM(TPARAM_COORD, "h", "0"),
            PARAM(TPARAM_COLOR, "tint", "ffffffff"),
            PARAM(TPARAM_INT, "flip?", "0"),
            PARAM(TPARAM_INT, "rotate?", "0"),
        }
    },{
        OP(TOP_DRAW_TEXT),
        PARAMS(5) {
            PARAM(TPARAM_STRING, "text", "hello world"),
            PARAM(TPARAM_COORD, "x", "0"),
            PARAM(TPARAM_COORD, "y", "0"),
            PARAM(TPARAM_COORD, "size", "16"),
            PARAM(TPARAM_COLOR, "color", PARAM_DEF_COLOR),
        }
    },{
        OP(TOP_DITHER),
        PARAMS(4) {
            PARAM(TPARAM_INT, "r_bpp", "4"),
            PARAM(TPARAM_INT, "g_bpp", "4"),
            PARAM(TPARAM_INT, "b_bpp", "4"),
            PARAM(TPARAM_INT, "a_bpp", "4"),
        }
    },{
        OP(TOP_RESIZE_IMAGE),
        PARAMS(3) {
            PARAM(TPARAM_COORD, "w", "64"),
            PARAM(TPARAM_COORD, "h", "64"),
            PARAM(TPARAM_COORD, "mode (0=nearest,1=bicubic)", "0"),
        }
    },{
        OP(TOP_COLOR_TWEAKS),
        PARAMS(5) {
            PARAM(TPARAM_SLIDER, "contrast", "0.5"),
            PARAM(TPARAM_SLIDER, "brightness", "0.5"),
            PARAM(TPARAM_COLOR, "tint", "FFFFFFFF"),
            PARAM(TPARAM_INT, "invert?", "0"),
            PARAM(TPARAM_INT, "grayscale?", "0"),
        }
    },{
        OP(TOP_COLOR_REPLACE),
        PARAMS(2) {
            PARAM(TPARAM_COLOR, "original", "FFFFFFFF"),
            PARAM(TPARAM_COLOR, "new", "FF0000FF"),
        }
    },{
        OP(TOP_IMAGE_GRAD_V),
        PARAMS(2) {
            PARAM(TPARAM_COLOR, "top", "ffffffff"),
            PARAM(TPARAM_COLOR, "bottom", "00000000"),
        }
    },{
        OP(TOP_IMAGE_GRAD_H),
        PARAMS(2) {
            PARAM(TPARAM_COLOR, "left", "ffffffff"),
            PARAM(TPARAM_COLOR, "right", "00000000"),
        }
    },{
        OP(TOP_IMAGE_GRAD_RAD),
        PARAMS(3) {
            PARAM(TPARAM_FLOAT, "density", "0.5"),
            PARAM(TPARAM_COLOR, "inner", "ffffffff"),
            PARAM(TPARAM_COLOR, "outer", "00000000"),
        }
    },{
        OP(TOP_IMAGE_CHECKED),
        PARAMS(4) {
            PARAM(TPARAM_COORD, "checks_x", "16"),
            PARAM(TPARAM_COORD, "checks_y", "16"),
            PARAM(TPARAM_COLOR, "color1", "ffffffff"),
            PARAM(TPARAM_COLOR, "color2", "00000000"),
        }
    },{
        OP(TOP_IMAGE_NOISE_WHITE),
        PARAMS(2) {
            PARAM(TPARAM_COORD, "seed", "1"),
            PARAM(TPARAM_FLOAT, "factor", "0.5"),
        }
    },{
        OP(TOP_IMAGE_NOISE_PERLIN),
        PARAMS(3) {
            PARAM(TPARAM_COORD, "offset_x", "0"),
            PARAM(TPARAM_COORD, "offset_y", "0"),
            PARAM(TPARAM_FLOAT, "scale", "1.0"),
        }
    },{
        OP(TOP_IMAGE_CELLULAR),
        PARAMS(2) {
            PARAM(TPARAM_COORD, "seed", "1"),
            PARAM(TPARAM_COORD, "tile_size", "16"),
        }
    }
};

#define DEF_OPS_LEN (int)(sizeof(default_ops) / (sizeof(default_ops[0])))

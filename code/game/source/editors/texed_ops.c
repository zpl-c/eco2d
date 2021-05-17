static inline
float texed_map_value(float v, float min, float max);

void texed_process_ops(void) {
    for (int i = 0; i <= ctx.img_pos; i+=1)
        UnloadImage(ctx.img[i]);
    ctx.img_pos = -1;
    
    for (int i = 0; i < zpl_array_count(ctx.ops); i += 1) {
        td_op *op = &ctx.ops[i];
        if (op->is_hidden) continue;
        zpl_printf("processing op: %s ... \n", op->name);
        
        switch (op->kind) {
            case TOP_PUSH_IMAGE:
            case TOP_NEW_IMAGE: {
                texed_img_push(op->params[0].i32, op->params[1].i32, op->params[2].color);
            }break;
            case TOP_POP_IMAGE: {
                texed_img_pop(op->params[0].i32,
                              op->params[1].i32,
                              op->params[2].i32,
                              op->params[3].i32,
                              op->params[4].color);
            }break;
            case TOP_DRAW_RECT: {
                ImageDrawRectangle(&ctx.img[ctx.img_pos], 
                                   op->params[0].i32,
                                   op->params[1].i32,
                                   op->params[2].i32,
                                   op->params[3].i32,
                                   op->params[4].color);
            }break;
            case TOP_DRAW_LINE: {
                ImageDrawLine(&ctx.img[ctx.img_pos], 
                              op->params[0].i32,
                              op->params[1].i32,
                              op->params[2].i32,
                              op->params[3].i32,
                              op->params[4].color);
            }break;
            case TOP_DITHER: {
                ImageDither(&ctx.img[ctx.img_pos],
                            op->params[0].i32,
                            op->params[1].i32,
                            op->params[2].i32,
                            op->params[3].i32);
            }break;
            case TOP_DRAW_IMAGE: {
                char const *str = zpl_bprintf("art/%s", op->params[0].str);
                if (FileExists(str)) {
                    Image img = LoadImage(str);
                    int x = op->params[1].i32;
                    int y = op->params[2].i32;
                    int w = op->params[3].i32;
                    int h = op->params[4].i32;
                    int flip = op->params[5].i32;
                    int rotate = op->params[6].i32;
                    
                    if (w != 0 || h != 0) {
                        ImageResize(&img, w != 0 ? w : img.width, h != 0 ? h : img.height);
                    }
                    
                    if (flip == 1) {
                        ImageFlipVertical(&img);
                    } else if (flip == 2) {
                        ImageFlipHorizontal(&img);
                    }
                    
                    if (rotate == 1) {
                        ImageRotateCW(&img);
                    } else if (rotate == 2) {
                        ImageRotateCCW(&img);
                    }
                    
                    ImageDraw(&ctx.img[ctx.img_pos], img, 
                              (Rectangle){0.0f, 0.0f, img.width, img.height},
                              (Rectangle){x, y, img.width, img.height},
                              op->params[5].color);
                    
                    UnloadImage(img);
                } else {
                    zpl_printf("TOP_LOAD_IMAGE: src %s not found!\n", str);
                }
            }break;
            case TOP_DRAW_TEXT: {
                char const *str = op->params[0].str;
                int x = op->params[1].i32;
                int y = op->params[2].i32;
                int size = op->params[3].i32;
                Color color = op->params[4].color;
                ImageDrawText(&ctx.img[ctx.img_pos], str, x, y, size, color);
            }break;
            case TOP_RESIZE_IMAGE: {
                if (ctx.img[ctx.img_pos].width == 0) break;
                int w = op->params[0].i32;
                int h = op->params[1].i32;
                int mode = op->params[2].i32;
                if (mode) {
                    ImageResize(&ctx.img[ctx.img_pos], w, h);
                } else {
                    ImageResizeNN(&ctx.img[ctx.img_pos], w, h);
                }
            }break;
            case TOP_COLOR_TWEAKS: {
                ImageColorContrast(&ctx.img[ctx.img_pos], texed_map_value(op->params[0].flt, -100.0f, 100.0f));
                ImageColorBrightness(&ctx.img[ctx.img_pos], (int)texed_map_value(op->params[1].flt, -255.0f, 255.0f));
                ImageColorTint(&ctx.img[ctx.img_pos], op->params[2].color);
                
                if (op->params[3].i32) {
                    ImageColorInvert(&ctx.img[ctx.img_pos]);
                }
                if (op->params[4].i32) {
                    ImageColorGrayscale(&ctx.img[ctx.img_pos]);
                }
            }break;
            default: {
                zpl_printf("%s\n", "unsupported op!");
            }break;
        }
    }
}

void texed_process_params(void) {
    for (int i = 0; i < zpl_array_count(ctx.ops); i += 1) {
        td_op *op = &ctx.ops[i];
        
        for (int j = 0; j < op->num_params; j += 1) {
            td_param *p = &op->params[j];
            
            switch (p->kind) {
                case TPARAM_SLIDER:
                case TPARAM_FLOAT: {
                    p->flt = (float)zpl_str_to_f64(p->str, NULL);
                }break;
                case TPARAM_INT:
                case TPARAM_COORD: {
                    p->i32 = (int32_t)zpl_str_to_i64(p->str, NULL, 10);
                }break;
                case TPARAM_COLOR: {
                    uint32_t color = (uint32_t)zpl_str_to_u64(p->str, NULL, 16);
                    p->color = GetColor(color);
                }break;
                case TPARAM_STRING: {
                    // NOTE(zaklaus): no-op
                }break;
                default: {
                    zpl_printf("%s\n", "unsupported param!");
                }break;
            }
        }
    }
}

static inline
float texed_map_value(float v, float min, float max) {
    float slope = max-min;
    return min + zpl_round(slope * v);
}
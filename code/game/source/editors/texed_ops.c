
void texed_process_ops(void) {
    for (int i = 0; i < zpl_array_count(ctx.ops); i += 1) {
        td_op *op = &ctx.ops[i];
        if (op->is_hidden) continue;
        zpl_printf("processing op: %s ... \n", op->name);
        
        switch (op->kind) {
            case TOP_CLEAR: {
                ImageClearBackground(&ctx.img, op->params[0].color);
            }break;
            case TOP_DRAW_RECT: {
                ImageDrawRectangle(&ctx.img, 
                                   op->params[0].i32,
                                   op->params[1].i32,
                                   op->params[2].i32,
                                   op->params[3].i32,
                                   op->params[4].color);
            }break;
            case TOP_DRAW_LINE: {
                ImageDrawLine(&ctx.img, 
                              op->params[0].i32,
                              op->params[1].i32,
                              op->params[2].i32,
                              op->params[3].i32,
                              op->params[4].color);
            }break;
            case TOP_DITHER: {
                ImageDither(&ctx.img,
                            op->params[0].i32,
                            op->params[1].i32,
                            op->params[2].i32,
                            op->params[3].i32);
            }break;
            case TOP_LOAD_IMAGE: {
                if (FileExists(op->params[0].str)) {
                    Image img = LoadImage(op->params[0].str);
                    int x = op->params[1].i32;
                    int y = op->params[2].i32;
                    int w = op->params[3].i32;
                    int h = op->params[4].i32;
                    
                    if (w != -1 || h != -1) {
                        ImageResize(&img, w != -1 ? w : img.width, h != -1 ? h : img.height);
                    }
                    
                    ImageDraw(&ctx.img, img, 
                              (Rectangle){0.0f, 0.0f, img.width, img.height},
                              (Rectangle){x, y, img.width, img.height},
                              op->params[5].color);
                    
                    UnloadImage(img);
                } else {
                    zpl_printf("TOP_LOAD_IMAGE: src %s not found!\n", op->params[0].str);
                }
            }break;
            case TOP_DRAW_TEXT: {
                char const *str = op->params[0].str;
                int x = op->params[1].i32;
                int y = op->params[2].i32;
                int size = op->params[3].i32;
                Color color = op->params[4].color;
                ImageDrawText(&ctx.img, str, x, y, size, color);
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
        if (op->is_hidden) continue;
        
        for (int j = 0; j < op->num_params; j += 1) {
            td_param *p = &op->params[j];
            
            switch (p->kind) {
                case TPARAM_FLOAT: {
                    p->flt = (float)zpl_str_to_f64(p->str, NULL);
                }break;
                case TPARAM_INT: {
                    p->u32 = (uint32_t)zpl_str_to_i64(p->str, NULL, 10);
                }break;
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

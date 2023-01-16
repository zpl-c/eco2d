// Tooltip system with multilevel modal support

typedef struct _tooltip {
	const char *name;
	const char *content;
	const char **links;
} tooltip;

static tooltip *tooltips = 0;

//~ registration

void tooltip_register(tooltip desc) {
	if (!tooltips) {
		zpl_array_init(tooltips, zpl_heap());
	}

	desc.links = 0;
	zpl_array_append(tooltips, desc);
}

void tooltip_destroy_all(void) {
	if (!tooltips) return;

	for (zpl_isize i = 0; i < zpl_array_count(tooltips); ++i) {
		tooltip *tp = (tooltips + i);

		if (tp->links) {
			zpl_array_free(tp->links);
		}
	}

	zpl_array_free(tooltips);
}

void tooltip_build_links(void) {
	for (zpl_isize i = 0; i < zpl_array_count(tooltips); ++i) {
		tooltip *tp = (tooltips + i);

		for (zpl_isize j = 0; j < zpl_array_count(tooltips); ++j) {
			tooltip *linked_tp = (tooltips + j);
			if (tp == linked_tp)
				continue;

			if (strstr(tp->content, linked_tp->name)) {
				if (!tp->links) {
					zpl_array_init(tp->links, zpl_heap());
				}

				zpl_array_append(tp->links, linked_tp->name);
			}
		}
	}
}

void tooltip_register_defaults(void) {
	// test
	tooltip_register( (tooltip) { .name = "ASSET_WOOD", .content = "Used as a building material or fuel for the ASSET_FURNACE." } );
	tooltip_register( (tooltip) { .name = "ASSET_FURNACE", .content = "Producer used to smelt ASSET_IRON_ORE into ASSET_IRON_INGOT." } );
	tooltip_register( (tooltip) { .name = "ASSET_IRON_ORE", .content = "Natural resource that can be smelted in ASSET_FURNACE." } );
	tooltip_register( (tooltip) { .name = "ASSET_IRON_INGOT", .content = "Used as a building material. It is smelted from ASSET_IRON_ORE" } );
	tooltip_register( (tooltip) { .name = "ASSET_SCREWS", .content = "Used as a building material. It is crafted from ASSET_IRON_PLATES" } );
}

//~ rendering

#define TOOLTIP_MOUSE_DIST 400.0f

typedef struct _tooltip_node {
	float xpos, ypos;
	tooltip *desc;
	struct _tooltip_node *next;
} tooltip_node;

static tooltip_node main_tooltip = { 0 };
static bool tooltip__should_stay_open = false;

tooltip *tooltip_find_desc(const char *name) {
	for (zpl_isize i = 0; i < zpl_array_count(tooltips); ++i) {
		tooltip *tp = (tooltips + i);

		if (!strcmp(tp->name, name)) 
			return tp;
	}

	return 0;
}

const char *tooltip_find_desc_contents(const char *name) {
	for (zpl_isize i = 0; i < zpl_array_count(tooltips); ++i) {
		tooltip *tp = (tooltips + i);

		if (!strcmp(tp->name, name)) 
			return tp->content;
	}

	return 0;
}         

void tooltip_clear(void);

void tooltip_show(const char* name, float xpos, float ypos) {
	if (!tooltips) return;

	tooltip *desc = tooltip_find_desc(name);
	if (!name) return;

	tooltip_clear();

	main_tooltip = (tooltip_node) {
		.xpos = xpos,
		.ypos = ypos,
		.desc = desc,
		.next = 0
	};
}

void tooltip_show_cursor(const char* name) {
	Vector2 mpos = GetMousePosition();
	tooltip_show(name, mpos.x + 15, mpos.y + 15);
}

void tooltip__clear_node(tooltip_node *node) {
	if (node->next) {
		tooltip__clear_node(node->next);
		zpl_mfree(node->next);
	}
}

void tooltip_clear(void) {
	tooltip__clear_node(&main_tooltip);
	main_tooltip = (tooltip_node) {0};
}

inline void tooltip_draw_contents(tooltip *desc) {
	if (!desc) return;

	nk_layout_row_dynamic(game_ui, 0, 1); 
	nk_label_wrap(game_ui, desc->content);
}

void tooltip__draw_node(tooltip_node *node) {
	if (!node) return;

	tooltip *desc = node->desc;
	Vector2 mpos = GetMousePosition();

	if (nk_begin_titled(game_ui, zpl_bprintf("%d%s", (int)node->xpos, desc->name), desc->name, nk_rect(node->xpos, node->ypos, 400, 3200), 
	             NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_DYNAMIC | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
		tooltip_draw_contents(desc);

		if (desc->links) {
			nk_label(game_ui, "See Also:", NK_TEXT_LEFT);
			nk_layout_row_dynamic(game_ui, 10, 2);

			for (zpl_isize i = 0; i < zpl_array_count(desc->links); ++i) {
				// todo styling
				if (nk_button_label(game_ui, desc->links[i])) {
					if (node->next) tooltip__clear_node(node->next);
					if (!node->next) node->next = zpl_malloc(sizeof(tooltip_node));
					*node->next = (tooltip_node) {
						.xpos = mpos.x+15,
						.ypos = mpos.y+15,
						.desc = tooltip_find_desc(desc->links[i]),
						.next = 0
					};
				}
			}
		}

		// suggest closing tooltip 
		struct  nk_vec2 wpos = nk_window_get_position(game_ui);
		struct  nk_vec2 wsize = nk_window_get_content_region_size(game_ui);
		Vector2 tp_pos = (Vector2) { .x = wpos.x + wsize.x/2.0f, .y = wpos.y };
		if (Vector2Distance(mpos, tp_pos) <= TOOLTIP_MOUSE_DIST) {
			tooltip__should_stay_open = true;
		}

#if 0
		{
			DrawCircleV(tp_pos, TOOLTIP_MOUSE_DIST, BLUE);
		}
#endif

		nk_end(game_ui);

		// draw nested tooltip
		if (node->next) {
			tooltip__draw_node(node->next);
		}
	}
}

void tooltip_draw(void) {
	if (!main_tooltip.desc) return;

	tooltip__draw_node(&main_tooltip);

	if (!tooltip__should_stay_open) {
		tooltip_clear();
	}

	tooltip__should_stay_open = false;
}

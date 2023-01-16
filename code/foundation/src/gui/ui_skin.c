enum theme {THEME_BLACK, THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK, THEME_ECO};

static void set_style(struct nk_context *ctx, enum theme theme)
{
	struct nk_color table[NK_COLOR_COUNT];
	if (theme == THEME_WHITE) {
		table[NK_COLOR_TEXT] = nk_rgba(70, 70, 70, 255);
		table[NK_COLOR_WINDOW] = nk_rgba(175, 175, 175, 255);
		table[NK_COLOR_HEADER] = nk_rgba(175, 175, 175, 255);
		table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 255);
		table[NK_COLOR_BUTTON] = nk_rgba(185, 185, 185, 255);
		table[NK_COLOR_BUTTON_HOVER] = nk_rgba(170, 170, 170, 255);
		table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(160, 160, 160, 255);
		table[NK_COLOR_TOGGLE] = nk_rgba(150, 150, 150, 255);
		table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(120, 120, 120, 255);
		table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(175, 175, 175, 255);
		table[NK_COLOR_SELECT] = nk_rgba(190, 190, 190, 255);
		table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(175, 175, 175, 255);
		table[NK_COLOR_SLIDER] = nk_rgba(190, 190, 190, 255);
		table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(80, 80, 80, 255);
		table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(70, 70, 70, 255);
		table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(60, 60, 60, 255);
		table[NK_COLOR_PROPERTY] = nk_rgba(175, 175, 175, 255);
		table[NK_COLOR_EDIT] = nk_rgba(150, 150, 150, 255);
		table[NK_COLOR_EDIT_CURSOR] = nk_rgba(0, 0, 0, 255);
		table[NK_COLOR_COMBO] = nk_rgba(175, 175, 175, 255);
		table[NK_COLOR_CHART] = nk_rgba(160, 160, 160, 255);
		table[NK_COLOR_CHART_COLOR] = nk_rgba(45, 45, 45, 255);
		table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba( 255, 0, 0, 255);
		table[NK_COLOR_SCROLLBAR] = nk_rgba(180, 180, 180, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(140, 140, 140, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(150, 150, 150, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(160, 160, 160, 255);
		table[NK_COLOR_TAB_HEADER] = nk_rgba(180, 180, 180, 255);
		nk_style_from_table(ctx, table);
	} else if (theme == THEME_RED) {
		table[NK_COLOR_TEXT] = nk_rgba(190, 190, 190, 255);
		table[NK_COLOR_WINDOW] = nk_rgba(30, 33, 40, 215);
		table[NK_COLOR_HEADER] = nk_rgba(181, 45, 69, 220);
		table[NK_COLOR_BORDER] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_BUTTON] = nk_rgba(181, 45, 69, 255);
		table[NK_COLOR_BUTTON_HOVER] = nk_rgba(190, 50, 70, 255);
		table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(195, 55, 75, 255);
		table[NK_COLOR_TOGGLE] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 60, 60, 255);
		table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(181, 45, 69, 255);
		table[NK_COLOR_SELECT] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(181, 45, 69, 255);
		table[NK_COLOR_SLIDER] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(181, 45, 69, 255);
		table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(186, 50, 74, 255);
		table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(191, 55, 79, 255);
		table[NK_COLOR_PROPERTY] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_EDIT] = nk_rgba(51, 55, 67, 225);
		table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 190, 255);
		table[NK_COLOR_COMBO] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_CHART] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_CHART_COLOR] = nk_rgba(170, 40, 60, 255);
		table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba( 255, 0, 0, 255);
		table[NK_COLOR_SCROLLBAR] = nk_rgba(30, 33, 40, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
		table[NK_COLOR_TAB_HEADER] = nk_rgba(181, 45, 69, 220);
		nk_style_from_table(ctx, table);
	} else if (theme == THEME_ECO) {
		table[NK_COLOR_TEXT] = nk_rgba(190, 190, 190, 255);
		table[NK_COLOR_WINDOW] = nk_rgba(40, 43, 10, 235);
		table[NK_COLOR_HEADER] = nk_rgba(32, 96, 64 , 220);
		table[NK_COLOR_BORDER] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_BUTTON] = nk_rgba(20, 23, 30, 215);
		table[NK_COLOR_BUTTON_HOVER] = nk_rgba(32, 96, 64 , 235);
		table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(52, 116, 84 , 255);
		table[NK_COLOR_TOGGLE] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 60, 60, 255);
		table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(181, 45, 69, 255);
		table[NK_COLOR_SELECT] = nk_rgba(20, 23, 30, 215);
		table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(181, 45, 69, 255);
		table[NK_COLOR_SLIDER] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(181, 45, 69, 255);
		table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(186, 50, 74, 255);
		table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(191, 55, 79, 255);
		table[NK_COLOR_PROPERTY] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_EDIT] = nk_rgba(51, 55, 67, 225);
		table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 190, 255);
		table[NK_COLOR_COMBO] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_CHART] = nk_rgba(51, 55, 67, 255);
		table[NK_COLOR_CHART_COLOR] = nk_rgba(170, 40, 60, 255);
		table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba( 255, 0, 0, 255);
		table[NK_COLOR_SCROLLBAR] = nk_rgba(30, 33, 40, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
		table[NK_COLOR_TAB_HEADER] = nk_rgba(32, 96, 64 , 220);
		nk_style_from_table(ctx, table);
	} else if (theme == THEME_BLUE) {
		table[NK_COLOR_TEXT] = nk_rgba(20, 20, 20, 255);
		table[NK_COLOR_WINDOW] = nk_rgba(202, 212, 214, 215);
		table[NK_COLOR_HEADER] = nk_rgba(137, 182, 224, 220);
		table[NK_COLOR_BORDER] = nk_rgba(140, 159, 173, 255);
		table[NK_COLOR_BUTTON] = nk_rgba(137, 182, 224, 255);
		table[NK_COLOR_BUTTON_HOVER] = nk_rgba(142, 187, 229, 255);
		table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(147, 192, 234, 255);
		table[NK_COLOR_TOGGLE] = nk_rgba(177, 210, 210, 255);
		table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(182, 215, 215, 255);
		table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(137, 182, 224, 255);
		table[NK_COLOR_SELECT] = nk_rgba(177, 210, 210, 255);
		table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(137, 182, 224, 255);
		table[NK_COLOR_SLIDER] = nk_rgba(177, 210, 210, 255);
		table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(137, 182, 224, 245);
		table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(142, 188, 229, 255);
		table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(147, 193, 234, 255);
		table[NK_COLOR_PROPERTY] = nk_rgba(210, 210, 210, 255);
		table[NK_COLOR_EDIT] = nk_rgba(210, 210, 210, 225);
		table[NK_COLOR_EDIT_CURSOR] = nk_rgba(20, 20, 20, 255);
		table[NK_COLOR_COMBO] = nk_rgba(210, 210, 210, 255);
		table[NK_COLOR_CHART] = nk_rgba(210, 210, 210, 255);
		table[NK_COLOR_CHART_COLOR] = nk_rgba(137, 182, 224, 255);
		table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba( 255, 0, 0, 255);
		table[NK_COLOR_SCROLLBAR] = nk_rgba(190, 200, 200, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
		table[NK_COLOR_TAB_HEADER] = nk_rgba(156, 193, 220, 255);
		nk_style_from_table(ctx, table);
	} else if (theme == THEME_DARK) {
		table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
		table[NK_COLOR_WINDOW] = nk_rgba(57, 67, 71, 215);
		table[NK_COLOR_HEADER] = nk_rgba(51, 51, 56, 220);
		table[NK_COLOR_BORDER] = nk_rgba(46, 46, 46, 255);
		table[NK_COLOR_BUTTON] = nk_rgba(48, 83, 111, 255);
		table[NK_COLOR_BUTTON_HOVER] = nk_rgba(58, 93, 121, 255);
		table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(63, 98, 126, 255);
		table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
		table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
		table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
		table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
		table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
		table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
		table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
		table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
		table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
		table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
		table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
		table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
		table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
		table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
		table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
		table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
		table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
		table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
		table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
		nk_style_from_table(ctx, table);
	} else {
		nk_style_default(ctx);
	}
}

#if 0
typedef struct media {
	GLint skin;
	struct nk_image menu;
	struct nk_image check;
	struct nk_image check_cursor;
	struct nk_image option;
	struct nk_image option_cursor;
	struct nk_image header;
	struct nk_image window;
	struct nk_image scrollbar_inc_button;
	struct nk_image scrollbar_inc_button_hover;
	struct nk_image scrollbar_dec_button;
	struct nk_image scrollbar_dec_button_hover;
	struct nk_image button;
	struct nk_image button_hover;
	struct nk_image button_active;
	struct nk_image tab_minimize;
	struct nk_image tab_maximize;
	struct nk_image slider;
	struct nk_image slider_hover;
	struct nk_image slider_active;
} media;

void nk_skin_gwen(void) {
	media.skin = image_load("../skins/gwen.png");
	media.check = nk_subimage_id(media.skin, 512,512, nk_rect(464,32,15,15));
	media.check_cursor = nk_subimage_id(media.skin, 512,512, nk_rect(450,34,11,11));
	media.option = nk_subimage_id(media.skin, 512,512, nk_rect(464,64,15,15));
	media.option_cursor = nk_subimage_id(media.skin, 512,512, nk_rect(451,67,9,9));
	media.header = nk_subimage_id(media.skin, 512,512, nk_rect(128,0,127,24));
	media.window = nk_subimage_id(media.skin, 512,512, nk_rect(128,23,127,104));
	media.scrollbar_inc_button = nk_subimage_id(media.skin, 512,512, nk_rect(464,256,15,15));
	media.scrollbar_inc_button_hover = nk_subimage_id(media.skin, 512,512, nk_rect(464,320,15,15));
	media.scrollbar_dec_button = nk_subimage_id(media.skin, 512,512, nk_rect(464,224,15,15));
	media.scrollbar_dec_button_hover = nk_subimage_id(media.skin, 512,512, nk_rect(464,288,15,15));
	media.button = nk_subimage_id(media.skin, 512,512, nk_rect(384,336,127,31));
	media.button_hover = nk_subimage_id(media.skin, 512,512, nk_rect(384,368,127,31));
	media.button_active = nk_subimage_id(media.skin, 512,512, nk_rect(384,400,127,31));
	media.tab_minimize = nk_subimage_id(media.skin, 512,512, nk_rect(451, 99, 9, 9));
	media.tab_maximize = nk_subimage_id(media.skin, 512,512, nk_rect(467,99,9,9));
	media.slider = nk_subimage_id(media.skin, 512,512, nk_rect(418,33,11,14));
	media.slider_hover = nk_subimage_id(media.skin, 512,512, nk_rect(418,49,11,14));
	media.slider_active = nk_subimage_id(media.skin, 512,512, nk_rect(418,64,11,14));

	/* window */
	ctx.style.window.background = nk_rgb(204,204,204);
	ctx.style.window.fixed_background = nk_style_item_image(media.window);
	ctx.style.window.border_color = nk_rgb(67,67,67);
	ctx.style.window.combo_border_color = nk_rgb(67,67,67);
	ctx.style.window.contextual_border_color = nk_rgb(67,67,67);
	ctx.style.window.menu_border_color = nk_rgb(67,67,67);
	ctx.style.window.group_border_color = nk_rgb(67,67,67);
	ctx.style.window.tooltip_border_color = nk_rgb(67,67,67);
	ctx.style.window.scrollbar_size = nk_vec2(16,16);
	ctx.style.window.border_color = nk_rgba(0,0,0,0);
	ctx.style.window.padding = nk_vec2(8,4);
	ctx.style.window.border = 3;

	/* window header */
	ctx.style.window.header.normal = nk_style_item_image(media.header);
	ctx.style.window.header.hover = nk_style_item_image(media.header);
	ctx.style.window.header.active = nk_style_item_image(media.header);
	ctx.style.window.header.label_normal = nk_rgb(95,95,95);
	ctx.style.window.header.label_hover = nk_rgb(95,95,95);
	ctx.style.window.header.label_active = nk_rgb(95,95,95);

	/* scrollbar */
	ctx.style.scrollv.normal          = nk_style_item_color(nk_rgb(184,184,184));
	ctx.style.scrollv.hover           = nk_style_item_color(nk_rgb(184,184,184));
	ctx.style.scrollv.active          = nk_style_item_color(nk_rgb(184,184,184));
	ctx.style.scrollv.cursor_normal   = nk_style_item_color(nk_rgb(220,220,220));
	ctx.style.scrollv.cursor_hover    = nk_style_item_color(nk_rgb(235,235,235));
	ctx.style.scrollv.cursor_active   = nk_style_item_color(nk_rgb(99,202,255));
	ctx.style.scrollv.dec_symbol      = NK_SYMBOL_NONE;
	ctx.style.scrollv.inc_symbol      = NK_SYMBOL_NONE;
	ctx.style.scrollv.show_buttons    = nk_true;
	ctx.style.scrollv.border_color    = nk_rgb(81,81,81);
	ctx.style.scrollv.cursor_border_color = nk_rgb(81,81,81);
	ctx.style.scrollv.border          = 1;
	ctx.style.scrollv.rounding        = 0;
	ctx.style.scrollv.border_cursor   = 1;
	ctx.style.scrollv.rounding_cursor = 2;

	/* scrollbar buttons */
	ctx.style.scrollv.inc_button.normal          = nk_style_item_image(media.scrollbar_inc_button);
	ctx.style.scrollv.inc_button.hover           = nk_style_item_image(media.scrollbar_inc_button_hover);
	ctx.style.scrollv.inc_button.active          = nk_style_item_image(media.scrollbar_inc_button_hover);
	ctx.style.scrollv.inc_button.border_color    = nk_rgba(0,0,0,0);
	ctx.style.scrollv.inc_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.scrollv.inc_button.text_normal     = nk_rgba(0,0,0,0);
	ctx.style.scrollv.inc_button.text_hover      = nk_rgba(0,0,0,0);
	ctx.style.scrollv.inc_button.text_active     = nk_rgba(0,0,0,0);
	ctx.style.scrollv.inc_button.border          = 0.0f;

	ctx.style.scrollv.dec_button.normal          = nk_style_item_image(media.scrollbar_dec_button);
	ctx.style.scrollv.dec_button.hover           = nk_style_item_image(media.scrollbar_dec_button_hover);
	ctx.style.scrollv.dec_button.active          = nk_style_item_image(media.scrollbar_dec_button_hover);
	ctx.style.scrollv.dec_button.border_color    = nk_rgba(0,0,0,0);
	ctx.style.scrollv.dec_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.scrollv.dec_button.text_normal     = nk_rgba(0,0,0,0);
	ctx.style.scrollv.dec_button.text_hover      = nk_rgba(0,0,0,0);
	ctx.style.scrollv.dec_button.text_active     = nk_rgba(0,0,0,0);
	ctx.style.scrollv.dec_button.border          = 0.0f;

	/* checkbox toggle */
	{struct nk_style_toggle *toggle;
	toggle = &ctx.style.checkbox;
	toggle->normal          = nk_style_item_image(media.check);
	toggle->hover           = nk_style_item_image(media.check);
	toggle->active          = nk_style_item_image(media.check);
	toggle->cursor_normal   = nk_style_item_image(media.check_cursor);
	toggle->cursor_hover    = nk_style_item_image(media.check_cursor);
	toggle->text_normal     = nk_rgb(95,95,95);
	toggle->text_hover      = nk_rgb(95,95,95);
	toggle->text_active     = nk_rgb(95,95,95);}

	/* option toggle */
	{struct nk_style_toggle *toggle;
	toggle = &ctx.style.option;
	toggle->normal          = nk_style_item_image(media.option);
	toggle->hover           = nk_style_item_image(media.option);
	toggle->active          = nk_style_item_image(media.option);
	toggle->cursor_normal   = nk_style_item_image(media.option_cursor);
	toggle->cursor_hover    = nk_style_item_image(media.option_cursor);
	toggle->text_normal     = nk_rgb(95,95,95);
	toggle->text_hover      = nk_rgb(95,95,95);
	toggle->text_active     = nk_rgb(95,95,95);}

	/* default button */
	ctx.style.button.normal = nk_style_item_image(media.button);
	ctx.style.button.hover = nk_style_item_image(media.button_hover);
	ctx.style.button.active = nk_style_item_image(media.button_active);
	ctx.style.button.border_color = nk_rgba(0,0,0,0);
	ctx.style.button.text_background = nk_rgba(0,0,0,0);
	ctx.style.button.text_normal = nk_rgb(95,95,95);
	ctx.style.button.text_hover = nk_rgb(95,95,95);
	ctx.style.button.text_active = nk_rgb(95,95,95);

	/* default text */
	ctx.style.text.color = nk_rgb(95,95,95);

	/* contextual button */
	ctx.style.contextual_button.normal = nk_style_item_color(nk_rgb(206,206,206));
	ctx.style.contextual_button.hover = nk_style_item_color(nk_rgb(229,229,229));
	ctx.style.contextual_button.active = nk_style_item_color(nk_rgb(99,202,255));
	ctx.style.contextual_button.border_color = nk_rgba(0,0,0,0);
	ctx.style.contextual_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.contextual_button.text_normal = nk_rgb(95,95,95);
	ctx.style.contextual_button.text_hover = nk_rgb(95,95,95);
	ctx.style.contextual_button.text_active = nk_rgb(95,95,95);

	/* menu button */
	ctx.style.menu_button.normal = nk_style_item_color(nk_rgb(206,206,206));
	ctx.style.menu_button.hover = nk_style_item_color(nk_rgb(229,229,229));
	ctx.style.menu_button.active = nk_style_item_color(nk_rgb(99,202,255));
	ctx.style.menu_button.border_color = nk_rgba(0,0,0,0);
	ctx.style.menu_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.menu_button.text_normal = nk_rgb(95,95,95);
	ctx.style.menu_button.text_hover = nk_rgb(95,95,95);
	ctx.style.menu_button.text_active = nk_rgb(95,95,95);

	/* tree */
	ctx.style.tab.text = nk_rgb(95,95,95);
	ctx.style.tab.tab_minimize_button.normal = nk_style_item_image(media.tab_minimize);
	ctx.style.tab.tab_minimize_button.hover = nk_style_item_image(media.tab_minimize);
	ctx.style.tab.tab_minimize_button.active = nk_style_item_image(media.tab_minimize);
	ctx.style.tab.tab_minimize_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.tab.tab_minimize_button.text_normal = nk_rgba(0,0,0,0);
	ctx.style.tab.tab_minimize_button.text_hover = nk_rgba(0,0,0,0);
	ctx.style.tab.tab_minimize_button.text_active = nk_rgba(0,0,0,0);

	ctx.style.tab.tab_maximize_button.normal = nk_style_item_image(media.tab_maximize);
	ctx.style.tab.tab_maximize_button.hover = nk_style_item_image(media.tab_maximize);
	ctx.style.tab.tab_maximize_button.active = nk_style_item_image(media.tab_maximize);
	ctx.style.tab.tab_maximize_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.tab.tab_maximize_button.text_normal = nk_rgba(0,0,0,0);
	ctx.style.tab.tab_maximize_button.text_hover = nk_rgba(0,0,0,0);
	ctx.style.tab.tab_maximize_button.text_active = nk_rgba(0,0,0,0);

	ctx.style.tab.node_minimize_button.normal = nk_style_item_image(media.tab_minimize);
	ctx.style.tab.node_minimize_button.hover = nk_style_item_image(media.tab_minimize);
	ctx.style.tab.node_minimize_button.active = nk_style_item_image(media.tab_minimize);
	ctx.style.tab.node_minimize_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.tab.node_minimize_button.text_normal = nk_rgba(0,0,0,0);
	ctx.style.tab.node_minimize_button.text_hover = nk_rgba(0,0,0,0);
	ctx.style.tab.node_minimize_button.text_active = nk_rgba(0,0,0,0);

	ctx.style.tab.node_maximize_button.normal = nk_style_item_image(media.tab_maximize);
	ctx.style.tab.node_maximize_button.hover = nk_style_item_image(media.tab_maximize);
	ctx.style.tab.node_maximize_button.active = nk_style_item_image(media.tab_maximize);
	ctx.style.tab.node_maximize_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.tab.node_maximize_button.text_normal = nk_rgba(0,0,0,0);
	ctx.style.tab.node_maximize_button.text_hover = nk_rgba(0,0,0,0);
	ctx.style.tab.node_maximize_button.text_active = nk_rgba(0,0,0,0);

	/* selectable */
	ctx.style.selectable.normal = nk_style_item_color(nk_rgb(206,206,206));
	ctx.style.selectable.hover = nk_style_item_color(nk_rgb(206,206,206));
	ctx.style.selectable.pressed = nk_style_item_color(nk_rgb(206,206,206));
	ctx.style.selectable.normal_active = nk_style_item_color(nk_rgb(185,205,248));
	ctx.style.selectable.hover_active = nk_style_item_color(nk_rgb(185,205,248));
	ctx.style.selectable.pressed_active = nk_style_item_color(nk_rgb(185,205,248));
	ctx.style.selectable.text_normal = nk_rgb(95,95,95);
	ctx.style.selectable.text_hover = nk_rgb(95,95,95);
	ctx.style.selectable.text_pressed = nk_rgb(95,95,95);
	ctx.style.selectable.text_normal_active = nk_rgb(95,95,95);
	ctx.style.selectable.text_hover_active = nk_rgb(95,95,95);
	ctx.style.selectable.text_pressed_active = nk_rgb(95,95,95);

	/* slider */
	ctx.style.slider.normal          = nk_style_item_hide();
	ctx.style.slider.hover           = nk_style_item_hide();
	ctx.style.slider.active          = nk_style_item_hide();
	ctx.style.slider.bar_normal      = nk_rgb(156,156,156);
	ctx.style.slider.bar_hover       = nk_rgb(156,156,156);
	ctx.style.slider.bar_active      = nk_rgb(156,156,156);
	ctx.style.slider.bar_filled      = nk_rgb(156,156,156);
	ctx.style.slider.cursor_normal   = nk_style_item_image(media.slider);
	ctx.style.slider.cursor_hover    = nk_style_item_image(media.slider_hover);
	ctx.style.slider.cursor_active   = nk_style_item_image(media.slider_active);
	ctx.style.slider.cursor_size     = nk_vec2(16.5f,21);
	ctx.style.slider.bar_height      = 1;

	/* progressbar */
	ctx.style.progress.normal = nk_style_item_color(nk_rgb(231,231,231));
	ctx.style.progress.hover = nk_style_item_color(nk_rgb(231,231,231));
	ctx.style.progress.active = nk_style_item_color(nk_rgb(231,231,231));
	ctx.style.progress.cursor_normal = nk_style_item_color(nk_rgb(63,242,93));
	ctx.style.progress.cursor_hover = nk_style_item_color(nk_rgb(63,242,93));
	ctx.style.progress.cursor_active = nk_style_item_color(nk_rgb(63,242,93));
	ctx.style.progress.border_color = nk_rgb(114,116,115);
	ctx.style.progress.padding = nk_vec2(0,0);
	ctx.style.progress.border = 2;
	ctx.style.progress.rounding = 1;

	/* combo */
	ctx.style.combo.normal = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.combo.hover = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.combo.active = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.combo.border_color = nk_rgb(95,95,95);
	ctx.style.combo.label_normal = nk_rgb(95,95,95);
	ctx.style.combo.label_hover = nk_rgb(95,95,95);
	ctx.style.combo.label_active = nk_rgb(95,95,95);
	ctx.style.combo.border = 1;
	ctx.style.combo.rounding = 1;

	/* combo button */
	ctx.style.combo.button.normal = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.combo.button.hover = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.combo.button.active = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.combo.button.text_background = nk_rgb(216,216,216);
	ctx.style.combo.button.text_normal = nk_rgb(95,95,95);
	ctx.style.combo.button.text_hover = nk_rgb(95,95,95);
	ctx.style.combo.button.text_active = nk_rgb(95,95,95);

	/* property */
	ctx.style.property.normal = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.hover = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.active = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.border_color = nk_rgb(81,81,81);
	ctx.style.property.label_normal = nk_rgb(95,95,95);
	ctx.style.property.label_hover = nk_rgb(95,95,95);
	ctx.style.property.label_active = nk_rgb(95,95,95);
	ctx.style.property.sym_left = NK_SYMBOL_TRIANGLE_LEFT;
	ctx.style.property.sym_right = NK_SYMBOL_TRIANGLE_RIGHT;
	ctx.style.property.rounding = 10;
	ctx.style.property.border = 1;

	/* edit */
	ctx.style.edit.normal = nk_style_item_color(nk_rgb(240,240,240));
	ctx.style.edit.hover = nk_style_item_color(nk_rgb(240,240,240));
	ctx.style.edit.active = nk_style_item_color(nk_rgb(240,240,240));
	ctx.style.edit.border_color = nk_rgb(62,62,62);
	ctx.style.edit.cursor_normal = nk_rgb(99,202,255);
	ctx.style.edit.cursor_hover = nk_rgb(99,202,255);
	ctx.style.edit.cursor_text_normal = nk_rgb(95,95,95);
	ctx.style.edit.cursor_text_hover = nk_rgb(95,95,95);
	ctx.style.edit.text_normal = nk_rgb(95,95,95);
	ctx.style.edit.text_hover = nk_rgb(95,95,95);
	ctx.style.edit.text_active = nk_rgb(95,95,95);
	ctx.style.edit.selected_normal = nk_rgb(99,202,255);
	ctx.style.edit.selected_hover = nk_rgb(99,202,255);
	ctx.style.edit.selected_text_normal = nk_rgb(95,95,95);
	ctx.style.edit.selected_text_hover = nk_rgb(95,95,95);
	ctx.style.edit.border = 1;
	ctx.style.edit.rounding = 2;

	/* property buttons */
	ctx.style.property.dec_button.normal = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.dec_button.hover = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.dec_button.active = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.dec_button.text_background = nk_rgba(0,0,0,0);
	ctx.style.property.dec_button.text_normal = nk_rgb(95,95,95);
	ctx.style.property.dec_button.text_hover = nk_rgb(95,95,95);
	ctx.style.property.dec_button.text_active = nk_rgb(95,95,95);
	ctx.style.property.inc_button = ctx.style.property.dec_button;

	/* property edit */
	ctx.style.property.edit.normal = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.edit.hover = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.edit.active = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.property.edit.border_color = nk_rgba(0,0,0,0);
	ctx.style.property.edit.cursor_normal = nk_rgb(95,95,95);
	ctx.style.property.edit.cursor_hover = nk_rgb(95,95,95);
	ctx.style.property.edit.cursor_text_normal = nk_rgb(216,216,216);
	ctx.style.property.edit.cursor_text_hover = nk_rgb(216,216,216);
	ctx.style.property.edit.text_normal = nk_rgb(95,95,95);
	ctx.style.property.edit.text_hover = nk_rgb(95,95,95);
	ctx.style.property.edit.text_active = nk_rgb(95,95,95);
	ctx.style.property.edit.selected_normal = nk_rgb(95,95,95);
	ctx.style.property.edit.selected_hover = nk_rgb(95,95,95);
	ctx.style.property.edit.selected_text_normal = nk_rgb(216,216,216);
	ctx.style.property.edit.selected_text_hover = nk_rgb(216,216,216);

	/* chart */
	ctx.style.chart.background = nk_style_item_color(nk_rgb(216,216,216));
	ctx.style.chart.border_color = nk_rgb(81,81,81);
	ctx.style.chart.color = nk_rgb(95,95,95);
	ctx.style.chart.selected_color = nk_rgb(255,0,0);
	ctx.style.chart.border = 1;
}
#endif
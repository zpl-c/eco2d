#define MAX_NOTIFICATIONS_ON_SCREEN 5

typedef struct {
	zpl_string title;
	zpl_string text;
} notification;

static notification *notifications = 0;

static bool show_notification_list = 0;

void notification_push(const char* title1, const char* text1) {
	if (!notifications) {
		zpl_array_init(notifications, zpl_heap());
	}

	zpl_string title = zpl_string_make(zpl_heap(), title1);
	zpl_string text = zpl_string_make(zpl_heap(), text1);

	zpl_array_append(notifications, ((notification) { title, text }));
}

void notification_clear(void) {
	for (zpl_isize i = 0; i < zpl_array_count(notifications); i++) {
		zpl_string_free(notifications[i].title);
		zpl_string_free(notifications[i].text);
	}
	zpl_array_clear(notifications);
}

void notification_draw(void) {
	float width = (float)GetScreenWidth();
	float height = (float)GetScreenHeight();
	// draw ctrl panel
	if (nk_begin(game_ui, "Notifications", nk_rect(width - 220, 20, 200, 80), 
	             NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_TITLE | NK_WINDOW_DYNAMIC)) {
		{
			nk_layout_row_dynamic(game_ui, 0, 2);

			if (nk_button_label(game_ui, "Clear All")) {
				notification_clear();
			}

			if (nk_button_label(game_ui, show_notification_list ? "Hide Manager" :  "Show All")) {
				show_notification_list ^= 1;
			}
		}
		nk_end(game_ui);
	}

	float ypos = 100;
	zpl_isize cnt = zpl_min(zpl_array_count(notifications), MAX_NOTIFICATIONS_ON_SCREEN)-1;

	for (zpl_isize i = cnt; i >= 0; --i) {
		notification *notif = (notifications + i);
		if (nk_begin_titled(game_ui, zpl_bprintf("%d%fnotif%s", i, ypos, notif->title), notif->title, nk_rect(width - 320, ypos, 300, 1200),
		                    NK_WINDOW_DYNAMIC|NK_WINDOW_NO_SCROLLBAR)) {
			{
				if (nk_tree_push_id(game_ui, NK_TREE_TAB, notif->title, NK_MAXIMIZED, (int)i)) {
					nk_label_wrap(game_ui, notif->text);

					if (nk_button_label(game_ui, "OK")) {
						zpl_string_free(notifications[i].title);
						zpl_string_free(notifications[i].text);
						zpl_array_remove_at(notifications, i);
					}
					nk_tree_pop(game_ui);
				}
			}
			ypos += nk_window_get_panel(game_ui)->row.height + 80;

			nk_end(game_ui);
		}
	}

	if (show_notification_list) {
		if (nk_begin(game_ui, "Notifications Manager", nk_rect(width/2.0f - 320, height/2.0f - 240, 640, 480),
		             NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_DYNAMIC )) {
			{
				nk_layout_row_dynamic(game_ui, 5 , 1);
				{
					for (zpl_isize i = 0; i < zpl_array_count(notifications); ++i) {
						notification *notif = (notifications + i);
						if (nk_tree_push_id(game_ui, NK_TREE_TAB, notif->title, NK_MINIMIZED, (int)i)) {
							{
								nk_label_wrap(game_ui, notif->text);

								if (nk_button_label(game_ui, "OK")) {
									zpl_string_free(notifications[i].title);
									zpl_string_free(notifications[i].text);
									zpl_array_remove_at(notifications, i); --i; 
								}
							}
							nk_tree_pop(game_ui);
						}
					}
				}

				nk_layout_row_dynamic(game_ui, 0, 2);

				if (nk_button_label(game_ui, "Clear All")) {
					notification_clear();
				}

				if (nk_button_label(game_ui, "Hide Manager")) {
					show_notification_list = 0 ;
				}
			}
			nk_end(game_ui);
		}
	}
}

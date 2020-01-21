#include <stdlib.h>
#include <Elementary.h>
#include <dlog.h>
#include <inputmethod.h>
#include <app_resource_manager.h>
#include "morseime.h"
#include "app_i18n.h"

static void ime_app_create_cb(void *user_data);
static void ime_app_terminate_cb(void *user_data);
static void ime_app_show_cb(int ic, ime_context_h context, void *user_data);
static void ime_app_hide_cb(int ic, void *user_data);

static void ime_app_cursor_position_updated_cb(int cursor_pos, void *user_data);
static void ime_app_focus_out_cb(int ic, void *user_data);
static void ime_app_focus_in_cb(int ic, void *user_data);
static void ime_app_return_key_type_set_cb(Ecore_IMF_Input_Panel_Return_Key_Type type, void *user_data);
static void ime_app_return_key_state_set_cb(bool disabled, void *user_data);
static void ime_app_layout_set_cb(Ecore_IMF_Input_Panel_Layout layout, void *user_data);
static bool ime_app_process_key_event_cb(ime_key_code_e keycode, ime_key_mask_e keymask, ime_device_info_h dev_info, void *user_data);
static void ime_app_display_language_changed_cb(const char *language, void *user_data);

struct info
{
	double timeout;
	double max_dit_time;
	char input_sequence[7];
	int input_sequence_pointer;
	Ecore_Timer *timer;
	Ecore_Timer *button_timer;
	int shift;
	int dit_or_dah;
};
struct info inf = {0.5, 0.25, "", 0};

static Evas_Object *enter_key_btn = NULL;
static Evas_Object *shift_key_btn = NULL;

static void add_button_icon(Evas_Object *btn, char *filename)
{
	char *img_path = NULL;
	app_resource_manager_get(APP_RESOURCE_TYPE_IMAGE, filename, &img_path);
	if (img_path != NULL) {
		Evas_Object *icon;
		icon = elm_icon_add(btn);
		elm_image_file_set(icon, img_path, NULL);
		elm_object_part_content_set(btn, "icon", icon);
		elm_object_text_set(btn, NULL);
		free(img_path);
	}
}

static void shift_toggle()
{
	inf.shift = 1 - inf.shift;
	if (inf.shift) {
		add_button_icon(shift_key_btn, "arrow-up-bold.png");
	}
	else {
		add_button_icon(shift_key_btn, "arrow-up-bold-outline.png");
	}
}

Eina_Bool commit_input_str(void *data)
{
	if (strcmp(inf.input_sequence, "111101") == 0) {
		shift_toggle();
	}
	else if (strcmp(inf.input_sequence, "0000") == 0) {
		ime_send_key_event(IME_KEY_BackSpace, IME_KEY_MASK_PRESSED, false);
		ime_send_key_event(IME_KEY_BackSpace, IME_KEY_MASK_RELEASED, false);
	}
	else if (strcmp(inf.input_sequence, "1100") == 0) {
		ime_send_key_event(IME_KEY_space, IME_KEY_MASK_PRESSED, true);
		ime_send_key_event(IME_KEY_space, IME_KEY_MASK_RELEASED, true);
	}
	else if (strcmp(inf.input_sequence, "1010") == 0) {
		ime_send_key_event(IME_KEY_Return, IME_KEY_MASK_PRESSED, true);
		ime_send_key_event(IME_KEY_Return, IME_KEY_MASK_RELEASED, true);
	}
	else {
		if (inf.shift) {
			char tmp[1];
			sprintf(tmp, "%c", toupper(i18n_get_text(inf.input_sequence)[0]));
			if (strcmp(tmp, inf.input_sequence)) {
				ime_commit_string(tmp);
				shift_toggle();
			}
		}
		else {
			char *tmp = i18n_get_text(inf.input_sequence);
			if (strcmp(tmp, inf.input_sequence)) {
				ime_commit_string(tmp);
			}
		}
	}
	inf.input_sequence_pointer = 0;
	return ECORE_CALLBACK_CANCEL;
}

Eina_Bool dah_cb(void *data)
{
	inf.dit_or_dah = 0;
	return ECORE_CALLBACK_CANCEL;
}

static void button_pressed(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	inf.dit_or_dah = 1;
	if (inf.timer) {
		ecore_timer_del(inf.timer);
	}
	if (inf.button_timer) {
		ecore_timer_del(inf.button_timer);
	}
	inf.button_timer = ecore_timer_add(inf.max_dit_time, dah_cb, NULL);
}

static void button_unpressed(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	if (inf.input_sequence_pointer < sizeof(inf.input_sequence)) {
		inf.input_sequence_pointer += sprintf(&inf.input_sequence[inf.input_sequence_pointer], "%d", inf.dit_or_dah);
		inf.timer = ecore_timer_add(inf.timeout, commit_input_str, NULL);
	}
	else {
		inf.input_sequence_pointer = 0;
	}
}

static void shift_key_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	shift_toggle();
}

static void back_key_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	ime_send_key_event(IME_KEY_BackSpace, IME_KEY_MASK_PRESSED, false);
	ime_send_key_event(IME_KEY_BackSpace, IME_KEY_MASK_RELEASED, false);
}

static void space_key_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	ime_send_key_event(IME_KEY_space, IME_KEY_MASK_PRESSED, true);
	ime_send_key_event(IME_KEY_space, IME_KEY_MASK_RELEASED, true);
}

static void return_key_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	ime_send_key_event(IME_KEY_Return, IME_KEY_MASK_PRESSED, true);
	ime_send_key_event(IME_KEY_Return, IME_KEY_MASK_RELEASED, true);
}

static Evas_Object *create_key_button(Evas_Object *parent, char *str)
{
	char *markup_str = NULL;

	Evas_Object *btn = elm_button_add(parent);
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);

	if (str) {
		markup_str = elm_entry_utf8_to_markup(str);

		if (markup_str) {
			elm_object_text_set(btn, markup_str);
			free(markup_str);
			markup_str = NULL;
		}
	}

	evas_object_show(btn);

	return btn;
}

static void set_return_key_type(Ecore_IMF_Input_Panel_Return_Key_Type type)
{
	elm_object_text_set(enter_key_btn, NULL);
}

static void ime_app_create_cb(void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "ime_app_create_cb");
	Evas_Object *btn = NULL;
	int w, h;

	Evas_Object *ime_win = ime_get_main_window();
	if (!ime_win) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "Can't get main window: %d", get_last_result());
		return;
	}

	elm_win_screen_size_get(ime_win, NULL, NULL, &w, &h);
	ime_set_size(w, h*2/3, h, w*3/5);

	Evas_Object *bg = elm_bg_add(ime_win);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ime_win, bg);
	evas_object_show(bg);

	Evas_Object *table = elm_table_add(ime_win);
	evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ime_win, table);
	elm_table_homogeneous_set(table, EINA_TRUE);
	elm_table_padding_set(table, 2, 2); // TODO delete

	btn = create_key_button(table, NULL);
	evas_object_smart_callback_add(btn, "pressed", button_pressed, NULL);
	evas_object_smart_callback_add(btn, "unpressed", button_unpressed, NULL);
	elm_table_pack(table, btn, 1, 0, 2, 3);

	/* row 2 */
	/* Add backspace key */
	btn = create_key_button(table, NULL);
	evas_object_smart_callback_add(btn, "clicked", back_key_clicked, NULL);
	add_button_icon(btn, "keyboard-backspace.png");
	elm_table_pack(table, btn, 0, 1, 1, 1);

	/* Add shift key */
	shift_key_btn = create_key_button(table, NULL);
	evas_object_smart_callback_add(shift_key_btn, "clicked", shift_key_clicked, NULL);
	add_button_icon(shift_key_btn, "arrow-up-bold-outline.png");
	elm_table_pack(table, shift_key_btn, 0, 0, 1, 1);

	/* Add space key */
	btn = create_key_button(table, NULL);
	evas_object_smart_callback_add(btn, "clicked", space_key_clicked, NULL);
	add_button_icon(btn, "space.png");
	elm_table_pack(table, btn, 3, 1, 1, 1);

	/* Add return key */
	enter_key_btn = create_key_button(table, NULL);
	evas_object_smart_callback_add(enter_key_btn, "clicked", return_key_clicked, NULL);
	add_button_icon(enter_key_btn, "keyboard-return.png");
	elm_table_pack(table, enter_key_btn, 3, 0, 1, 1);

	evas_object_show(table);
}

static void ime_app_terminate_cb(void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "ime_app_terminate_cb");
	/* Release the resources */
}

static void ime_app_show_cb(int ic, ime_context_h context, void *user_data)
{
	Ecore_IMF_Input_Panel_Layout layout;
	ime_layout_variation_e layout_variation;
	int cursor_pos;
	Ecore_IMF_Autocapital_Type autocapital_type;
	Ecore_IMF_Input_Panel_Return_Key_Type return_key_type;
	bool return_key_state, prediction_mode, password_mode;

	dlog_print(DLOG_DEBUG, LOG_TAG, "%s, %d", "ime_app_show_cb", ic);

	if (ime_context_get_layout(context, &layout) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "layout: %d", layout);
	if (ime_context_get_layout_variation(context, &layout_variation) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "layout variation: %d", layout_variation);
	if (ime_context_get_cursor_position(context, &cursor_pos) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "cursor position: %d", cursor_pos);
	if (ime_context_get_autocapital_type(context, &autocapital_type) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "autocapital_type: %d", autocapital_type);
	if (ime_context_get_return_key_type(context, &return_key_type) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "return_key_type: %d", return_key_type);
	if (ime_context_get_return_key_state(context, &return_key_state) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "return_key_state: %d", return_key_state);
	if (ime_context_get_prediction_mode(context, &prediction_mode) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "prediction_mode: %d", prediction_mode);
	if (ime_context_get_password_mode(context, &password_mode) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "password_mode: %d", password_mode);

	set_return_key_type(return_key_type);

	Evas_Object *ime_win = ime_get_main_window();
	if (!ime_win) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%d", get_last_result());
		return;
	}

	evas_object_show(ime_win);
}

static void ime_app_hide_cb(int ic, void *user_data)
{
	Evas_Object *ime_win = ime_get_main_window();
	if (!ime_win) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%d", get_last_result());
		return;
	}

	evas_object_hide(ime_win);
}

static void ime_app_cursor_position_updated_cb(int cursor_pos, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "cursor position: %d", cursor_pos);
}

static void ime_app_focus_out_cb(int ic, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "focus out: %d", ic);
}

static void ime_app_focus_in_cb(int ic, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "focus in: %d", ic);
}

static void ime_app_return_key_type_set_cb(Ecore_IMF_Input_Panel_Return_Key_Type type, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Return key type: %d", type);

	set_return_key_type(type);
}

static void ime_app_return_key_state_set_cb(bool disabled, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Return key disabled: %d", disabled);
}

static void ime_app_layout_set_cb(Ecore_IMF_Input_Panel_Layout layout, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "layout: %d", layout);
}

static bool ime_app_process_key_event_cb(ime_key_code_e keycode, ime_key_mask_e keymask, ime_device_info_h dev_info, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "keycode=0x%x, keymask=0x%x", keycode, keymask);

	if ((keymask & IME_KEY_MASK_CONTROL) || (keymask & IME_KEY_MASK_ALT) || (keymask & IME_KEY_MASK_META) || (keymask & IME_KEY_MASK_WIN) || (keymask & IME_KEY_MASK_HYPER))
		return false;

	return false;
}

static void ime_app_display_language_changed_cb(const char *language, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "language: %s", language);
}

void input_device_cb(ime_input_device_type_e device_type, ime_input_device_event_h device_event, void *user_data)
{
	if (device_type == IME_INPUT_DEVICE_TYPE_ROTARY) {
		ime_input_device_rotary_direction_e direction;
		if (IME_ERROR_NONE == ime_input_device_rotary_get_direction(device_event, &direction)) {
			dlog_print(DLOG_DEBUG, LOG_TAG, "Direction : %d", direction);
			if (inf.timer) {
				ecore_timer_del(inf.timer);
			}
			if (inf.input_sequence_pointer < sizeof(inf.input_sequence)) {
				inf.input_sequence_pointer += sprintf(&inf.input_sequence[inf.input_sequence_pointer], "%d", direction);
				inf.timer = ecore_timer_add(inf.timeout, commit_input_str, NULL);
			}
			else {
				inf.input_sequence_pointer = 0;
			}
		}
	}
}

void ime_app_main(int argc, char **argv)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Start main function !");

	ime_callback_s basic_callback = {
		ime_app_create_cb,
		ime_app_terminate_cb,
		ime_app_show_cb,
		ime_app_hide_cb,
	};

	/* Set the necessary callback functions */
	ime_event_set_focus_in_cb(ime_app_focus_in_cb, NULL);
	ime_event_set_focus_out_cb(ime_app_focus_out_cb, NULL);
	ime_event_set_cursor_position_updated_cb(ime_app_cursor_position_updated_cb, NULL);
	ime_event_set_layout_set_cb(ime_app_layout_set_cb, NULL);
	ime_event_set_return_key_type_set_cb(ime_app_return_key_type_set_cb, NULL);
	ime_event_set_return_key_state_set_cb(ime_app_return_key_state_set_cb, NULL);
	ime_event_set_process_key_event_cb(ime_app_process_key_event_cb, NULL);
	ime_event_set_display_language_changed_cb(ime_app_display_language_changed_cb, NULL);
	ime_event_set_process_input_device_event_cb(input_device_cb, NULL);

	/* Start IME */
	ime_run(&basic_callback, NULL);
}

int
main(int argc, char *argv[])
{
	ime_app_main(argc, argv);

	return 0;
}

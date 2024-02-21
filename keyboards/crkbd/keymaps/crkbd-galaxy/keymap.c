#include QMK_KEYBOARD_H
#include <stdio.h>


typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_SINGLE_TAP,
    TD_DOUBLE_HOLD,
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;


// Tapdance enums
enum {
    LAYER_DANCE,
    ALT_DANCE,
    CTRL_DANCE,
};

// Forward declare for use in keymap
td_state_t cur_dance(tap_dance_state_t *state);

void layer_td_finished(tap_dance_state_t *state, void *user_data);
void layer_td_reset(tap_dance_state_t *state, void *user_data);


enum my_keycodes {
	STRINPT = SAFE_RANGE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT_split_3x6_3(

			KC_LGUI,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,            KC_Y,     KC_U,    KC_I,    KC_O,    KC_P, KC_BSPC,
			 KC_TAB,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,            KC_H,     KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
			KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,            KC_N,     KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_RSFT,
			                 TD(CTRL_DANCE),  KC_SPC,   TD(LAYER_DANCE),            TD(LAYER_DANCE),  KC_ENT, TD(ALT_DANCE)

), [1] = LAYOUT_split_3x6_3(

			KC_LGUI, XXXXXXX,    KC_7,    KC_8,    KC_9, KC_EXLM,           KC_GRV, KC_LCBR, KC_RCBR, KC_CIRC, KC_AMPR, _______,
			 KC_TAB,    KC_0,    KC_4,    KC_5,    KC_6, KC_ASTR,           KC_EQL, KC_LPRN, KC_RPRN, KC_UNDS, KC_PIPE,  KC_ESC,
			KC_LSFT, XXXXXXX,    KC_1,    KC_2,    KC_3, KC_SLSH,          KC_MINS, KC_LBRC, KC_RBRC, XXXXXXX, KC_BSLS, KC_RSFT,
			                           _______,  _______, _______,          _______,  _______, _______

), [2] = LAYOUT_split_3x6_3(

	    KC_LGUI, CG_TOGG,   KC_F7,   KC_F8,   KC_F9,  KC_F12,          KC_HOME, LCA(KC_DOWN), LCA(KC_UP),  KC_END, XXXXXXX, _______,
			 KC_TAB, XXXXXXX,   KC_F4,   KC_F5,   KC_F6,  KC_F11,          KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX,  STRINPT,
			KC_LSFT, LSFT(KC_LGUI), KC_F1, KC_F2, KC_F3,  KC_F10,          XXXXXXX, KC_PGDN, KC_PGUP, XXXXXXX, XXXXXXX, KC_PSCR,
			                           _______,  _______, _______,          _______,  _______, _______

)};

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  }
  return rotation;
}

#define L_BASE 0
#define L_WTF 1
#define L_LOWER 2
#define L_RAISE 4

void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
		if (layer_state < L_LOWER) {
			oled_write_P(PSTR("WASD\n"), false);
			return;
		}
		if (layer_state < L_RAISE) {
			oled_write_P(PSTR("Symb\n"), false);
			return;
		}
		oled_write_P(PSTR("Nav\n"), false);
}


bool is_strinpt_mode = false;
uint8_t strinpt_ind = 0;
char strinpt_buf[21] = { };
bool shift_down = false;

char keylog_str[24] = {};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

void set_keylog(uint16_t keycode, keyrecord_t *record) {
  char name = ' ';
  if (keycode < 60) {
    name = code_to_name[keycode];
  }

  // update keylog
  snprintf(keylog_str, sizeof(keylog_str), "%dx%d, k%2d : %c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

void oled_render_keylog(void) {
    oled_write(keylog_str, false);
}

void render_bootmagic_status(bool status) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    if (status) {
        oled_write_ln_P(logo[0][0], false);
        oled_write_ln_P(logo[0][1], false);
    } else {
        oled_write_ln_P(logo[1][0], false);
        oled_write_ln_P(logo[1][1], false);
    }
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_render_logo();
    }
    return false;
}

#endif  // OLED_ENABLE

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) {
            return TD_SINGLE_TAP;
        }
        // Key still held
        return TD_SINGLE_HOLD;
    }
    else if (state->count == 2) {
        if (state->pressed) {
            return TD_DOUBLE_HOLD;
        }
        else {
            return TD_DOUBLE_TAP;
        }
    }
    return TD_UNKNOWN;
}

// Static state for layer dance
static td_tap_t layer_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void layer_td_finished(tap_dance_state_t *state, void *user_data) {
    layer_tap_state.state = cur_dance(state);
    switch (layer_tap_state.state) {
        case TD_SINGLE_TAP:
            // If a higher layer is off, turn all off
            if (layer_state_is(2)) {
                layer_off(2);
                layer_off(1);
            }
            else if (layer_state_is(1)) {
                // Already on, turn off
                layer_off(1);
            }
            else {
                // Off, turn it on
                layer_on(1);
            }
            break;

        case TD_SINGLE_HOLD:
            layer_on(1);
            break;

        case TD_DOUBLE_TAP:
            if (layer_state_is(2)) {
                // Already on, turn off
                layer_off(2);
                layer_off(1);
            }
            else {
                // Off, turn it on
                layer_on(2);
            }
            break;

        case TD_DOUBLE_HOLD:
            layer_on(2);
            break;

        default:
            break;
    }
}

void layer_td_reset(tap_dance_state_t *state, void *user_data) {
    // If the key was held down and is released, switch layer off
    if (layer_tap_state.state == TD_SINGLE_HOLD) {
        layer_off(2);
        layer_off(1);
    }
    if (layer_tap_state.state == TD_DOUBLE_HOLD) {
        layer_off(2);
        layer_off(1);
    }
    layer_tap_state.state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [LAYER_DANCE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, layer_td_finished, layer_td_reset),
    [ALT_DANCE] = ACTION_TAP_DANCE_DOUBLE(KC_LALT, KC_RALT),
    [CTRL_DANCE] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, KC_RCTL),
};

// Overrides
const key_override_t delete_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);

const key_override_t **key_overrides = (const key_override_t *[]){
    &delete_key_override,
    NULL
};

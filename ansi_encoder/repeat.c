#include "quantum.h"


#define REPEAT_FIRST KC_A
#define REPEAT_LAST  KC_UP

#define REPEAT_DELAY 115
#define REPEAT_RATE  13

static bool     enable_repeat_rate = true;

// Customize per key repeat rate. -1 means not active
static int8_t   repeat_rate [255] = {
    [0 ... 254] = REPEAT_RATE,
    [KC_SPACE]  = -1,
};

static uint16_t repeat_keycode = 0;
static uint16_t repeat_timer   = 0;
static uint16_t repeat_count   = 0;
static bool     repeat_active  = false;

enum custom_keycodes {
	CUST_ENABLE_REPEAT = SAFE_RANGE,
};

void keyboard_post_init_user (void) {
    rgblight_setrgb(0, 0, 255);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CUST_ENABLE_REPEAT:
            if (record->event.pressed){
                enable_repeat_rate = !enable_repeat_rate;
            }

            if (enable_repeat_rate) {
                rgblight_setrgb(0, 0, 255);
            } else {
                rgblight_setrgb(255, 0, 0);

                if (repeat_active) {
                    unregister_code(repeat_keycode);
                    repeat_active = false;
                }
            }

            return false;
            
        case REPEAT_FIRST ... REPEAT_LAST:
            if (repeat_rate[keycode] == -1) {
                return true;
            }
            if (enable_repeat_rate) {
                if (record->event.pressed) {
                    if (repeat_active) {
                        unregister_code(repeat_keycode);
                    }

                    repeat_keycode = keycode;
                    repeat_count = 0;
                    repeat_active = true;
                    repeat_timer = timer_read();

                    register_code(keycode);
                } else if (repeat_keycode == keycode) {
                    repeat_active = false;
                    unregister_code(keycode);
                }

                return false;
            }
    }

    return true;
}


void matrix_scan_user(void) {
    if (repeat_active && enable_repeat_rate) {
        if (timer_elapsed(repeat_timer) > REPEAT_DELAY + repeat_count * repeat_rate[repeat_keycode]) {
            unregister_code(repeat_keycode);
            register_code(repeat_keycode);
            repeat_count++;
        }
    }
}
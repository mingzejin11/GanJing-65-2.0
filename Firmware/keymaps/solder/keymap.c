/* Copyright 2021 mingzejin11
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GanJing65.h"
#include "analog.h"
#include <stdio.h>

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
#define MN 0
#define FN 1

char wpm_str[10];

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	// Main layer
	[MN] = LAYOUT_solder(
		/* esc     1        2        3        4        5        6        7        8        9        0        -        =       bkspc    mute  */
		KC_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_MUTE,
		/* tab     Q        W        E        R        T        Y        U        I        O        P        [        ]        \      delete */ 
		KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
		/* caps    A        S        D        F        G        H        J        K        L        ;        '        #      enter      up   */
		KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT,  KC_UP,
		/* shift   \        Z        X        C        V        B        N        M        ,        .        /      shift    enter    insert */
		KC_LSFT, KC_BSLS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_ENT,  KC_INS,
		/* ctrl   win      alt                                space               alt     win      ctrl     fn       left     down     right */
		KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,           KC_RALT, KC_RGUI, KC_RCTL, MO(FN),  KC_LEFT, KC_DOWN, KC_RGHT),

	// Function layer
	[FN] = LAYOUT_solder(
		/* esc     F1       F2       F3       F4       F5       F6       F7       F8       F9       F0       F11      F12     bkspc    OLED  */
		KC_TRNS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_TRNS, KC_TRNS,
		/* tab     Q        W        E      reset      T        Y        U        I        O        P        [        ]        \      delete */ 
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RESET,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		/* caps    A        S        D        F        G        H        J        K        L        ;        '        #      enter      up   */
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		/* shift   \        Z        X        C        V        B        N        M        ,        .        /      shift    enter    insert */
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		/* ctrl   win      alt                                space               alt     win      ctrl     fn       prev     play     next  */
		KC_TRNS, KC_TRNS, KC_TRNS,                            KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPRV, KC_MPLY, KC_MNXT)
};

// Encoder function definition
void encoder_update_user(uint16_t index, bool clockwise) {
	// On function layer, knob controls brightness
	if (IS_LAYER_ON(FN)) {
		if (clockwise) {
			tap_code(KC_BRIU);
		} else {
			tap_code(KC_BRID);
		}
	// Otherwise, knob controls volume by default
	} else {
		if (clockwise) {
			tap_code(KC_VOLU);
		} else {
			tap_code(KC_VOLD);
		}
	}
}

#ifdef OLED_DRIVER_ENABLE

// oled_rotation_t oled_init_user(oled_rotation_t rotation) {
//     return OLED_ROTATION_180;
// }

// WPM-responsive animation stuff here
// Idle parameters
#define IDLE_FRAMES 5
#define IDLE_SPEED 10 				// Below this WPM value your animation will idle

// Start parameters
#define START_FRAMES 1 				// Uncomment if > 1
#define START_SPEED 20

// Tap parameters
#define TAP_FRAMES 2
#define TAP_SPEED 60 				// Above this WPM value the tapping gets hard

// Surrender parameters
#define SURRENDER_FRAMES 5
#define SURRENDER_SPEED 100

// How long each frame lasts in ms
#define DEFAULT_FRAME_DURATION 200 	// 5 fps	
#define MEDIUM_FRAME_DURATION 166	// 6 fps
#define FAST_FRAME_DURATION 125		// 8 fps 
#define FASTEST_FRAME_DURATION 100	// 10 fps

// Number of bytes in array, minimize for adequate firmware size, max is 1024
#define ANIM_SIZE 512 				// Number of hex values per image

uint32_t anim_timer = 0;
uint32_t anim_sleep = 0;
uint32_t current_wpm = 0;
uint8_t current_idle_frame = 0;
uint8_t current_tap_frame = 0;
uint8_t current_surrender_frame = 0;

static void render_anim(void) {
	static const char PROGMEM idle[IDLE_FRAMES][ANIM_SIZE] = {
		{ // Idle 1
		},
		{ // Idle 2


// Set current WPM to this variable
	current_wpm = get_current_wpm();

	// Assumes 1 frame prep stage
	void animation_phase(void) {
		// Idle
		if (current_wpm < IDLE_SPEED) {
			current_idle_frame = (current_idle_frame + 1) % IDLE_FRAMES;
			oled_write_raw_P(idle[abs((IDLE_FRAMES - 1) - current_idle_frame)], ANIM_SIZE);
		}

		// Start
		if (current_wpm >= IDLE_SPEED && current_wpm < START_SPEED) {
			oled_write_raw_P(start[0], ANIM_SIZE);
		}

		// Soft tap
		if (current_wpm >= START_SPEED && current_wpm < TAP_SPEED) {
			current_tap_frame = (current_tap_frame + 1) % TAP_FRAMES;
			oled_write_raw_P(soft[abs((TAP_FRAMES - 1) - current_tap_frame)], ANIM_SIZE);
		}

		// Hard tap
		if (current_wpm >= TAP_SPEED && current_wpm < SURRENDER_SPEED) {
			current_tap_frame = (current_tap_frame + 1) % TAP_FRAMES;
			oled_write_raw_P(hard[abs((TAP_FRAMES - 1) - current_tap_frame)], ANIM_SIZE);
		}

		// Surrender
		if (current_wpm >= SURRENDER_SPEED) {
			current_surrender_frame = (current_surrender_frame + 1) % SURRENDER_FRAMES;
			oled_write_raw_P(surrender[abs((SURRENDER_FRAMES - 1) - current_surrender_frame)], ANIM_SIZE);
		}
	}

	if (current_wpm != 000) {	// When the user starts typing
		oled_on(); // Not essential but turns on animation OLED with any alpha keypress

		// Changing animation speed depending on WPM
		if (current_wpm >= 40 && current_wpm < 60) {			// 6 fps
			if (timer_elapsed32(anim_timer) > MEDIUM_FRAME_DURATION) {
				anim_timer = timer_read32();
				animation_phase();
			}
		} else if (current_wpm >= 60 && current_wpm < 80) {		// 8 fps
			if (timer_elapsed32(anim_timer) > FAST_FRAME_DURATION) {
				anim_timer = timer_read32();
				animation_phase();
			}
		} else if (current_wpm >= 80 && current_wpm < 300) {	// 10 fps
			if (timer_elapsed32(anim_timer) > FASTEST_FRAME_DURATION) {
				anim_timer = timer_read32();
				animation_phase();
			}
		} else {												// 5 fps
			if (timer_elapsed32(anim_timer) > DEFAULT_FRAME_DURATION) {
				anim_timer = timer_read32();
				animation_phase();
			}
		}

		// Resetting sleep timer
		anim_sleep = timer_read32();
	} else {					// When the user is not typing
		// After 5 minutes, OLED turns off
		if (timer_elapsed32(anim_sleep) > OLED_TIMEOUT) {
			oled_off();
		} else {
			if (timer_elapsed32(anim_timer) > DEFAULT_FRAME_DURATION) {
				anim_timer = timer_read32();
				animation_phase();
			}
		}
	}
}

void oled_task_user(void) {
	render_anim();
	oled_set_cursor(0, 6);
	sprintf(wpm_str, " WPM: %03d", get_current_wpm());
	oled_write(wpm_str, false);
}

#endif

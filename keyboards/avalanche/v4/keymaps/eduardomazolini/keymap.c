// Copyright 2022 Vitaly Volkov (@vlkv)
// SPDX-License-Identifier: GPL-2.0-or-later

/*
qmk compile -kb avalanche/v4 -km default
qmk flash -kb avalanche/v4 -km default
*/

#include QMK_KEYBOARD_H
// ──────────────────────────────────────────────
// LED config — gerado a partir do led_map do keyboard.json
// ──────────────────────────────────────────────
led_config_t g_led_config = {
    { // Matrix map: [row][col] → índice do LED físico (255 = sem LED)
        {  28,  30,  31,  29,  24,  25, 255 },
        {  23,  18,  14,  15,  16,  17, 255 },
        {   7,   8,   4,   3,   2,   1,  41 },
        {  39,  40,  45,  44,  43,  42,  36 },
        {   6, 255,  46,  55,  56,  57,  58 },
        {  19,  20,  21,  22,  27,  26, 255 },
        {   5,  13,   9,  10,  11,  12, 255 },
        {  37,  35,  33,  32,  34,   0,  47 },
        {  51,  52,  53,  54,  50,  49,  48 },
        {  38, 255,  63,  59,  60,  61,  62 },
    },
    { // Posições físicas: {x, y} — x: 0-224, y: 0-64
        {149, 30}, // LED  0
        { 75, 30}, // LED  1
        { 62, 29}, // LED  2
        { 50, 25}, // LED  3
        { 37, 29}, // LED  4
        {212, 22}, // LED  5
        {  0, 36}, // LED  6
        { 12, 35}, // LED  7
        { 25, 35}, // LED  8
        {187, 16}, // LED  9
        {174, 13}, // LED 10
        {162, 16}, // LED 11
        {149, 17}, // LED 12
        {199, 22}, // LED 13
        { 37, 16}, // LED 14
        { 50, 13}, // LED 15
        { 62, 16}, // LED 16
        { 75, 17}, // LED 17
        { 25, 22}, // LED 18
        {212, 10}, // LED 19
        {199, 10}, // LED 20
        {187,  3}, // LED 21
        {174,  0}, // LED 22
        { 12, 22}, // LED 23
        { 62,  3}, // LED 24
        { 75,  4}, // LED 25
        {149,  4}, // LED 26
        {162,  3}, // LED 27
        { 12, 10}, // LED 28
        { 50,  0}, // LED 29
        { 25, 10}, // LED 30
        { 37,  3}, // LED 31
        {174, 25}, // LED 32
        {187, 29}, // LED 33
        {162, 29}, // LED 34
        {199, 35}, // LED 35
        { 87, 47}, // LED 36
        {212, 35}, // LED 37
        {224, 36}, // LED 38
        { 12, 48}, // LED 39
        { 25, 48}, // LED 40
        {100, 51}, // LED 41
        { 75, 42}, // LED 42
        { 62, 41}, // LED 43
        { 50, 38}, // LED 44
        { 37, 41}, // LED 45
        { 42, 58}, // LED 46
        {124, 51}, // LED 47
        {137, 47}, // LED 48
        {149, 42}, // LED 49
        {162, 41}, // LED 50
        {212, 48}, // LED 51
        {199, 48}, // LED 52
        {187, 41}, // LED 53
        {174, 38}, // LED 54
        { 57, 56}, // LED 55
        { 72, 56}, // LED 56
        { 87, 60}, // LED 57
        {100, 64}, // LED 58
        {167, 56}, // LED 59
        {152, 56}, // LED 60
        {137, 60}, // LED 61
        {124, 64}, // LED 62
        {182, 58}, // LED 63
    },
    { // Flags: 4 = LED_FLAG_KEYLIGHT (per-key)
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    }
};

// ──────────────────────────────────────────────
// Layers
// ──────────────────────────────────────────────
enum layer {
    LAYER_0,
    LAYER_1,
    LAYER_2,
};

#define FN_1    MO(LAYER_1)
#define LFN_2   LT(LAYER_2, KC_GRV)
#define RFN_2   MO(LAYER_2)
#define LFN_3   LSFT_T(KC_EQL)
#define RFN_3   RSFT_T(KC_MINS)
#define EN_LALT LALT_T(KC_ENT)
#define EN_RALT RALT_T(KC_ENT)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
                 KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                                       KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_BSLS,
                 KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                                        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
        KC_CAPS, KC_LSFT, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                                        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_RBRC,
                 KC_LCTL, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_DEL,  KC_LGUI, KC_INS,  KC_BSPC, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RCTL,
                                            KC_MUTE, LFN_2,   LFN_3,   KC_SPC,  EN_LALT, EN_RALT, FN_1,    RFN_3,   RFN_2,   KC_PSCR
    ),
    [LAYER_1] = LAYOUT(
                 _______, KC_F11,  KC_F12,  XXXXXXX, XXXXXXX, XXXXXXX,                                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_F11,
                 _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_BSPC,                                     KC_DEL,  KC_HOME, KC_UP,   KC_END,  KC_PGUP, XXXXXXX,
        _______, _______, KC_APP,  XXXXXXX, XXXXXXX, XXXXXXX, KC_DEL,                                      KC_BSPC, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, XXXXXXX, KC_F12,
                 _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, _______, _______, XXXXXXX, KC_PGUP, KC_PGDN, XXXXXXX, XXXXXXX, _______,
                                            _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
    [LAYER_2] = LAYOUT(
                 _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                                     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                 _______, KC_0,    KC_1,    KC_2,    KC_3,    KC_BSPC,                                     KC_DEL,  KC_EXLM, KC_AT,   KC_HASH, XXXXXXX, KC_LPRN,
        _______, _______, KC_0,    KC_4,    KC_5,    KC_6,    KC_DEL,                                      KC_BSPC, KC_DLR,  KC_PERC, KC_CIRC, XXXXXXX, XXXXXXX, KC_RPRN,
                 _______, KC_0,    KC_7,    KC_8,    KC_9,    KC_DOT,  _______, _______, _______, _______, XXXXXXX, KC_AMPR, KC_ASTR, XXXXXXX, XXXXXXX, _______,
                                            _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
};

// ──────────────────────────────────────────────
// RGB Matrix — indicadores por layer e CapsLock
// ──────────────────────────────────────────────

// Encontra o LED físico correspondente a uma posição [row][col] da matrix
// Retorna 255 se não houver LED nessa posição
static uint8_t led_for_key(uint8_t row, uint8_t col) {
    return g_led_config.matrix_co[row][col];
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // CapsLock
    if (host_keyboard_led_state().caps_lock) {
        uint8_t caps_led = led_for_key(4, 0);
        if (caps_led != 255 && caps_led >= led_min && caps_led < led_max) {
            rgb_matrix_set_color(caps_led, RGB_WHITE);
        }
    }

    // Tecla pressionada — roda por cima de tudo, inclusive layer colors
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            if (matrix_is_on(row, col)) {
                uint8_t led = led_for_key(row, col);
                if (led != 255 && led >= led_min && led < led_max) {
                    rgb_matrix_set_color(led, RGB_WHITE);
                }
            }
        }
    }

    // Indicador de layer
    uint8_t layer = get_highest_layer(layer_state);
    if (layer == LAYER_0) return false;

    RGB color = (layer == LAYER_1) ? (RGB){RGB_RED} : (RGB){RGB_BLUE};

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t kc = keymap_key_to_keycode(layer, (keypos_t){col, row});
            if (kc != KC_TRNS && kc != KC_NO) {
                uint8_t led = led_for_key(row, col);
                if (led != 255 && led >= led_min && led < led_max) {
                    // Só pinta se não está pressionada (preserva o branco)
                    if (!matrix_is_on(row, col)) {
                        rgb_matrix_set_color(led, color.r, color.g, color.b);
                    }
                }
            }
        }
    }
    return false;
}


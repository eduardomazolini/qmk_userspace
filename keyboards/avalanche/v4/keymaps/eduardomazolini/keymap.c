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
    {
        {27, 26,  18, 17,  8,  7, 255},
        {28, 25,  19, 16,  9,  6, 255},
        {29, 24,  20, 15, 10,  5,  2},
        {30, 23,  21, 14, 11,  4,  3},
        {31, 255, 22, 13, 12,  0,  1},

        {59,  58, 50, 49, 40, 39, 255},
        {60,  57, 51, 48, 41, 38, 255},
        {61,  56, 52, 47, 42, 37,  34},
        {62,  55, 53, 46, 43, 36,  35},
        {63, 255, 54, 45, 44, 32,  33}
    },
    { // Posições físicas: {x, y} — x: 0-224, y: 0-64
        { 84, 58}, // LED  0
        { 98, 64}, // LED  1
        { 106, 54}, // LED  2
        { 90, 46}, // LED  3
        { 72, 39}, // LED  4
        { 72, 27}, // LED  5
        { 72, 15}, // LED  6
        { 72,  3}, // LED  7
        { 60,  1}, // LED  8
        { 60, 13}, // LED  9
        { 60, 15}, // LED 10
        { 60, 27}, // LED 11
        { 68, 52}, // LED 12
        { 56, 49}, // LED 13
        { 48, 36}, // LED 14
        { 48, 24}, // LED 15
        { 48, 12}, // LED 16
        { 48,  0}, // LED 17
        { 36,  1}, // LED 18
        { 36, 13}, // LED 19
        { 36, 25}, // LED 20
        { 36, 37}, // LED 21
        { 42, 49}, // LED 22
        { 24, 40}, // LED 23
        { 24, 28}, // LED 24
        { 24, 16}, // LED 25
        { 24,  4}, // LED 26
        { 12,  4}, // LED 27
        { 12, 16}, // LED 28
        { 12, 28}, // LED 29
        { 12, 40}, // LED 30
        { 00, 28}, // LED 31

        //Direito
        {140, 58}, // LED 32
        {126, 64}, // LED 33
        {118, 54}, // LED 34
        {134, 46}, // LED 35
        {152, 39}, // LED 36
        {152, 27}, // LED 37
        {152, 15}, // LED 38
        {152,  3}, // LED 39
        {164,  1}, // LED 40
        {164, 13}, // LED 41
        {164, 15}, // LED 42
        {164, 27}, // LED 43
        {156, 52}, // LED 44
        {168, 49}, // LED 45
        {176, 36}, // LED 46
        {176, 24}, // LED 47
        {176, 12}, // LED 48
        {176,  0}, // LED 49
        {188,  1}, // LED 50
        {188, 13}, // LED 51
        {188, 25}, // LED 52
        {188, 37}, // LED 53
        {180, 49}, // LED 54
        {200, 40}, // LED 55
        {200, 28}, // LED 56
        {200, 16}, // LED 57
        {200,  4}, // LED 58
        {212,  4}, // LED 59
        {212, 16}, // LED 60
        {212, 28}, // LED 61
        {212, 40}, // LED 62
        {224, 28}, // LED 63
    },
    { // Flags: 0x04 = LED_FLAG_KEYLIGHT (per-key)
             // 0x08 = LED_FLAG_INDICATOR
             // 0x02 = LED_FLAG_UNDERGLOW
             // 0x01 = LED_FLAG_MODIFIER
        4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 
        4, 4, 4, 4, 4, 4, 4, 
        4,    4, 4, 4, 4, 4,

        4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4,
        4,    4, 4, 4, 4, 4,
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

#define DESKTOP_1 LCTL(KC_F1)
#define DESKTOP_2 LCTL(KC_F2)
#define DESKTOP_3 LCTL(KC_F3)

#define MUTE_MIC LSFT(LCTL(LALT(KC_ESCAPE)))

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
                 KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSLS,
                 KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                                        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
        KC_CAPS, KC_LSFT, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                                        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_RBRC,
                 KC_LCTL, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_MUTE, KC_LGUI, KC_INS,  KC_PSCR, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_INT1,
                                            KC_DEL,  LFN_2,   LFN_3,   KC_SPC,  EN_LALT, EN_RALT, FN_1,    RFN_3,   RFN_2,  KC_BSPC
    ),
    [LAYER_1] = LAYOUT(
                 _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                                       KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,   KC_F11,
                 _______, KC_F11,  KC_F12,  XXXXXXX, XXXXXXX, KC_BSPC,                                     KC_DEL,  KC_HOME, KC_UP,   KC_END,  KC_PGUP, XXXXXXX,
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

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] =   { ENCODER_CCW_CW(KC_VOLD, KC_VOLU),           ENCODER_CCW_CW(KC_PGUP, KC_PGDN)  },
    [1] =   { ENCODER_CCW_CW(RGB_HUD, RGB_HUI),           ENCODER_CCW_CW(RGB_SAD, RGB_SAI)  },
    [2] =   { ENCODER_CCW_CW(RGB_VAD, RGB_VAI),           ENCODER_CCW_CW(RGB_SPD, RGB_SPI)  },
    //                  Encoder 1                                     Encoder 2
};
#endif

// ──────────────────────────────────────────────
// RGB Matrix — indicadores por layer e CapsLock
// ──────────────────────────────────────────────

// Encontra o LED físico correspondente a uma posição [row][col] da matrix
// Retorna 255 se não houver LED nessa posição
static uint8_t led_for_key(uint8_t row, uint8_t col) {
    uint8_t led = g_led_config.matrix_co[row][col];
    //imprime teste
    //char buf[16];
    //snprintf(buf, sizeof(buf), "%d, %d, %d\n", row, col, led);
    //oled_write(buf, false);
    return led;
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
                //imprime teste
            //    char buf[16];
            //    snprintf(buf, sizeof(buf), "%d, %d, %d\n", led_min, led_max, led);
            //    oled_write(buf, false);
                if (led != 255 && led >= led_min && led < led_max) {
                    rgb_matrix_set_color(led, RGB_WHITE);
                }
            }
        }
    }

    // Indicador de layer
    uint8_t layer = get_highest_layer(layer_state);
    if (layer == LAYER_0) return false;

    RGB layer_color = (layer == LAYER_1) ? (RGB){RGB_GREEN} : (RGB){RGB_BLUE};
    RGB arrow_color = (RGB){RGB_RED};
    RGB color = layer_color;


       for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint16_t kc = keymap_key_to_keycode(layer, (keypos_t){col, row});
            if (kc == KC_UP || kc == KC_DOWN || kc == KC_LEFT || kc == KC_RIGHT){
                color = arrow_color;
            } else {
                color = layer_color;
            }

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


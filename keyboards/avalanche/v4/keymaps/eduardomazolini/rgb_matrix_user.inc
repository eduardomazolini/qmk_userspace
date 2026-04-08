// !!! DO NOT ADD #pragma once !!! //

#define RGB_MATRIX_EFFECT_MY_COOL_EFFECT
RGB_MATRIX_EFFECT(MY_COOL_EFFECT)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#define IN_LIST(val, list) in_list_impl(val, list, sizeof(list))

static bool in_list_impl(uint8_t val, const uint8_t *list, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        if (list[i] == val) return true;
    }
    return false;
}

static bool MY_COOL_EFFECT(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    static const uint8_t blue_list[]   = { 2,  3, 34, 35,  4,  5,  6,  7,  8,  9, 10, 11, 36, 37, 38, 39, 40, 41, 42, 43};
    static const uint8_t yellow_list[] = {14, 15, 16, 17, 46, 47, 48, 49};
    static const uint8_t orange_list[] = {18, 19, 20, 21, 50, 51, 52, 53};
    static const uint8_t red_list[]    = {23, 24, 25, 26, 27, 28, 29, 30, 31, 55, 56, 57, 58, 59, 60, 61, 62, 63};
    static const uint8_t green_list[]  = { 0,  1, 12, 13, 22, 32, 33, 44, 45, 54};

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        if      (IN_LIST(i, blue_list))   rgb_matrix_set_color(i, RGB_BLUE);
        else if (IN_LIST(i, yellow_list)) rgb_matrix_set_color(i, RGB_YELLOW);
        else if (IN_LIST(i, orange_list)) rgb_matrix_set_color(i, RGB_ORANGE);
        else if (IN_LIST(i, red_list))    rgb_matrix_set_color(i, RGB_RED);
        else if (IN_LIST(i, green_list))  rgb_matrix_set_color(i, RGB_GREEN);
        else                              rgb_matrix_set_color(i, RGB_WHITE);
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
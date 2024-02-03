/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_key_press_on_language

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include <zmk/language.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct behavior_key_press_on_lang_config {};

struct behavior_key_press_on_lang_data {};

static int behavior_key_press_on_lang_init(const struct device *dev) { return 0; };

static int key_press_on_lang_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                                    struct zmk_behavior_binding_event event) {
    LOG_DBG("CURRENT_LANG current_lang %d", zmk_language_state());
    return ZMK_BEHAVIOR_OPAQUE;
}

static int key_press_on_lang_keymap_binding_released(struct zmk_behavior_binding *binding,
                                                     struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_key_press_on_lang_driver_api = {
    .binding_pressed = key_press_on_lang_keymap_binding_pressed,
    .binding_released = key_press_on_lang_keymap_binding_released};

#define KEY_PRESS_ON_LANG_INST(n)                                                                  \
    static struct behavior_key_press_on_lang_data behavior_key_press_on_lang_data_##n = {};        \
    static struct behavior_key_press_on_lang_config behavior_key_press_on_lang_config_##n = {};    \
    BEHAVIOR_DT_INST_DEFINE(                                                                       \
        n, behavior_key_press_on_lang_init, NULL, &behavior_key_press_on_lang_data_##n,            \
        &behavior_key_press_on_lang_config_##n, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,  \
        &behavior_key_press_on_lang_driver_api);

DT_INST_FOREACH_STATUS_OKAY(KEY_PRESS_ON_LANG_INST)

/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_language

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include <zmk/events/keycode_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct behavior_lang_config {
    struct zmk_behavior_binding behavior;
    uint8_t layer;
};

struct behavior_lang_data {};

static int behavior_lang_init(const struct device *dev) { return 0; };

static int lang_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_lang_config *config = dev->config;

    LOG_DBG("LANG position %d default_layer %d", event.position, zmk_keymap_layer_default());
    LOG_DBG("LANG position %d config_layer %d", event.position, config->layer);
    LOG_DBG("LANG position %d config_layer_active %d", event.position,
            zmk_keymap_layer_active(config->layer));
    if (!zmk_keymap_layer_active(config->layer)) {
        zmk_behavior_queue_add(event.position, config->behavior, true, 30);
        zmk_behavior_queue_add(event.position, config->behavior, false, 0);
        zmk_keymap_layer_to(config->layer);
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static int lang_keymap_binding_released(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_lang_driver_api = {
    .binding_pressed = lang_keymap_binding_pressed,
    .binding_released = lang_keymap_binding_released};

#define LANG_INST(n)                                                                               \
    static struct behavior_lang_data behavior_lang_data_##n = {};                                  \
    static struct behavior_lang_config behavior_lang_config_##n = {                                \
        .behavior = ZMK_KEYMAP_EXTRACT_BINDING(0, DT_DRV_INST(n)),                                 \
        .layer = DT_INST_PROP(n, layer),                                                           \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_lang_init, NULL, &behavior_lang_data_##n,                  \
                            &behavior_lang_config_##n, APPLICATION,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_lang_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LANG_INST)

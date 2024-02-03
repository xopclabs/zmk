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
    uint8_t layers_count;
    uint8_t layers[];
};

struct behavior_lang_data {
    uint8_t current_lang;
};

static int behavior_lang_init(const struct device *dev) {
    struct behavior_lang_data *data = dev->data;
    data->current_lang = 0;
    return 0;
};

static int get_number_of_switches(struct behavior_lang_config *config,
                                  struct behavior_lang_data *data, uint8_t target_lang) {
    if (data->current_lang == target_lang)
        return 0;
    if (data->current_lang < target_lang) {
        return target_lang - data->current_lang;
    } else {
        return config->layers_count - data->current_lang + target_lang;
    }
};

static int lang_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_lang_config *config = dev->config;
    struct behavior_lang_data *data = dev->data;

    const uint8_t number_of_switches = get_number_of_switches(config, data, binding->param1);
    LOG_DBG("LANG current_lang %d target_lang %d number_of_switches %d", data->current_lang,
            binding->param1, number_of_switches);
    // Switch needed number of times
    if (number_of_switches > 0) {
        for (uint8_t i = 0; i < number_of_switches; i++) {
            zmk_behavior_queue_add(event.position, config->behavior, true, 0);
            zmk_behavior_queue_add(event.position, config->behavior, false, 0);
            LOG_DBG("LANG switch");
        }
        data->current_lang = binding->param1;
        zmk_keymap_layer_to(binding->param1);
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
        .layers = DT_INST_PROP(n, layers),                                                         \
        .layers_count = DT_INST_PROP_LEN(n, layers),                                               \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_lang_init, NULL, &behavior_lang_data_##n,                  \
                            &behavior_lang_config_##n, APPLICATION,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_lang_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LANG_INST)

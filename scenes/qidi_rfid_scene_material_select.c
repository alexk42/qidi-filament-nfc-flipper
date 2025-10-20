#include "../qidi_rfid_app_i.h"

static void qidi_rfid_scene_material_select_var_list_change_callback(VariableItem* item) {
    QidiRfidApp* app = variable_item_get_context(item);
    app->material_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, qidi_materials[app->material_index].name);
}

static void qidi_rfid_scene_material_select_var_list_enter_callback(void* context, uint32_t index) {
    UNUSED(index);
    QidiRfidApp* app = context;
    scene_manager_next_scene(app->scene_manager, QidiRfidSceneColorSelect);
}

void qidi_rfid_scene_material_select_on_enter(void* context) {
    QidiRfidApp* app = context;
    VariableItemList* var_list = app->variable_item_list;

    VariableItem* item = variable_item_list_add(
        var_list,
        "Material:",
        QIDI_MATERIAL_COUNT,
        qidi_rfid_scene_material_select_var_list_change_callback,
        app);

    variable_item_set_current_value_index(item, app->material_index);
    variable_item_set_current_value_text(item, qidi_materials[app->material_index].name);

    variable_item_list_set_enter_callback(
        var_list, qidi_rfid_scene_material_select_var_list_enter_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewVariableItemList);
}

bool qidi_rfid_scene_material_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void qidi_rfid_scene_material_select_on_exit(void* context) {
    QidiRfidApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}

#include "../qidi_rfid_app_i.h"

typedef enum {
    SubmenuIndexRead,
    SubmenuIndexWrite,
    SubmenuIndexAbout,
} SubmenuIndex;

static void qidi_rfid_scene_start_submenu_callback(void* context, uint32_t index) {
    QidiRfidApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void qidi_rfid_scene_start_on_enter(void* context) {
    QidiRfidApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu, "Read Tag", SubmenuIndexRead, qidi_rfid_scene_start_submenu_callback, app);
    submenu_add_item(
        submenu, "Write Tag", SubmenuIndexWrite, qidi_rfid_scene_start_submenu_callback, app);
    submenu_add_item(
        submenu, "About", SubmenuIndexAbout, qidi_rfid_scene_start_submenu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewSubmenu);
}

bool qidi_rfid_scene_start_on_event(void* context, SceneManagerEvent event) {
    QidiRfidApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            scene_manager_next_scene(app->scene_manager, QidiRfidSceneRead);
            consumed = true;
        } else if(event.event == SubmenuIndexWrite) {
            scene_manager_next_scene(app->scene_manager, QidiRfidSceneMaterialSelect);
            consumed = true;
        } else if(event.event == SubmenuIndexAbout) {
            scene_manager_next_scene(app->scene_manager, QidiRfidSceneAbout);
            consumed = true;
        }
    }

    return consumed;
}

void qidi_rfid_scene_start_on_exit(void* context) {
    QidiRfidApp* app = context;
    submenu_reset(app->submenu);
}

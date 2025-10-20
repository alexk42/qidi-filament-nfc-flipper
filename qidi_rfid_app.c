#include "qidi_rfid_app_i.h"

static bool qidi_rfid_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    QidiRfidApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool qidi_rfid_back_event_callback(void* context) {
    furi_assert(context);
    QidiRfidApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static QidiRfidApp* qidi_rfid_app_alloc(void) {
    QidiRfidApp* app = malloc(sizeof(QidiRfidApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->nfc = nfc_alloc();

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&qidi_rfid_scene_handlers, app);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, qidi_rfid_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, qidi_rfid_back_event_callback);

    // Submenu
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, QidiRfidViewSubmenu, submenu_get_view(app->submenu));

    // Popup
    app->popup = popup_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, QidiRfidViewPopup, popup_get_view(app->popup));

    // Variable Item List
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        QidiRfidViewVariableItemList,
        variable_item_list_get_view(app->variable_item_list));

    // Widget
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        QidiRfidViewWidget,
        widget_get_view(app->widget));

    // Initialize data
    app->material_index = 0;
    app->color_index = 0;
    app->worker_thread = NULL;
    app->is_writing = false;

    return app;
}

static void qidi_rfid_app_free(QidiRfidApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewSubmenu);
    submenu_free(app->submenu);

    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewPopup);
    popup_free(app->popup);

    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewVariableItemList);
    variable_item_list_free(app->variable_item_list);

    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewWidget);
    widget_free(app->widget);

    // View Dispatcher and Scene Manager
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    nfc_free(app->nfc);

    free(app);
}

int32_t qidi_rfid_app(void* p) {
    UNUSED(p);

    QidiRfidApp* app = qidi_rfid_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, QidiRfidSceneStart);

    view_dispatcher_run(app->view_dispatcher);

    qidi_rfid_app_free(app);

    return 0;
}

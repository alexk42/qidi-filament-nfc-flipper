#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification_messages.h>
#include <lib/nfc/nfc.h>

#include "qidi_rfid_materials.h"
#include "qidi_rfid_colors.h"
#include "qidi_rfid_nfc.h"

#define TAG "QidiRFID"

typedef enum {
    QidiRfidViewSubmenu,
    QidiRfidViewMaterialSelect,
    QidiRfidViewColorSelect,
    QidiRfidViewPopup,
} QidiRfidView;

typedef enum {
    QidiRfidEventSuccess = 100,
    QidiRfidEventFail,
} QidiRfidEvent;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    VariableItemList* material_list;
    VariableItemList* color_list;
    Popup* popup;
    NotificationApp* notifications;
    Nfc* nfc;
    FuriThread* worker_thread;

    uint8_t material_index;
    uint8_t color_index;
    bool is_writing;
} QidiRfidApp;

// Worker thread for NFC operations
static int32_t qidi_worker_thread(void* context) {
    QidiRfidApp* app = context;
    bool success = false;

    if(app->is_writing) {
        QidiTagData tag_data = {
            .material_code = qidi_materials[app->material_index].code,
            .color_code = qidi_colors[app->color_index].code,
            .manufacturer_code = 1
        };
        success = qidi_rfid_write_tag(app->nfc, &tag_data);
    } else {
        QidiTagData tag_data;
        success = qidi_rfid_read_tag(app->nfc, &tag_data);
    }

    view_dispatcher_send_custom_event(
        app->view_dispatcher,
        success ? QidiRfidEventSuccess : QidiRfidEventFail);

    return 0;
}

// Custom event callback
static bool custom_event_callback(void* context, uint32_t event) {
    QidiRfidApp* app = context;

    notification_message(app->notifications, &sequence_blink_stop);

    if(event == QidiRfidEventSuccess) {
        if(app->is_writing) {
            popup_set_header(app->popup, "Write Success!", 64, 10, AlignCenter, AlignTop);
            char text[64];
            snprintf(text, sizeof(text), "%s\n%s",
                     qidi_materials[app->material_index].name,
                     qidi_colors[app->color_index].name);
            popup_set_text(app->popup, text, 64, 25, AlignCenter, AlignTop);
        } else {
            popup_set_header(app->popup, "Read Success!", 64, 10, AlignCenter, AlignTop);
            popup_set_text(app->popup, "Check logs for data", 64, 25, AlignCenter, AlignTop);
        }
        notification_message(app->notifications, &sequence_success);
        return true;
    } else if(event == QidiRfidEventFail) {
        popup_set_header(app->popup, "Failed", 64, 10, AlignCenter, AlignTop);
        popup_set_text(app->popup, "No tag detected or\nauthentication failed", 64, 25, AlignCenter, AlignTop);
        notification_message(app->notifications, &sequence_error);
        return true;
    }

    return false;
}

// Material/Color callbacks
static void material_change_callback(VariableItem* item) {
    QidiRfidApp* app = variable_item_get_context(item);
    app->material_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, qidi_materials[app->material_index].name);
}

static void color_change_callback(VariableItem* item) {
    QidiRfidApp* app = variable_item_get_context(item);
    app->color_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, qidi_colors[app->color_index].name);
}

// Start read/write operation
static void start_nfc_operation(QidiRfidApp* app, bool is_write) {
    app->is_writing = is_write;

    popup_set_header(app->popup, is_write ? "Writing" : "Reading", 64, 10, AlignCenter, AlignTop);
    popup_set_text(app->popup, "Hold card next to\nFlipper's back", 64, 25, AlignCenter, AlignTop);
    view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewPopup);

    notification_message(app->notifications, &sequence_blink_start_cyan);

    // Start worker thread
    app->worker_thread = furi_thread_alloc();
    furi_thread_set_name(app->worker_thread, "QidiNfcWorker");
    furi_thread_set_stack_size(app->worker_thread, 2048);
    furi_thread_set_context(app->worker_thread, app);
    furi_thread_set_callback(app->worker_thread, qidi_worker_thread);
    furi_thread_start(app->worker_thread);
}

// Submenu callback
static void submenu_callback(void* context, uint32_t index) {
    QidiRfidApp* app = context;
    if(index == 0) { // Read
        start_nfc_operation(app, false);
    } else if(index == 1) { // Write
        view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewMaterialSelect);
    } else { // About
        popup_set_header(app->popup, "QIDI RFID v1.0", 64, 10, AlignCenter, AlignTop);
        popup_set_text(app->popup,
            "Program RFID tags for\n"
            "QIDI Box filament system\n\n"
            "Sector: 1, Block: 0\n"
            "13.56MHz MIFARE Classic",
            64, 25, AlignCenter, AlignTop);
        view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewPopup);
    }
}

// Navigation callbacks
static uint32_t exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static uint32_t back_to_submenu_callback(void* context) {
    QidiRfidApp* app = context;
    // Clean up worker thread if running
    if(app->worker_thread) {
        furi_thread_join(app->worker_thread);
        furi_thread_free(app->worker_thread);
        app->worker_thread = NULL;
    }
    return QidiRfidViewSubmenu;
}

static uint32_t material_to_color_callback(void* context) {
    UNUSED(context);
    return QidiRfidViewColorSelect;
}

static void material_enter_callback(void* context, uint32_t index) {
    UNUSED(index);
    QidiRfidApp* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewColorSelect);
}

static void color_enter_callback(void* context, uint32_t index) {
    UNUSED(index);
    QidiRfidApp* app = context;
    start_nfc_operation(app, true);
}

// Alloc/Free
static QidiRfidApp* qidi_rfid_app_alloc() {
    QidiRfidApp* app = malloc(sizeof(QidiRfidApp));
    app->material_index = 0;
    app->color_index = 0;
    app->worker_thread = NULL;
    app->is_writing = false;

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, custom_event_callback);

    app->nfc = nfc_alloc();
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "Read Tag", 0, submenu_callback, app);
    submenu_add_item(app->submenu, "Write Tag", 1, submenu_callback, app);
    submenu_add_item(app->submenu, "About", 2, submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), exit_callback);
    view_dispatcher_add_view(app->view_dispatcher, QidiRfidViewSubmenu, submenu_get_view(app->submenu));

    // Material list
    app->material_list = variable_item_list_alloc();
    VariableItem* item = variable_item_list_add(
        app->material_list, "Material:", QIDI_MATERIAL_COUNT, material_change_callback, app);
    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, qidi_materials[0].name);
    view_set_previous_callback(variable_item_list_get_view(app->material_list), back_to_submenu_callback);
    variable_item_list_set_enter_callback(app->material_list, material_enter_callback, app);
    view_dispatcher_add_view(app->view_dispatcher, QidiRfidViewMaterialSelect, variable_item_list_get_view(app->material_list));

    // Color list
    app->color_list = variable_item_list_alloc();
    item = variable_item_list_add(
        app->color_list, "Color:", QIDI_COLOR_COUNT, color_change_callback, app);
    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, qidi_colors[0].name);
    view_set_previous_callback(variable_item_list_get_view(app->color_list), material_to_color_callback);
    variable_item_list_set_enter_callback(app->color_list, color_enter_callback, app);
    view_dispatcher_add_view(app->view_dispatcher, QidiRfidViewColorSelect, variable_item_list_get_view(app->color_list));

    // Popup
    app->popup = popup_alloc();
    view_set_previous_callback(popup_get_view(app->popup), back_to_submenu_callback);
    view_dispatcher_add_view(app->view_dispatcher, QidiRfidViewPopup, popup_get_view(app->popup));

    return app;
}

static void qidi_rfid_app_free(QidiRfidApp* app) {
    // Clean up worker thread if running
    if(app->worker_thread) {
        furi_thread_join(app->worker_thread);
        furi_thread_free(app->worker_thread);
    }

    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewMaterialSelect);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewColorSelect);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewPopup);

    submenu_free(app->submenu);
    variable_item_list_free(app->material_list);
    variable_item_list_free(app->color_list);
    popup_free(app->popup);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_NOTIFICATION);
    nfc_free(app->nfc);
    free(app);
}

// Entry point
int32_t qidi_rfid_app(void* p) {
    UNUSED(p);

    QidiRfidApp* app = qidi_rfid_app_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewSubmenu);
    view_dispatcher_run(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    qidi_rfid_app_free(app);
    return 0;
}

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification_messages.h>
#include <lib/nfc/nfc.h>

#include "qidi_rfid_materials.h"
#include "qidi_rfid_colors.h"
#include "qidi_rfid_nfc.h"
#include "qidi_rfid_worker.h"

#define TAG "QidiRFID"

typedef enum {
    QidiRfidViewSubmenu,
    QidiRfidViewMaterialSelect,
    QidiRfidViewColorSelect,
    QidiRfidViewWidget,
} QidiRfidView;

typedef enum {
    QidiRfidCustomEventWorkerResult,
} QidiRfidCustomEvent;

typedef enum {
    QidiRfidModeRead,
    QidiRfidModeWrite,
    QidiRfidModeAbout,
} QidiRfidMode;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    VariableItemList* material_list;
    VariableItemList* color_list;
    Widget* widget;
    NotificationApp* notifications;
    Nfc* nfc;
    QidiRfidWorker* worker;

    uint8_t material_index;
    uint8_t color_index;
    QidiRfidMode current_mode;
} QidiRfidApp;

// Material selection callback
static void material_change_callback(VariableItem* item) {
    QidiRfidApp* app = variable_item_get_context(item);
    app->material_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, qidi_materials[app->material_index].name);
}

// Color selection callback
static void color_change_callback(VariableItem* item) {
    QidiRfidApp* app = variable_item_get_context(item);
    app->color_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, qidi_colors[app->color_index].name);
}

// Worker callback
static void worker_callback(QidiRfidWorkerEvent event, void* context) {
    QidiRfidApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event.type);
}

// Custom event handler
static bool custom_event_callback(void* context, uint32_t event) {
    QidiRfidApp* app = context;
    if(!app || !app->widget || !app->notifications) return false;

    if(event == QidiRfidWorkerEventReadSuccess) {
        notification_message(app->notifications, &sequence_blink_stop);
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, "Read Success!");
        widget_add_string_element(app->widget, 64, 40, AlignCenter, AlignCenter, FontSecondary, "Check logs");
        notification_message(app->notifications, &sequence_success);
        return true;
    } else if(event == QidiRfidWorkerEventReadFail) {
        notification_message(app->notifications, &sequence_blink_stop);
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, "Read Failed");
        widget_add_string_element(app->widget, 64, 35, AlignCenter, AlignCenter, FontSecondary, "No tag detected or");
        widget_add_string_element(app->widget, 64, 45, AlignCenter, AlignCenter, FontSecondary, "auth failed");
        notification_message(app->notifications, &sequence_error);
        return true;
    } else if(event == QidiRfidWorkerEventWriteSuccess) {
        notification_message(app->notifications, &sequence_blink_stop);
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, "Write Success!");
        widget_add_string_element(app->widget, 64, 40, AlignCenter, AlignCenter, FontSecondary, qidi_materials[app->material_index].name);
        widget_add_string_element(app->widget, 64, 50, AlignCenter, AlignCenter, FontSecondary, qidi_colors[app->color_index].name);
        notification_message(app->notifications, &sequence_success);
        return true;
    } else if(event == QidiRfidWorkerEventWriteFail) {
        notification_message(app->notifications, &sequence_blink_stop);
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 64, 30, AlignCenter, AlignCenter, FontPrimary, "Write Failed!");
        notification_message(app->notifications, &sequence_error);
        return true;
    }

    return false;
}

// Widget enter callback - triggers NFC operation after view is shown
static void widget_enter_callback(void* context) {
    QidiRfidApp* app = context;
    if(!app || !app->widget || !app->worker) return;

    if(app->current_mode == QidiRfidModeRead) {
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 64, 10, AlignCenter, AlignCenter, FontPrimary, "Reading");
        widget_add_string_element(app->widget, 64, 32, AlignCenter, AlignCenter, FontSecondary, "Hold card next to");
        widget_add_string_element(app->widget, 64, 44, AlignCenter, AlignCenter, FontSecondary, "Flipper's back");

        // Start blinking notification
        if(app->notifications) {
            notification_message(app->notifications, &sequence_blink_start_cyan);
        }

        // Delay slightly to let UI render
        furi_delay_ms(100);
        qidi_rfid_worker_read(app->worker);

    } else if(app->current_mode == QidiRfidModeWrite) {
        widget_reset(app->widget);
        widget_add_string_element(app->widget, 64, 10, AlignCenter, AlignCenter, FontPrimary, "Writing");
        widget_add_string_element(app->widget, 64, 32, AlignCenter, AlignCenter, FontSecondary, "Hold card next to");
        widget_add_string_element(app->widget, 64, 44, AlignCenter, AlignCenter, FontSecondary, "Flipper's back");

        // Start blinking notification
        if(app->notifications) {
            notification_message(app->notifications, &sequence_blink_start_cyan);
        }

        QidiTagData tag_data = {
            .material_code = qidi_materials[app->material_index].code,
            .color_code = qidi_colors[app->color_index].code,
            .manufacturer_code = 1
        };

        // Delay slightly to let UI render
        furi_delay_ms(100);
        qidi_rfid_worker_write(app->worker, &tag_data);
    }
}

// Submenu callback
static void submenu_callback(void* context, uint32_t index) {
    QidiRfidApp* app = context;
    if(index == 0) { // Read
        app->current_mode = QidiRfidModeRead;
        view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewWidget);
    } else if(index == 1) { // Write
        view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewMaterialSelect);
    } else { // About
        app->current_mode = QidiRfidModeAbout;
        widget_reset(app->widget);
        widget_add_text_scroll_element(app->widget, 0, 0, 128, 64,
            "QIDI RFID v1.0\n\n"
            "Program RFID tags for\n"
            "QIDI Box filament system\n\n"
            "Sector: 1, Block: 0\n"
            "Chip: FM11RF08S\n"
            "13.56MHz MIFARE Classic");
        view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewWidget);
    }
}

// Navigation callbacks
static uint32_t exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static uint32_t back_to_submenu_callback(void* context) {
    UNUSED(context);
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
    app->current_mode = QidiRfidModeWrite;
    view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewWidget);
}

// Alloc app
static QidiRfidApp* qidi_rfid_app_alloc() {
    QidiRfidApp* app = malloc(sizeof(QidiRfidApp));
    app->material_index = 0;
    app->color_index = 0;
    app->current_mode = QidiRfidModeAbout;

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, custom_event_callback);

    app->nfc = nfc_alloc();
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Worker
    app->worker = qidi_rfid_worker_alloc(app->nfc);
    qidi_rfid_worker_start(app->worker, worker_callback, app);

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

    // Widget
    app->widget = widget_alloc();
    View* widget_view = widget_get_view(app->widget);
    view_set_previous_callback(widget_view, back_to_submenu_callback);
    view_set_enter_callback(widget_view, widget_enter_callback);
    view_set_context(widget_view, app);
    view_dispatcher_add_view(app->view_dispatcher, QidiRfidViewWidget, widget_view);

    return app;
}

// Free app
static void qidi_rfid_app_free(QidiRfidApp* app) {
    // Stop worker
    qidi_rfid_worker_stop(app->worker);
    qidi_rfid_worker_free(app->worker);

    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewMaterialSelect);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewColorSelect);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewWidget);

    submenu_free(app->submenu);
    variable_item_list_free(app->material_list);
    variable_item_list_free(app->color_list);
    widget_free(app->widget);
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

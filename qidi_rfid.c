#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification_messages.h>
#include <lib/nfc/nfc_device.h>
#include <lib/nfc/protocols/mf_classic/mf_classic.h>
#include <lib/nfc/protocols/mf_classic/mf_classic_poller.h>

#include "qidi_rfid_materials.h"
#include "qidi_rfid_colors.h"

#define TAG "QidiRFID"

#define QIDI_SECTOR 1
#define QIDI_BLOCK 0
#define QIDI_MANUFACTURER_CODE 1

typedef enum {
    QidiRfidViewSubmenu,
    QidiRfidViewMaterialSelect,
    QidiRfidViewColorSelect,
    QidiRfidViewWrite,
    QidiRfidViewRead,
} QidiRfidView;

typedef enum {
    QidiRfidSubmenuRead,
    QidiRfidSubmenuWrite,
    QidiRfidSubmenuAbout,
} QidiRfidSubmenuIndex;

typedef struct {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    VariableItemList* variable_item_list_material;
    VariableItemList* variable_item_list_color;
    Widget* widget;
    NotificationApp* notifications;

    uint8_t material_index;
    uint8_t color_index;

    Nfc* nfc;
    NfcDevice* nfc_device;
} QidiRfidApp;

// Forward declarations
static void qidi_rfid_submenu_callback(void* context, uint32_t index);
static uint32_t qidi_rfid_exit_callback(void* context);

// Material selection callbacks
static void qidi_rfid_material_change_callback(VariableItem* item) {
    QidiRfidApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->material_index = index;
    variable_item_set_current_value_text(item, qidi_materials[index].name);
}

// Color selection callbacks
static void qidi_rfid_color_change_callback(VariableItem* item) {
    QidiRfidApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->color_index = index;
    variable_item_set_current_value_text(item, qidi_colors[index].name);
}

static void qidi_rfid_material_select_enter_callback(void* context) {
    QidiRfidApp* app = context;
    variable_item_list_reset(app->variable_item_list_material);

    VariableItem* item = variable_item_list_add(
        app->variable_item_list_material,
        "Material:",
        QIDI_MATERIAL_COUNT,
        qidi_rfid_material_change_callback,
        app);

    variable_item_set_current_value_index(item, app->material_index);
    variable_item_set_current_value_text(item, qidi_materials[app->material_index].name);
}

static void qidi_rfid_color_select_enter_callback(void* context) {
    QidiRfidApp* app = context;
    variable_item_list_reset(app->variable_item_list_color);

    VariableItem* item = variable_item_list_add(
        app->variable_item_list_color,
        "Color:",
        QIDI_COLOR_COUNT,
        qidi_rfid_color_change_callback,
        app);

    variable_item_set_current_value_index(item, app->color_index);
    variable_item_set_current_value_text(item, qidi_colors[app->color_index].name);
}

static void qidi_rfid_write_tag(QidiRfidApp* app) {
    FURI_LOG_I(TAG, "Writing QIDI tag: Material=%d, Color=%d",
               qidi_materials[app->material_index].code,
               qidi_colors[app->color_index].code);

    // Show popup
    widget_reset(app->widget);
    widget_add_string_element(app->widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, "Place tag on");
    widget_add_string_element(app->widget, 64, 32, AlignCenter, AlignCenter, FontPrimary, "Flipper Zero");
    widget_add_string_element(app->widget, 64, 50, AlignCenter, AlignCenter, FontSecondary, "Writing...");

    // TODO: Implement actual NFC writing
    // This requires:
    // 1. Detect MIFARE Classic 1K tag
    // 2. Authenticate with default keys (FF FF FF FF FF FF)
    // 3. Write to Sector 1, Block 0:
    //    - Byte 0: Material code
    //    - Byte 1: Color code
    //    - Byte 2: Manufacturer code (1)

    notification_message(app->notifications, &sequence_success);
}

static void qidi_rfid_read_tag(QidiRfidApp* app) {
    FURI_LOG_I(TAG, "Reading QIDI tag");

    // Show popup
    widget_reset(app->widget);
    widget_add_string_element(app->widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, "Place tag on");
    widget_add_string_element(app->widget, 64, 32, AlignCenter, AlignCenter, FontPrimary, "Flipper Zero");
    widget_add_string_element(app->widget, 64, 50, AlignCenter, AlignCenter, FontSecondary, "Reading...");

    // TODO: Implement actual NFC reading
    // This requires:
    // 1. Detect MIFARE Classic 1K tag
    // 2. Authenticate with default keys (FF FF FF FF FF FF)
    // 3. Read from Sector 1, Block 0
    // 4. Display material and color info

    notification_message(app->notifications, &sequence_success);
}

static void qidi_rfid_submenu_callback(void* context, uint32_t index) {
    QidiRfidApp* app = context;

    switch(index) {
        case QidiRfidSubmenuRead:
            view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewRead);
            qidi_rfid_read_tag(app);
            break;
        case QidiRfidSubmenuWrite:
            view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewMaterialSelect);
            break;
        case QidiRfidSubmenuAbout:
            widget_reset(app->widget);
            widget_add_text_scroll_element(
                app->widget,
                0,
                0,
                128,
                64,
                "QIDI RFID v1.0\n\n"
                "Program RFID tags for\n"
                "QIDI Box filament system\n\n"
                "Sector: 1, Block: 0\n"
                "Chip: FM11RF08S\n"
                "13.56MHz MIFARE Classic");
            view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewRead);
            break;
    }
}

static uint32_t qidi_rfid_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static uint32_t qidi_rfid_submenu_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static uint32_t qidi_rfid_material_to_color_callback(void* context) {
    UNUSED(context);
    return QidiRfidViewColorSelect;
}

static uint32_t qidi_rfid_color_to_write_callback(void* context) {
    QidiRfidApp* app = context;
    qidi_rfid_write_tag(app);
    return QidiRfidViewWrite;
}

static uint32_t qidi_rfid_back_to_submenu_callback(void* context) {
    UNUSED(context);
    return QidiRfidViewSubmenu;
}

static QidiRfidApp* qidi_rfid_app_alloc() {
    QidiRfidApp* app = malloc(sizeof(QidiRfidApp));

    app->material_index = 0; // Default to PLA
    app->color_index = 0;    // Default to White

    // View Dispatcher
    app->view_dispatcher = view_dispatcher_alloc();

    // Submenu
    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "Read Tag", QidiRfidSubmenuRead, qidi_rfid_submenu_callback, app);
    submenu_add_item(app->submenu, "Write Tag", QidiRfidSubmenuWrite, qidi_rfid_submenu_callback, app);
    submenu_add_item(app->submenu, "About", QidiRfidSubmenuAbout, qidi_rfid_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), qidi_rfid_submenu_exit_callback);
    view_dispatcher_add_view(app->view_dispatcher, QidiRfidViewSubmenu, submenu_get_view(app->submenu));

    // Material selection
    app->variable_item_list_material = variable_item_list_alloc();
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_material),
        qidi_rfid_back_to_submenu_callback);
    view_set_enter_callback(
        variable_item_list_get_view(app->variable_item_list_material),
        qidi_rfid_material_select_enter_callback);
    variable_item_list_set_enter_callback(
        app->variable_item_list_material,
        NULL,
        app);
    view_dispatcher_add_view(
        app->view_dispatcher,
        QidiRfidViewMaterialSelect,
        variable_item_list_get_view(app->variable_item_list_material));

    // Color selection
    app->variable_item_list_color = variable_item_list_alloc();
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_color),
        qidi_rfid_material_to_color_callback);
    view_set_enter_callback(
        variable_item_list_get_view(app->variable_item_list_color),
        qidi_rfid_color_select_enter_callback);
    variable_item_list_set_enter_callback(
        app->variable_item_list_color,
        NULL,
        app);
    view_dispatcher_add_view(
        app->view_dispatcher,
        QidiRfidViewColorSelect,
        variable_item_list_get_view(app->variable_item_list_color));

    // Widget for write/read/about
    app->widget = widget_alloc();
    view_set_previous_callback(widget_get_view(app->widget), qidi_rfid_back_to_submenu_callback);
    view_dispatcher_add_view(app->view_dispatcher, QidiRfidViewWrite, widget_get_view(app->widget));
    view_dispatcher_add_view(app->view_dispatcher, QidiRfidViewRead, widget_get_view(app->widget));

    // Notifications
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // NFC
    app->nfc = nfc_alloc();
    app->nfc_device = nfc_device_alloc();

    return app;
}

static void qidi_rfid_app_free(QidiRfidApp* app) {
    furi_assert(app);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewMaterialSelect);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewColorSelect);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewWrite);
    view_dispatcher_remove_view(app->view_dispatcher, QidiRfidViewRead);

    submenu_free(app->submenu);
    variable_item_list_free(app->variable_item_list_material);
    variable_item_list_free(app->variable_item_list_color);
    widget_free(app->widget);

    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_NOTIFICATION);

    nfc_device_free(app->nfc_device);
    nfc_free(app->nfc);

    free(app);
}

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

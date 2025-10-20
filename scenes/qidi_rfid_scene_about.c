#include "../qidi_rfid_app_i.h"

void qidi_rfid_scene_about_on_enter(void* context) {
    QidiRfidApp* app = context;
    Widget* widget = app->widget;

    widget_add_text_scroll_element(
        widget,
        0,
        0,
        128,
        64,
        "QidiBox Filament v0.2\n"
        "\n"
        "Read and write RFID tags\n"
        "for QidiBox filament\n"
        "management system\n"
        "\n"
        "Author: Alex Kilimnik\n"
        "\n"
        "github.com/alexk42/\n"
        "qidi-filament-nfc-flipper\n"
        "\n"
        "MIFARE Classic 1K\n"
        "Default keys (FF...)\n"
        "Sector 1, Block 0");

    view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewWidget);
}

bool qidi_rfid_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void qidi_rfid_scene_about_on_exit(void* context) {
    QidiRfidApp* app = context;
    widget_reset(app->widget);
}

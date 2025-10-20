#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include <lib/nfc/nfc.h>
#include <lib/nfc/nfc_scanner.h>

#include "qidi_rfid_materials.h"
#include "qidi_rfid_colors.h"
#include "qidi_rfid_nfc.h"
#include "scenes/qidi_rfid_scene.h"

#define QIDI_TEXT_STORE_SIZE 64

typedef struct QidiRfidApp QidiRfidApp;

enum QidiRfidCustomEvent {
    QidiRfidEventCardDetected = 100,
    QidiRfidEventWorkerExit,
    QidiRfidEventReadSuccess,
    QidiRfidEventReadFail,
    QidiRfidEventWriteSuccess,
    QidiRfidEventWriteFail,
};

struct QidiRfidApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    Nfc* nfc;

    // Views
    Submenu* submenu;
    Popup* popup;
    VariableItemList* variable_item_list;
    Widget* widget;

    // Data
    uint8_t material_index;
    uint8_t color_index;
    char text_store[QIDI_TEXT_STORE_SIZE + 1];

    // Scanner and worker
    NfcScanner* scanner;
    FuriThread* worker_thread;
    bool is_writing;
    QidiTagData tag_data;
};

typedef enum {
    QidiRfidViewSubmenu,
    QidiRfidViewPopup,
    QidiRfidViewVariableItemList,
    QidiRfidViewWidget,
} QidiRfidView;

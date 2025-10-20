#include "../qidi_rfid_app_i.h"

static void qidi_rfid_scene_write_scanner_callback(NfcScannerEvent event, void* context) {
    furi_assert(context);
    QidiRfidApp* app = context;

    if(event.type == NfcScannerEventTypeDetected) {
        // Card detected, send event to proceed with writing
        view_dispatcher_send_custom_event(app->view_dispatcher, QidiRfidEventCardDetected);
    }
}

static int32_t qidi_rfid_write_worker_thread(void* context) {
    QidiRfidApp* app = context;

    bool success = qidi_rfid_write_tag(app->nfc, &app->tag_data);

    view_dispatcher_send_custom_event(
        app->view_dispatcher,
        success ? QidiRfidEventWriteSuccess : QidiRfidEventWriteFail);

    return 0;
}

void qidi_rfid_scene_write_on_enter(void* context) {
    QidiRfidApp* app = context;
    Popup* popup = app->popup;

    popup_set_header(popup, "Writing", 64, 10, AlignCenter, AlignTop);

    // Prepare tag data
    app->tag_data.material_code = qidi_materials[app->material_index].code;
    app->tag_data.color_code = qidi_colors[app->color_index].code;
    app->tag_data.manufacturer_code = 1;

    snprintf(
        app->text_store,
        QIDI_TEXT_STORE_SIZE,
        "%s\n%s\nHold card to back",
        qidi_materials[app->material_index].name,
        qidi_colors[app->color_index].name);
    popup_set_text(popup, app->text_store, 64, 25, AlignCenter, AlignTop);

    view_dispatcher_switch_to_view(app->view_dispatcher, QidiRfidViewPopup);

    // Start scanner to detect card
    app->scanner = nfc_scanner_alloc(app->nfc);
    nfc_scanner_start(app->scanner, qidi_rfid_scene_write_scanner_callback, app);

    notification_message(app->notifications, &sequence_blink_start_magenta);
}

bool qidi_rfid_scene_write_on_event(void* context, SceneManagerEvent event) {
    QidiRfidApp* app = context;
    Popup* popup = app->popup;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == QidiRfidEventCardDetected) {
            // Card detected, stop scanner and start worker to write
            nfc_scanner_stop(app->scanner);
            nfc_scanner_free(app->scanner);
            app->scanner = NULL;

            popup_set_text(popup, "Writing tag data...", 64, 25, AlignCenter, AlignTop);

            // Start worker thread to write tag
            app->worker_thread = furi_thread_alloc_ex(
                "QidiWriteWorker",
                2048,
                qidi_rfid_write_worker_thread,
                app);
            furi_thread_start(app->worker_thread);
            consumed = true;
        } else if(event.event == QidiRfidEventWriteSuccess) {
            notification_message(app->notifications, &sequence_success);
            popup_set_header(popup, "Write Success!", 64, 10, AlignCenter, AlignTop);
            snprintf(
                app->text_store,
                QIDI_TEXT_STORE_SIZE,
                "%s\n%s",
                qidi_materials[app->material_index].name,
                qidi_colors[app->color_index].name);
            popup_set_text(popup, app->text_store, 64, 25, AlignCenter, AlignTop);
            consumed = true;
        } else if(event.event == QidiRfidEventWriteFail) {
            notification_message(app->notifications, &sequence_error);
            popup_set_header(popup, "Write Failed", 64, 10, AlignCenter, AlignTop);
            popup_set_text(popup, "Could not write tag", 64, 25, AlignCenter, AlignTop);
            consumed = true;
        }
    }

    return consumed;
}

void qidi_rfid_scene_write_on_exit(void* context) {
    QidiRfidApp* app = context;

    notification_message(app->notifications, &sequence_blink_stop);
    popup_reset(app->popup);

    // Stop scanner if still running
    if(app->scanner) {
        nfc_scanner_stop(app->scanner);
        nfc_scanner_free(app->scanner);
        app->scanner = NULL;
    }

    // Stop worker if still running
    if(app->worker_thread) {
        furi_thread_join(app->worker_thread);
        furi_thread_free(app->worker_thread);
        app->worker_thread = NULL;
    }
}

#pragma once

#include <furi.h>
#include <lib/nfc/nfc.h>
#include "qidi_rfid_nfc.h"

typedef enum {
    QidiRfidWorkerEventReadSuccess,
    QidiRfidWorkerEventReadFail,
    QidiRfidWorkerEventWriteSuccess,
    QidiRfidWorkerEventWriteFail,
} QidiRfidWorkerEventType;

typedef struct {
    QidiRfidWorkerEventType type;
    QidiTagData tag_data;
} QidiRfidWorkerEvent;

typedef struct QidiRfidWorker QidiRfidWorker;

typedef void (*QidiRfidWorkerCallback)(QidiRfidWorkerEvent event, void* context);

QidiRfidWorker* qidi_rfid_worker_alloc(Nfc* nfc);

void qidi_rfid_worker_free(QidiRfidWorker* worker);

void qidi_rfid_worker_start(
    QidiRfidWorker* worker,
    QidiRfidWorkerCallback callback,
    void* context);

void qidi_rfid_worker_stop(QidiRfidWorker* worker);

void qidi_rfid_worker_read(QidiRfidWorker* worker);

void qidi_rfid_worker_write(QidiRfidWorker* worker, QidiTagData* tag_data);

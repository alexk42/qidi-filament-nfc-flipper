#include "qidi_rfid_worker.h"
#include "qidi_rfid_nfc.h"
#include <furi.h>

#define TAG "QidiRFIDWorker"

typedef enum {
    QidiRfidWorkerStateIdle,
    QidiRfidWorkerStateRead,
    QidiRfidWorkerStateWrite,
    QidiRfidWorkerStateStop,
} QidiRfidWorkerState;

struct QidiRfidWorker {
    FuriThread* thread;
    QidiRfidWorkerCallback callback;
    void* context;

    QidiRfidWorkerState state;
    QidiTagData tag_data;
    Nfc* nfc;
};

static int32_t qidi_rfid_worker_thread(void* context) {
    QidiRfidWorker* worker = context;

    while(worker->state != QidiRfidWorkerStateStop) {
        if(worker->state == QidiRfidWorkerStateRead) {
            FURI_LOG_D(TAG, "Worker: Reading tag");
            bool success = qidi_rfid_read_tag(worker->nfc, &worker->tag_data);

            QidiRfidWorkerEvent event = {
                .type = success ? QidiRfidWorkerEventReadSuccess : QidiRfidWorkerEventReadFail,
                .tag_data = worker->tag_data,
            };

            if(worker->callback) {
                worker->callback(event, worker->context);
            }

            worker->state = QidiRfidWorkerStateIdle;

        } else if(worker->state == QidiRfidWorkerStateWrite) {
            FURI_LOG_D(TAG, "Worker: Writing tag");
            bool success = qidi_rfid_write_tag(worker->nfc, &worker->tag_data);

            QidiRfidWorkerEvent event = {
                .type = success ? QidiRfidWorkerEventWriteSuccess : QidiRfidWorkerEventWriteFail,
                .tag_data = worker->tag_data,
            };

            if(worker->callback) {
                worker->callback(event, worker->context);
            }

            worker->state = QidiRfidWorkerStateIdle;
        }

        furi_delay_ms(100);
    }

    return 0;
}

QidiRfidWorker* qidi_rfid_worker_alloc(Nfc* nfc) {
    QidiRfidWorker* worker = malloc(sizeof(QidiRfidWorker));
    worker->nfc = nfc;
    worker->state = QidiRfidWorkerStateIdle;
    worker->callback = NULL;
    worker->context = NULL;
    worker->thread = furi_thread_alloc();
    furi_thread_set_name(worker->thread, "QidiRfidWorker");
    furi_thread_set_stack_size(worker->thread, 4096);
    furi_thread_set_context(worker->thread, worker);
    furi_thread_set_callback(worker->thread, qidi_rfid_worker_thread);

    return worker;
}

void qidi_rfid_worker_free(QidiRfidWorker* worker) {
    furi_assert(worker);
    furi_thread_free(worker->thread);
    free(worker);
}

void qidi_rfid_worker_start(
    QidiRfidWorker* worker,
    QidiRfidWorkerCallback callback,
    void* context) {
    furi_assert(worker);
    worker->callback = callback;
    worker->context = context;
    furi_thread_start(worker->thread);
}

void qidi_rfid_worker_stop(QidiRfidWorker* worker) {
    furi_assert(worker);
    worker->state = QidiRfidWorkerStateStop;
    furi_thread_join(worker->thread);
}

void qidi_rfid_worker_read(QidiRfidWorker* worker) {
    furi_assert(worker);
    worker->state = QidiRfidWorkerStateRead;
}

void qidi_rfid_worker_write(QidiRfidWorker* worker, QidiTagData* tag_data) {
    furi_assert(worker);
    worker->tag_data = *tag_data;
    worker->state = QidiRfidWorkerStateWrite;
}

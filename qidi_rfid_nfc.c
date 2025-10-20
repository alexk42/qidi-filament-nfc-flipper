#include "qidi_rfid_nfc.h"
#include <furi.h>
#include <lib/nfc/protocols/mf_classic/mf_classic.h>
#include <lib/nfc/protocols/mf_classic/mf_classic_poller_sync.h>

#define TAG "QidiRFID_NFC"

// QIDI tag specifications
#define QIDI_DATA_SECTOR 1
#define QIDI_DATA_BLOCK 0
#define QIDI_ABSOLUTE_BLOCK ((QIDI_DATA_SECTOR * 4) + QIDI_DATA_BLOCK) // Block 4

// Default MIFARE Classic keys
static const uint8_t default_key_a[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bool qidi_rfid_read_tag(Nfc* nfc, QidiTagData* tag_data) {
    furi_assert(nfc);
    furi_assert(tag_data);

    // Prepare key
    MfClassicKey key = {0};
    memcpy(key.data, default_key_a, sizeof(default_key_a));

    MfClassicBlock block;

    // Read the data block (Sector 1, Block 0 = absolute block 4)
    // The card has already been detected by the scanner
    MfClassicError error = mf_classic_poller_sync_read_block(
        nfc,
        QIDI_ABSOLUTE_BLOCK,
        &key,
        MfClassicKeyTypeA,
        &block
    );

    if(error == MfClassicErrorNone) {
        // Parse QIDI data
        tag_data->material_code = block.data[0];
        tag_data->color_code = block.data[1];
        tag_data->manufacturer_code = block.data[2];

        FURI_LOG_I(TAG, "Read QIDI tag: Material=%d, Color=%d, Manufacturer=%d",
                  tag_data->material_code,
                  tag_data->color_code,
                  tag_data->manufacturer_code);

        return true;
    } else {
        FURI_LOG_E(TAG, "Failed to read block %d, error=%d", QIDI_ABSOLUTE_BLOCK, error);
        return false;
    }
}

bool qidi_rfid_write_tag(Nfc* nfc, const QidiTagData* tag_data) {
    furi_assert(nfc);
    furi_assert(tag_data);

    // Prepare key
    MfClassicKey key = {0};
    memcpy(key.data, default_key_a, sizeof(default_key_a));

    // Prepare data block
    MfClassicBlock block = {0};
    block.data[0] = tag_data->material_code;
    block.data[1] = tag_data->color_code;
    block.data[2] = tag_data->manufacturer_code;
    // Remaining bytes are 0x00 (unused)

    // Write the data block (Sector 1, Block 0 = absolute block 4)
    // The card has already been detected by the scanner
    MfClassicError error = mf_classic_poller_sync_write_block(
        nfc,
        QIDI_ABSOLUTE_BLOCK,
        &key,
        MfClassicKeyTypeA,
        &block
    );

    if(error == MfClassicErrorNone) {
        FURI_LOG_I(TAG, "Wrote QIDI tag: Material=%d, Color=%d, Manufacturer=%d",
                  tag_data->material_code,
                  tag_data->color_code,
                  tag_data->manufacturer_code);

        return true;
    } else {
        FURI_LOG_E(TAG, "Failed to write block %d, error=%d", QIDI_ABSOLUTE_BLOCK, error);
        return false;
    }
}

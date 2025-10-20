#pragma once

#include <lib/nfc/nfc.h>

typedef struct {
    uint8_t material_code;
    uint8_t color_code;
    uint8_t manufacturer_code;
} QidiTagData;

/**
 * Read QIDI tag data from a MIFARE Classic tag
 * @param nfc NFC instance
 * @param tag_data Pointer to store read tag data
 * @return true if successful, false otherwise
 */
bool qidi_rfid_read_tag(Nfc* nfc, QidiTagData* tag_data);

/**
 * Write QIDI tag data to a MIFARE Classic tag
 * @param nfc NFC instance
 * @param tag_data Tag data to write
 * @return true if successful, false otherwise
 */
bool qidi_rfid_write_tag(Nfc* nfc, const QidiTagData* tag_data);

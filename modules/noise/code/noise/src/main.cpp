#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>

// LFSR States (initialize with different seeds)
uint16_t lfsr1 = 0xACE1; // LFSR 1
uint16_t lfsr2 = 0xBEEF; // LFSR 2
uint16_t lfsr3 = 0xCAFE; // LFSR 3

// Feedback taps for each LFSR (e.g., bits 0, 2, 3, 5 for 16-bit LFSR)
uint8_t updateLFSR(uint16_t *lfsr, uint16_t taps) {
    uint8_t bit = (*lfsr & 1) ^ ((*lfsr >> 1) & 1) ^ ((*lfsr >> 2) & 1) ^ ((*lfsr >> 5) & 1);
    *lfsr = (*lfsr >> 1) | (bit << 15); // Shift and add feedback
    return (*lfsr & 0xFF); // Return 8-bit value
}

uint8_t generateWhiteNoise() {
    uint8_t lfsrOut1 = updateLFSR(&lfsr1, 0xACE1); // Update LFSR 1
    uint8_t lfsrOut2 = updateLFSR(&lfsr2, 0xBEEF); // Update LFSR 2
    uint8_t lfsrOut3 = updateLFSR(&lfsr3, 0xCAFE); // Update LFSR 3

    // Combine the outputs with XOR and bit-shifting
    uint8_t combined = (lfsrOut1 ^ (lfsrOut2 >> 1)) ^ (lfsrOut3 >> 2);

    return combined; // Output mixed noise
}

void setup() {
  // DAC0 setup for sending velocity via the PA6 pin
  VREF.CTRLA |= VREF_DAC0REFSEL_4V34_gc; //this will force it to use VDD as the VREF
  VREF.CTRLB |= VREF_DAC0REFEN_bm;
  DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;
  DAC0.DATA = 128;
}

void outputToDAC(uint8_t value) {
  DAC0.DATA = value;
}

void loop() {
  uint8_t metallicNoise = generateWhiteNoise(); // Generate noise
  outputToDAC(metallicNoise); // Output noise to DAC
  _delay_us(50); // Adjust sampling rate (~20kHz)
}
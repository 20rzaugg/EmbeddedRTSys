#include <stdint.h>

//if we use a 12-bit, 3.3v DAC, values between 0 and 310 won't clip our 5v rails with a 20x gain op-amp
//This table has 44 entries, thought that might make math easy.

const uint16_t sinLUT[] = {
    310, 340, 370, 400, 428, 456, 482, 506,
    530, 550, 568, 584, 596, 606, 614, 618,
    620, 618, 614, 606, 596, 584, 568, 550,
    530, 506, 482, 456, 428, 400, 370, 340,
    310, 280, 250, 220, 192, 164, 138, 114,
    90, 70, 52, 36, 24, 14, 6, 2,
    0, 2, 6, 14, 24, 36, 52, 70,
    90, 114, 138, 164, 192, 220, 250, 280};

//this table assumes a 16MHz clock and a prescaler of 1(+1)
struct notePair {
    uint16_t pitch;
    char name[4];
};

const struct notePair note_table[] = {
    {1912, "C3"},    // C3
    {1804, "C#3"},   // C#3
    {1702, "D3"},    // D3
    {1608, "D#3"},   // D#3
    {1516, "E3"},    // E3
    {1432, "F3"},    // F3
    {1352, "F#3"},   // F#3
    {1276, "G3"},    // G3
    {1204, "G#3"},   // G#3
    {1136, "A3"},    // A3
    {1072, "A#3"},   // A#3
    {1012, "B3"},    // B3
    {956, "C4"},    // C4
    {902, "C#4"},   // C#4
    {852, "D4"},    // D4
    {804, "D#4"},   // D#4
    {758, "E4"},    // E4
    {716, "F4"},    // F4
    {676, "F#4"},   // F#4
    {638, "G4"},    // G4
    {602, "G#4"},   // G#4
    {568, "A4"},    // A4
    {536, "A#4"},   // Bb4
    {506, "B4"},    // B4
    {478, "C5"}     // C5
};

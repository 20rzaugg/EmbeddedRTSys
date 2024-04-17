#include <stdint.h>

//if we use a 12-bit, 3.3v DAC, values between 0 and 310 won't clip our 5v rails with a 20x gain op-amp
//This table has 44 entries, thought that might make math easy.

const uint16_t sinLUT[] = {
155, 170, 185, 200, 214, 228, 241, 253,
265, 275, 284, 292, 298, 303, 307, 309,
310, 309, 307, 303, 298, 292, 284, 275,
265, 253, 241, 228, 214, 200, 185, 170,
155, 140, 125, 110,  96,  82,  69,  57,
 45,  35,  26,  18,  12,   7,   3,   1,
  0,   1,   3,   7,  12,  18,  26,  35,
 45,  57,  69,  82,  96, 110, 125, 140};

//this table assumes a 16MHz clock and a prescaler of 1(+1)
struct notePair {
    uint16_t pitch;
    char[4] name;
}

const struct notePair note_table[] = {
    {956, "C3"},    // C3
    {902, "C#3"},   // C#3
    {851, "D3"},    // D3
    {804, "D#3"},   // D#3
    {758, "E3"},    // E3
    {716, "F3"},    // F3
    {676, "F#3"},   // F#3
    {638, "G3"},    // G3
    {602, "G#3"},   // G#3
    {568, "A3"},    // A3
    {536, "A#3"},   // A#3
    {506, "B3"},    // B3
    {478, "C4"},    // C4
    {451, "C#4"},   // C#4
    {426, "D4"},    // D4
    {402, "D#4"},   // D#4
    {379, "E4"},    // E4
    {358, "F4"},    // F4
    {338, "F#4"},   // F#4
    {319, "G4"},    // G4
    {301, "G#4"},   // G#4
    {284, "A4"},    // A4
    {268, "Bb4"},   // Bb4
    {253, "B4"},    // B4
    {239, "C5"}     // C5
};

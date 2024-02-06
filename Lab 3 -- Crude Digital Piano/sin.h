#include <stdint.h>

//if we use a 12-bit, 3.3v DAC, values between 0 and 310 won't clip our 5v rails with a 20x gain op-amp
//This table has 44 entries, thought that might make math easy.

//const uint16_t sinLUT[] = { 0x9bu, 0xaau, 0xb9u, 0xc8u, 0xd6u, 0xe4u, 0xf1u, 0xfdu, 0x109u, 0x113u, 0x11cu, 0x124u, 0x12au, 0x12fu, 0x133u, 0x135u,
//    0x136u, 0x135u, 0x133u, 0x12fu, 0x12au, 0x124u, 0x11cu, 0x113u, 0x109u, 0xfdu, 0xf1u, 0xe4u, 0xd6u, 0xc8u, 0xb9u, 0xaau,
//    0x9bu, 0x8cu, 0x7du, 0x6eu, 0x60u, 0x52u, 0x45u, 0x39u, 0x2du, 0x23u, 0x1au, 0x12u, 0x0cu, 0x07u, 0x03u, 0x01u,
//    0x00u, 0x01u, 0x03u, 0x07u, 0x0cu, 0x12u, 0x1au, 0x23u, 0x2du, 0x39u, 0x45u, 0x52u, 0x60u, 0x6eu, 0x7du, 0x8cu};

//const uint16_t sinLUT[] = {
//0x400, 0x464, 0x4c7, 0x529, 0x587, 0x5e2, 0x638, 0x689, 0x6d3, 0x717, 0x753, 0x786, 0x7b1, 0x7d3, 0x7eb, 0x7fa,
//0x7ff, 0x7fa, 0x7eb, 0x7d3, 0x7b1, 0x786, 0x753, 0x717, 0x6d3, 0x689, 0x638, 0x5e2, 0x587, 0x529, 0x4c7, 0x464,
//0x400, 0x39b, 0x338, 0x2d6, 0x278, 0x21d, 0x1c7, 0x176, 0x12c, 0xe8, 0xac, 0x79, 0x4e, 0x2c, 0x14, 0x05,
//0x00, 0x05, 0x14, 0x2c, 0x4e, 0x79, 0xac, 0xe8, 0x12c, 0x176, 0x1c7, 0x21d, 0x278, 0x2d6, 0x338, 0x39b};
		
const uint16_t sinLUT[] = {
155, 170, 185, 200, 214, 228, 241, 253,
265, 275, 284, 292, 298, 303, 307, 309,
310, 309, 307, 303, 298, 292, 284, 275,
265, 253, 241, 228, 214, 200, 185, 170,
155, 140, 125, 110, 96, 82, 69, 57,
45, 35, 26, 18, 12, 7, 3, 1,
0, 1, 3, 7, 12, 18, 26, 35,
45, 57, 69, 82, 96, 110, 125, 140};

//this table assumes a 16MHz clock and a prescaler of 1(+1)
const uint16_t note_table[] = {
    //4545,   // A0
    //4290,   // 
    //4050,   // 
    //3822,   // 
    //3608,   // 
    //3405,   // 
    //3214,   // 
    //3034,   // 
    //2863,   // 
    //2703,   // 
    //2551,   // 
    //2408,   // 
    //2273,   // A1
    //2145,   // 
    //2025,   // 
    //1911,   // 
    //1804,   // 
    //1703,   // 
    //1607,   // 
    //1517,   // 
    //1432,   // 
    //1351,   // 
    //1276,   // 
    //1204,   // 
    //1136,   // A2
    //1073,   // 
    //1012,   // 
    //956,    // 
    //902,    // 
    //851,    // 
    //804,    // 
    //758,    // 
    //716,    // 
    //676,    // 
    //638,    // 
    //602,    // 
    //568,    // A3
    //536,    // 
    //506,    // 
    //478,    // 
    //451,    // 
    //426,    // 
    //402,    // 
    //379,    // 
    //358,    // 
    //338,    // 
    //319,    // 
    //301,    // 
    284,    // A4
    //268,    // Bb4
    253,    // B4
    239,    // C5
    //225,    // Db5
    213,    // D5
    //201,    // Eb5
    190,    // E5
    179,    // F5
    //169,    // Gb5
    159,    // G5
    //150,    // Ab5
    142,    // A5
    //134,    // 
    //127,    // 
    //119,    // 
    //113,    // 
    //106,    // 
    //100,    // 
    //95,     // 
    //89,     // 
    //84,     // 
    //80,     // 
    //75,     // 
    //71,     // A6
    //67,     // 
    //63,     // 
    //60,     // 
    //56,     // 
    //53,     // 
    //50,     // 
    //47,     // 
    //45,     // 
    //42,     // 
    //40,     // 
    //38,     // 
    //36,     // A7
    //34,     // 
    //32,     // 
    //30      // 
};

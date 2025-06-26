// File: src/utils.c (or within kernel.c)
#include <stdint.h>
#include <stddef.h> // For size_t
#include "base_kernel.h" // For common includes like terminal_writestring if desired

void uint16_to_hex_str(uint16_t val, char* buf) {
    int i = 0;
    if (val == 0) { buf[0] = '0'; i = 1; }
    else { uint16_t temp = val; while (temp > 0) { uint8_t rem = temp % 16; if (rem < 10) buf[i++] = rem + '0'; else buf[i++] = rem + 'A' - 10; temp /= 16; } }
    buf[i] = '\0';
    for (int start = 0, end = i - 1; start < end; start++, end--) { char tmp = buf[start]; buf[start] = buf[end]; buf[end] = tmp; }
    if (i == 0) { buf[0] = '0'; buf[1] = '\0'; }
}

void uint8_to_hex_str(uint8_t val, char* buf) {
    uint16_to_hex_str((uint16_t)val, buf); // Simply reuse the 16-bit version
}

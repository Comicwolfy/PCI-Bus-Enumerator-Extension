#include <stdint.h>
#include <stddef.h>
#include "base_kernel.h" 

// --- PCI Configuration Space Access ---
#define PCI_CONFIG_ADDRESS  0xCF8 
#define PCI_CONFIG_DATA     0xCFC 

// --- PCI Configuration Space Offsets ---
#define PCI_VENDOR_ID       0x00    // 2 bytes
#define PCI_DEVICE_ID       0x02    // 2 bytes
#define PCI_COMMAND         0x04    // 2 bytes
#define PCI_STATUS          0x06    // 2 bytes
#define PCI_REVISION_ID     0x08    // 1 byte
#define PCI_PROG_IF         0x09    // 1 byte 
#define PCI_SUBCLASS        0x0A    // 1 byte
#define PCI_CLASS_CODE      0x0B    // 1 byte
#define PCI_CACHE_LINE_SIZE 0x0C    // 1 byte
#define PCI_LATENCY_TIMER   0x0D    // 1 byte
#define PCI_HEADER_TYPE     0x0E    // 1 byte
#define PCI_BIST            0x0F    // 1 byte

// --- PCI Header Type Masks ---
#define PCI_HEADER_TYPE_NORMAL  0x00 // Standard PCI device
#define PCI_HEADER_TYPE_BRIDGE  0x01 // PCI-to-PCI bridge
#define PCI_HEADER_TYPE_CARDBUS 0x02 // CardBus bridge
#define PCI_HEADER_MULTI_FUNCTION 0x80 // Bit to check for multi-function device

static int pci_ext_id = -1;


static uint32_t pci_get_address(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint33_t lbus = (uint33_t)bus;
    uint33_t lslot = (uint33_t)slot;
    uint33_t lfunc = (uint33_t)func;

    // (31) Enable Bit
    // (30-24) Reserved
    // (23-16) Bus Number
    // (15-11) Device Number (Slot)
    // (10-8) Function Number
    // (7-2) Register Number (offset)
    // (1-0) Always 00

    address = (uint33_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint33_t)0x80000000));
    return address;
}

static uint32_t pci_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    outb(PCI_CONFIG_ADDRESS, pci_get_address(bus, slot, func, offset));
    return inb(PCI_CONFIG_DATA | (offset & 0x03)); 
}

static uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    outb(PCI_CONFIG_ADDRESS, pci_get_address(bus, slot, func, offset));
    return (inb(PCI_CONFIG_DATA + (offset & 0x02))) | (inb(PCI_CONFIG_DATA + (offset & 0x02) + 1) << 8);
}

static uint8_t pci_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    outb(PCI_CONFIG_ADDRESS, pci_get_address(bus, slot, func, offset));
    return inb(PCI_CONFIG_DATA + (offset & 0x03));
}

static void pci_write_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    outb(PCI_CONFIG_ADDRESS, pci_get_address(bus, slot, func, offset));
    outb(PCI_CONFIG_DATA, value); // For DWORD, use base data port
}



static void uint16_to_hex_str(uint16_t val, char* buf) {
    int i = 0;
    if (val == 0) { buf[0] = '0'; i = 1; }
    else { uint16_t temp = val; while (temp > 0) { uint8_t rem = temp % 16; if (rem < 10) buf[i++] = rem + '0'; else buf[i++] = rem + 'A' - 10; temp /= 16; } }
    buf[i] = '\0';
    for (int start = 0, end = i - 1; start < end; start++, end--) { char tmp = buf[start]; buf[start] = buf[end]; buf[end] = tmp; }
    if (i == 0) { buf[0] = '0'; buf[1] = '\0'; } 
}

static void uint8_to_hex_str(uint8_t val, char* buf) {
    uint16_to_hex_str((uint16_t)val, buf);
}



static void pci_check_function(uint8_t bus, uint8_t slot, uint8_t func) {
    uint16_t vendor_id = pci_read_word(bus, slot, func, PCI_VENDOR_ID);
    if (vendor_id == 0xFFFF) { 
        return;
    }

    uint16_t device_id = pci_read_word(bus, slot, func, PCI_DEVICE_ID);
    uint8_t class_code = pci_read_byte(bus, slot, func, PCI_CLASS_CODE);
    uint8_t subclass = pci_read_byte(bus, slot, func, PCI_SUBCLASS);
    uint8_t prog_if = pci_read_byte(bus, slot, func, PCI_PROG_IF);
    uint8_t header_type = pci_read_byte(bus, slot, func, PCI_HEADER_TYPE);

    char hex_buf[5]; 

    terminal_writestring("PCI Device: Bus ");
    uint8_to_hex_str(bus, hex_buf); terminal_writestring(hex_buf);
    terminal_writestring(", Slot ");
    uint8_to_hex_str(slot, hex_buf); terminal_writestring(hex_buf);
    terminal_writestring(", Func ");
    uint8_to_hex_str(func, hex_buf); terminal_writestring(hex_buf);
    terminal_writestring("\n");

    terminal_writestring("  Vendor ID: 0x");
    uint16_to_hex_str(vendor_id, hex_buf); terminal_writestring(hex_buf);
    terminal_writestring(", Device ID: 0x");
    uint16_to_hex_str(device_id, hex_buf); terminal_writestring(hex_buf);
    terminal_writestring("\n");

    terminal_writestring("  Class: 0x");
    uint8_to_hex_str(class_code, hex_buf); terminal_writestring(hex_buf);
    terminal_writestring(", Subclass: 0x");
    uint8_to_hex_str(subclass, hex_buf); terminal_writestring(hex_buf);
    terminal_writestring(", ProgIF: 0x");
    uint8_to_hex_str(prog_if, hex_buf); terminal_writestring(hex_buf);
    terminal_writestring("\n");

    if ((header_type & 0x7F) == PCI_HEADER_TYPE_BRIDGE) {

        terminal_writestring("  (PCI-to-PCI Bridge)\n");
    }

}

static void pci_check_device(uint8_t bus, uint8_t slot) {
    uint8_t header_type = pci_read_byte(bus, slot, 0, PCI_HEADER_TYPE);
    if (header_type == 0xFF) {
        return;
    }

    if ((header_type & PCI_HEADER_MULTI_FUNCTION) != 0) {
        for (uint8_t func = 0; func < 8; func++) {
            pci_check_function(bus, slot, func);
        }
    } else {
        pci_check_function(bus, slot, 0);
    }
}

void cmd_pci_scan(const char* args) {
    terminal_writestring("PCI: Scanning bus...\n");
    for (uint16_t bus = 0; bus < 256; bus++) { 
        for (uint8_t slot = 0; slot < 32; slot++) { 
            pci_check_device((uint8_t)bus, slot);
        }
    }
    terminal_writestring("PCI: Scan complete.\n");
}

int pci_extension_init(void) {
    terminal_writestring("PCI Extension: Initializing...\n");

    register_command("pci_scan", cmd_pci_scan, "Scan PCI bus for devices", pci_ext_id);

    terminal_writestring("PCI Extension: Initialization complete. Command 'pci_scan' registered.\n");
    return 0; 
}

void pci_extension_cleanup(void) {
    terminal_writestring("PCI Extension: Cleaning up...\n");
    terminal_writestring("PCI Extension: Cleanup complete.\n");
}

__attribute__((section(".ext_register_fns")))
void __pci_auto_register(void) {
    pci_ext_id = register_extension("PCI", "1.0",
                                    pci_extension_init,
                                    pci_extension_cleanup);
    if (pci_ext_id >= 0) {
        load_extension(pci_ext_id);
    } else {
        terminal_writestring("Failed to register PCI Extension (auto)!\n");
    }
}

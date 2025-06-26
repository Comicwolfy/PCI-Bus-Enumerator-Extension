# PCI-Bus-Enumerator-Extension
## ESSENTIAL FOR WORKING ON REAL HARDWARE!!
## You WILL need this for bare-metal in any capacity!

PCI Bus Enumerator Extension for Core-Kernel: Formal Documentation

This document delineates the operational parameters and integration protocols pertaining to the PCI Bus Enumerator Extension, a constituent module within the Core-Kernel architecture.
Introduction (Overview)

The Peripheral Component Interconnect (PCI) Bus Enumerator constitutes an indispensable driver within the architectural framework of any contemporary operating system. Its primary function involves the dynamic ascertainment and interrogation of hardware devices interfaced via the PCI bus. In contrast to legacy interfaces, which are characterized by fixed address assignments, PCI devices necessitate a systematic scanning procedure for their identification and subsequent configuration.

The implementation of this functionality is deemed CRITICAL FOR THE OPERATION UPON PHYSICAL HARDWARE PLATFORMS. Its presence is a requisite for the development of drivers applicable to a vast array of prevalent peripheral components, encompassing, but not limited to, advanced network interfaces, graphical processing units, Universal Serial Bus controllers, and mass storage controllers. For any low-level, bare-metal development endeavor, the capacity to identify and interact with PCI hardware is fundamentally imperative.
Functional Modalities (Functionality)

The PCI Bus Enumerator extension provides the following principal functional modalities:

    Bus and Device Discovery: The system systematically traverses the range of potential PCI bus identifiers (0x00 to 0xFF) and device slot identifiers (0x00 to 0x1F) for the purpose of identifying active PCI devices resident within the computing apparatus.

    Multi-Function Device Resolution: Upon the detection of a given device, an assessment is conducted to ascertain its configuration as either a single-function or multi-function entity. In instances of multi-functionality, all eight potential functions (0x00 to 0x07) associated with the device are enumerated.

    Configuration Space Interrogation: For each identified PCI function, critical configuration data is retrieved from its dedicated PCI Configuration Space registers. This information comprises, but is not limited to:

        Vendor Identification (Vendor ID): An identifier specifying the manufacturer of the device.

        Device Identification (Device ID): A distinct identifier particularizing the device model from its respective vendor.

        Class Code: A categorization delineating the general functional purpose of the device (e.g., Network Controller, Display Controller).

        Subclass Code: A refined categorization further specifying the device's function (e.g., Ethernet Controller, VGA Compatible Controller).

        Programming Interface (ProgIF): An indicator specifying the register-level programming interface utilized by the device.

        Header Type: A designator indicating the structural layout of the device's configuration space (e.g., a standard device, a PCI-to-PCI bridge).

    Basic Device Delineation: The collected information, comprising the bus, slot, function identifiers, and the discovered Vendor/Device/Class/Subclass codes, is outputted to the console. This immediate presentation of data serves as a critical diagnostic aid for developers in verifying hardware presence and characteristics within either an emulated (QEMU) or physical operational environment.

Ancillary Consideration: The current iteration of the enumerator is primarily restricted to the examination of Bus 0. In advanced PCI topologies, prevalent in both physical hardware and sophisticated virtual machine environments, devices may reside on secondary buses interconnected via PCI-to-PCI bridge components. A comprehensive PCI enumerator would typically incorporate recursive scanning mechanisms to traverse these bridges and effectuate the discovery of all devices throughout the entire PCI topology. The incorporation of this recursive capability is a designated future enhancement.
Command Line Interface (Commands)

This extension registers a singular, fundamental command for direct user interaction via the kernel's command-line interface:

    pci_scan

        Description: This command initiates a comprehensive scan of the PCI bus. Subsequent to the scan, details of all detected devices and their essential configuration data are displayed on the terminal.

        Usage Protocol: The command is invoked by typing pci_scan.

        Illustrative Output:

        PCI: Scanning bus...
        PCI Device: Bus 0, Slot 0, Func 0
          Vendor ID: 0x8086, Device ID: 0x1237
          Class: 0x06, Subclass: 0x00, ProgIF: 0x00
          (PCI-to-PCI Bridge)
        PCI Device: Bus 0, Slot 1, Func 0
          Vendor ID: 0x8086, Device ID: 0x7000
          Class: 0x01, Subclass: 0x01, ProgIF: 0x80
        PCI Device: Bus 0, Slot 2, Func 0
          Vendor ID: 0x8086, Device ID: 0x100E
          Class: 0x02, Subclass: 0x00, ProgIF: 0x00
        PCI: Scan complete (Note: Only Bus 0 scanned).

        (It is to be understood that the actual output will be contingent upon the specific hardware configuration detected within the operational environment of the Core-Kernel.)

Integration Directives (Integration)

To effectuate the integration of the PCI Bus Enumerator Extension into a Core-Kernel build, the following directives shall be observed:

    Extension Source File Placement:

        The pci_extension.c source file shall be positioned within the kernel's src/extensions/ directory.

    Makefile Amendment:

        The Makefile, situated in the root directory of the Core-Kernel project, shall be edited.

        The C_SOURCES variable, which enumerates all C files designated for compilation, shall be appended with the path to the newly introduced extension's source file.

    # Exemplary snippet from Makefile
    C_SOURCES += src/extensions/pci_extension.c # This line is to be appended

    Shared Utility Functions (Mandatory Recommendation):
    The pci_extension.c module employs auxiliary functions (uint16_to_hex_str, uint8_to_hex_str) for the conversion of numerical values into hexadecimal string representations. For the purposes of modularity, maintainability, and reusability across the kernel (given that analogous functionalities are likely to be required by other driver modules), it is stringently advised that these functions be:

        Declared: Within the includes/base_kernel.h header file.

        Defined: Within a distinct, common utility source file (e.g., src/utils.c).

        Included: The aforementioned src/utils.c file must also be incorporated into the C_SOURCES list of the Makefile.

        Refinement: Upon the establishment of these globally accessible utility functions, all local or static definitions of uint16_to_hex_str and uint8_to_hex_str within pci_extension.c (and any other modules, such as the network driver, that may contain redundant implementations) shall be expunged. This ensures a singular point of definition and consistent usage across the kernel.

Subsequent to the adherence to these integration protocols and the successful recompilation of the kernel, the pci_scan command shall become accessible within the command-line interface of the Core-Kernel.

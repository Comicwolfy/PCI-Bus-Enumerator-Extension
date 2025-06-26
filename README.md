# PCI-Bus-Enumerator-Extension
## ESSENTIAL FOR WORKING ON REAL HARDWARE!!
## You WILL need this for bare-metal in any capacity!

PCI Bus Enumerator Extension for Core-Kernel: A Friendly Guide

This document is all about the PCI Bus Enumerator Extension. It's a key part of our Core-Kernel!
Getting Started (Overview)

So, the PCI Bus Enumerator is a really important bit of software in any modern operating system. Its main job is to find and check out all the hardware devices connected through the PCI bus. Unlike older connections, where everything had a fixed spot, PCI devices need to be actively scanned for so your computer can find them and set them up.

Getting this part working is SUPER IMPORTANT IF YOU EVER PLAN TO USE REAL COMPUTER HARDWARE. You'll absolutely need this for any low-level programming. It's the first step to being able to find and talk to all sorts of common computer parts, like advanced network cards, graphics cards, USB controllers, and even hard drive controllers. For anyone getting into bare-metal development, being able to identify and use PCI hardware is truly fundamental.
What It Actually Does (Functionality)

The PCI Bus Enumerator extension helps out with a few main things:

    Finding Devices: It systematically goes through all the possible PCI "bus numbers" (from 0 to 255) and "device slot numbers" (0 to 31) to pinpoint which PCI devices are actually plugged into your computer.

    Handling Multi-Function Devices: When it finds a device, it checks if it's a "single-function" device or a "multi-function" one (that's when one physical piece of hardware can do several different things). If it's multi-function, the extension will list all eight possible functions (0 to 7) that the device offers.

    Reading Device Info: For each PCI function it finds, it pulls out crucial setup details from a special area called the "PCI Configuration Space." This info includes:

        Vendor ID: This tells you who made the device.

        Device ID: This identifies the specific model of the device from that manufacturer.

        Class Code: This puts the device into a general category (like "Network Card" or "Graphics Card").

        Subclass Code: This narrows down the category even more (like "Ethernet Controller" or "VGA Compatible Controller").

        Programming Interface (ProgIF): This tells you how to program or control the device.

        Header Type: This shows the layout of the device's setup information (like if it's a standard device or a PCI-to-PCI bridge).

    Showing You What's There: All the information it collects—like the bus, slot, and function numbers, plus the Vendor/Device/Class/Subclass codes—gets printed to your screen. This is incredibly helpful for developers to confirm what hardware is recognized, whether they're using a virtual machine (like QEMU) or a real computer.

Just a thought: Right now, this part of the code mainly looks at "Bus 0." In more complex computer setups (which are common in real computers and some virtual machines), devices can be on other "secondary buses" that are connected through "PCI-to-PCI bridges." A complete PCI tool would usually go through these bridges to find all devices across the entire PCI system. Adding this deeper scanning is something we're planning for the future!
What Commands You Can Use

This extension adds just one main command for you to use in the kernel's command line:

    pci_scan

        What it does: This command kicks off a full scan of the PCI bus. After it's done, you'll see all the details about the devices it found printed right on your screen.

        How to use it: Just type pci_scan. It's that simple!

        What it might look like:

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



    (Just a heads-up, the exact stuff you see will depend on the specific hardware, whether it's a virtual machine or a real computer, where you're running Core-Kernel.)

How to Hook It Up (Integration)

To get the PCI Bus Enumerator Extension working in your Core-Kernel, just follow these steps:

    Get the Source File:

        You'll need the pci_extension.c source file.

    Put It in Place:

        Take the pci_extension.c file and put it in your kernel's src/extensions/ folder.

    Update the Makefile:

        Open up the Makefile (it's in your Core-Kernel project's main folder).

        Find the C_SOURCES list (that's where all the C files that get compiled are listed) and add the path to your new extension's file to it.

    # Here's an example from your Makefile
    C_SOURCES += src/extensions/pci_extension.c # Just add this line!

    Shared Helper Functions (Super Important!):
    The pci_extension.c code uses some helper functions (uint16_to_hex_str, uint8_to_hex_str) to change numbers into those hex strings. To keep things nice and tidy (and because other parts of your kernel or other drivers will probably need these same functions), it's a really smart idea to:

        Declare Them: In your includes/base_kernel.h file.

        Define Them: In a separate, common utility file (like src/utils.c).

        Include That File: Make sure you add that new src/utils.c file to your Makefile's C_SOURCES list too.

        Clean Up: Once you've set up these shared helper functions, you should remove any local or static versions of uint16_to_hex_str and uint8_to_hex_str from pci_extension.c (and any other modules, like the network driver, that might have them). This makes sure everyone uses the same, central version, which is much better!

After you follow these steps and rebuild your kernel successfully, the pci_scan command will be ready to use right in your Core-Kernel's command line!

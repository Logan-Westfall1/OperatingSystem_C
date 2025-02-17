# OperatingSystem_C

This is a fully functional MPX Operating System Programmed in C in a group with 3 other students.

![](https://giphy.com/explore/operating-system) 

#Description

This operating system is a Unix based operating system reading a text based user input in a custom made command system. The Operating System, titled "The C Team" started out polling based and could perform operations such as process management, context switching, dynamic memory management, and has robust input validation and error messaging. The final change made to the operating system is a switch from Polling based I/O to interrupt driven I/O through the use of an interrupt service routine. The main uses of the operating system is being able to create alarms, set and get the date, create and delete processes, and checking memory allocation for error handling.

#Usage

To launch the OS follow the following steps:

  Windows:
    1. Download Windows Subsystem for Linux: wsl --install -d ubuntu
    2. Update and download Qemu and GDB for Debugging: sudo apt update
                                                     : sudo apt install -y clang make nasm git binutils-i686-linux-gnu qemu-system-x86 gdb
    3. Download the R6 Tag (or previous tags for previous versions / iterations) and run the program in your IDE of choice.

  Mac:
    1. Download Homebrew: ![](https://brew.sh)
    2. Install GDB: brew install nasm qemu i686-elf-binutils i386-elf-gdb
    3. Download the R6 Tag (or previous tags for previous versions / iterations) and run the program in your IDE of choice.

#Authors
  Logan Westfall, Jacob Bock, Evan Anderson, Brenden Bolduck


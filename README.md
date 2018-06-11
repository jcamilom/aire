# Air quality with mbed OS and K64F

This is a repository for an air queality device using the K64F and mbed OS 5.

It consists of some demo's, which you can select in ``source/select-demo.h``.

Please install [mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).

Linux installation guide: [mbed linux toolchain](https://forums.mbed.com/t/installed-toolchain-and-mbed-cli-on-linux-tutorial-mbed-compile-failes/2809) 

Quick start install: [mbed quick start](https://os.mbed.com/docs/v5.8/tutorials/quick-start-offline.html#linux)

## Import the application

From the command-line, import the applications:

```
mbed import https://github.com/jcamilom/aire
cd aire
```

### Now compile

Invoke `mbed compile`, and specify the name of your platform and your favorite toolchain (`GCC_ARM`, `ARM`, `IAR`). For example, for the ARM Compiler 5:

```
mbed compile -m K64F -t ARM
```

### Program your board

1. Connect your mbed device to the computer over USB.
1. Copy the binary file to the mbed device.
1. Press the reset button to start the program.

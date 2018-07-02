# Air quality with mbed OS and K64F

This is a repository for an air queality device using the K64F and mbed OS 5.

It consists of some demo's, which you can select in ``source/select-demo.h``.

Please install [mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).

Linux installation guide: [mbed linux toolchain](https://forums.mbed.com/t/installed-toolchain-and-mbed-cli-on-linux-tutorial-mbed-compile-failes/2809) 

Quick start install: [mbed quick start](https://os.mbed.com/docs/v5.8/tutorials/quick-start-offline.html#linux)

## Installation guide
```bash
# mbed works with python 2.7 and 3.6 only with mbed versions greater than 5.8 
pip install mbed-cli
# Additional packages
pip install msgpack mbed-greentea mbed-host-tests mbed-ls mbed-test-wrapper
pip install colorama jinja2 intelhex junit_xml pyyaml mbed_host_tests mbed_greentea beautifulsoup4 fuzzywuzzy
# installing i386 architecture packages
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386
sudo apt-get update
sudo apt install gcc-arm-none-eabi -y
mbed config -G ARM_PATH "/usr/bin/arm-none-eabi-gcc"
sudo apt install mercurial -y
sudo apt-get -y install lib32ncurses5
# and mbed is installed...
```
## Import the application

From the command-line, import the applications:

```bash
mbed import https://github.com/jcamilom/aire
cd aire
```

### Now compile

Invoke `mbed compile`, and specify the name of your platform and your favorite toolchain (`GCC_ARM`, `ARM`, `IAR`). For example, for the GCC_ARM Compiler 5:

```bash
# compile
mbed compile -m K64F -t GCC_ARM
# compile & run
mbed compile -m K64F -t GCC_ARM --flash
```

### Program your board

1. Connect your mbed device to the computer over USB.
1. Copy the binary file to the mbed device.
1. Press the reset button to start the program.

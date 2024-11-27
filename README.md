# OrchestrOS

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white&color=orange)
![](https://tokei.rs/b1/github/garnek0/orchestros?type=c,c++,cheader,assembly,assemblyGAS,Rust&style=for-the-badge&color=orange)

A simple, hobby operating system. This is a rewrite of my first OS that focuses on simplicity, portability and maintainability.

## Features
- 64-bit kernel
- Bitmap physical memory allocator

## Building and Running

### Toolchain
You first need to build the toolchain by running the `build-toolchain` script. In the root project directory, run:
```
./scripts/build-toolchain <target_architecture> <num_jobs>
```
Replace `<target_architecture>` with your target architecture (e.g. `x86_64`).
Replace `<num_jobs>` with the number of parallel jobs you want make to use when building the toolchain (A good value for this generally ranges between 4-8).

It may take a while for the toolchain to build.

### Building and Running OrchestrOS
In the root directory, run either of the following commands:
```
make all # Build the OS and create an ISO image (MIGHT NOT WORK PROPERLY)
make all-hdd # Build the OS and create a HDD image

# NOTE: You can run the 'run*' commands without running any of the build commands listed above. The OS will be automatically built.

make run # (Build and) Run an ISO image of the OS using QEMU (x86_64 EFI) (MIGHT NOT WORK PROPERLY)
make run-hdd # (Build and) Run a HDD image of the OS using QEMU (x86_64 EFI)

make run-bios # (Build and) Run an ISO image of the OS using QEMU (x86_64 Legacy BIOS) (MIGHT NOT WORK PROPERLY)
make run-hdd-bios # (Build and) Run a HDD image of the OS using QEMU (x86_64 Legacy BIOS)

# You can also change the target architecture (Make sure you have the required cross-toolchain).
# Example: aarch64

make all-hdd ARCH=aarch64
make run-hdd ARCH=aarch64

# WARNING: Although the Makefile has specific run-* targets for non-x86 architectures, they are purely for the internal functioning of the build system and using them without specifying ARCH= will break stuff.
```

### Cross-platform Support
Even though the targets for non-x86 architectures exist, OrchestrOS will most likely not work properly if ran on anything other than an x86 PC (emulated or not). At this time, i'm only focusing on writing the base features of the OS. After those are done, i'll start working on cross-platform support.

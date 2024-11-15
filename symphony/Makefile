MAKEFLAGS += -rR
.SUFFIXES:

override OUTPUT := symphony.elf

# Convenience macro to reliably declare user overridable variables.
override USER_VARIABLE = $(if $(filter $(origin $(1)),default undefined),$(eval override $(1) := $(2)))

# Target architecture to build for. Default to x86_64.
$(call USER_VARIABLE,ARCH,x86_64)

# Check if the architecture is supported.
ifeq ($(filter $(ARCH),aarch64 riscv64 x86_64),)
    $(error Architecture $(ARCH) not supported)
endif

# User controllable C compiler command.
$(call USER_VARIABLE,CC,../toolchain/build/$(ARCH)/bin/$(ARCH)-elf-gcc)

# User controllable archiver command.
$(call USER_VARIABLE,AR,../toolchain/build/$(ARCH)/bin/$(ARCH)-elf-ar)

# User controllable linker command.
$(call USER_VARIABLE,LD,../toolchain/build/$(ARCH)/bin/$(ARCH)-elf-ld)

# User controllable C flags.
$(call USER_VARIABLE,CFLAGS,-g -O2 -pipe)

# User controllable C preprocessor flags. We set none by default.
$(call USER_VARIABLE,CPPFLAGS,)

ifeq ($(ARCH),x86_64)
    # User controllable nasm flags.
    $(call USER_VARIABLE,NASMFLAGS,-F dwarf -g)
endif

# User controllable linker flags. We set none by default.
$(call USER_VARIABLE,LDFLAGS,)

# Ensure the dependencies have been obtained.
ifneq ($(shell ( test '$(MAKECMDGOALS)' = clean || test '$(MAKECMDGOALS)' = distclean ); echo $$?),0)
    ifeq ($(shell ( ! test -d ../deps/freestnd-c-hdrs-0bsd || ! test -d ../deps/cc-runtime || ! test -f limine.h ); echo $$?),0)
        $(error Please run the get-deps script first)
    endif
endif

# Check if CC is Clang.
override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)

# Internal C flags that should not be changed by the user.
override CFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -nostdinc \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections

# Internal C preprocessor flags that should not be changed by the user.
override CPPFLAGS := \
    -I . \
	-I ../include/ \
    -isystem ../deps/freestnd-c-hdrs-0bsd \
    $(CPPFLAGS) \
    -MMD \
    -MP

ifeq ($(ARCH),x86_64)
    # Internal nasm flags that should not be changed by the user.
    override NASMFLAGS += \
        -Wall
endif

# Architecture specific internal flags.
ifeq ($(ARCH),x86_64)
    ifeq ($(CC_IS_CLANG),1)
        override CC += \
            -target x86_64-unknown-none
    endif
    override CFLAGS += \
        -m64 \
        -march=x86-64 \
        -mno-80387 \
        -mno-mmx \
        -mno-sse \
        -mno-sse2 \
        -mno-red-zone \
        -mcmodel=kernel
    override LDFLAGS += \
        -m elf_x86_64
    override NASMFLAGS += \
        -f elf64
endif
ifeq ($(ARCH),aarch64)
    ifeq ($(CC_IS_CLANG),1)
        override CC += \
            -target aarch64-unknown-none
    endif
    override CFLAGS += \
        -mgeneral-regs-only
    override LDFLAGS += \
        -m aarch64elf
endif
ifeq ($(ARCH),riscv64)
    ifeq ($(CC_IS_CLANG),1)
        override CC += \
            -target riscv64-unknown-none
        override CFLAGS += \
            -march=rv64imac
    else
        override CFLAGS += \
            -march=rv64imac_zicsr_zifencei
    endif
    override CFLAGS += \
        -mabi=lp64 \
        -mno-relax
    override LDFLAGS += \
        -m elf64lriscv \
        --no-relax
endif

# Internal linker flags that should not be changed by the user.
override LDFLAGS += \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -gc-sections \
    -T arch/$(ARCH)/linker.ld

# Use "find" to glob all *.c, *.S, and *.asm files in the tree and obtain the
# object and header dependency file names.
override CFILES := $(shell find -L * -type f -name '*.c' | LC_ALL=C sort)
override ASFILES := $(shell find -L * -type f -name '*.S' | LC_ALL=C sort)
ifeq ($(ARCH),x86_64)
override NASMFILES := $(shell find -L * -type f -name '*.asm' | LC_ALL=C sort)
endif
override OBJ := $(addprefix ../build/$(ARCH)/symphony/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o))
ifeq ($(ARCH),x86_64)
override OBJ += $(addprefix ../build/$(ARCH)/symphony/,$(NASMFILES:.asm=.asm.o))
endif
override HEADER_DEPS := $(addprefix ../build/$(ARCH)/symphony/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

# Default target.
.PHONY: all
all: ../build/$(ARCH)/symphony/$(OUTPUT)

# Link rules for building the C compiler runtime.
../deps/cc-runtime-$(ARCH)/cc-runtime.a: ../deps/cc-runtime/*
	rm -rf ../deps/cc-runtime-$(ARCH)
	cp -r ../deps/cc-runtime ../deps/cc-runtime-$(ARCH)
	$(MAKE) -C ../deps/cc-runtime-$(ARCH) -f cc-runtime.mk \
		CC="../$(CC)" \
		AR="../$(AR)" \
		CFLAGS="$(CFLAGS)" \
		CPPFLAGS='-isystem ../../deps/freestnd-c-hdrs-0bsd -DCC_RUNTIME_NO_FLOAT'

# Link rules for the final executable.
../build/$(ARCH)/symphony/$(OUTPUT): Makefile arch/$(ARCH)/linker.ld $(OBJ) ../deps/cc-runtime-$(ARCH)/cc-runtime.a
	mkdir -p "$$(dirname $@)"
	$(LD) $(OBJ) ../deps/cc-runtime-$(ARCH)/cc-runtime.a $(LDFLAGS) -o $@

# Include header dependencies.
-include $(HEADER_DEPS)

# Compilation rules for *.c files.
../build/$(ARCH)/symphony/%.c.o: %.c Makefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.S files.
../build/$(ARCH)/symphony/%.S.o: %.S Makefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

ifeq ($(ARCH),x86_64)
# Compilation rules for *.asm (nasm) files.
../build/$(ARCH)/symphony/%.asm.o: %.asm Makefile
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@
endif

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf ../deps/cc-runtime-$(ARCH)

# Remove everything built and generated including downloaded dependencies.
.PHONY: distclean
distclean: 
	rm -rf ../deps/freestnd-c-hdrs-0bsd ../deps/cc-runtime* limine.h

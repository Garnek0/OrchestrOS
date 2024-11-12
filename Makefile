MAKEFLAGS += -rR
.SUFFIXES:

# Convenience macro to reliably declare user overridable variables.
override USER_VARIABLE = $(if $(filter $(origin $(1)),default undefined),$(eval override $(1) := $(2)))

# Target architecture to build for. Default to x86_64.
$(call USER_VARIABLE,ARCH,x86_64)

# Default user QEMU flags. These are appended to the QEMU command calls.
$(call USER_VARIABLE,QEMUFLAGS,-m 2G)

.PHONY: all
all: orchestros.iso

.PHONY: all-hdd
all_hdd: orchestros.hdd

.PHONY: run
run: run-$(ARCH)

.PHONY: run-hdd
run-hdd: run-hdd-$(ARCH)

.PHONY: run-x86_64
run-x86_64: ovmf/ovmf-code-$(ARCH).fd orchestros.iso
	qemu-system-$(ARCH) \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-cdrom orchestros.iso \
		$(QEMUFLAGS)

.PHONY: run-hdd-x86_64
run-hdd-x86_64: ovmf/ovmf-code-$(ARCH).fd orchestros.hdd
	qemu-system-$(ARCH) \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-hda orchestros.hdd \
		$(QEMUFLAGS)

.PHONY: run-aarch64
run-aarch64: ovmf/ovmf-code-$(ARCH).fd orchestros.iso
	qemu-system-$(ARCH) \
		-M virt \
		-cpu cortex-a72 \
		-device ramfb \
		-device qemu-xhci \
		-device usb-kbd \
		-device usb-mouse \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-cdrom orchestros.iso \
		$(QEMUFLAGS)

.PHONY: run-hdd-aarch64
run-hdd-aarch64: ovmf/ovmf-code-$(ARCH).fd orchestros.hdd
	qemu-system-$(ARCH) \
		-M virt \
		-cpu cortex-a72 \
		-device ramfb \
		-device qemu-xhci \
		-device usb-kbd \
		-device usb-mouse \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-hda orchestros.hdd \
		$(QEMUFLAGS)

.PHONY: run-riscv64
run-riscv64: ovmf/ovmf-code-$(ARCH).fd orchestros.iso
	qemu-system-$(ARCH) \
		-M virt \
		-cpu rv64 \
		-device ramfb \
		-device qemu-xhci \
		-device usb-kbd \
		-device usb-mouse \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-cdrom orchestros.iso \
		$(QEMUFLAGS)

.PHONY: run-hdd-riscv64
run-hdd-riscv64: ovmf/ovmf-code-$(ARCH).fd orchestros.hdd
	qemu-system-$(ARCH) \
		-M virt \
		-cpu rv64 \
		-device ramfb \
		-device qemu-xhci \
		-device usb-kbd \
		-device usb-mouse \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-hda orchestros.hdd \
		$(QEMUFLAGS)

.PHONY: run-loongarch64
run-loongarch64: ovmf/ovmf-code-$(ARCH).fd orchestros.iso
	qemu-system-$(ARCH) \
		-M virt \
		-cpu la464 \
		-device ramfb \
		-device qemu-xhci \
		-device usb-kbd \
		-device usb-mouse \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-cdrom orchestros.iso \
		$(QEMUFLAGS)

.PHONY: run-hdd-loongarch64
run-hdd-loongarch64: ovmf/ovmf-code-$(ARCH).fd orchestros.hdd
	qemu-system-$(ARCH) \
		-M virt \
		-cpu la464 \
		-device ramfb \
		-device qemu-xhci \
		-device usb-kbd \
		-device usb-mouse \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-$(ARCH).fd,readonly=on \
		-hda orchestros.hdd \
		$(QEMUFLAGS)


.PHONY: run-bios
run-bios: orchestros.iso
	qemu-system-$(ARCH) \
		-M q35 \
		-cdrom orchestros.iso \
		-boot d \
		$(QEMUFLAGS)

.PHONY: run-hdd-bios
run-hdd-bios: orchestros.hdd
	qemu-system-$(ARCH) \
		-M q35 \
		-hda orchestros.hdd \
		$(QEMUFLAGS)

ovmf/ovmf-code-$(ARCH).fd:
	mkdir -p ovmf
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-$(ARCH).fd
	case "$(ARCH)" in \
		aarch64) dd if=/dev/zero of=$@ bs=1 count=0 seek=67108864 2>/dev/null;; \
		riscv64) dd if=/dev/zero of=$@ bs=1 count=0 seek=33554432 2>/dev/null;; \
	esac

limine/limine:
	rm -rf limine
	git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1
	$(MAKE) -C limine

symphony-deps:
	./scripts/get-deps
	touch symphony-deps

.PHONY: symphony
symphony: symphony-deps symphony
	mkdir -p build
	$(MAKE) -C symphony

orchestros.iso: limine/limine symphony
	rm -rf iso_root
	cp -v build/$(ARCH)/symphony/symphony.elf iso_root/
	mkdir -p iso_root/limine
	cp -v limine.conf iso_root/limine/
	mkdir -p iso_root/EFI/BOOT
ifeq ($(ARCH),x86_64)
	cp -v limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/limine/
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J -b limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o orchestros.iso
	./limine/limine bios-install orchestros.iso
endif
ifeq ($(ARCH),aarch64)
	cp -v limine/limine-uefi-cd.bin iso_root/limine/
	cp -v limine/BOOTAA64.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J \
		-hfsplus -apm-block-size 2048 \
		--efi-boot limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o orchestros.iso
endif
ifeq ($(ARCH),riscv64)
	cp -v limine/limine-uefi-cd.bin iso_root/limine/
	cp -v limine/BOOTRISCV64.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J \
		-hfsplus -apm-block-size 2048 \
		--efi-boot limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o orchestros.iso
endif
ifeq ($(ARCH),loongarch64)
	cp -v limine/limine-uefi-cd.bin iso_root/limine/
	cp -v limine/BOOTLOONGARCH64.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J \
		-hfsplus -apm-block-size 2048 \
		--efi-boot limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o orchestros.iso
endif
	rm -rf iso_root

orchestros.hdd: limine/limine symphony
	rm -f orchestros.hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=orchestros.hdd
	sgdisk orchestros.hdd -n 1:2048 -t 1:ef00
ifeq ($(ARCH),x86_64)
	./limine/limine bios-install orchestros.hdd
endif
	mformat -i orchestros.hdd@@1M
	mmd -i orchestros.hdd@@1M ::/EFI ::/EFI/BOOT ::/limine
	mcopy -i orchestros.hdd@@1M build/$(ARCH)/symphony/symphony.elf ::/
	mcopy -i orchestros.hdd@@1M limine.conf ::/limine
ifeq ($(ARCH),x86_64)
	mcopy -i orchestros.hdd@@1M limine/limine-bios.sys ::/limine
	mcopy -i orchestros.hdd@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i orchestros.hdd@@1M limine/BOOTIA32.EFI ::/EFI/BOOT
endif
ifeq ($(ARCH),aarch64)
	mcopy -i orchestros.hdd@@1M limine/BOOTAA64.EFI ::/EFI/BOOT
endif
ifeq ($(ARCH),riscv64)
	mcopy -i orchestros.hdd@@1M limine/BOOTRISCV64.EFI ::/EFI/BOOT
endif
ifeq ($(ARCH),loongarch64)
	mcopy -i orchestros.hdd@@1M limine/BOOTLOONGARCH64.EFI ::/EFI/BOOT
endif

.PHONY: clean
clean:
	$(MAKE) -C symphony clean
	rm -rf iso_root *.iso *.hdd build

.PHONY: distclean
distclean:
	$(MAKE) -C symphony distclean
	rm -rf iso_root *.iso *.hdd symphony-deps deps limine ovmf build

/*
 * File: mm/pmm.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * Bitmap-based page frame allocator.
 */

#include <symphony/mm.h>
#include <symphony/debug.h>
#include <symphony/boot_proto.h>

static uint8_t* bitmap;
static size_t bitmapSize;

// Mark page as allocated in the bitmap.
static void pmm_bitmap_set(uint64_t page) {
	assert(page/8 < bitmapSize, "PMM bitmap set operation out of bounds!\n");

	bitmap[page/8] |= (0b10000000 >> (page%8));
}

// Mark page as free in the bitmap.
static void pmm_bitmap_clear(uint64_t page) {
	assert(page/8 < bitmapSize, "PMM bitmap clear operation out of bounds!\n");

	bitmap[page/8] &= ~((0b10000000 >> (page%8)));
}

// We dont need to track every page in the physical address space. The last
// page that needs to be tracked by the bitmap is (generally) the last usable
// page. Not tracking every physical page saves a lot of memory that would
// otherwise be needed for the bitmap.
static uint64_t pmm_bitmap_last_tracked_page() {
	struct boot_proto_memmap_entry entry;

	for (uint64_t i = boot_proto_memmap_entry_count()-1; i > 0; i--) {
		entry = boot_proto_memmap_entry_get(i);
		if (entry.type == BOOT_PROTO_MEMMAP_USABLE)
			return (entry.base+entry.length)/PAGE_SIZE;
	}

	return 0;
}

// Find free pages using the bitmap.
static uint64_t pmm_find_free_pages(int pages) {
	bool inChunk = false;
	int pagesFoundInChunk = 0;
	uint64_t basePage;

	for (size_t i = 0; i < bitmapSize; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			if (!(bitmap[i] & (0b10000000 >> j))) {
				if (!inChunk) {
					basePage = (i*8)+j;
					inChunk = true;
				}

				pagesFoundInChunk++;
				if (pagesFoundInChunk == pages)
					return basePage;
			} else {
				inChunk = false;
				pagesFoundInChunk = 0;
			}
		}
	}

	debug_panic("Out of Memory!");
	
	__builtin_unreachable();
}

int pmm_init(void) {
	bitmapSize = ALIGN_UP(pmm_bitmap_last_tracked_page(), 8) / 8;
	
	assert(bitmapSize, "Something is wrong with the PMM bitmap size!\n");

	struct boot_proto_memmap_entry entry;

	for (uint64_t i = 0; i < boot_proto_memmap_entry_count(); i++) {
		entry = boot_proto_memmap_entry_get(i);

		if (entry.type != BOOT_PROTO_MEMMAP_USABLE)
			continue;

		if (entry.base == 0) {
			entry.base += PAGE_SIZE;
			if (entry.length >= PAGE_SIZE) 
				entry.length -= PAGE_SIZE;
			else
				entry.length = 0;
		}

		if (entry.length >= bitmapSize) {
			bitmap = (uint8_t*)(entry.base + boot_proto_hhdm_offset());
			break;
		}
	}

	assert((uint64_t)bitmap, "Bitmap not properly allocated!\n");

	memset(bitmap, 0xff, bitmapSize);

	for (uint64_t i = 0; i < boot_proto_memmap_entry_count(); i++) {
		entry = boot_proto_memmap_entry_get(i);

		// Mark all "Usable" memmap entries as free.
		if (entry.type != BOOT_PROTO_MEMMAP_USABLE)
			continue;

		for (uint64_t j = (entry.base >> 12); j < ((entry.base + entry.length) >> 12); j++)
			pmm_bitmap_clear(j);
	}

	// Now, mark all the bitmap pages as allocated.
	
	uint64_t bitmapPhysAddr = ((uint64_t)bitmap - boot_proto_hhdm_offset());

	for (uint64_t i = bitmapPhysAddr; i < bitmapPhysAddr + ALIGN_UP(bitmapSize, PAGE_SIZE); i+=PAGE_SIZE)
		pmm_bitmap_set(i/PAGE_SIZE);

	// Also, the first page (page 0) is never used, so it should be marked
	// as allocated as well.
	pmm_bitmap_set(0);

	debug_log(LOGLEVEL_INFO, "PMM initialized\n");

	return 0;
}

void* pmm_alloc(int pages) {
	uint64_t base = pmm_find_free_pages(pages);

	for (uint64_t i = base; i < base + pages; i++)
		pmm_bitmap_set(i);

	return (void*)(base << 12);
}

int pmm_free(void* base, int pages) {
	for (uint64_t i = ((uint64_t)base >> 12); i < ((uint64_t)base >> 12) + pages; i++)
		pmm_bitmap_clear(i);

	memset(base + boot_proto_hhdm_offset(), 0xff, PAGE_SIZE*pages);

	return 0;
}

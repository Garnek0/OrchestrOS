/*
 * File: mm/kheap.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * Kernel Heap Implementation.
 */

#include <symphony/mm.h>
#include <symphony/boot_proto.h>
#include <symphony/string.h>
#include <symphony/debug.h>

struct kheap_block_header {
	size_t size;
	bool free;
	struct kheap_block_header* next;
	struct kheap_block_header* prev;
};

static struct kheap_block_header* blockListStart;
static struct kheap_block_header* blockListEnd;
static size_t kheapSize;
static size_t kheapSizeUsable;

// Add more pages to the kernel heap
static void kheap_extend(size_t size) {
	size += sizeof(struct kheap_block_header);
	if (size % PAGE_SIZE != 0) size = ALIGN_UP(size, PAGE_SIZE);

	struct kheap_block_header* newbh = (struct kheap_block_header*)((uint64_t)pmm_alloc(size/PAGE_SIZE) + boot_proto_hhdm_offset());
	memset(newbh, 0, size);

	newbh->size = size - sizeof(struct kheap_block_header);
	newbh->free = true;
	
	newbh->prev = blockListEnd;
	blockListEnd->next = newbh;
	blockListEnd = newbh;
	newbh->next = NULL;

	kheapSize += size;
	kheapSizeUsable += newbh->size;
}

// Create a block with the specified usable size from another larger block.
static void kheap_create_block(struct kheap_block_header* bh, size_t size) {
	struct kheap_block_header* newbh = (struct kheap_block_header*)((uint64_t)bh + sizeof(struct kheap_block_header) + size);
	memset(newbh, 0, sizeof(struct kheap_block_header));

	// bh will have the usable size requested
	// newbh will have whatever is left of the original block

	newbh->size = bh->size - (size + sizeof(struct kheap_block_header));
	bh->size = size;

	newbh->next = bh->next;
	bh->next = newbh;
	newbh->prev = bh;
	if (newbh->next)
		newbh->next->prev = newbh;

	newbh->free = true;

	if (bh == blockListEnd)
		blockListEnd = newbh;
}

int kheap_init() {
	blockListStart = blockListEnd = (struct kheap_block_header*)((uint64_t)pmm_alloc(KHEAP_INIT_PAGES) + boot_proto_hhdm_offset());
	memset(blockListStart, 0, sizeof(struct kheap_block_header));
	
	blockListStart->size = (KHEAP_INIT_PAGES * PAGE_SIZE) - sizeof(struct kheap_block_header);
	blockListStart->prev = NULL;
	blockListStart->next = NULL;
	blockListStart->free = true;

	kheapSize = KHEAP_INIT_PAGES * PAGE_SIZE;
	kheapSizeUsable = blockListStart->size;

	return 0;
}

void* kmalloc(size_t size) {
	if (size % 10 != 0) size = ALIGN_UP(size, 0x10);
	
	for (struct kheap_block_header* bh = blockListStart; bh; bh = bh->next) {
		if (!(bh->free) || bh->size < size)
			continue;

		if (bh->size == size || ((bh->size > size) && (bh->size <= (size + sizeof(struct kheap_block_header))))) {
			bh->free = false;
			return (void*)((uint64_t)bh + sizeof(struct kheap_block_header));
		} else if (bh->size > (size + sizeof(struct kheap_block_header))) {
			kheap_create_block(bh, size);
			bh->free = false;
			return (void*)((uint64_t)bh + sizeof(struct kheap_block_header));
		}
	}

	kheap_extend(size);

	return kmalloc(size);
}

void kfree(void* ptr) {
	if (ptr == NULL)
		return;

	struct kheap_block_header* bh = (struct kheap_block_header*)((uint64_t)ptr - sizeof(struct kheap_block_header));

	if (bh->free) {
		debug_log(LOGLEVEL_WARN, "Invalid kfree() address!\n");
		return;
	}

	bh->free = true;

	// Poison the block
	memset((void*)((uint64_t)bh + sizeof(struct kheap_block_header)), 0xFF, bh->size);

	// Try to merge with the next/previous block.

	if (bh->next) {
		if (bh->next->free && ((void*)((uint64_t)bh + (uint64_t)bh->size + sizeof(struct kheap_block_header)) == bh->next)) {
			if (bh->next == blockListEnd)
				blockListEnd = bh;

			bh->size += bh->next->size + sizeof(struct kheap_block_header);

			if (bh->next->next)
				bh->next->next->prev = bh;

			bh->next = bh->next->next;
		}
	}

	if (bh->prev) {
		if (bh->prev->free && ((void*)((uint64_t)bh->prev + (uint64_t)bh->prev->size + sizeof(struct kheap_block_header)) == bh)) {
			if (bh == blockListEnd)
				blockListEnd = bh->prev;

			bh->prev->size += bh->size + sizeof(struct kheap_block_header);

			if (bh->next)
				bh->next->prev = bh->prev;

			bh->prev->next = bh->next;
		}
	}
}

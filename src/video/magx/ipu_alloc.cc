/*
 * Copyright 2005-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*!
 * @file ipu_alloc.c
 *
 * @brief simple ipu allocation driver for linux
 *
 * @ingroup IPU
 */

#include "ipu_alloc.h"

#include <stdio.h>
#include <stdlib.h>

static unsigned long int gIPUPoolStart = 0;
static unsigned long int gTotalPages = 0;
static unsigned long int gAlignment = IPU_PAGE_ALIGN;
static memDesc *gAllocatedDesc = NULL;
/*!
 * ipu_pool_initialize
 *
 * @param       memPool         start address of the pool
 * @param       poolSize        memory pool size
 * @param       alignment       alignment for example page alignmnet will be 4K
 *
 * @return      0 for success  -1 for errors
 */
int ipu_pool_initialize(unsigned long int memPool, unsigned long int poolSize, unsigned long int alignment)
{
	gAlignment = alignment;
	if (gAlignment == 0) {
		printf("ipu_pool_initialize : gAlignment can not be zero.\n");
		gAlignment = IPU_PAGE_ALIGN;
	}

	gTotalPages = poolSize / gAlignment;
	gIPUPoolStart = (unsigned long int) memPool;

	gAllocatedDesc = (memDesc *)malloc(sizeof(memDesc));
	if (!gAllocatedDesc) {
		printf("ipu_pool_initialize : malloc failed \n");
		return (-1);
	}

	gAllocatedDesc->start = 0;
	gAllocatedDesc->end = 0;
	gAllocatedDesc->next = NULL;

	return (0);
}

/*!
 * ipu_malloc
 *
 * @param       size        memory pool size
 *
 * @return      physical address, 0 for error
 */
unsigned long int ipu_malloc(unsigned long int size)
{
	memDesc *prevDesc = NULL;
	memDesc *nextDesc = NULL;
	memDesc *currentDesc = NULL;
	unsigned long int pages = (size + gAlignment - 1) / gAlignment;

	printf("ipu_malloc alloacte page %d  gTotalPages %d\n", pages,
		gTotalPages);

	if ((size == 0) || (pages > gTotalPages))
		return 0;
	
	currentDesc = (memDesc *)malloc(sizeof(memDesc));
	if (!currentDesc) {
		printf("ipu_malloc: malloc failed \n");
		return 0;
	}

	/* Create the first Allocated descriptor */
	if (!gAllocatedDesc->next) {
		gAllocatedDesc->next = currentDesc;
		currentDesc->start = 0;
		currentDesc->end = pages;
		currentDesc->next = NULL;
		printf("ipu_malloc: 1st current->start %d current->end %d\n",
			currentDesc->start, currentDesc->end);
		return (gIPUPoolStart + currentDesc->start * gAlignment);
	}

	/* Find the free spot */
	prevDesc = gAllocatedDesc;
	while (prevDesc->next) {
		nextDesc = prevDesc->next;
		if (pages <= nextDesc->start - prevDesc->end) {
			currentDesc->start = prevDesc->end;
			currentDesc->end = currentDesc->start + pages;
			currentDesc->next = nextDesc;
			prevDesc->next = currentDesc;
			printf("ipu_malloc: find middle cur->start %d cur->end %d\n",
				currentDesc->start, currentDesc->end);
			break;
		}
		prevDesc = nextDesc;
	}

	/* Do not find the free spot inside the chain, append to the end */
	if (!prevDesc->next) {
		if (pages > (gTotalPages - prevDesc->end)) {
			printf("ipu_malloc: page %d gTotalPages %d prevDesc->end %d\n",
				pages, gTotalPages, prevDesc->end);
			return 0;
		} else {
			currentDesc->start = prevDesc->end;
			currentDesc->end = currentDesc->start + pages;
			currentDesc->next = NULL;
			prevDesc->next = currentDesc;
			printf("ipu_malloc: append end:cur->start %d cur->end %d\n",
				currentDesc->start, currentDesc->end);
		}
	}

	printf("ipu_malloc: return %x\n",
		gIPUPoolStart + currentDesc->start * gAlignment);

	return (gIPUPoolStart + currentDesc->start * gAlignment);
}

/*!
 * ipu_free
 *
 * @param       physical        physical address try to free
 *
 */
void ipu_free(unsigned long int physical)
{
	memDesc *prevDesc = NULL;
	memDesc *nextDesc = NULL;
	unsigned long int pages = (physical - gIPUPoolStart) / gAlignment;

	printf("ipu_free alloacte page %d \n", pages);
	/* Protect the memory pool data structures. */
	prevDesc = gAllocatedDesc;
	while (prevDesc->next) {
		nextDesc = prevDesc->next;
		if (nextDesc->start == pages) {
			prevDesc->next = nextDesc->next;
			printf("ipu_free next->start %x next->end %d \n",
				nextDesc->start, nextDesc->end);
			printf("ipu_free prev->start %x prev->end %d \n",
				prevDesc->start, prevDesc->end);
			free(nextDesc);
			break;
		}
		prevDesc = prevDesc->next;
	}
	/* All done with memory pool data structures. */
}


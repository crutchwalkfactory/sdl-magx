/*
 * Copyright 2005-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU Lesser General 
 * Public License.  You may obtain a copy of the GNU Lesser General 
 * Public License Version 2.1 or later at the following locations:
 *
 * http://www.opensource.org/licenses/lgpl-license.html
 * http://www.gnu.org/copyleft/lgpl.html
 */

/*!
 * @file ipu_alloc.h
 *
 * @brief simple ipu allocation driver for linux
 *
 * @ingroup IPU
 */
#ifndef _IPU_ALLOC_H_
#define _IPU_ALLOC_H_

//Added
#include <sys/types.h>
#define IPU_MEM_ALIGN(a) ( (((a)+IPU_PAGE_ALIGN-1)/IPU_PAGE_ALIGN)*IPU_PAGE_ALIGN )
//

#define IPU_PAGE_ALIGN   ((unsigned long int) 0x00001000)

typedef struct _memDesc {
	unsigned long int start;
	unsigned long int end;
	struct _memDesc *next;
} memDesc;

/*!
 * ipu_pool_initialize
 *
 * @param       memPool         start address of the pool
 * @param       poolSize        memory pool size
 * @param       alignment       alignment for example page alignmnet will be 0x1000
 *
 * @return      0 for success  -1 for errors
 */
int ipu_pool_initialize(unsigned long int memPool, unsigned long int poolSize, unsigned long int alignment);

/*!
 * ipu_malloc
 *
 * @param       size        memory pool size
 *
 * @return      physical address, 0 for error
 */
unsigned long int ipu_malloc(unsigned long int size);

/*!
 * ipu_free
 *
 * @param       physical        physical address try to free
 *
 */
void ipu_free(unsigned long int physical);

#endif

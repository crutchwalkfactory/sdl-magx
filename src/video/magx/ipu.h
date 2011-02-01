/*
 * Copyright 2005-2006 Freescale Semiconductor, Inc.
 * Copyright (C) 2006-2007 Motorola, Inc. 
 */
/*
 * The code contained herein is licensed under the GNU Lesser General
 * Public License.  You may obtain a copy of the GNU Lesser General
 * Public License Version 2.1 or later at the following locations:
 *
 * http://www.opensource.org/licenses/lgpl-license.html
 * http://www.gnu.org/copyleft/lgpl.html
 *
 * Date     Author      Comment
 * 10/2006  Motorola    Added support for QVGA Sanyo display, additional pixel
 *                      packing formats, and a configurable memory location.
 * 01/2007  Motorola    Added support for configurable IPU memory size.
 * 06/2007  Motorola    Fix to support setting of ADC serial interface bit
 *                      width in ipu driver.
 * 06/2007  Motorola    Removed lines related HVGA.
 * 11/2007  Motorola    Add declaration of a function
 */

/*!
 * @defgroup IPU MXC Image Processing Unit (IPU) Driver
 */
/*!
 * @file ipu.h
 * 
 * @brief This file contains the IPU driver API declarations. 
 * 
 * @ingroup IPU
 */

#ifndef _INCLUDE_IPU_H_
#define _INCLUDE_IPU_H_

#include <linux/types.h>

#if defined(CONFIG_MOT_FEAT_IPU_MEM_ADDR)
extern unsigned long ipu_mem_addr;	/* dynamically defined in setup.c */ 
extern unsigned long ipu_mem_size;
extern int ipu_dynamic_pool;
#define MXCIPU_MEM_ADDRESS ipu_mem_addr
#define MXCIPU_MEM_SIZE ipu_mem_size
#endif

/*!
 * Enumeration of IPU rotation modes
 */
typedef enum {
	/* Note the enum values correspond to BAM value	*/
	IPU_ROTATE_NONE = 0,
	IPU_ROTATE_VERT_FLIP = 1,
	IPU_ROTATE_HORIZ_FLIP = 2,
	IPU_ROTATE_180 = 3,
	IPU_ROTATE_90_RIGHT = 4,
	IPU_ROTATE_90_RIGHT_VFLIP = 5,
	IPU_ROTATE_90_RIGHT_HFLIP = 6,
	IPU_ROTATE_90_LEFT = 7,
} ipu_rotate_mode_t;

/*!
 * Enumeration of Post Filter modes
 */
typedef enum {
	PF_DISABLE_ALL = 0,
	PF_MPEG4_DEBLOCK = 1,
	PF_MPEG4_DERING = 2,
	PF_MPEG4_DEBLOCK_DERING = 3,
	PF_H264_DEBLOCK = 4,
} pf_operation_t;

/*!
 * Enumeration of Synchronous (Memory-less) panel types
 */
typedef enum {
	IPU_PANEL_SHARP_TFT,
	IPU_PANEL_TFT,
	IPU_PANEL_SANYO_TFT,
} ipu_panel_t;

/*  IPU Pixel format definitions */
/*  Four-character-code (FOURCC) */
#define ipu_fourcc(a,b,c,d)\
        (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

/*!
 * @name IPU Pixel Formats
 *
 * Pixel formats are defined with ASCII FOURCC code. The pixel format codes are
 * the same used by V4L2 API.
 */

/*! @{ */
/*! @name Generic or Raw Data Formats *//*! @{ */
#define IPU_PIX_FMT_GENERIC ipu_fourcc('I','P','U','0')	/*!< IPU Generic Data */
#define IPU_PIX_FMT_GENERIC_32 ipu_fourcc('I','P','U','1')	/*!< IPU Generic Data */
/*! @} */

/*! @name RGB Formats *//*! @{ */
#define IPU_PIX_FMT_RGB332  ipu_fourcc('R','G','B','1')	/*!<  8  RGB-3-3-2     */
#define IPU_PIX_FMT_RGB555  ipu_fourcc('R','G','B','O')	/*!< 16  RGB-5-5-5     */
#define IPU_PIX_FMT_RGB565  ipu_fourcc('R','G','B','P')	/*!< 16  RGB-5-6-5     */
#if defined(CONFIG_MOT_FEAT_FB_MXC_RGB)
#define IPU_PIX_FMT_BGR565  ipu_fourcc('B','G','R','P') /*!< 16  BGR-5-6-5     */
#endif
#define IPU_PIX_FMT_RGB666  ipu_fourcc('R','G','B','6')	/*!< 18  RGB-6-6-6     */
#define IPU_PIX_FMT_BGR666  ipu_fourcc('B','G','R','0') /*!< 18  BGR-6-6-6     */
#define IPU_PIX_FMT_BGR24   ipu_fourcc('B','G','R','3')	/*!< 24  BGR-8-8-8     */
#define IPU_PIX_FMT_RGB24   ipu_fourcc('R','G','B','3')	/*!< 24  RGB-8-8-8     */
#define IPU_PIX_FMT_BGRA6666  ipu_fourcc('B','G','R','6') /*!< 24  BGR-6-6-6-6 */
#define IPU_PIX_FMT_BGR32   ipu_fourcc('B','G','R','4')	/*!< 32  BGR-8-8-8-8   */
#define IPU_PIX_FMT_BGRA32  ipu_fourcc('B','G','R','A')	/*!< 32  BGR-8-8-8-8   */
#define IPU_PIX_FMT_RGB32   ipu_fourcc('R','G','B','4')	/*!< 32  RGB-8-8-8-8   */
#define IPU_PIX_FMT_RGBA32  ipu_fourcc('R','G','B','A')	/*!< 32  RGB-8-8-8-8   */
#define IPU_PIX_FMT_ABGR32  ipu_fourcc('A','B','G','R')	/*!< 32  ABGR-8-8-8-8  */
/*! @} */

/*! @name YUV Interleaved Formats *//*! @{ */
#define IPU_PIX_FMT_YUYV    ipu_fourcc('Y','U','Y','V')	/*!< 16  YUV 4:2:2     */
#define IPU_PIX_FMT_UYVY    ipu_fourcc('U','Y','V','Y')	/*!< 16  YUV 4:2:2     */
#define IPU_PIX_FMT_Y41P    ipu_fourcc('Y','4','1','P')	/*!< 12  YUV 4:1:1     */
/*! @} */

/*! @name YUV Planar Formats *//*! @{ */
#define IPU_PIX_FMT_GREY    ipu_fourcc('G','R','E','Y')	/*!< 8  Greyscale     */
#define IPU_PIX_FMT_YVU410P ipu_fourcc('Y','V','U','9')	/*!< 9  YVU 4:1:0     */
#define IPU_PIX_FMT_YUV410P ipu_fourcc('Y','U','V','9')	/*!< 9  YUV 4:1:0     */
#define IPU_PIX_FMT_YVU420P ipu_fourcc('Y','V','1','2')	/*!< 12  YVU 4:2:0     */
#define IPU_PIX_FMT_YUV420P ipu_fourcc('I','4','2','0')	/*!< 12  YUV420 planar */
#define IPU_PIX_FMT_YUV420P2 ipu_fourcc('Y','U','1','2')	/*!< 12  YUV 4:2:0 planar */
#define IPU_PIX_FMT_YVU422P ipu_fourcc('Y','V','1','6')	/*!< 16  YVU422 planar */
/*! @} */

/*#define IPU_PIX_FMT_YUV422P ipu_fourcc('4','2','2','P')  16  YVU422 planar */
/*#define IPU_PIX_FMT_YUV411P ipu_fourcc('4','1','1','P')  16  YVU411 planar */
/*#define IPU_PIX_FMT_YYUV    ipu_fourcc('Y','Y','U','V')  16  YUV 4:2:2     */

/* IPU Driver channels definitions.	*/
/* Note these are different from IDMA channels */
#define _MAKE_CHAN(num, in, out, sec)    ((num << 24) | (sec << 16) | (out << 8) | in)
#define IPU_CHAN_ID(ch)         (ch >> 24)
#define IPU_CHAN_SEC_DMA(ch)    ((uint32_t) (ch >> 16) & 0xFF)
#define IPU_CHAN_OUT_DMA(ch)    ((uint32_t) (ch >> 8) & 0xFF)
#define IPU_CHAN_IN_DMA(ch)     ((uint32_t) (ch & 0xFF))


// ...

#endif //_INCLUDE_IPU_H_

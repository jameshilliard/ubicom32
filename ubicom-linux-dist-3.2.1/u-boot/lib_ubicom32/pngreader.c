/*
 * pngreader.c
 *	Small PNG decompressor
 *
 * (C) Copyright 2010
 * Ubicom, Inc. www.ubicom.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include "png/png.h"

#ifdef DEBUG
#define PNGREADER_DEBUG(x...) printf(x)
#else
#define PNGREADER_DEBUG(...)
#endif

struct pngreader_info {
	void	*fb;
	int	width;
	int	height;
};

/*
* pngreader_info_callback
*/
static void pngreader_info_callback(png_structp pngp, png_infop infop)
{
	PNGREADER_DEBUG("Width    :   %lu\n", png_get_image_width(pngp, infop));
	PNGREADER_DEBUG("Height   :   %lu\n", png_get_image_height(pngp, infop));
	PNGREADER_DEBUG("Depth    :   %d\n", png_get_bit_depth(pngp, infop));
	PNGREADER_DEBUG("Col Type :   %d\n", png_get_color_type(pngp, infop));
	PNGREADER_DEBUG("Int Type :   %d\n", png_get_interlace_type(pngp, infop));
	PNGREADER_DEBUG("Row bytes:   %d\n", png_get_rowbytes(pngp, infop));

#ifdef DEBUG
	if (png_get_bit_depth(pngp, infop) != 8) {
		PNGREADER_DEBUG("PNG bit depth is %d, only 8-bit is supported!", png_get_bit_depth(pngp, infop));
	}
#endif

	png_start_read_image(pngp);
}

/*
* pngreader_row_callback
*/
static void pngreader_row_callback(png_structp pngp, png_bytep new_row, png_uint_32 row_num, int pass)
{
	struct pngreader_info *pi = png_get_progressive_ptr(pngp);

	if (row_num >= pi->height) {
		return;
	}

	u32_t cnt = pi->width;
	u32_t pix;
	void *dst = pi->fb + (row_num * cnt * 2);

	asm volatile (
		"1:                                                     \n\t"
		"       shmrg.1         %[pix], (%[src])1++, %[pix]     \n\t"
		"       shmrg.1         %[pix], (%[src])1++, %[pix]     \n\t"
		"       shmrg.1         %[pix], (%[src])1++, %[pix]     \n\t"
		"       pxcnv           (%[dst])2++, %[pix]             \n\t"
		"       add.4           %[cnt], #-1, %[cnt]             \n\t"
		"       jmpne.t         1b                              \n\t"
		: [cnt] "+d" (cnt),
		[pix] "=d" (pix),
		[src] "+a" (new_row),
		[dst] "+a" (dst)
		:
		: "cc", "memory"
	);

}

/*
 * pngreader_decode
 */
int pngreader_decode(void *fb, const void *png_data, size_t png_size, int width, int height)
{
	struct pngreader_info pi;
	pi.fb = fb;
	pi.width = width;
	pi.height = height;

	/*
	 * Load bitmap
	 */
	png_structp pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!pngp) {
		PNGREADER_DEBUG("Could not create PNG\n");
		return -1;
	}
	png_infop infop = png_create_info_struct(pngp);
	if (!infop) {
		PNGREADER_DEBUG("Could not create PNG info\n");
		png_destroy_read_struct(&pngp, (png_infopp)NULL, (png_infopp)NULL);
		return -1;
	}

	png_set_progressive_read_fn(pngp, &pi, pngreader_info_callback, pngreader_row_callback, NULL);

	png_process_data(pngp, infop, (void *)png_data, png_size);

	png_destroy_read_struct(&pngp, &infop, (png_infopp)NULL);

	return 0;
}


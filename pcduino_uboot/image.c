#include "sunxi-common.h"
#include "types.h"
#include "cmd_nvedit.h"
#include "vsprintf.h"
#include "stddef.h"
#include "image.h"
#include "malloc.h"
#include "crc.h"

ulong load_addr = CONFIG_SYS_LOAD_ADDR;	/* Default Load Address */

/**
 * genimg_get_format - get image format type
 * @img_addr: image start address
 *
 * genimg_get_format() checks whether provided address points to a valid
 * legacy or FIT image.
 *
 * New uImage format and FDT blob are based on a libfdt. FDT blob
 * may be passed directly or embedded in a FIT image. In both situations
 * genimg_get_format() must be able to dectect libfdt header.
 *
 * returns:
 *     image format type or IMAGE_FORMAT_INVALID if no image is present
 */
int genimg_get_format(const void *img_addr)
{
	ulong format = IMAGE_FORMAT_INVALID;
	const image_header_t *hdr;

	hdr = (const image_header_t *)img_addr;
	if (image_check_magic(hdr))
		format = IMAGE_FORMAT_LEGACY;

	return format;
}

/*****************************************************************************/
/* Legacy format routines */
/*****************************************************************************/
int image_check_hcrc(const image_header_t *hdr)
{
	ulong hcrc;
	ulong len = image_get_header_size();
	image_header_t header;

	/* Copy header so we can blank CRC field for re-calculation */
	memmove(&header, (char *)hdr, image_get_header_size());
	image_set_hcrc(&header, 0);

	hcrc = crc32(0, (unsigned char *)&header, len);

	return (hcrc == image_get_hcrc(hdr));
}

int image_check_dcrc(const image_header_t *hdr)
{
	ulong data = image_get_data(hdr);
	ulong len = image_get_data_size(hdr);
	ulong dcrc = crc32_wd(0, (unsigned char *)data, len, CHUNKSZ_CRC32);

	return (dcrc == image_get_dcrc(hdr));
}

void memmove_wd(void *to, void *from, size_t len, ulong chunksz)
{
	if (to == from)
		return;

	memmove(to, from, len);
}

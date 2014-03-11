#ifndef __NS_IMAGE_PIXELS_DYNAMIC_RANGE_H__
#define __NS_IMAGE_PIXELS_DYNAMIC_RANGE_H__

#include <image/nspixels.h>

NS_DECLS_BEGIN

enum
	{
	NS_PIXEL_PROC_DYNAMIC_RANGE_PIXEL_TYPE,
	NS_PIXEL_PROC_DYNAMIC_RANGE_PIXELS,
	NS_PIXEL_PROC_DYNAMIC_RANGE_WIDTH,
	NS_PIXEL_PROC_DYNAMIC_RANGE_HEIGHT,
	NS_PIXEL_PROC_DYNAMIC_RANGE_LENGTH,
	NS_PIXEL_PROC_DYNAMIC_RANGE_ROW_ALIGN,
	NS_PIXEL_PROC_DYNAMIC_RANGE_CHANNEL_MIN,
	NS_PIXEL_PROC_DYNAMIC_RANGE_CHANNEL_MAX,
	NS_PIXEL_PROC_DYNAMIC_RANGE_REGION,
	NS_PIXEL_PROC_DYNAMIC_RANGE_PROGRESS,

	NS_PIXEL_PROC_DYNAMIC_RANGE_NUM_PARAMS
	};


NS_IMPEXP NsProc* ns_pixel_proc_dynamic_range( void );

NS_DECLS_END

#endif/* __NS_IMAGE_PIXELS_DYNAMIC_RANGE_H__ */
#include "libuvc/libuvc.h"
#include "libuvc/libuvc_internal.h"
#include "../../libjpeg-turbo/turbojpeg.h"

extern uvc_error_t uvc_ensure_frame_size(uvc_frame_t *frame, size_t need_bytes);

static uvc_error_t uvc_mjpeg_convert(uvc_frame_t *in, uvc_frame_t *out) {
  int ret;

  tjhandle tjh = tjInitDecompress();

  ret = tjDecompress2(tjh, in->data, in->data_bytes, out->data,
                out->width, 0, out->height, TJPF_RGB, 0);

  tjDestroy(tjh);

  return ret == 0 ? UVC_SUCCESS : UVC_ERROR_OTHER;
}

/** @brief Convert an MJPEG frame to RGB
 * @ingroup frame
 *
 * @param in MJPEG frame
 * @param out RGB frame
 */
uvc_error_t uvc_mjpeg2rgb(uvc_frame_t *in, uvc_frame_t *out) {
  if (in->frame_format != UVC_FRAME_FORMAT_MJPEG)
    return UVC_ERROR_INVALID_PARAM;

  if (uvc_ensure_frame_size(out, in->width * in->height * 3) < 0)
    return UVC_ERROR_NO_MEM;

  out->width = in->width;
  out->height = in->height;
  out->frame_format = UVC_FRAME_FORMAT_RGB;
  out->step = in->width * 3;
  out->sequence = in->sequence;
  out->capture_time = in->capture_time;
  out->capture_time_finished = in->capture_time_finished;
  out->source = in->source;

  return uvc_mjpeg_convert(in, out);
}

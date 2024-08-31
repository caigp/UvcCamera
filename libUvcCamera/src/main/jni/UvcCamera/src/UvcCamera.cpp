//
// Created by android_01 on 2024/8/27.
//

#include "UvcCamera.h"


void handleFrame(uvc_frame_t *pFrame, UvcCamera *uvcCamera) {
    JNIEnv *env = NULL;
    uvcCamera->jvm->AttachCurrentThread(&env, NULL);
    if (env) {
        jbyteArray array = env->NewByteArray(pFrame->data_bytes);
        env->SetByteArrayRegion(array, 0, pFrame->data_bytes, (const jbyte *)pFrame->data);

        jclass cls = env->GetObjectClass(uvcCamera->callback);
        jmethodID mid = env->GetMethodID(cls, "callback", "([BIILcom/xiaocai/app/DataType;)V");

        env->CallVoidMethod(uvcCamera->callback, mid, array, pFrame->width, pFrame->height, uvcCamera->dataType);
    }
    uvcCamera->jvm->DetachCurrentThread();
}

void cb(uvc_frame_t *frame, void *ptr) {
    uvc_frame_t *rgb;
    uvc_error_t ret;
    auto *uvcCamera = (UvcCamera *) ptr;

    rgb = uvc_allocate_frame(frame->width * frame->height * 3);
    if (!rgb) {
        printf("unable to allocate bgr frame!\n");
        return;
    }

    ret = uvc_any2rgb(frame, rgb);
    if (ret) {
        uvc_perror(ret, "uvc_any2bgr");
        uvc_free_frame(rgb);
        return;
    }

    if (uvcCamera->preview) {
        uvcCamera->preview->draw(rgb);
    }

    if (uvcCamera->callback) {
        handleFrame(rgb, uvcCamera);
    }

    uvc_free_frame(rgb);
}

UvcCamera::UvcCamera() {

}

UvcCamera::~UvcCamera() {

}

void UvcCamera::open(int fd) {
    res = uvc_init(&ctx, NULL);
    if (res < 0) {
        LOGD("uvc init error, error=%d", res);
        return;
    }
    LOGD("uvc initialized");

    libusb_device_handle *usbdevh = NULL;
    libusb_wrap_sys_device(ctx->usb_ctx, fd, &usbdevh);

    dev = (uvc_device_t *) malloc(sizeof(*dev));
    dev->ctx = ctx;
    dev->ref = 0;
    dev->usb_dev = libusb_get_device(usbdevh);

    uvc_ref_device(dev);

    res = uvc_open(dev, &devh);
    if (res < 0) {
        LOGD("unable to open device"); /* unable to open device */
        return;
    }
    LOGD("uvc open success");

//    uvc_print_diag(devh, stderr);
}

void UvcCamera::close() {
    LOGD("close");
    if (devh) {
        uvc_close(devh);
        devh = NULL;
    }

    if (dev) {
        uvc_unref_device(dev);
        dev = NULL;
    }

    if (preview) {
        delete preview;
        preview = NULL;
    }

    if (window) {
        ANativeWindow_release(window);
        window = NULL;
    }

    if (ctx) {
        uvc_exit(ctx);
        ctx = NULL;
    }
    LOGD("uvc exit");
}

void UvcCamera::startPreview() {
    uvc_stream_ctrl_t ctrl;

    enum uvc_frame_format frame_format = UVC_FRAME_FORMAT_MJPEG;

    res = uvc_get_stream_ctrl_format_size(
            devh, &ctrl, /* result stored in ctrl */
            frame_format,
            width, height, interval /* width, height, fps */
    );
//    uvc_print_stream_ctrl(&ctrl, stderr);

    if (res < 0) {
        LOGD("device doesn't provide a matching stream");
//        return;
    }

    res = uvc_start_streaming(devh, &ctrl, cb, this, 0);
    if (res < 0) {
        LOGD("unable to start stream"); /* unable to start stream */
        return;
    }
    LOGD("Streaming...");
}

void UvcCamera::stopPreview() {
    LOGD("stop streaming");
    uvc_stop_streaming(devh);
}

void UvcCamera::setPreview(ANativeWindow *window) {
    this->window = window;
    if (window) {
        preview = new Preview(window);
        return;
    }
    if (preview) {
        delete preview;
        preview = NULL;
    }
}

void UvcCamera::setDataCallback(jobject callback) {
    this->callback = callback;
}

std::vector<UvcInfo> UvcCamera::fetchSupportSize() {
    std::vector<UvcInfo> supportSize;

    if (devh->info->ctrl_if.bcdUVC) {
        uvc_streaming_interface_t *stream_if;


        DL_FOREACH(devh->info->stream_ifs, stream_if) {
            uvc_format_desc_t *fmt_desc;

            DL_FOREACH(stream_if->format_descs, fmt_desc) {
                uvc_frame_desc_t *frame_desc;

                switch (fmt_desc->bDescriptorSubtype) {
//                    case UVC_VS_FORMAT_UNCOMPRESSED:
                    case UVC_VS_FORMAT_MJPEG:
//                    case UVC_VS_FORMAT_FRAME_BASED:
//                        fprintf(stream,
//                                "\t\%s(%d)\n"
//                                "\t\t  bits per pixel: %d\n"
//                                "\t\t  GUID: ",
//                                _uvc_name_for_format_subtype(fmt_desc->bDescriptorSubtype),
//                                fmt_desc->bFormatIndex,
//                                fmt_desc->bBitsPerPixel);
//
//                        for (i = 0; i < 16; ++i)
//                            fprintf(stream, "%02x", fmt_desc->guidFormat[i]);

//                        fprintf(stream, " (%4s)\n", fmt_desc->fourccFormat );

//                        fprintf(stream,
//                                "\t\t  default frame: %d\n"
//                                "\t\t  aspect ratio: %dx%d\n"
//                                "\t\t  interlace flags: %02x\n"
//                                "\t\t  copy protect: %02x\n",
//                                fmt_desc->bDefaultFrameIndex,
//                                fmt_desc->bAspectRatioX,
//                                fmt_desc->bAspectRatioY,
//                                fmt_desc->bmInterlaceFlags,
//                                fmt_desc->bCopyProtect);

                        DL_FOREACH(fmt_desc->frame_descs, frame_desc) {
                            uint32_t *interval_ptr;

                            struct UvcInfo uvcInfo;
                            uvcInfo.width = frame_desc->wWidth;
                            uvcInfo.height = frame_desc->wHeight;
                            uvcInfo.interval = 10000000 / frame_desc->dwDefaultFrameInterval;

//                            fprintf(stream,
//                                    "\t\t\tFrameDescriptor(%d)\n"
//                                    "\t\t\t  capabilities: %02x\n"
//                                    "\t\t\t  size: %dx%d\n"
//                                    "\t\t\t  bit rate: %d-%d\n"
//                                    "\t\t\t  max frame size: %d\n"
//                                    "\t\t\t  default interval: 1/%d\n",
//                                    frame_desc->bFrameIndex,
//                                    frame_desc->bmCapabilities,
//                                    frame_desc->wWidth,
//                                    frame_desc->wHeight,
//                                    frame_desc->dwMinBitRate,
//                                    frame_desc->dwMaxBitRate,
//                                    frame_desc->dwMaxVideoFrameBufferSize,
//                                    10000000 / frame_desc->dwDefaultFrameInterval);
                            if (frame_desc->intervals) {

                                uvcInfo.intervals += "[";

                                bool first = true;
                                for (interval_ptr = frame_desc->intervals;
                                     *interval_ptr;
                                     ++interval_ptr) {
//                                    fprintf(stream,
//                                            "\t\t\t  interval[%d]: 1/%d\n",
//                                            (int) (interval_ptr - frame_desc->intervals),
//                                            10000000 / *interval_ptr);
                                    if (!first) {
                                        uvcInfo.intervals += ", ";
                                    }

                                    char buffer[50];
                                    sprintf(buffer, "%d", 10000000 / *interval_ptr);
                                    uvcInfo.intervals += std::string(buffer);

                                    first = false;
                                }

                                uvcInfo.intervals += "]";
                            } else {
//                                fprintf(stream,
//                                        "\t\t\t  min interval[%d] = 1/%d\n"
//                                        "\t\t\t  max interval[%d] = 1/%d\n",
//                                        frame_desc->dwMinFrameInterval,
//                                        10000000 / frame_desc->dwMinFrameInterval,
//                                        frame_desc->dwMaxFrameInterval,
//                                        10000000 / frame_desc->dwMaxFrameInterval);
//                                if (frame_desc->dwFrameIntervalStep)
//                                    fprintf(stream,
//                                            "\t\t\t  interval step[%d] = 1/%d\n",
//                                            frame_desc->dwFrameIntervalStep,
//                                            10000000 / frame_desc->dwFrameIntervalStep);
                            }

                            supportSize.push_back(uvcInfo);
                        }

                        break;
                    default:
                        break;
                }
            }
        }

    }

    return supportSize;
}

void UvcCamera::setSize(int width, int height, int interval) {
    this->width = width;
    this->height = height;
    this->interval = interval;
}


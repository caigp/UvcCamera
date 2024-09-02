package com.xiaocai.app

import android.content.Context
import android.content.Context.USB_SERVICE
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbDeviceConnection
import android.hardware.usb.UsbManager
import android.view.Surface

class UvcCamera(var context: Context) {

    private var nativePtr: Long = 0

    private var usbDeviceConnection: UsbDeviceConnection? = null

    var usbDevice: UsbDevice? = null

    init {
        System.loadLibrary("uvc-camera")
    }

    fun open(usbDevice: UsbDevice) {
        this.usbDevice = usbDevice

        val usbManager = context.getSystemService(USB_SERVICE) as UsbManager
        usbDeviceConnection = usbManager.openDevice(usbDevice)
        val fileDescriptor = usbDeviceConnection!!.fileDescriptor

        open(fileDescriptor)
    }

    fun close() {
        _close()

        usbDeviceConnection?.close()
        usbDeviceConnection = null

        usbDevice = null
    }

    private external fun open(fd: Int)
    private external fun _close()
    external fun startPreview()
    external fun stopPreview()
    external fun setPreview(surface: Surface)
    external fun setDataCallback(callback: UvcDataCallback)
    external fun setSize(width: Int, height: Int, interval: Int)
    external fun getSupportSize(): List<Size>
    external fun setPreviewMirror(mirror: Boolean)
}
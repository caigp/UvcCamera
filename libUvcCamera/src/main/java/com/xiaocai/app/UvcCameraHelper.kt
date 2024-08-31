package com.xiaocai.app

import android.hardware.usb.UsbDevice

object UvcCameraHelper {

    fun isUvcDevice(usbDevice: UsbDevice): Boolean {
        if ( 0x199e == usbDevice.vendorId && usbDevice.productId  >= 0x8201 &&
            usbDevice.productId <= 0x8208 ) {
            return false
        }

        var has = false
        for (i in 0 until usbDevice.configurationCount) {
            val usbConfiguration = usbDevice.getConfiguration(i)
            for (j in 0 until usbConfiguration.interfaceCount) {
                val usbInterface = usbConfiguration.getInterface(j)

                if (usbInterface.interfaceClass == 14 && usbInterface.interfaceSubclass == 2) {
                    return true
                }

                if (usbInterface.interfaceClass == 255 && usbInterface.interfaceSubclass == 2) {
                    has = true
                }
            }
        }

        if (0x199e == usbDevice.vendorId && (0x8101 == usbDevice.productId ||
                    0x8102 == usbDevice.productId ) && has) {
            return true
        }

        return false
    }
}
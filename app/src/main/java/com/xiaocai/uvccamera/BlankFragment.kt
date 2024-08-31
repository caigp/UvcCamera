package com.xiaocai.uvccamera

import android.Manifest
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.os.Build
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import com.xiaocai.app.DataType
import com.xiaocai.app.UvcCamera
import com.xiaocai.app.UvcCameraHelper
import com.xiaocai.app.UvcDataCallback

// TODO: Rename parameter arguments, choose names that match
// the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
private const val ARG_PARAM1 = "vid"
private const val ARG_PARAM2 = "pid"

/**
 * A simple [Fragment] subclass.
 * Use the [BlankFragment.newInstance] factory method to
 * create an instance of this fragment.
 */
class BlankFragment : Fragment() {
    // TODO: Rename and change types of parameters
    private var param1: Int? = null
    private var param2: Int? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        arguments?.let {
            param1 = it.getInt(ARG_PARAM1)
            param2 = it.getInt(ARG_PARAM2)
        }
    }

    private val TAG = "BlankFragment"
    private val ACTION_PERMISSION_USB = "action.android.permission.usb"

    private lateinit var surfaceView: SurfaceView

    var uvcCamera: UvcCamera? = null

    private val broadcast = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            val action = intent?.action
            if (ACTION_PERMISSION_USB == action) {
                val usbDevice = intent.getParcelableExtra<UsbDevice>(UsbManager.EXTRA_DEVICE)
                val ret = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)
                if (ret) {
                    openUvc(usbDevice!!)
                }
            } else if (UsbManager.ACTION_USB_DEVICE_ATTACHED == action) {
                val usbDevice = intent.getParcelableExtra<UsbDevice>(UsbManager.EXTRA_DEVICE)
                preOpen(usbDevice!!)
            } else if (UsbManager.ACTION_USB_DEVICE_DETACHED == action) {
                val usbDevice = intent.getParcelableExtra<UsbDevice>(UsbManager.EXTRA_DEVICE)
                if (usbDevice!! == uvcCamera?.usbDevice) {
                    uvcCamera?.close()
                    uvcCamera = null
                }
            }
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)

        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            findUvc()
        }
    }

    private fun findUvc() {
        val usbManager = requireContext().getSystemService(AppCompatActivity.USB_SERVICE) as UsbManager
        val deviceList: HashMap<String, UsbDevice> = usbManager.deviceList
        for (usbDevice in deviceList.values) {
            if (!UvcCameraHelper.isUvcDevice(usbDevice)) {
                continue
            }
//            if (usbDevice.vendorId != param1 && usbDevice.productId != param2) {
//                continue
//            }

            preOpen(usbDevice)
            break
        }
    }

    fun preOpen(usbDevice: UsbDevice) {

        val usbManager = requireContext().getSystemService(AppCompatActivity.USB_SERVICE) as UsbManager
        if (usbManager.hasPermission(usbDevice)) {
            openUvc(usbDevice)
        } else {

            val pi = PendingIntent.getBroadcast(
                activity,
                0,
                Intent(ACTION_PERMISSION_USB),
                PendingIntent.FLAG_UPDATE_CURRENT
            )
            usbManager.requestPermission(usbDevice, pi)
        }
    }

    fun openUvc(usbDevice: UsbDevice) {
        if (uvcCamera != null) return

        uvcCamera = UvcCamera(requireContext())
        uvcCamera?.open(usbDevice)
        uvcCamera?.setDataCallback(object : UvcDataCallback {
            override fun callback(
                data: ByteArray,
                width: Int,
                height: Int,
                dataType: DataType?
            ) {

            }
        })
        val supportSize = uvcCamera?.getSupportSize()
        supportSize?.first().let {
            uvcCamera?.setSize(it!!.width, it.height, it.interval)
        }
        uvcCamera?.setPreview(surfaceView.holder.surface)
        uvcCamera?.startPreview()
    }

    override fun onDestroy() {
        super.onDestroy()


    }

    override fun onDestroyView() {
        super.onDestroyView()
        context?.unregisterReceiver(broadcast)

    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_blank, container, false).apply {

            val intentFilter = IntentFilter()
            intentFilter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED)
            intentFilter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED)
            intentFilter.addAction(ACTION_PERMISSION_USB)
            ContextCompat.registerReceiver(context, broadcast, intentFilter, ContextCompat.RECEIVER_EXPORTED)

            surfaceView = findViewById(R.id.surface)
            surfaceView.holder.addCallback(object : SurfaceHolder.Callback {
                override fun surfaceCreated(holder: SurfaceHolder) {

                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                        if (context.checkSelfPermission(Manifest.permission.CAMERA) == PackageManager.PERMISSION_DENIED) {
                            requestPermissions(arrayOf(Manifest.permission.CAMERA), 0)
                            return
                        }
                    }
                    findUvc()
                }

                override fun surfaceChanged(
                    holder: SurfaceHolder,
                    format: Int,
                    width: Int,
                    height: Int
                ) {

                }

                override fun surfaceDestroyed(holder: SurfaceHolder) {
                    uvcCamera?.close()
                    uvcCamera = null
                }
            })
        }
    }

    companion object {
        /**
         * Use this factory method to create a new instance of
         * this fragment using the provided parameters.
         *
         * @param param1 Parameter 1.
         * @param param2 Parameter 2.
         * @return A new instance of fragment BlankFragment.
         */
        // TODO: Rename and change types and number of parameters
        @JvmStatic
        fun newInstance(param1: Int, param2: Int) =
            BlankFragment().apply {
                arguments = Bundle().apply {
                    putInt(ARG_PARAM1, param1)
                    putInt(ARG_PARAM2, param2)
                }
            }
    }
}
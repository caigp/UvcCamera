package com.xiaocai.app

interface UvcDataCallback {

    fun callback(data: ByteArray, width: Int, height: Int, dataType: DataType?)
}
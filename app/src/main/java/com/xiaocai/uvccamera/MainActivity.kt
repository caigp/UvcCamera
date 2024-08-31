package com.xiaocai.uvccamera

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        supportFragmentManager
            .beginTransaction()
            .replace(R.id.frag, BlankFragment.newInstance(11391, 19040))
//            .replace(R.id.frag1, BlankFragment.newInstance(3141, 25376))
            .commit()
    }
}
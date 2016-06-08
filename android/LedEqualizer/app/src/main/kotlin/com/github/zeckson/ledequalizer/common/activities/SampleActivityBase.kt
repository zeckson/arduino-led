package com.github.zeckson.ledequalizer.common.activities

import android.os.Bundle
import android.support.v4.app.FragmentActivity

import com.github.zeckson.ledequalizer.common.logger.Log
import com.github.zeckson.ledequalizer.common.logger.LogWrapper

open class SampleActivityBase : FragmentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    override fun onStart() {
        super.onStart()
        initializeLogging()
    }

    /** Set up targets to receive log data  */
    open fun initializeLogging() {
        val logWrapper = LogWrapper()
        Log.logNode = logWrapper

        Log.i(TAG, "Ready")
    }

    companion object {

        val TAG = "SampleActivityBase"
    }
}

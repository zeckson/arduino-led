package com.github.zeckson.ledequalizer.common.activities;

import android.os.Bundle;
import android.support.v4.app.FragmentActivity;

import com.github.zeckson.ledequalizer.common.logger.Log;
import com.github.zeckson.ledequalizer.common.logger.LogWrapper;

public class SampleActivityBase extends FragmentActivity {

    public static final String TAG = "SampleActivityBase";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected  void onStart() {
        super.onStart();
        initializeLogging();
    }

    /** Set up targets to receive log data */
    public void initializeLogging() {
        LogWrapper logWrapper = new LogWrapper();
        Log.setLogNode(logWrapper);

        Log.i(TAG, "Ready");
    }
}

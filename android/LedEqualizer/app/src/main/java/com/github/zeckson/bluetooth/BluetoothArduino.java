package com.github.zeckson.bluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import com.github.zeckson.ledequalizer.common.logger.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class BluetoothArduino extends Thread {
    private static BluetoothArduino __blue = null;
    OutputStream mOut;
    InputStream mIn;
    private BluetoothAdapter mBlueAdapter = null;
    private BluetoothSocket mBlueSocket = null;
    private BluetoothDevice mBlueRobo = null;
    private boolean robotFound = false;
    private volatile boolean connected = false;
    private int REQUEST_BLUE_ATIVAR = 10;
    private String robotName;
    private List<String> mMessages = new ArrayList<String>();
    private String TAG = "BluetoothConnector";
    private char DELIMITER = '#';

    private BluetoothArduino(String Name) {
        __blue = this;
        try {
            for (int i = 0; i < 2048; i++) {
                mMessages.add("");
            }
            robotName = Name;
            mBlueAdapter = BluetoothAdapter.getDefaultAdapter();
            if (mBlueAdapter == null) {
                LogError("\t\t[#]Phone does not support bluetooth!!");
                return;
            }
            if (!isBluetoothEnabled()) {
                LogError("[#]Bluetooth is not activated!!");
            }

            Set<BluetoothDevice> paired = mBlueAdapter.getBondedDevices();
            if (paired.size() > 0) {
                for (BluetoothDevice d : paired) {
                    if (d.getName().equals(robotName)) {
                        mBlueRobo = d;
                        robotFound = true;
                        break;
                    }
                }
            }

            if (!robotFound)
                LogError("\t\t[#]There is not robot paired!!");

        } catch (Exception e) {
            LogError("\t\t[#]Error creating Bluetooth! : " + e.getMessage());
        }

    }

    BluetoothArduino() {
        this("Arduino-Robot");
    }

    public static BluetoothArduino getInstance(String n) {
        return __blue == null ? new BluetoothArduino(n) : __blue;
    }

    public static BluetoothArduino getInstance() {
        return __blue == null ? new BluetoothArduino() : __blue;
    }

    public boolean isBluetoothEnabled() {
        return mBlueAdapter.isEnabled();
    }

    public boolean connect(String address) {
        BluetoothDevice device = mBlueAdapter.getRemoteDevice(address);
        return connect(device);
    }

    private boolean connect(final BluetoothDevice device) {
        try {
            LogMessage("\t\tConnecting to the device...");

            UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
            mBlueSocket = device.createRfcommSocketToServiceRecord(uuid);

            try {
                mBlueSocket.connect();
            } catch (IOException e) {
                //Fallback of BT issue
                //See http://stackoverflow.com/questions/18657427/ioexception-read-failed-socket-might-closed-bluetooth-on-android-4-3
                Log.INSTANCE.e(TAG, e.getMessage());
                Log.INSTANCE.e(TAG, "trying fallback...");

                mBlueSocket = (BluetoothSocket) device.getClass().getMethod("createRfcommSocket", new Class[]{int.class}).invoke(device, 1);
                mBlueSocket.connect();

                Log.INSTANCE.i(TAG, "Connected");
            }
            mOut = mBlueSocket.getOutputStream();
            mIn = mBlueSocket.getInputStream();
            connected = true;
            if (!(this.isAlive())) {
                this.start();
            }
            LogMessage("\t\t\t" + mBlueAdapter.getName());
            LogMessage("\t\tOk!!");
            return true;

        } catch (Exception e) {
            LogError("\t\t[#]Error while conecting: " + e.getMessage());
            return false;
        }
    }

    public boolean connect() {
        return robotFound && connect(mBlueRobo);
    }

    public boolean disconnect() {
        if (!connected) return false;
        try {
            mBlueSocket.close();
            connected = false;
            return true;
        } catch (IOException e) {
            LogError("Failed to disconnect " + e.getMessage());
            e.printStackTrace();
            return false;
        }
    }

    public void run() {

        while (true) {
            if (connected) {
                try {
                    byte ch, buffer[] = new byte[1024];
                    int i = 0;

                    String s = "";
                    while ((ch = (byte) mIn.read()) != DELIMITER) {
                        buffer[i++] = ch;
                    }
                    buffer[i] = '\0';

                    final String msg = new String(buffer);

                    MessageReceived(msg.trim());
                    LogMessage("[Blue]:" + msg);

                } catch (IOException e) {
                    LogError("->[#]Failed to receive message: " + e.getMessage());
                }
            }
        }
    }

    private void MessageReceived(String msg) {
        try {

            mMessages.add(msg);
            try {
                this.notify();
            } catch (IllegalMonitorStateException e) {
                //
            }
        } catch (Exception e) {
            LogError("->[#] Failed to receive message: " + e.getMessage());
        }
    }

    public boolean hasMensagem(int i) {
        try {
            String s = mMessages.get(i);
            if (s.length() > 0)
                return true;
            else
                return false;
        } catch (Exception e) {
            return false;
        }
    }

    public String getMenssage(int i) {
        return mMessages.get(i);
    }

    public void clearMessages() {
        mMessages.clear();
    }

    public int countMessages() {
        return mMessages.size();
    }

    public String getLastMessage() {
        if (countMessages() == 0)
            return "";
        return mMessages.get(countMessages() - 1);
    }

    public void sendMessage(String msg) {
        try {
            if (connected) {
                mOut.write(msg.getBytes());
            }

        } catch (IOException e) {
            LogError("->[#]Error while sending message: " + e.getMessage());
        }
    }

    private void LogMessage(String msg) {
        com.github.zeckson.ledequalizer.common.logger.Log.INSTANCE.d(TAG, msg);
    }

    private void LogError(String msg) {
        com.github.zeckson.ledequalizer.common.logger.Log.INSTANCE.e(TAG, msg);
    }

    public char getDelimiter() {
        return DELIMITER;
    }

    public void setDelimiter(char d) {
        DELIMITER = d;
    }

}

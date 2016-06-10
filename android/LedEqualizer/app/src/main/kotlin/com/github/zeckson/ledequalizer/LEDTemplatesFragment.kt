package com.github.zeckson.ledequalizer

import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Message
import android.support.v4.app.Fragment
import android.view.*
import android.view.inputmethod.EditorInfo
import android.widget.*
import com.github.zeckson.activity.DeviceListActivity
import com.github.zeckson.ledequalizer.common.logger.Log

private val TEMPLATES = arrayOf("black", "red", "green", "blue", "theaterWhite", "theaterRed", "theaterBlue",
        "rainbow", "rainbowCycle", "theaterChaseRainbow", "pulseWhite", "rainbowFade2White",
        "whiteOverRainbow", "fullWhite")

/**
 * This fragment controls Bluetooth to communicate with other devices.
 */
class LEDTemplatesFragment : Fragment() {

    // Layout Views
    private var mTemplatesListView: ListView? = null
    private var mOutEditText: EditText? = null
    private var mCurrentDevice: TextView? = null
    private var mSendButton: Button? = null

    /**
     * Name of the connected device
     */
    private var mConnectedDeviceName: String? = null


    /**
     * String buffer for outgoing messages
     */
    private var mOutStringBuffer: StringBuffer? = null

    /**
     * Local Bluetooth adapter
     */
    private var mBluetoothAdapter: BluetoothAdapter? = null

    /**
     * Member object for the chat services
     */
    private var mService: BluetoothService? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setHasOptionsMenu(true)
        // Get local Bluetooth adapter
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        // If the adapter is null, then Bluetooth is not supported
        if (mBluetoothAdapter == null) {
            val activity = activity
            Toast.makeText(activity, "Bluetooth is not available", Toast.LENGTH_LONG).show()
            activity.finish()
        }
    }


    override fun onStart() {
        super.onStart()
        //In case we are running on emulator
        if (mBluetoothAdapter == null) {
            Toast.makeText(activity, "Bluetooth adapter is not found or forbidden", Toast.LENGTH_SHORT).show()
            setupBluetoothService()
        } else if (!mBluetoothAdapter!!.isEnabled) {
            // If BT is not on, request that it be enabled.
            // setupChat() will then be called during onActivityResult
            val enableIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT)
        } else if (mService == null) {
            // Otherwise, setup the chat session
            setupBluetoothService()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        if (mService != null) {
            mService!!.stop()
        }
    }

    override fun onResume() {
        super.onResume()
    }

    override fun onCreateView(inflater: LayoutInflater?, container: ViewGroup?,
                              savedInstanceState: Bundle?): View? {
        return inflater!!.inflate(R.layout.fragment_led_templates, container, false)
    }

    override fun onViewCreated(view: View?, savedInstanceState: Bundle?) {
        mTemplatesListView = view!!.findViewById(R.id.`templates`) as ListView
        mOutEditText = view.findViewById(R.id.edit_text_out) as EditText
        mSendButton = view.findViewById(R.id.button_send) as Button
        mCurrentDevice = view.findViewById(R.id.selected_device) as TextView
    }

    /**
     * Set up the UI and background operations for chat.
     */
    private fun setupBluetoothService() {
        Log.d(TAG, "setupBTService()")

        // Initialize the array adapter for the conversation thread
        val templateAdapter = ArrayAdapter<String>(activity, R.layout.template_name, TEMPLATES)

        mTemplatesListView!!.adapter = templateAdapter

        mTemplatesListView!!.onItemClickListener = object : AdapterView.OnItemClickListener {
            override fun onItemClick(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                val template = TEMPLATES[position]
                sendMessage(template + "#")
            }
        }

        // Initialize the compose field with a listener for the return key
        mOutEditText!!.setOnEditorActionListener(mWriteListener)

        // Initialize the send button with a listener that for click events
        mSendButton!!.setOnClickListener {
            // Send a message using content of the edit text widget
            val view = view
            if (null != view) {
                val textView = view.findViewById(R.id.edit_text_out) as TextView
                val message = textView.text.toString()
                sendMessage(message)
            }
        }

        // Initialize the BluetoothService to perform bluetooth connections
        mService = BluetoothService(activity, mHandler)

        // Initialize the buffer for outgoing messages
        mOutStringBuffer = StringBuffer("")
    }

    /**
     * Makes this device discoverable.
     */
    private fun ensureDiscoverable() {
        if (mBluetoothAdapter!!.scanMode != BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            val discoverableIntent = Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE)
            discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300)
            startActivity(discoverableIntent)
        }
    }

    /**
     * Sends a message.

     * @param message A string of text to send.
     */
    private fun sendMessage(message: String) {
        // Check that we're actually connected before trying anything

        // Check that there's actually something to send
        if (message.length > 0) {
            // Get the message bytes and tell the BluetoothService to write
            mService!!.sendMessageToCurrentDevice(message)

            // Reset out string buffer to zero and clear the edit text field
            mOutStringBuffer!!.setLength(0)
            mOutEditText!!.setText(mOutStringBuffer)
        }
    }

    /**
     * The action listener for the EditText widget, to listen for the return key
     */
    private val mWriteListener = TextView.OnEditorActionListener { view, actionId, event ->
        // If the action is a key-up event on the return key, send the message
        if (actionId == EditorInfo.IME_NULL && event.action == KeyEvent.ACTION_UP) {
            val message = view.text.toString()
            sendMessage(message)
        }
        true
    }

    /**
     * The Handler that gets information back from the BluetoothService
     */
    private val mHandler = object : Handler() {
        override fun handleMessage(msg: Message) {
            val activity = activity
            when (msg.what) {
                Constants.DEVICE_SELECTED -> {
                    // save the connected device's name
                    mConnectedDeviceName = msg.data.getString(Constants.DEVICE_NAME)
                    if (null != activity) {
                        val message = "Connected to " + mConnectedDeviceName!!
                        Toast.makeText(activity, message, Toast.LENGTH_SHORT).show()
                        mCurrentDevice!!.text = message;
                    }
                }
                Constants.MESSAGE_TOAST -> if (null != activity) {
                    Toast.makeText(activity, msg.data.getString(Constants.TOAST),
                            Toast.LENGTH_SHORT).show()
                }
            }
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        if (data == null) return
        when (requestCode) {
            REQUEST_CONNECT_DEVICE_SECURE ->
                // When DeviceListActivity returns with a device to connect
                if (resultCode == Activity.RESULT_OK) {
                    connectDevice(data, true)
                }
            REQUEST_CONNECT_DEVICE_INSECURE ->
                // When DeviceListActivity returns with a device to connect
                if (resultCode == Activity.RESULT_OK) {
                    connectDevice(data, false)
                }
            REQUEST_ENABLE_BT ->
                // When the request to enable Bluetooth returns
                if (resultCode == Activity.RESULT_OK) {
                    // Bluetooth is now enabled, so set up a chat session
                    setupBluetoothService()
                } else {
                    // User did not enable Bluetooth or an error occurred
                    Log.d(TAG, "BT not enabled")
                    Toast.makeText(activity, R.string.bt_not_enabled_leaving,
                            Toast.LENGTH_SHORT).show()
                    activity.finish()
                }
        }
    }

    /**
     * Establish connection with other divice

     * @param data   An [Intent] with [DeviceListActivity.EXTRA_DEVICE_ADDRESS] extra.
     * *
     * @param secure Socket Security type - Secure (true) , Insecure (false)
     */
    private fun connectDevice(data: Intent, secure: Boolean) {
        // Get the device MAC address
        val address = data.extras.getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS)
        // Get the BluetoothDevice object
        Log.i(TAG, "Connecting to device by $address")

        mService!!.selectDevice(mBluetoothAdapter!!.getRemoteDevice(address));

    }

    override fun onCreateOptionsMenu(menu: Menu?, inflater: MenuInflater?) {
        inflater!!.inflate(R.menu.bluetooth_chat, menu)
    }

    override fun onOptionsItemSelected(item: MenuItem?): Boolean {
        when (item!!.itemId) {
            R.id.secure_connect_scan -> {
                // Launch the DeviceListActivity to see devices and do scan
                val serverIntent = Intent(activity, DeviceListActivity::class.java)
                startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE_SECURE)
                return true
            }
            R.id.insecure_connect_scan -> {
                // Launch the DeviceListActivity to see devices and do scan
                val serverIntent = Intent(activity, DeviceListActivity::class.java)
                startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE_INSECURE)
                return true
            }
            R.id.discoverable -> {
                // Ensure this device is discoverable by others
                ensureDiscoverable()
                return true
            }
        }
        return false
    }

    companion object {

        private val TAG = "LEDTemplatesFragment"

        // Intent request codes
        private val REQUEST_CONNECT_DEVICE_SECURE = 1
        private val REQUEST_CONNECT_DEVICE_INSECURE = 2
        private val REQUEST_ENABLE_BT = 3
    }

}

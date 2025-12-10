package com.example.myapplication

import android.annotation.SuppressLint
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattDescriptor
import android.bluetooth.BluetoothProfile
import android.content.Context
import android.util.Log
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import java.util.UUID
import javax.inject.Inject
import javax.inject.Singleton
import dagger.hilt.android.qualifiers.ApplicationContext
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import androidx.lifecycle.viewModelScope


// Glove Events (from firmware)
enum class GloveEvent(val value: Int) {
    UNKNOWN(0x00),
    STARTED_ACTIVITY(0x01),
    REP_COMPLETED(0x02),
    ACTIVITY_COMPLETED(0x03),
    ACTIVITY_PAUSED(0x04),
    ACTIVITY_RESUMED(0x05),
    ACTIVITY_CANCELED(0x06),
    ERROR(0x70);

    companion object {
        fun fromByte(byte: Byte): GloveEvent {
            return entries.find { it.value == byte.toInt() } ?: UNKNOWN
        }
    }
}

data class GloveState(
    val isConnected: Boolean = false,
    val isExercising: Boolean = false,
    val isPaused: Boolean = false,
    val currentReps: Int = 0,
    val lastEvent: GloveEvent = GloveEvent.UNKNOWN
)

@HiltViewModel
class GloveManager @Inject constructor(
    @ApplicationContext private val context: Context
) : ViewModel() {
    private val bluetoothAdapter: android.bluetooth.BluetoothAdapter? =
        (context.getSystemService(Context.BLUETOOTH_SERVICE) as? android.bluetooth.BluetoothManager)?.adapter
    companion object {
        private const val TAG = "GloveManager"
        private val SERVICE_UUID = UUID.fromString("c13f3ca3-55f5-4ffa-857a-1c3f40ccb300")
        private val AUTO_START_UUID = UUID.fromString("3861a947-7b94-495b-ae3d-c2d669d9f168")

        private val AUTO_PAUSE_UUID = UUID.fromString("f9b94111-c76a-42a3-92a8-8609cfd2c28f")

        private val AUTO_STOP_UUID = UUID.fromString("ca11594c-0a4d-40dc-994c-b64e2b556e74")
        private val AUTO_STATE_UUID = UUID.fromString("c045a031-c506-4756-8bdd-63d55ef3eced")
        private val CCCD_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")
    }

    private var bluetoothGatt: BluetoothGatt? = null

    private val _gloveState = MutableStateFlow(GloveState())
    val gloveState: StateFlow<GloveState> = _gloveState.asStateFlow()

    @SuppressLint("MissingPermission")
    fun connectToDevice(device: ScannedDevice) {
        Log.d(TAG, "Attempting to connect to ${device.displayName}")

//        bluetoothGatt = device.connectGatt(
//            context,
//            false,
//            gattCallback
//        )
        val adapter = bluetoothAdapter ?: run {
            Log.e(TAG, "Bluetooth adapter is null")
            return
        }
        val bluetoothDevice = bluetoothAdapter.getRemoteDevice(device.device.address)
        bluetoothGatt = bluetoothDevice.connectGatt(
            context,
            false,
            gattCallback
        )
    }

    @SuppressLint("MissingPermission")
    fun disconnect() {
        bluetoothGatt?.disconnect()
        bluetoothGatt?.close()
        bluetoothGatt = null
        _gloveState.value = GloveState()
    }

    @SuppressLint("MissingPermission")
    fun startExercise(exerciseIndex: Int) {
        val gatt = bluetoothGatt ?: run {
            Log.e(TAG, "Cannot start: not connected")
            return
        }

        val service = gatt.getService(SERVICE_UUID) ?: run {
            Log.e(TAG, "Service not found")
            return
        }

        val byteToSend = byteArrayOf(exerciseIndex.toByte())
        val characteristic = service.getCharacteristic(AUTO_START_UUID) ?: run {
            Log.e(TAG, "Start characteristic not found")
            return
        }

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
            gatt.writeCharacteristic(
                characteristic,
                byteArrayOf(0x01),
                BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
            )
        } else {
            @Suppress("DEPRECATION")
            characteristic.value = byteToSend
            @Suppress("DEPRECATION")
            gatt.writeCharacteristic(characteristic)
        }

        Log.d(TAG, "Start command sent")
    }

    @SuppressLint("MissingPermission")
    fun pauseExercise(exerciseIndex: Int) {
        val gatt = bluetoothGatt ?: run {
            Log.e(TAG, "Cannot pause: not connected")
            return
        }

        val service = gatt.getService(SERVICE_UUID) ?: run {
            Log.e(TAG, "Service not found")
            return
        }

        val characteristic = service.getCharacteristic(AUTO_PAUSE_UUID) ?: run {
            Log.e(TAG, "Pause characteristic not found")
            return
        }
        val byteToSend = byteArrayOf(exerciseIndex.toByte())
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
            gatt.writeCharacteristic(
                characteristic,
                byteArrayOf(0x01),
                BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
            )
        } else {
            @Suppress("DEPRECATION")
            characteristic.value = byteToSend
            @Suppress("DEPRECATION")
            gatt.writeCharacteristic(characteristic)
        }

        Log.d(TAG, "Pause command sent")
    }

    @SuppressLint("MissingPermission")
    fun stopExercise() {
        val gatt = bluetoothGatt ?: run {
            Log.e(TAG, "Cannot stop: not connected")
            return
        }

        val service = gatt.getService(SERVICE_UUID) ?: run {
            Log.e(TAG, "Service not found")
            return
        }

        val characteristic = service.getCharacteristic(AUTO_STOP_UUID) ?: run {
            Log.e(TAG, "Stop characteristic not found")
            return
        }

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
            gatt.writeCharacteristic(
                characteristic,
                byteArrayOf(0x01),
                BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
            )
        } else {
            @Suppress("DEPRECATION")
            characteristic.value = byteArrayOf(0x01)
            @Suppress("DEPRECATION")
            gatt.writeCharacteristic(characteristic)
        }

        Log.d(TAG, "Stop command sent")
    }

    private val gattCallback = object : BluetoothGattCallback() {
        @SuppressLint("MissingPermission")
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            when (newState) {
                BluetoothProfile.STATE_CONNECTED -> {
                    Log.d(TAG, "Connected to GATT server")
                    _gloveState.value = _gloveState.value.copy(isConnected = true)
                    gatt.discoverServices()
                }
                BluetoothProfile.STATE_DISCONNECTED -> {
                    Log.d(TAG, "Disconnected from GATT server")
                    _gloveState.value = GloveState()
                }
            }
        }

        @SuppressLint("MissingPermission")
        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d(TAG, "Services discovered")

                val service = gatt.getService(SERVICE_UUID)
                val stateChar = service?.getCharacteristic(AUTO_STATE_UUID)

                if (stateChar != null) {
                    // Enable notifications for progress updates
                    gatt.setCharacteristicNotification(stateChar, true)

                    val descriptor = stateChar.getDescriptor(CCCD_UUID)

                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
                        gatt.writeDescriptor(
                            descriptor,
                            BluetoothGattDescriptor.ENABLE_INDICATION_VALUE
                        )
                    } else {
                        @Suppress("DEPRECATION")
                        descriptor?.value = BluetoothGattDescriptor.ENABLE_INDICATION_VALUE
                        @Suppress("DEPRECATION")
                        gatt.writeDescriptor(descriptor)
                    }

                    Log.d(TAG, "Subscribed to glove state updates")
                } else {
                    Log.e(TAG, "State characteristic not found")
                }
            }
        }

        override fun onCharacteristicChanged(
            gatt: BluetoothGatt,
            characteristic: BluetoothGattCharacteristic
        ) {
            if (characteristic.uuid == AUTO_STATE_UUID) {
                // temp fix
                @Suppress("DEPRECATION")
                val eventByte = characteristic.value[0]
                val event = GloveEvent.fromByte(eventByte)

                Log.d(TAG, "Received event: $event")

                val currentState = _gloveState.value
                when (event) {
                    GloveEvent.STARTED_ACTIVITY -> {
                        _gloveState.value = currentState.copy(
                            isExercising = true,
                            currentReps = 0,
                            lastEvent = event
                        )
                    }
                    GloveEvent.REP_COMPLETED -> {
                        _gloveState.value = currentState.copy(
                            currentReps = currentState.currentReps + 1,
                            lastEvent = event
                        )
                    }
                    GloveEvent.ACTIVITY_COMPLETED -> {
                        _gloveState.value = currentState.copy(
                            isExercising = false,
                            lastEvent = event
                        )
                    }
                    GloveEvent.ACTIVITY_PAUSED -> {
                        _gloveState.value = currentState.copy(
                            isPaused = true,
                            lastEvent = event
                        )
                    }
                    GloveEvent.ACTIVITY_RESUMED -> {
                        _gloveState.value = currentState.copy(
                            isPaused = false,
                            lastEvent = event
                        )
                    }
                    GloveEvent.ACTIVITY_CANCELED -> {
                        _gloveState.value = currentState.copy(
                            isExercising = false,
                            isPaused = false,
                            lastEvent = event
                        )
                    }
                    else -> {
                        _gloveState.value = currentState.copy(lastEvent = event)
                    }
                }
            }
        }
    }
    fun simulateExercise(reps: Int, delayMs: Long) {
        viewModelScope.launch {
            repeat(reps) { i ->
                delay(delayMs)
                _gloveState.value = _gloveState.value.copy(
                    currentReps = i + 1,
                    lastEvent = GloveEvent.REP_COMPLETED
                )
            }

            delay(delayMs)
            _gloveState.value = _gloveState.value.copy(
                isExercising = false,
                lastEvent = GloveEvent.ACTIVITY_COMPLETED
            )
        }
    }
}


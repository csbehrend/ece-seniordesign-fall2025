package com.example.myapplication

import android.bluetooth.BluetoothDevice
import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.Job
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.catch
import kotlinx.coroutines.flow.launchIn
import kotlinx.coroutines.flow.onCompletion
import kotlinx.coroutines.flow.onEach
import kotlinx.coroutines.flow.onStart
import kotlinx.coroutines.flow.update
import no.nordicsemi.android.kotlin.ble.core.scanner.BleScanResultData
import no.nordicsemi.android.kotlin.ble.scanner.BleScanner
import no.nordicsemi.android.kotlin.ble.scanner.aggregator.BleScanResultAggregator
import javax.inject.Inject
import no.nordicsemi.android.kotlin.ble.core.ServerDevice
import android.annotation.SuppressLint

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
data class ScannedDevice(
    val device: ServerDevice,
    val name: String?,
    val rssi: Int,
    val highestRssi: Int = rssi,
    val isConnectable: Boolean = true
) {
    val displayName: String
        get() = name ?: device.address
}

sealed class ScanningState {
    data class DevicesDiscovered(val devices: List<ScannedDevice>) : ScanningState()
    data class Error(val message: String?) : ScanningState()
    object Idle : ScanningState()
}

data class ScannerUiState(
    val isScanning: Boolean = false,
    val scanningState: ScanningState = ScanningState.Idle
)

@HiltViewModel
class SimpleBleScanner @Inject constructor(
    private val bleScanner: BleScanner
) : ViewModel() {

    private val _uiState = MutableStateFlow(ScannerUiState())
    val uiState = _uiState.asStateFlow()

    private var scanningJob: Job? = null
    private val scanResults = mutableMapOf<String, ScannedDevice>()

    @SuppressLint("MissingPermission")
    fun startScanning() {
        if (scanningJob != null) return

        scanningJob = bleScanner.scan()
            .onStart {
                scanResults.clear()
                _uiState.update {
                    it.copy(
                        isScanning = true,
                        scanningState = ScanningState.DevicesDiscovered(emptyList())
                    )
                }
            }
            .onEach { scanResult ->
                val address = scanResult.device.address
                val existing = scanResults[address]

                val device = ScannedDevice(
                    device = scanResult.device,
                    name = scanResult.device.name,
                    rssi = scanResult.data?.rssi ?: -100,
                    highestRssi = maxOf(existing?.highestRssi ?: scanResult.data?.rssi ?: -100, scanResult.data?.rssi ?: -100)
                )

                scanResults[address] = device

                _uiState.update {
                    it.copy(
                        isScanning = true,
                        scanningState = ScanningState.DevicesDiscovered(
                            scanResults.values.sortedByDescending { it.highestRssi }
                        )
                    )
                }
            }
            .onCompletion {
                _uiState.update { it.copy(isScanning = false) }
                scanningJob?.cancel()
                scanningJob = null
            }
            .catch { throwable ->
                _uiState.update {
                    it.copy(
                        isScanning = false,
                        scanningState = ScanningState.Error(throwable.message)
                    )
                }
                scanningJob?.cancel()
                scanningJob = null
            }
            .launchIn(viewModelScope)
    }

    fun stopScanning() {
        scanningJob?.cancel()
        scanningJob = null
        _uiState.update { it.copy(isScanning = false) }
    }

    fun reload() {
        stopScanning()
        scanResults.clear()
        startScanning()
    }

    override fun onCleared() {
        super.onCleared()
        stopScanning()
    }
}
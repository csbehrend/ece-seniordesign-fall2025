/*
 * Copyright 2023 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.platform.app

import android.os.Build
import androidx.compose.runtime.Composable
import androidx.fragment.compose.AndroidFragment
import com.example.platform.connectivity.bluetooth.ble.BLEScanIntentSample
import com.example.platform.connectivity.bluetooth.ble.ConnectGATTSample
import com.example.platform.connectivity.bluetooth.ble.FindBLEDevicesSample
import com.example.platform.connectivity.bluetooth.ble.server.GATTServerSample
import com.example.platform.shared.MinSdkBox

interface SampleDemo : CatalogItem {
    override val id: String
    override val name: String
    override val description: String?
    val documentation: String?
    val minSdk: Int
    val tags: List<String>
    val apiSurface: ApiSurface
    val content: Any
}

data class ComposableSampleDemo(
    override val id: String,
    override val name: String,
    override val description: String? = null,
    override val documentation: String? = null,
    override val minSdk: Int = Build.VERSION_CODES.LOLLIPOP,
    override val apiSurface: ApiSurface,
    override val tags: List<String> = emptyList(),
    override val content: @Composable () -> Unit,
) : SampleDemo

data class ActivitySampleDemo(
    override val id: String,
    override val name: String,
    override val description: String? = null,
    override val documentation: String? = null,
    override val minSdk: Int = Build.VERSION_CODES.LOLLIPOP,
    override val apiSurface: ApiSurface,
    override val tags: List<String> = emptyList(),
    override val content: Class<*>,
) : SampleDemo

val SAMPLE_DEMOS by lazy {
    listOf(
        ComposableSampleDemo(
            id = "create-gatt-server",
            name = "Create a GATT server",
            description = "Shows how to create a GATT server and communicate with the GATT client",
            documentation = "https://developer.android.com/reference/android/bluetooth/BluetoothGattServer",
            apiSurface = ConnectivityBluetoothBleApiSurface,
            tags = listOf("Bluetooth"),
            content = {
                MinSdkBox(minSdk = Build.VERSION_CODES.M) {
                    //noinspection NewApi
                    GATTServerSample()
                }
            },
        ),
        ComposableSampleDemo(
            id = "scan-with-ble-intent",
            name = "Scan with BLE Intent",
            description = "This samples shows how to use the BLE intent to scan for devices",
            documentation = "https://developer.android.com/reference/android/bluetooth/le/BluetoothLeScanner#startScan(java.util.List%3Candroid.bluetooth.le.ScanFilter%3E,%20android.bluetooth.le.ScanSettings,%20android.app.PendingIntent)",
            apiSurface = ConnectivityBluetoothBleApiSurface,
            tags = listOf("Bluetooth"),
            content = {
                MinSdkBox(minSdk = Build.VERSION_CODES.O) {
                    //noinspection NewApi
                    BLEScanIntentSample()
                }
            },
        ),
        ComposableSampleDemo(
            id = "connect-gatt-server",
            name = "Connect to a GATT server",
            description = "Shows how to connect to a GATT server hosted by the BLE device and perform simple operations",
            documentation = "https://developer.android.com/guide/topics/connectivity/bluetooth/connect-gatt-server",
            apiSurface = ConnectivityBluetoothBleApiSurface,
            tags = listOf("Bluetooth"),
            content = {
                MinSdkBox(minSdk = Build.VERSION_CODES.M) {
                    //noinspection NewApi
                    ConnectGATTSample()
                }
            },
        ),
        ComposableSampleDemo(
            id = "find-devices",
            name = "Find devices",
            description = "This example will demonstrate how to scanning for Low Energy Devices",
            documentation = "https://developer.android.com/guide/topics/connectivity/bluetooth",
            apiSurface = ConnectivityBluetoothBleApiSurface,
            tags = listOf("Bluetooth"),
            content = {
                MinSdkBox(minSdk = Build.VERSION_CODES.M) {
                    //noinspection NewApi
                    FindBLEDevicesSample()
                }
            },
        ),
    ).associateBy { it.id }
}
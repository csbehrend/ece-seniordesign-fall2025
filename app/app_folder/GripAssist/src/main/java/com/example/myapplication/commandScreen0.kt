//package com.example.myapplication
//import androidx.compose.foundation.background
//import androidx.compose.foundation.layout.Arrangement
//import androidx.compose.foundation.layout.Box
//import androidx.compose.foundation.layout.Column
//import androidx.compose.foundation.layout.fillMaxSize
//import androidx.compose.foundation.layout.fillMaxWidth
//import androidx.compose.foundation.layout.height
//import androidx.compose.foundation.layout.padding
//import androidx.compose.foundation.layout.width
//import androidx.compose.foundation.shape.RoundedCornerShape
//import androidx.compose.material3.Text
//import androidx.compose.runtime.Composable
//import androidx.compose.ui.Alignment
//import androidx.compose.ui.Modifier
//import androidx.compose.ui.graphics.Color
//import androidx.compose.ui.text.font.FontWeight
//import androidx.compose.ui.unit.dp
//import androidx.compose.ui.unit.sp
//
//import androidx.compose.material3.Icon
//import androidx.compose.material.icons.Icons
//import androidx.compose.material.icons.filled.PlayArrow
//import androidx.compose.material.icons.filled.Stop
//import androidx.compose.material.icons.automirrored.filled.ArrowForward
//import androidx.compose.material3.Button
//import androidx.compose.material3.ButtonDefaults
//import androidx.compose.runtime.remember
//import androidx.compose.material3.Slider
//import androidx.compose.runtime.getValue
//import androidx.compose.runtime.setValue
//import androidx.compose.material3.SliderDefaults
//import androidx.compose.foundation.layout.Row
//import androidx.compose.runtime.mutableStateOf
//import androidx.lifecycle.ViewModel
//import androidx.lifecycle.viewmodel.compose.viewModel
//import androidx.compose.foundation.shape.CircleShape
//import androidx.compose.material3.AlertDialog
//import androidx.compose.runtime.collectAsState
//
//class SpeedViewModel : ViewModel() {
//    var sliderPosition by mutableStateOf(1f)
//    var speed by mutableStateOf(0f)
//}
//
//@Composable
//fun command(modifier: Modifier = Modifier) {
//    //var sliderPosition by remember { mutableFloatStateOf(0f) }
//    //var speed by remember { mutableFloatStateOf(0f) }
//    val scannerViewModel: SimpleBleScanner = viewModel()
//    val viewModel: SpeedViewModel = viewModel()
//    val sliderPosition = viewModel.sliderPosition
//    val speed = viewModel.speed
//    var showPopup by remember { mutableStateOf(false) }
//    val scannerUiState by scannerViewModel.uiState.collectAsState()
//    Box(
//        modifier = Modifier
//            .fillMaxSize()
//            .background(
//                color = Color(0xFFE6F4F3)
//            ),
//        contentAlignment = Alignment.Center
//    )
//    {
//        Box (
//            modifier = Modifier
//                .fillMaxWidth(1f)
//                .height(100.dp)
//                .background(
//                    color = Color(0xFFB0E5E8)
//                    //shape = RoundedCornerShape(50.dp)
//                )
//                .align(Alignment.TopCenter),
//            contentAlignment = Alignment.Center
//        )
//        {
//            Text(
//                text = "Exercise Control",
//                fontSize = 30.sp,
//                color = Color.Black,
//                fontWeight = FontWeight.Bold
//            )
//        }
//        Column(
//            modifier = Modifier
//                .padding(top = 10.dp),
//            horizontalAlignment = Alignment.CenterHorizontally,
//            verticalArrangement = Arrangement.spacedBy(30.dp)
//
//        ) {
//            Button (
//                onClick = {
//                    showPopup = true
//                    scannerViewModel.startScanning()},
//                colors = ButtonDefaults.buttonColors(
//                    containerColor = Color(0xFF5DAFA7)),
//                shape = CircleShape,
//                modifier = Modifier
//                    .fillMaxWidth(.3f)
//                    .height(50.dp)
//                    .background(
//                        color = Color(0xFF5DAFA7),
//                        shape = RoundedCornerShape(50.dp)
//                    )
//            )
//            {
//                Text(
//                    text = "Connect",
//                    fontSize = 30.sp,
//                    color = Color.White,
//                    fontWeight = FontWeight.Bold
//                )
//            }
//            if (showPopup) {
//                AlertDialog(
//                    onDismissRequest = {
//                        showPopup = false
//                        scannerViewModel.stopScanning()},
//                    title = { Text("Devices") },
//                    text = {
//                        when (val state = scannerUiState.scanningState) {
//                            is ScanningState.DevicesDiscovered -> {
//                                if (state.devices.isEmpty()) {
//                                    Text("Scanning for devices...")
//                                } else {
//                                    Column {
//                                        state.devices.forEach { device ->
//                                            Text(
//                                                text = "${device.name ?: "Unknown"} (${device.rssi} dBm)",
//                                                modifier = Modifier.padding(8.dp)
//                                            )
//                                        }
//                                    }
//                                }
//                            }
//                            is ScanningState.Error -> {
//                                Text("Error: ${state.message}")
//                            }
//                            ScanningState.Idle -> {
//                                Text("Starting scan...")
//                            }
//                        }
//                    },
//                    confirmButton = {
//                        Button(onClick = {
//                            showPopup = false
//                            scannerViewModel.stopScanning()}) {
//                            Text("DONE")
//                        }
//                    }
//                )
//            }
//
//            Button (
//                onClick = {},
//                colors = ButtonDefaults.buttonColors(
//                    containerColor = Color(0xFF5DAFA7)),
//                modifier = Modifier
//                    .fillMaxWidth(.3f)
//                    .height(50.dp)
//                    .background(
//                        color = Color(0xFF5DAFA7),
//                        shape = RoundedCornerShape(50.dp)
//                    )
//            )
//            {
//                Icon(
//                   imageVector = Icons.Default.PlayArrow,
//                   contentDescription = "play",
//                    tint = Color.White,
//                    modifier = Modifier
//                      // .align(Alignment.CenterStart)
//                       .width(100.dp)
//                        .height(100.dp)
//                )
//                Text(
//                    text = "Start",
//                    fontSize = 30.sp,
//                    color = Color.White,
//                    fontWeight = FontWeight.Bold
//                )
//            }
//            Button (
//                onClick = {},
//                colors = ButtonDefaults.buttonColors(
//                    containerColor = Color(0xFFE8825C)),
//                modifier = Modifier
//                    .fillMaxWidth(.3f)
//                    .height(50.dp)
//                    .background(
//                        color = Color(0xFF5DAFA7),
//                        shape = RoundedCornerShape(50.dp)
//                    )
//            )
//            {
//                Icon(
//                    imageVector = Icons.Default.Stop,
//                    contentDescription = "stop",
//                    tint = Color.White,
//                    modifier = Modifier
//                       // .align(Alignment.CenterStart)
//                        .width(100.dp)
//                        .height(100.dp)
//                )
//                Text(
//                    text = "Stop",
//                    fontSize = 30.sp,
//                    color = Color.White,
//                    fontWeight = FontWeight.Bold
//                )
//            }
//            Button (
//                onClick = {},
//                colors = ButtonDefaults.buttonColors(
//                    containerColor = Color(0xFF5DAFA7)),
//                modifier = Modifier
//                    .fillMaxWidth(.3f)
//                    .height(50.dp)
//                    .background(
//                        color = Color(0xFF5DAFA7),
//                        shape = RoundedCornerShape(50.dp)
//                    )
//            )
//            {
//                Icon(
//                    imageVector = Icons.AutoMirrored.Filled.ArrowForward,
//                    contentDescription = "continue",
//                    tint = Color.White,
//                    modifier = Modifier
//                        //.align(Alignment.CenterStart)
//                        .width(100.dp)
//                        .height(100.dp)
//                )
//                Text(
//                    text = "Continue",
//                    fontSize = 30.sp,
//                    color = Color.White,
//                    fontWeight = FontWeight.Bold
//                )
//            }
//            Text(
//                text = "Select a Speed",
//                fontSize = 30.sp,
//                color = Color.Black,
//                fontWeight = FontWeight.Bold
//            )
//            Row(
//                modifier = Modifier
//                    .fillMaxWidth(0.7f),
//                horizontalArrangement = Arrangement.SpaceBetween
//            )
//            {
//                for (i in 1..10) {
//                    Text(
//                        text = i.toString(),
//                        color = Color.Black)
//                }
//            }
//            Slider(
//                value = sliderPosition,
//                onValueChange = { position ->
//                    viewModel.sliderPosition = position
//                    viewModel.speed = when (position.toInt()) {
//                        1 -> 0.5f
//                        2 -> 1f
//                        3 -> 1.5f
//                        4 -> 2f
//                        5 -> 2.5f
//                        6 -> 3f
//                        7 -> 3.5f
//                        8 -> 4f
//                        9 -> 4.5f
//                        10 -> 5f
//                        else -> 1f
//                    }
//                },
//                modifier = Modifier
//                            .fillMaxWidth(0.7f),
//                            //.height(150.dp),
//                colors = SliderDefaults.colors(
//                    thumbColor = Color.Black,
//                    activeTrackColor = Color(0xFF5DAFA7),
//                    inactiveTrackColor = Color.White
//                ),
//                valueRange = 1f..10f,
//                steps = 8
//            )
//
//        }
//    }
//}

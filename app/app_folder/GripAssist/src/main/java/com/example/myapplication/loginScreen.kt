package com.example.myapplication
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.fillMaxHeight

import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.withStyle
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

import androidx.compose.material3.Icon
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Stop
import androidx.compose.material.icons.automirrored.filled.ArrowForward

import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.compose.material3.TextField
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.ui.unit.dp
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.material3.Button
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.style.TextAlign
import androidx.navigation.NavController
import com.google.firebase.firestore.FirebaseFirestore
import android.content.Context
import androidx.compose.ui.platform.LocalContext
import android.util.Log
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import androidx.core.content.ContextCompat.getSystemService
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow
import android.net.NetworkRequest
import kotlinx.coroutines.channels.awaitClose

/*sealed interface NetworkConnectionState {
    data object Available: NetworkConnectionState
    data object Unavailable: NetworkConnectionState
}

// callback?
private fun networkCallBack(callback: (NetworkConnectionState) -> Unit) = object: ConnectivityManager.NetworkCallback() {
        override fun onAvailable(network: Network) {
            callback(NetworkConnectionState.Available)
        }
        override fun onLost(network: Network) {
            callback(NetworkConnectionState.Unavailable)
        }
}

fun getCurrentConnectivityState(connectivityManager: ConnectivityManager): NetworkConnectionState
{
    val network = connectivityManager.activeNetwork
    val isConnected = connectivityManager
        .getNetworkCapabilities(network)
        ?.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET) ?: false
    return if (isConnected) NetworkConnectionState.Available else NetworkConnectionState.Unavailable

}
fun Context.observeConnectivityAsFlow(): Flow<NetworkConnectionState> = callbackFlow {
    val ConnectivityManager = getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
    val callback = networkCallback { connectionState ->
        trySend(connectionState)
    }
    val networkRequest = NetworkRequest.Builder()
        .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
        .build()
    connectivityManager.registerNetworkCallback(networkRequest, callback)
    val currentState = getCurrentConnectivityState(connectivityManager)
    trySend(currentState)
    awaitClose {
        connectivityManager.unregisterNetworkCallback(callback)
    }
}
val Context.currentConnectivityState: NetworkConnectionState
    get() {
        val connectivityManager = getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        return getCurrentConnectivityState(connectivityManager)
    }*/

@Composable
fun hasInternetConnection(): Boolean {
    val context = LocalContext.current
    val connectivityManager =
        context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager

    val network = connectivityManager.activeNetwork ?: return false
    val capabilities = connectivityManager.getNetworkCapabilities(network) ?: return false

    return capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
}


@Composable
fun loginScreen(modifier: Modifier = Modifier, navController: NavController) {
    var username by remember { mutableStateOf("") }
    val context = LocalContext.current
    val db = FirebaseFirestore.getInstance()
    val isOnline = hasInternetConnection()

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(
                color = Color(0xFFE6F4F3)
            ),
        contentAlignment = Alignment.Center
    )
    {
        Column (
            modifier = Modifier
                //.padding(top = 10.dp),
                .padding(bottom = 30.dp), // temp fix
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.spacedBy(30.dp)

        )
        {
            if (isOnline) {
                Text("Connected to internet")
            } else {
                Text("No internet connection")
            }
            Image(
                painter = painterResource(id = R.drawable.login_image),
                contentDescription = "login image",
                modifier = Modifier
                    .fillMaxHeight(.3f)
                    .fillMaxWidth(.3f)
            )
            Text (
                text = "Welcome to Rehab GloveAssist",
                fontSize = 30.sp,
                color = Color.Black,
                fontWeight = FontWeight.Bold
            )
            Text (
                text = "Track your progress and manage your rehabiliation exercises.",
                fontSize = 20.sp,
                color = Color(0xFF008000),
               // fontWeight = FontWeight.Bold
            )

            Box(
                modifier = Modifier
                    .fillMaxWidth(0.5f)
                    .height(50.dp)
                    .background(
                        color = Color.White,
                        shape = RoundedCornerShape(50.dp)
                    ),
                contentAlignment = Alignment.Center
            )
            {
                BasicTextField(
                    value = username,
                    onValueChange = { username = it },
                    singleLine = true,
                    textStyle = TextStyle(
                        color = Color.Black,
                        fontSize = 30.sp,
                        textAlign = TextAlign.Center
                    ),
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(horizontal = 16.dp),
                    decorationBox = { innerTextField ->
                        if (username.isEmpty()) {
                            Text(
                                text = "Enter Username",
                                fontSize = 30.sp,
                                color = Color(0xFF59CA3AF).copy(alpha = 0.5f),
                                textAlign = TextAlign.Center,
                                modifier = Modifier.fillMaxWidth()
                            )
                        }
                        innerTextField()
                    }
                )
            }
            Button (
                onClick = {
                    Log.d("LoginScreen", "Login button clicked. Username: $username")
                    if (username.isNotEmpty()){
                        Log.d("LoginScreen", "Username is not empty. Checking Firestore...")
                        db.collection("users").document(username).get()
                            .addOnSuccessListener { document ->
                                if (document.exists()) {
                                    Log.d("LoginScreen", "User found in Firestore: $username")
                                    val role = document.getString("role")
                                    Log.d("LoginScreen", "role is $role")
                                    val prefs = context.getSharedPreferences("user_prefs", Context.MODE_PRIVATE)
                                    prefs.edit().putString("username", username).apply()
                                    if (role == "patient") {
                                        Log.d("LoginScreen", "Navigating to patient screens")
                                        navController.navigate("goalScreen")
                                    }
                                    else if (role == "therapist"){
                                        navController.navigate("therapistScreen")
                                    }

                                }
                            }
                            .addOnFailureListener { e ->
                                Log.e("LoginScreen", "Error getting document", e)
                            }
                    }
                },
                colors = ButtonDefaults.buttonColors(
                    containerColor = Color(0xFF5DAFA7)),
                 modifier = Modifier
                     .fillMaxWidth(.5f)
                )
            {
                Text(
                    text = "Log In",
                    fontSize = 30.sp,
                    color = Color.White,
                    fontWeight = FontWeight.Bold
                )
            }

        }
    }
}
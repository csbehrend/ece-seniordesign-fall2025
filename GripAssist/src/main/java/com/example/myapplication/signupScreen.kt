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
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.material3.RadioButton
import androidx.compose.material3.RadioButtonDefaults
import com.google.firebase.firestore.FirebaseFirestore
import androidx.navigation.NavController
@Composable
// handle cases of repeated username or already existing in database
fun signupScreen(modifier: Modifier = Modifier, navController: NavController) {
    var username by remember { mutableStateOf("") }
    var selectedOption by remember { mutableStateOf("") }
    val db = FirebaseFirestore.getInstance()
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
                .padding(bottom = 30.dp), // temp fix
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.spacedBy(20.dp)
        )
        {
            Image(
                painter = painterResource(id = R.drawable.signup_image),
                contentDescription = "signup image",
                modifier = Modifier
                    .fillMaxHeight(.2f)
                    .fillMaxWidth(1f)
            )
            Text (
                text = "Welcome to Your Path to Recovery",
                fontSize = 30.sp,
                color = Color(0xFF4A7C59),
                fontWeight = FontWeight.Bold
            )
            Text (
                text = "Select Your Role",
                fontSize = 30.sp,
                color = Color(0xFF4A7C59),
                fontWeight = FontWeight.Bold
            )
            Row (
                modifier = Modifier.fillMaxWidth(.4f),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            )
            {
                Box (
                    modifier = Modifier
                       // .fillMaxWidth(.3f)
                        .weight(1f)
                        .height(50.dp)
                        .background(
                            color = Color.White,
                            shape = RoundedCornerShape(50.dp)
                        ),
                    contentAlignment = Alignment.Center
                )
                {
                    Row (
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceEvenly,
                        verticalAlignment = Alignment.CenterVertically
                    )
                    {
                        Image(
                            painter = painterResource(id = R.drawable.user_md),
                            contentDescription = "user_md",
                            modifier = Modifier
                                .fillMaxHeight(.8f)
                                .fillMaxWidth(.3f)
                        )
                        RadioButton(
                            selected = selectedOption == "therapist",
                            onClick = { selectedOption = "therapist" },
                            colors = RadioButtonDefaults.colors(
                                selectedColor = Color(0xFF5DAFA7)
                            )
                        )
                       Text (
                            text = "Therapist",
                            fontSize = 15.sp,
                            color = Color(0xFF4A7C59),
                            //fontWeight = FontWeight.Bold
                        )
                    }

                }
                Spacer(modifier = Modifier.width(50.dp))
                Box (
                    modifier = Modifier
                        //.fillMaxWidth(.3f)
                        .weight(1f)
                        .height(50.dp)
                        .background(
                            color = Color.White,
                            shape = RoundedCornerShape(50.dp)
                        ),
                    contentAlignment = Alignment.Center
                )
                {
                    Row (
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceEvenly,
                        verticalAlignment = Alignment.CenterVertically
                    )
                    {
                        Image(
                            painter = painterResource(id = R.drawable.user),
                            contentDescription = "user",
                            modifier = Modifier
                                .fillMaxHeight(.8f)
                                .fillMaxWidth(.3f)
                        )
                        RadioButton(
                            selected = selectedOption == "patient",
                            onClick = { selectedOption = "patient" },
                            colors = RadioButtonDefaults.colors(
                                selectedColor = Color(0xFF5DAFA7)
                            )
                        )
                       // Text("Patient")
                        Text (
                            text = "Patient",
                            fontSize = 15.sp,
                            color = Color(0xFF4A7C59),
                            //fontWeight = FontWeight.Bold
                        )
                    }

                }
            }

            Spacer(Modifier.width(60.dp))

            Box(
                modifier = Modifier
                    .fillMaxWidth(0.7f)
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
                    val user = hashMapOf(
                        "username" to username,
                        "role" to selectedOption,
                        "totalRepsCompleted" to 0
                    )
                    db.collection("users")
                        .document(username)
                        .set(user)
                },
                colors = ButtonDefaults.buttonColors(
                    containerColor = Color(0xFF5DAFA7)),
                modifier = Modifier
                    .fillMaxWidth(.7f)
            )
            {
                Text(
                    text = "Sign Up",
                    fontSize = 30.sp,
                    color = Color.White,
                    fontWeight = FontWeight.Bold
                )
            }

            Button (
                onClick = {
                    navController.navigate("loginScreen")
                },
                colors = ButtonDefaults.buttonColors(
                    containerColor = Color(0xFF5DAFA7)),
                modifier = Modifier
                    .fillMaxWidth(.7f)
            )
            {
                Text(
                    text = "Login for Existing Users",
                    fontSize = 30.sp,
                    color = Color.White,
                    fontWeight = FontWeight.Bold
                )
            }

        }
    }
}
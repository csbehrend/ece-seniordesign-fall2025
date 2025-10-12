package com.example.myapplication
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
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
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults


@Composable
fun commandScreen(modifier: Modifier = Modifier) {

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(
                color = Color(0xFFE6F4F3)
            ),
        contentAlignment = Alignment.Center
    )
    {
        Box (
            modifier = Modifier
                .fillMaxWidth(1f)
                .height(100.dp)
                .background(
                    color = Color(0xFFB0E5E8)
                    //shape = RoundedCornerShape(50.dp)
                )
                .align(Alignment.TopCenter),
            contentAlignment = Alignment.Center
        )
        {
            Text(
                text = "Exercise Control",
                fontSize = 30.sp,
                color = Color.Black,
                fontWeight = FontWeight.Bold
            )
        }
        Column(
            modifier = Modifier
                .padding(top = 10.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.spacedBy(30.dp)

        ) {
            Button (
                onClick = {},
                colors = ButtonDefaults.buttonColors(
                    containerColor = Color(0xFF5DAFA7)),
                modifier = Modifier
                    .fillMaxWidth(.5f)
                    .height(100.dp)
                    .background(
                        color = Color(0xFF5DAFA7),
                        shape = RoundedCornerShape(50.dp)
                    )
            )
            {
                Icon(
                   imageVector = Icons.Default.PlayArrow,
                   contentDescription = "play",
                    tint = Color.White,
                    modifier = Modifier
                      // .align(Alignment.CenterStart)
                       .width(100.dp)
                        .height(100.dp)
                )
                Text(
                    text = "Start",
                    fontSize = 30.sp,
                    color = Color.White,
                    fontWeight = FontWeight.Bold
                )
            }
            Button (
                onClick = {},
                colors = ButtonDefaults.buttonColors(
                    containerColor = Color(0xFFE8825C)),
                modifier = Modifier
                    .fillMaxWidth(.5f)
                    .height(100.dp)
                    .background(
                        color = Color(0xFF5DAFA7),
                        shape = RoundedCornerShape(50.dp)
                    )
            )
            {
                Icon(
                    imageVector = Icons.Default.Stop,
                    contentDescription = "stop",
                    tint = Color.White,
                    modifier = Modifier
                       // .align(Alignment.CenterStart)
                        .width(100.dp)
                        .height(100.dp)
                )
                Text(
                    text = "Stop",
                    fontSize = 30.sp,
                    color = Color.White,
                    fontWeight = FontWeight.Bold
                )
            }
            Button (
                onClick = {},
                colors = ButtonDefaults.buttonColors(
                    containerColor = Color(0xFF5DAFA7)),
                modifier = Modifier
                    .fillMaxWidth(.5f)
                    .height(100.dp)
                    .background(
                        color = Color(0xFF5DAFA7),
                        shape = RoundedCornerShape(50.dp)
                    )
            )
            {
                Icon(
                    imageVector = Icons.AutoMirrored.Filled.ArrowForward,
                    contentDescription = "continue",
                    tint = Color.White,
                    modifier = Modifier
                        //.align(Alignment.CenterStart)
                        .width(100.dp)
                        .height(100.dp)
                )
                Text(
                    text = "Continue",
                    fontSize = 30.sp,
                    color = Color.White,
                    fontWeight = FontWeight.Bold
                )
            }
        }
    }


}
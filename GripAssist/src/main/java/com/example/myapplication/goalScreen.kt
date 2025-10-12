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
import androidx.compose.foundation.layout.padding

@Composable
// add row to center icon and text
// add more relative positioning instead of absolute for it to be versatile
fun goalScreen(
    modifier: Modifier = Modifier,
    name: String,
    exercise: String,
    sessions: Int,
    sets: Int
) {
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
                text = "Goal",
                fontSize = 30.sp,
                color = Color.Black,
                fontWeight = FontWeight.Bold
            )
        }
        Column(
            modifier = Modifier
                .padding(top = 5.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.spacedBy(30.dp)

        ) {
            Image(
                painter = painterResource(id = R.drawable.hand), // your file name
                contentDescription = "hand image",
                modifier = Modifier
                    .height(200.dp)
                    .fillMaxWidth(1f),
                contentScale = ContentScale.Fit
            )
            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.spacedBy(6.dp)
            )
            {
                Text(
                    text = "Exercise",
                    fontSize = 30.sp,
                    color = Color.Black,
                    fontWeight = FontWeight.Bold,
                    modifier = Modifier.padding(bottom = 2.dp)
                )
                Box(
                    modifier = Modifier
                        .fillMaxWidth(0.6f)
                        .height(50.dp)
                        .background(
                            color = Color.White,
                            shape = RoundedCornerShape(16.dp)
                        ),
                    contentAlignment = Alignment.Center

                )
                {
                    Image(
                        painter = painterResource(id = R.drawable.dumbbell),
                        contentDescription = "dumbbell image",
                        modifier = Modifier
                            .height(50.dp)
                            .width(50.dp)
                            .align(Alignment.CenterStart)
                    )
                    Text(
                        buildAnnotatedString {
                            withStyle(style = SpanStyle(fontWeight = FontWeight.Bold)) {
                                append(
                                    exercise
                                )
                            }
                        },
                        fontSize = 30.sp,
                        color = Color.Black
                    )
                }
            }
            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.spacedBy(6.dp)
            )
            {
                Text(
                    text = "Number of Sessions",
                    fontSize = 30.sp,
                    color = Color.Black,
                    fontWeight = FontWeight.Bold
                )
                Box(
                    modifier = Modifier
                        .fillMaxWidth(0.6f)
                        .height(50.dp)
                        .background(
                            color = Color.White,
                            shape = RoundedCornerShape(16.dp)
                        ),
                    contentAlignment = Alignment.Center
                )
                {
                    Image(
                        painter = painterResource(id = R.drawable.calendar),
                        contentDescription = "calendar image",
                        modifier = Modifier
                            .height(50.dp)
                            .width(50.dp)
                            .align(Alignment.CenterStart)
                    )
                    Text(
                        text = "$sessions",
                        fontSize = 30.sp,
                        color = Color.Black,
                        fontWeight = FontWeight.Bold
                    )
                }

            }
            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.spacedBy(6.dp)
            )
            {
                Text(
                    text = "Sets per Session",
                    fontSize = 30.sp,
                    color = Color.Black,
                    fontWeight = FontWeight.Bold
                )
                Box(
                    modifier = Modifier
                        .fillMaxWidth(0.6f)
                        .height(50.dp)
                        .background(
                            color = Color.White,
                            shape = RoundedCornerShape(16.dp)
                        ),
                    contentAlignment = Alignment.Center
                )
                {
                    Image(
                        painter = painterResource(id = R.drawable.ellipsis),
                        contentDescription = "ellipsis image",
                        modifier = Modifier
                            .height(50.dp)
                            .width(50.dp)
                            .align(Alignment.CenterStart)
                    )
                    Text(
                        text = "$sets",
                        fontSize = 30.sp,
                        color = Color.Black,
                        fontWeight = FontWeight.Bold
                    )
                }
            }


        }
    }
}

package com.example.myapplication
import android.content.Context
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
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight

import androidx.compose.foundation.layout.Spacer
import androidx.compose.runtime.*
import androidx.compose.ui.platform.LocalContext
import androidx.hilt.lifecycle.viewmodel.compose.hiltViewModel
import com.google.firebase.firestore.FirebaseFirestore


@Composable
// fix tickmarks 
fun progressScreen( modifier: Modifier = Modifier,
                    counts: Int)
{
    val gloveManager = hiltViewModel<GloveManager>()
    val gloveState by gloveManager.gloveState.collectAsState()
    val db = FirebaseFirestore.getInstance()
    val context = LocalContext.current
    val prefs = context.getSharedPreferences("user_prefs", Context.MODE_PRIVATE)
    val username = prefs.getString("username", "") ?: ""
    var sessions by remember { mutableIntStateOf(0) }
    var sets by remember { mutableIntStateOf(0) }

    if (username.isNotEmpty()){
        db.collection("users").document(username).get()
            .addOnSuccessListener { document ->
                if (document.exists()) {
                    sessions = document.getLong("sessions")?.toInt() ?: 0
                    sets = document.getLong("sets")?.toInt() ?: 0
                }
            }
    }
    val countsFromGlove = gloveState.currentReps

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(
                color = Color(0xFFE6F4F3)
            ),
        contentAlignment = Alignment.Center
    )
    {
        Box(
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
                text = "Progress",
                fontSize = 30.sp,
                color = Color.Black,
                fontWeight = FontWeight.Bold
            )
        }
        Column (
            verticalArrangement = Arrangement.spacedBy(30.dp)
        )
        {
            var counts by remember { mutableStateOf(counts) }
            //var total = counts
            var total = countsFromGlove
            repeat(sessions) {index ->
                val toFill = if (total >= sets) {
                    total -= sets
                    1f
                } else {
                    val fraction = total.toFloat() / sets.toFloat()
                    total = 0
                    fraction
                }
                Column {
                    Row (
                        modifier = Modifier.fillMaxWidth(0.8f),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    )
                    {
                        Text(
                            text = "Session ${index + 1}",
                            color = Color(0xFF4A6C6C),
                            fontSize = 15.sp,
                            fontWeight = FontWeight.Bold
                        )
                        Image(
                            painter = painterResource(id = R.drawable.chartline),
                            contentDescription = "chartline",
                            modifier = Modifier
                                .height(25.dp)
                                .width(25.dp)

                        )
                    }

                    Box(
                        modifier = Modifier
                            .fillMaxWidth(.8f)
                            .height(50.dp)
                            .background(
                                color = Color(0xFFE8F1F2),
                                shape = RoundedCornerShape(50.dp))
                    )
                    {
                        Row (
                            modifier = Modifier.fillMaxSize(),
                            horizontalArrangement = Arrangement.SpaceEvenly,
                            verticalAlignment = Alignment.CenterVertically
                        )
                        {
                            repeat(sets - 1) {index ->

                                Box(
                                    modifier = Modifier
                                        .width(2.dp)
                                        .fillMaxHeight(.7f)
                                        .background(color = Color(0xFF4A6C6C))
                                )
                            }
                        }
                        Box(
                            modifier = Modifier
                                .fillMaxWidth(toFill)
                                .fillMaxHeight()
                                .background(
                                    color = Color(0xFF5F9B92),
                                    shape = RoundedCornerShape(50.dp))
                        )

                    }
                }

            }
        }

    }


}
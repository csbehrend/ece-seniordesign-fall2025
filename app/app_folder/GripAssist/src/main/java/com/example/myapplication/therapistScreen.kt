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
import androidx.compose.runtime.remember
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.style.TextAlign
//import androidx.compose.ui.text.SolidColor
import android.content.Context
import android.net.Uri
import android.provider.OpenableColumns
import android.util.Log
import android.widget.Toast
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import com.google.firebase.firestore.FirebaseFirestore
import com.google.firebase.firestore.SetOptions
import org.w3c.dom.Element
import org.w3c.dom.Document
import org.xml.sax.InputSource
import java.io.StringReader
import javax.xml.parsers.DocumentBuilderFactory

@Composable
// box to text field
// file upload to button
fun therapistScreen(modifier: Modifier = Modifier) {
    var username by remember { mutableStateOf("") }
    val db = FirebaseFirestore.getInstance()
    var selectedFileName by remember { mutableStateOf<String?>(null) }
    val context = LocalContext.current
    val filePickerLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.GetContent()
    ) { uri ->
        uri?.let {selectedUri ->
            val cursor = context.contentResolver.query(selectedUri, null, null, null, null)
            cursor?.use {
                if (it.moveToFirst()) {
                    val nameIndex = it.getColumnIndex(OpenableColumns.DISPLAY_NAME)
                    selectedFileName = it.getString(nameIndex)
                }
            }
            val fileContent = context.contentResolver.openInputStream(selectedUri)
                ?.bufferedReader()
                ?.use { it.readText() }
            fileContent?.let { content ->
                selectedFileName?.let { filename->
                    if (filename.endsWith(".json", ignoreCase = true)) {
                        val jsonText = content.trim()
                        if (jsonText.isNotEmpty()) {
                            val jsonObject = org.json.JSONObject(jsonText)
                            val exercise = jsonObject.optString("exercise", "")
                            val sessions = jsonObject.optInt("sessions", 0)
                            val sets = jsonObject.optInt("sets", 0)

                            val regimen = hashMapOf(
                                "exercise" to exercise,
                                "sessions" to sessions,
                                "sets" to sets
                            )
                            db.collection("users")
                                .document(username)
                                .set(regimen, SetOptions.merge())
                        }

                    } else if (filename.endsWith(".xml", ignoreCase = true)) {
                        val xmlText = content.trim()
                        if (xmlText.startsWith("<")) { // makes sure it's actual XML
                            val factory = DocumentBuilderFactory.newInstance()
                            val builder = factory.newDocumentBuilder()
                            val xmlDoc = builder.parse(org.xml.sax.InputSource(StringReader(xmlText)))
                            xmlDoc.documentElement.normalize()

                            val root = xmlDoc.documentElement
                            val exercise = root.getElementsByTagName("exercise").item(0).textContent
                            val sessions = root.getElementsByTagName("sessions").item(0).textContent.toInt()
                            val sets = root.getElementsByTagName("sets").item(0).textContent.toInt()

                            val regimen = hashMapOf(
                                "exercise" to exercise,
                                "sessions" to sessions,
                                "sets" to sets
                            )

                            db.collection("users")
                                .document(username)
                                .set(regimen, SetOptions.merge())
                        }
                    }
                }
            }

        }


    }

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(
                color = Color(0xFFF5F5F5)
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
            Image(
                painter = painterResource(id = R.drawable.user_md),
                contentDescription = "user md",
                modifier = Modifier
                    .height(50.dp)
                    .width(50.dp)
            )
        }
        Column(
            modifier = Modifier
                .padding(top = 10.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.spacedBy(30.dp)
        ) {

            Box(
                modifier = Modifier
                    .fillMaxWidth(0.7f)
                    .height(80.dp)
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
                                text = "Enter Patient's Username",
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

            Box (
                modifier = Modifier
                    .fillMaxWidth(.7f)
                    .height(160.dp)
                    .background(
                        color = Color.White,
                        shape = RoundedCornerShape(20.dp)
                    ),
                contentAlignment = Alignment.Center
            )
            {
                Column (
                    modifier = Modifier
                        .padding(top = 15.dp)
                        .padding(bottom = 15.dp),
                    horizontalAlignment = Alignment.CenterHorizontally,
                    verticalArrangement = Arrangement.spacedBy(30.dp)
                )
                {
                    Image(
                        painter = painterResource(id = if (selectedFileName != null) R.drawable.file_selected
                        else R.drawable.file_upload
                        ),
                        contentDescription = if (selectedFileName != null) "file selected" else "file upload",
                        modifier = Modifier
                            .height(50.dp)
                            .width(50.dp)
                    )
                    // add file name at the bottom or the side
                    Button (
                        onClick = {
                            filePickerLauncher.launch("*/*") // change to JSON or XML
                        },
                        colors = ButtonDefaults.buttonColors(
                            containerColor = Color(0xFF5B9C94)),
                        modifier = Modifier
                            .fillMaxWidth(.5f)
                            .height(100.dp)
                            .background(
                                color = Color(0xFF5B9C94),
                                shape = RoundedCornerShape(50.dp)
                            )
                    )
                    {
                        Text (
                            text = "Upload",
                            fontSize = 30.sp,
                            color = Color.White,
                            fontWeight = FontWeight.Bold
                        )
                    }
                }
            }
        }

    }
}

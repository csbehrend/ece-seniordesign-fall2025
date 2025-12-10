package com.example.myapplication

//import goalScreen.kt
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.TrendingUp
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.automirrored.filled.ShowChart
import androidx.compose.material3.Icon
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.Scaffold
import androidx.compose.ui.Modifier
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import com.example.myapplication.ui.theme.MyApplicationTheme
import androidx.compose.material3.*
import androidx.compose.material.icons.filled.Build
import androidx.compose.material.icons.filled.Flag
import androidx.compose.material.icons.automirrored.filled.TrendingUp
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color
import androidx.navigation.NavHostController
import androidx.navigation.compose.currentBackStackEntryAsState
import androidx.compose.ui.unit.dp
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.navigation.compose.composable
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.unit.dp
import androidx.compose.ui.Alignment
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Settings
import androidx.compose.foundation.layout.padding
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import androidx.navigation.NavController
import dagger.hilt.android.AndroidEntryPoint
@AndroidEntryPoint
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()


        setContent {
            val navController = rememberNavController()
            val navBackStackEntry by navController.currentBackStackEntryAsState()
            val currentDestination = navBackStackEntry?.destination?.route

            val navItemList = listOf(
                navItem("Goal", Icons.Default.Home),
                navItem("Command", Icons.Default.Settings),
                navItem("Progress", Icons.AutoMirrored.Filled.ShowChart) // change this icon if needed
            )

            MyApplicationTheme {
                var selectedIndex by rememberSaveable { mutableIntStateOf(0) }

                val shouldShowBottomBar = currentDestination in listOf(
                    "goalScreen",
                    "commandScreen",
                    "progressScreen"
                )

                Scaffold(
                    bottomBar = {
                        if (shouldShowBottomBar) {
                            NavigationBar(
                                containerColor = Color(0xFFC5E8ED),
                               // containerColor = Color.Transparent,
                                tonalElevation = 0.dp,
                                modifier = Modifier
                                    //.height(65.dp)
                                    //.offset(y = 20.dp)

                            ) {
                                navItemList.forEachIndexed { index, navItem ->
                                    NavigationBarItem(
                                        selected = selectedIndex == index,
                                        onClick = {
                                            selectedIndex = index
                                        },
                                        icon = {
                                            Icon(
                                                imageVector = navItem.icon,
                                                contentDescription = "Icon"
                                            )
                                        },
                                        label = {
                                            Text(text = navItem.label)
                                        }
                                    )
                                }
                            }
                        }
                    },
                    modifier = Modifier.fillMaxSize()
                ) { innerPadding ->
                    if (shouldShowBottomBar) {
                        ContentScreen(
                            selectedIndex = selectedIndex,
                            navController = navController
                        )
                    } else {
                        NavHost(
                            navController = navController,
                            startDestination = "signupScreen"
                        ) {
                            composable("goalScreen") {

                                goalScreen(
                                    modifier = Modifier.padding(innerPadding)
                                )
                            }
                            composable("commandScreen") {
                                commandScreen()
                            }
                            composable("progressScreen") {
                                progressScreen(
                                    modifier = Modifier.padding(innerPadding),
                                    counts = 3
                                )
                            }
                            composable("loginScreen") {
                                loginScreen(navController = navController)
                            }
                            composable("signupScreen") {
                                signupScreen(navController = navController)
                            }
                            composable("therapistScreen") {
                                therapistScreen()
                            }
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun ContentScreen(
    selectedIndex: Int,
    navController: NavController
) {
    when (selectedIndex) {
        0 -> goalScreen(
            modifier = Modifier.padding()
        )
        1 -> commandScreen(modifier = Modifier.padding())
        2 -> progressScreen(
            modifier = Modifier.padding(),
            counts = 3
        )
    }
}
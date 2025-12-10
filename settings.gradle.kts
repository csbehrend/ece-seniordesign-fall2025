//include(":ble-common")
//
//include(":examples:trivia")
//
//include(":test")
//
//include(":ble")
//
//include(":examples:ble-gatt-client")
//
//include(":examples:ble-gatt-server")
//
//include(":ble-livedata")
//
//include(":ble-ktx")
//
//pluginManagement {
//    repositories {
//        google {
//            content {
//                includeGroupByRegex("com\\.android.*")
//                includeGroupByRegex("com\\.google.*")
//                includeGroupByRegex("androidx.*")
//            }
//        }
//        mavenCentral()
//        gradlePluginPortal()
//        mavenLocal()
//        gradlePluginPortal()
//        maven(url = "https://jitpack.io")
//        maven { url = uri("https://developer.nordicsemi.com/.m2/repository") }
//    }
//}
//dependencyResolutionManagement {
//    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
//    repositories {
//        google()
//        mavenCentral()
//        maven(url = "https://jitpack.io")
//        maven { url = uri("https://developer.nordicsemi.com/.m2/repository") }
//    }
//}
//
////rootProject.name = "My Application"
//rootProject.name = "ece-seniordesign-fall2025"
//include(":GripAssist")
//

pluginManagement {
    repositories {
        google {
            content {
                includeGroupByRegex("com\\.android.*")
                includeGroupByRegex("com\\.google.*")
                includeGroupByRegex("androidx.*")
            }
        }
        mavenCentral()
        gradlePluginPortal()
        maven(url = "https://jitpack.io")
        maven { url = uri("https://developer.nordicsemi.com/.m2/repository") }
    }
}

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
        maven(url = "https://jitpack.io")
        maven { url = uri("https://developer.nordicsemi.com/.m2/repository") }
    }
}

rootProject.name = "ece-seniordesign-fall2025"
//include(":app:GripAssist")  // This is your actual app_folder module
//include(":scanner")
//include(":ble-common")
include(":GripAssist")
project(":GripAssist").projectDir = file("app/GripAssist")

include(":scanner")
project(":scanner").projectDir = file("app/scanner")
cmake_minimum_required(VERSION 3.16)

project(FinalGui_all VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_add_executable(appFinalGui_all
    main.cpp
)

qt_add_qml_module(appFinalGui_all
    URI FinalGui_all
    VERSION 1.0
    QML_FILES
    Main.qml
    QML_FILES GarageIntriorMap.qml
    QML_FILES DateTime.qml
    QML_FILES GarageSlotsList.qml
    QML_FILES MapToGarage.qml
    QML_FILES OnSlotParking.qml
    RESOURCES
        assets/CarTopView.jpg
        assets/CarTopViewRound1.png
        assets/newpic.jpg
        assets/reverseCameraGridlines.png
        assets/Left_Signal_Off.png
        assets/Left_arrow_On.png
        assets/Right_signal_Off.png
        assets/Right_arrow_On.png
        assets/Turn-signal_Off.png
        assets/turn-signal_On.png
        assets/ABS_off.png
        assets/ABS_on.png
        assets/battery_off.png
        assets/battery_on.png
        assets/cruise_off.png
        assets/cruise_on.png
        assets/engine_off.png
        assets/engine_on.png
        assets/lane_off.png
        assets/lane_on.png
        assets/oil_off.png
        assets/oil_on.png
        assets/parking.png
        assets/applications.png
        assets/settings.png
        assets/logo.png
        assets/car.jpg
        assets/car3.png
        assets/next-button.png
        assets/pause.png
        assets/play-button.png
        assets/previous.png
        assets/compass.png
        assets/youtube.png
        assets/chrome.png
        assets/music-note.png
        assets/netflix.png
        assets/spotify.png
        assets/home.png
        assets/download.svg
        assets/speedometer.png
        assets/background.jpg
        assets/cloudy.png
        assets/rain.png
        assets/sunny.png
        assets/thunderstorm.png
        assets/musicBG-modified.png
        assets/tempBG-modified.png

    QML_FILES VideoScreen.qml
    QML_FILES Applications.qml
    QML_FILES CustomSliderThreeLevels.qml
    QML_FILES CustomSliderTwoLevels.qml
    QML_FILES FullCircleGauge.qml
    QML_FILES GaugeScreen.qml
    QML_FILES HomeScreen.qml
    QML_FILES SettingsScreen.qml
    SOURCES filereading.h filereading.cpp
    QML_FILES AppState.qml
    QML_FILES MusicPlayer.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appFinalGui_all PROPERTIES
    #    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appFinalGui_all
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appFinalGui_all
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appFinalGui_all
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

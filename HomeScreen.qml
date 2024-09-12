import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import QtLocation
import QtPositioning
import QtMultimedia



Rectangle{
    id : homeScreen

    function setHomeColor(newColor) {
       homeScreen.color = newColor
    }

    //-----------Stack Help------------------
    // property int parkingHelp :1
    // property int  musicState: 1

    // MapToGarage{
    //     id:  mapToGarage
    //     onDataChanged: handleData(data)
    //     visible: false
    // }

    // function handleData(data) {
    //     parkingHelp=2
    // }


    //----------------------------------------------------
    property string homeBackgroundColor: "#00172D"
    property string rectangleColor: "grey"
    property string rectangleColorBorder: "#C0C0C0"
    property string textColor: "white"
    property real rectangleOpacity: 0.2
    property real textOpacity: 1
    property string rectColor:"#00172D"
    property string rectBorderColor: "#C0C0C0"

    //--------------Temperture Part------------//
    property string temperature: "Loading..." // Initial text while fetching data
    property string humidity: "Loading..." // Initial text while fetching data
    property string weatherCondition: "Loading..." // Property to hold the weather condition
    property string weatherIconSource: "" // Property to hold the image source
    property string fontColor: "white"




    property int currentIndex: 0

    property var playlist: [
        "file:///D:/filter/maryam.mp3",
        "file:///D:/filter/qatami.mp3",
        "file:///D:/filter/title.mp3",
        "file:///D:/filter/maryam.mp3",
        "file:///D:/filter/qatami.mp3",
        "file:///D:/filter/title.mp3",
        "file:///D:/filter/maryam.mp3",
        "file:///D:/filter/qatami.mp3",
        "file:///D:/filter/title.mp3"
    ]
    function getFileName(filePath) {
        var parts = filePath.split("/")
        return parts[parts.length - 1]
    }

    function nextTrack() {
        currentIndex = (currentIndex + 1) % playlist.length
        player.source = playlist[currentIndex]
        player.play()
    }

    function previousTrack() {
        currentIndex = (currentIndex - 1 + playlist.length) % playlist.length
        player.source = playlist[currentIndex]
        player.play()
    }
    visible: true
    width: 1024
    height: 600
    color: homeBackgroundColor

    //-------------------------------------------------Home Top Toolbar----------------------------------------------------//
    Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.leftMargin:60
        anchors.rightMargin:60
        color: rectangleColor
        opacity: rectangleOpacity
        height: parent.height * 1/12
        radius: 15
    }
    Rectangle {
        id: homeTopToolbar
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.leftMargin:60
        anchors.rightMargin:60
        color: "transparent"
        height: parent.height * 1/12
        radius: 15
        border.color: rectangleColorBorder
        border.width:2
        Row {
            anchors.left: parent.left
            anchors.leftMargin:10
            anchors.right: parent.right
            anchors.rightMargin:10
            anchors.verticalCenter:parent.verticalCenter
            spacing: 200

            // Date display
            DateTime{
                id: dateDisplay
                showDate: true
                showTime: false
                anchors.verticalCenter: parent.verticalCenter // Align center vertically

            }

            // Image in the center
            Image {
                source: "qrc:/FinalGui_all/assets/logo.png" // Replace with the path to your logo
                width: 100
                height: 100
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter // Align center vertically

            }

            // Time display
            DateTime{
                id: timeDisplay
                showDate: false
                showTime: true
                anchors.verticalCenter: parent.verticalCenter // Align center vertically

            }
        }
    }

    //-------------------------------------------------Home Map screen----------------------------------------------------//
    Rectangle {
        id: homeMapScreen
        anchors.top: homeTopToolbar.bottom
        anchors.right: parent.right
        height: homeScreen.height * 0.5
        width: homeScreen.width * 0.6
        anchors.margins: 15
        radius: 20
        color: rectangleColor
        opacity: rectangleOpacity
        border.width: 8
        border.color: rectangleColor // Modify the border color as needed
        z: 2
    }
    // Child Rectangle for map display
    Rectangle {
        anchors.fill: homeMapScreen
        color: "transparent"
        anchors.margins: 5
        radius: 20

        Plugin {
            id: mapPlugin
            name: "osm"
        }

        Map {
            anchors.fill: parent
            plugin: mapPlugin
            center: QtPositioning.coordinate(59.91, 10.75) // Oslo
            zoomLevel: 14
            property geoCoordinate startCentroid

            PinchHandler {
                id: pinch
                target: null
                onActiveChanged: if (active) {
                                     map.startCentroid = map.toCoordinate(pinch.centroid.position, false)
                                 }
                onScaleChanged: (delta) => {
                                    map.zoomLevel += Math.log2(delta)
                                    map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
                                }
                onRotationChanged: (delta) => {
                                       map.bearing -= delta
                                       map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
                                   }
                grabPermissions: PointerHandler.TakeOverForbidden
            }
            WheelHandler {
                id: wheel
                // workaround for QTBUG-87646 / QTBUG-112394 / QTBUG-112432:
                // Magic Mouse pretends to be a trackpad but doesn't work with PinchHandler
                // and we don't yet distinguish mice and trackpads on Wayland either
                acceptedDevices: Qt.platform.pluginName === "cocoa" || Qt.platform.pluginName === "wayland"
                                 ? PointerDevice.Mouse | PointerDevice.TouchPad
                                 : PointerDevice.Mouse
                rotationScale: 1/120
                property: "zoomLevel"
            }
            DragHandler {
                id: drag
                target: null
                onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
            }
            Shortcut {
                enabled: map.zoomLevel < map.maximumZoomLevel
                sequence: StandardKey.ZoomIn
                onActivated: map.zoomLevel = Math.round(map.zoomLevel + 1)
            }
            Shortcut {
                enabled: map.zoomLevel > map.minimumZoomLevel
                sequence: StandardKey.ZoomOut
                onActivated: map.zoomLevel = Math.round(map.zoomLevel - 1)
            }
        }
    }
    //-------------------------------------------------Home Music----------------------------------------------------//
    Rectangle {
        id: musicScreen
        anchors.top: homeMapScreen.bottom
        anchors.bottom: homeBottomToolBar.top
        anchors.right: temperatureScreen.left
        height: homeScreen.height * 0.25
        color: "transparent"
        width: homeScreen.width * 0.292
        anchors.margins: 15
        radius:20
        border.color: rectangleColorBorder
        border.width: 2
        Image {
            source: "qrc:/FinalGui_all/assets/musicBG-modified.png"
            anchors.fill: parent
            //fillMode: Image.PreserveAspectCrop
            z:-1
        }

        MusicPlayer
        {
            id:player
            anchors.fill: parent
            // if(musicState===1)
            // {
            //     player.play()
            // }

        }


        // property int currentIndex: 0

        // MediaPlayer {
        //     id: player
        //     source: playlist[currentIndex]
        //     audioOutput: audioOutput

        //     onPlaybackStateChanged: {
        //         if (playbackState === MediaPlayer.EndOfMedia) {
        //             nextTrack()
        //         }
        //     }
        // }

        // AudioOutput {
        //     id: audioOutput
        // }

        // Column {
        //     anchors.fill: parent
        //     spacing: 10
        //     anchors.margins: 20

        //     Text {
        //         id: trackName
        //         text: getFileName(playlist[currentIndex])
        //         font.pointSize: 18
        //         color: "white"
        //         horizontalAlignment: Text.AlignHCenter
        //         anchors.horizontalCenter: parent.horizontalCenter
        //     }

        //     Slider {
        //         id: progressSlider
        //         width: parent.width
        //         height: 8
        //         enabled: player.seekable
        //         value: player.duration > 0 ? player.position / player.duration : 0

        //         background: Rectangle {
        //             implicitHeight: 8
        //             color: "gray"
        //             radius: 3
        //         }

        //         Rectangle {
        //             width: progressSlider.visualPosition * progressSlider.width
        //             height: progressSlider.height
        //             color: "#1D8BF8"
        //             radius: 3
        //         }

        //         onMoved: {
        //             player.position = progressSlider.position * player.duration
        //         }
        //     }

        //     Row {
        //         anchors.horizontalCenter: parent.horizontalCenter
        //         spacing: 20

        //         Image {
        //             source: "qrc:/FinalGui_all/assets/previous.png" // Replace with your previous icon path
        //             width: 40
        //             height: 40
        //             fillMode: Image.PreserveAspectFit
        //             MouseArea {
        //                 anchors.fill: parent
        //                 onClicked: previousTrack()
        //             }
        //         }

        //         Image {
        //             source: player.playbackState === MediaPlayer.PlayingState ? "qrc:/FinalGui_all/assets/pause.png" : "qrc:/FinalGui_all/assets/play-button.png" // Replace with your play/pause paths
        //             width: 40
        //             height: 40
        //             fillMode: Image.PreserveAspectFit
        //             MouseArea {
        //                 anchors.fill: parent
        //                 onClicked: {
        //                     if (player.playbackState === MediaPlayer.PlayingState) {
        //                         player.pause()
        //                     } else {
        //                         player.play()
        //                     }
        //                 }
        //             }
        //         }

        //         Image {
        //             source: "qrc:/FinalGui_all/assets/next-button.png"
        //             width: 40
        //             height: 40
        //             fillMode: Image.PreserveAspectFit
        //             MouseArea {
        //                 anchors.fill: parent
        //                 onClicked: nextTrack()
        //             }
        //         }
        //     }
        // }



        // onCurrentIndexChanged: {
        //     trackName.text = getFileName(playlist[currentIndex])
        // }

        // Component.onCompleted: {
        //     player.play()
        //     trackName.text = getFileName(playlist[currentIndex])
        // }
    }


    //---------------------------------------------------Home Temperature-------------------------------------------------------//
    Rectangle {
        id: temperatureScreen
        anchors.top: homeMapScreen.bottom
        anchors.bottom: homeBottomToolBar.top
        anchors.right: parent.right
        height: homeScreen.height * 0.25
        width: homeScreen.width * 0.29
        anchors.margins: 15
        radius: 20
        color: "transparent"
        border.color: rectangleColorBorder
        border.width: 2

        Image {
            source: "qrc:/FinalGui_all/assets/tempBG-modified.png"
            anchors.fill: parent
            z:-1
        }

        Column {
            //anchors.centerIn: parent
            anchors.left:parent.left
            anchors.leftMargin:10
            anchors.top:parent.top
            anchors.topMargin:10
            spacing: 5
            Text {
                text: "Temperature in Cairo:"
                font.pointSize: 10
                color: fontColor
            }

            Row {
                spacing: 10
                Image {
                    id: weatherIcon
                    source: weatherIconSource
                    width: 50
                    height: 50
                    visible: weatherIconSource !== ""
                }

                Text {
                    id: temperatureText
                    text: temperature
                    font.pointSize: 24
                    color: fontColor
                }
            }
            Row {
                spacing: 5
                Text {
                    text: "Weather Condition:"
                    font.pointSize: 10
                    color: fontColor
                }

                Text {
                    id: weatherConditionText
                    text: weatherCondition
                    font.pointSize: 14
                    color: fontColor
                }
            }
            Row
            {

                spacing: 5

                Text {
                    text: "Humidity in Cairo:"
                    font.pointSize: 10
                    color: fontColor
                }

                Text {
                    id: humidityText
                    text: humidity
                    font.pointSize: 14
                    color: fontColor
                }
            }
        }
    }

    Component.onCompleted: {
        fetchTemperature();
    }

    function fetchTemperature() {
        var xhr = new XMLHttpRequest();
        var url = "https://api.openweathermap.org/data/2.5/weather?q=Cairo&appid=d43de76e6decddf8c569b584433d9bcf&units=metric";

        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                if (xhr.status === 200) {
                    var response = JSON.parse(xhr.responseText);
                    temperature = response.main.temp + " °C";
                    humidity = response.main.humidity + " %";
                    weatherCondition = response.weather[0].main;

                    switch (weatherCondition) {
                    case "Clear":
                        weatherIconSource = "qrc:/FinalGui_all/assets/sunny.png"; // Path to your sunny image
                        break;
                    case "Clouds":
                        weatherIconSource = "qrc:/FinalGui_all/assets/cloudy.png"; // Path to your cloudy image
                        break;
                    case "Rain":
                        weatherIconSource = "qrc:/FinalGui_all/assets/rain.png"; // Path to your rainy image
                        break;
                    case "Snow":
                        weatherIconSource = "qrc:/FinalGui_all/assets/snow.png"; // Path to your snowy image
                        break;
                    case "Thunderstorm":
                        weatherIconSource = "qrc:/FinalGui_all/assets/thunderstorm.png"; // Path to your thunderstorm image
                        break;
                        // default:
                        //     weatherIconSource = "qrc:/FinalGui_all/default.png"; // Path to a default image
                    }
                } else {
                    console.log("Error fetching data: " + xhr.status);
                    temperature = "Error fetching data";
                    humidity = "Error fetching data";
                    weatherCondition = "Error fetching data";
                    weatherIconSource = "";
                }
            }
        };

        xhr.open("GET", url);
        xhr.send();
    }

    //-------------------------------------------------Home left Screen--------------------------------------------------------//
    Rectangle {
        anchors.top: homeTopToolbar.bottom
        anchors.bottom: homeBottomToolBar.top
        anchors.left: parent.left
        width: parent.width * 1/3
        anchors.margins: 15
        radius:20
        color: rectangleColor
        opacity: rectangleOpacity
    }
    Rectangle {
        id: homeleftScreen
        anchors.top: homeTopToolbar.bottom
        anchors.bottom: homeBottomToolBar.top
        anchors.left: parent.left
        width: parent.width * 1/3
        anchors.margins: 15
        radius:20
        color: "transparent"
        border.color: rectangleColorBorder
        border.width:2
    }
    //-------------------------------------------------Home Top left Screen--------------------------------------------------------//

    Rectangle {
        id: topleftScreen
        anchors.bottom: middleleftScreen.top
        anchors.left: homeleftScreen.left
        anchors.right: homeleftScreen.right
        height: parent.height * 1/3.1
        anchors.margins: 15
        radius: 20
        color: "transparent"

        Image {
            anchors.fill: parent
            source: "qrc:/FinalGui_all/assets/car3.png"
            fillMode: Image.PreserveAspectFit
            smooth: true
        }
    }
    //-------------------------------------------------Home Bottom left Screen--------------------------------------------------------//


    Rectangle
    {
        id: bottomleftScreen
        anchors.bottom: homeleftScreen.bottom
        anchors.left: homeleftScreen.left
        anchors.right: homeleftScreen.right
        height: parent.height *1/4
        anchors.margins: 15
        radius:20
        color: "transparent"

        Column {
            anchors.centerIn: parent
            spacing: 20

            // First slider instance
            CustomSliderTwoLevels {
                id: frontLight
                sliderWidth: 250
                sliderHeight: 25
                sliderColor: "#00172D"
                handlerWidth:  sliderWidth/2
                handlerHeight:  sliderHeight
                handlerColor: "#C0C0C0"
            }

            // Second slider instance
            CustomSliderTwoLevels {
                id: backLight
                sliderWidth: 250
                sliderHeight: 25
                sliderColor: "#00172D"
                handlerWidth: sliderWidth/2 // Different handler width
                handlerHeight:  sliderHeight
                handlerColor: "#C0C0C0"
            }
        }

    }
    //-------------------------------------------------Home Middle left Screen--------------------------------------------------------//

    Rectangle{
        id: middleleftScreen
        anchors.bottom: bottomleftScreen.top
        anchors.right: homeleftScreen.right
        anchors.left: homeleftScreen.left
        height:bottomleftScreen.height *1/2
        anchors.margins:15
        color: "transparent"
        // Row {
        //     spacing:70
        //     anchors.horizontalCenter: parent.horizontalCenter
        //     anchors.verticalCenter: parent.verticalCenter

        //     Image {
        //         id: signalImage
        //         width: 40
        //         height: 40
        //         source: "qrc:/FinalGui_all/assets/Left_Signal_Off.png"

        //         property bool isSignalOn: false

        //         MouseArea {
        //             anchors.fill: parent
        //             onClicked: {
        //                 // homeScreen.changeColor();
        //                 signalImage.isSignalOn = !signalImage.isSignalOn // Toggle the state
        //                 signalImage.source = signalImage.isSignalOn ? "qrc:/FinalGui_all/assets/Left_arrow_On.png" : "qrc:/FinalGui_all/assets/Left_Signal_Off.png"
        //             }
        //         }
        //         NumberAnimation {
        //             id: flashAnimation
        //             target: signalImage
        //             property: "opacity"
        //             from: 1.0
        //             to: 0.0
        //             duration: 1000 // milliseconds
        //             running: signalImage.isSignalOn // Start animation only when isSignalOn is true
        //             loops: Animation.Infinite // Loop the animation indefinitely when isSignalOn is true
        //         }

        //         // Watch for changes in isSignalOn and trigger animation
        //         onIsSignalOnChanged: {
        //             if (isSignalOn) {
        //                 flashAnimation.running = true; // Start the flashing animation
        //             } else {
        //                 flashAnimation.running = false; // Stop the animation when isSignalOn is false
        //                 signalImage.opacity = 1.0; // Reset opacity
        //             }
        //         }
        //     }

        //     Image {
        //         id: signalImageMiddle
        //         width: 100
        //         height: 100
        //         source: signalImageMiddle.isSignalOn ? "qrc:/FinalGui_all/assets/turn-signal_On.png" : "qrc:/FinalGui_all/assets/Turn-signal_Off.png"

        //         property bool isSignalOn: false

        //         MouseArea {
        //             anchors.fill: parent
        //             onClicked: {
        //                 // Toggle the signal state
        //                 signalImageMiddle.isSignalOn = !signalImageMiddle.isSignalOn;
        //             }
        //         }

        //         // Animation to flash the image when isSignalOn is true
        //         NumberAnimation {
        //             id: flashAnimationMiddle
        //             target:signalImageMiddle
        //             property: "opacity"
        //             from: 1.0
        //             to: 0.0
        //             duration: 1000 // milliseconds
        //             running: signalImageMiddle.isSignalOn // Start animation only when isSignalOn is true
        //             loops: Animation.Infinite // Loop the animation indefinitely when isSignalOn is true
        //         }

        //         // Watch for changes in isSignalOn and trigger animation
        //         onIsSignalOnChanged: {
        //             if (isSignalOn) {
        //                 flashAnimationMiddle.running = true; // Start the flashing animation
        //             } else {
        //                 flashAnimationMiddle.running = false; // Stop the animation when isSignalOn is false
        //                 signalImageMiddle.opacity = 1.0; // Reset opacity
        //             }
        //         }
        //     }
        //     Image {
        //         id: signalImageRight
        //         width: 40
        //         height: 40
        //         source: signalImageRight.isSignalOn ? "qrc:/FinalGui_all/assets/Right_arrow_On.png" : "qrc:/FinalGui_all/assets/Right_signal_Off.png"

        //         property bool isSignalOn: false

        //         MouseArea {
        //             anchors.fill: parent
        //             onClicked: {
        //                 // Toggle the signal state
        //                 signalImageRight.isSignalOn = !signalImageRight.isSignalOn;
        //             }
        //         }

        //         NumberAnimation {
        //             id: flashAnimationRight
        //             target: signalImageRight
        //             property: "opacity"
        //             from: 1.0
        //             to: 0.0
        //             duration: 1000 // milliseconds
        //             running: signalImageRight.isSignalOn
        //             loops: Animation.Infinite
        //         }
        //         onIsSignalOnChanged: {
        //             if (isSignalOn) {
        //                 flashAnimationRight.running = true;
        //             } else {
        //                 flashAnimationRight.running = false;
        //                 signalImageRight.opacity = 1.0;
        //             }
        //         }
        //     }
        // }

    }



    //-------------------------------------------------Home bottom ToolBar--------------------------------------------------------//

    Popup {
        id: applicationsPopup
        width: parent.width * 0.8
        height: parent.height * 0.8
        modal: true
        closePolicy: Popup.CloseOnPressOutside
        anchors.centerIn: parent

        Item {
            width: parent.width
            height: parent.height

            // Loader to load the applications QML
            Loader {
                anchors.fill: parent
                source: "qrc:/FinalGui_all/Applications.qml"
            }

            // Close button
            Button {
                text: "Close"
                width: 100
                height: 40
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 20
                onClicked: applicationsPopup.close()
            }
        }

        onClosed: {
            applications.color = "transparent"
            applications.border.color = "transparent"
            home.color = rectColor
            home.border.color = rectBorderColor
            parkingRect.color = "transparent"
            parkingRect.border.color = "transparent"
            speedometerRect.color = "transparent"
            speedometerRect.border.color = "transparent"
            settingsRect.color = "transparent"
            settingsRect.border.color = "transparent"
        }
    }

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        z: 1
        color: rectangleColor
        height: parent.height * 1 / 12
        width: parent.width * 1 / 3
        radius: 20
        opacity: rectangleOpacity
    }


    Rectangle {
        id: homeBottomToolBar
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        z: 1
        color: "transparent"
        height: parent.height * 1 / 12
        width: parent.width * 1 / 3
        radius: 20
        border.color: rectangleColorBorder
        border.width: 2

        Row {
            spacing: 30
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            // Home with border
            Rectangle {
                id: home
                width: 35
                height: 35
                radius: 20
                color: rectColor
                border.color: rectBorderColor
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 20
                    height: 20
                    anchors.centerIn: parent
                    source: "qrc:/FinalGui_all/assets/home.png"
                }
            }

            // Parking
            Rectangle {
                id: parkingRect
                width: 35
                height: 35
                radius: 20
                color: "transparent"
                border.color: "transparent"
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 20
                    height: 20
                    id: parking
                    source: "qrc:/FinalGui_all/assets/parking.png"
                    anchors.centerIn: parent
                    MouseArea {
                        anchors.fill: parent


                        onClicked: {
                            console.log(parkingHelp)
                            if(parkingHelp===1)
                            {
                                stackLayout.currentIndex=3
                                stackLayout2.currentIndex=0
                            }
                            else if (parkingHelp===2)
                            {
                                stackLayout.currentIndex=3
                                stackLayout2.currentIndex=1
                            }
                            else if (parkingHelp===3)
                            {
                                stackLayout.currentIndex=3
                                stackLayout2.currentIndex=2
                            }
                            else if (parkingHelp===4)
                            {
                                stackLayout.currentIndex=3
                                stackLayout2.currentIndex=3
                            }
                            else
                            {

                            }

                        }
                    }
                }
            }

            // Speedometer
            Rectangle {
                id: speedometerRect
                width: 35
                height: 35
                radius: 20
                color: "transparent"
                border.color: "transparent"
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 20
                    height: 20
                    id: speedometer
                    source: "qrc:/FinalGui_all/assets/speedometer.png"
                    anchors.centerIn: parent
                    MouseArea {
                        anchors.fill: parent
                        onClicked:
                        {
                            // stackView.pop();  // Pop the current item and store a reference to it
                            // stackView.push(gaugeScreen,{reverseState:2,firstTime:2});
                            stackLayout.currentIndex=1
                            swipeView.currentIndex=0
                        }
                    }
                }
            }

            // Settings
            Rectangle {
                id: settingsRect
                width: 35
                height: 35
                radius: 20
                color: "transparent"
                border.color: "transparent"
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 20
                    height: 20
                    id: settings
                    source: "qrc:/FinalGui_all/assets/settings.png"
                    anchors.centerIn: parent
                    MouseArea {
                        anchors.fill: parent
                        onClicked:
                        {
                            stackLayout.currentIndex=2

                        }
                    }
                }
            }

            // Applications
            Rectangle {
                id: applications
                width: 35
                height: 35
                radius: 20
                color: "transparent"
                border.color: "transparent"
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 20
                    height: 20
                    source: "qrc:/FinalGui_all/assets/applications.png"
                    anchors.centerIn: parent
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            applicationsPopup.open()
                            applications.color = rectColor
                            applications.border.color = rectBorderColor
                            home.color = "transparent"
                            home.border.color = "transparent"
                            parkingRect.color = "transparent"
                            parkingRect.border.color = "transparent"
                            speedometerRect.color = "transparent"
                            speedometerRect.border.color = "transparent"
                            settingsRect.color = "transparent"
                            settingsRect.border.color = "transparent"
                        }
                    }
                }
            }
        }
    }

    //------------------------------------------------Right Home Bottom ToolBar--------------------------------------------------------//

    Rectangle{
        id:righthomeBottomToolBar
        height: parent.height * 1/12
        anchors.top: temperatureScreen.bottom
        anchors.left: homeBottomToolBar.right
        anchors.right: parent.right
        anchors.margins:20
        color: "transparent"
        Row {
            spacing: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id: signalImageABS
                width: 35
                height: 35
                source: signalImageABS.isSignalOn_ABS ? "qrc:/FinalGui_all/assets/ABS_on.png" : "qrc:/FinalGui_all/assets/ABS_off.png"
                property bool isSignalOn_ABS: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalImageABS.isSignalOn_ABS = !signalImageABS.isSignalOn_ABS;
                    }
                }
            }

            Image {
                id: signalImagelane
                width: 35
                height: 35

                source: signalImagelane.isSignalOn_lane ? "qrc:/FinalGui_all/assets/lane_on.png" : "qrc:/FinalGui_all/assets/lane_off.png"
                property bool isSignalOn_lane: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalImagelane.isSignalOn_lane = !signalImagelane.isSignalOn_lane;
                    }
                }
            }

            Image {
                id: signalImagecurise
                width: 35
                height: 35

                source: signalImagecurise.isSignalOn_curise ? "qrc:/FinalGui_all/assets/cruise_on.png" : "qrc:/FinalGui_all/assets/cruise_off.png"
                property bool isSignalOn_curise: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalImagecurise.isSignalOn_curise = !signalImagecurise.isSignalOn_curise;
                    }
                }
            }
        }
    }
    //------------------------------------------------Left Home Bottom ToolBar--------------------------------------------------------//

    Rectangle{
        id:lefthomeBottomToolBar
        anchors.top: homeleftScreen.bottom
        anchors.right: homeBottomToolBar.left
        anchors.left: parent.left
        anchors.margins: 20
        height: parent.height * 1/12
        color: "transparent"
        Row {
            spacing: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter


            Image {
                id: signalOil
                width: 35
                height: 35
                source: signalOil.isSignalOn_Oil ? "qrc:/FinalGui_all/assets/oil_on.png" : "qrc:/FinalGui_all/assets/oil_off.png"
                property bool isSignalOn_Oil: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalOil.isSignalOn_Oil = !signalOil.isSignalOn_Oil;
                    }
                }
            }

            Image {
                id: signalEngine
                width: 35
                height: 35
                source: signalEngine.isSignalOn_engine ? "qrc:/FinalGui_all/assets/engine_on.png" : "qrc:/FinalGui_all/assets/engine_off.png"
                property bool isSignalOn_engine: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalEngine.isSignalOn_engine = !signalEngine.isSignalOn_engine;
                    }
                }
            }

            Image {
                id: signalcharging
                width: 35
                height: 35
                source: signalcharging.isSignalOn_charging ? "qrc:/FinalGui_all/assets/battery_on.png" : "qrc:/FinalGui_all/assets/battery_off.png"
                property bool isSignalOn_charging: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalcharging.isSignalOn_charging = !signalcharging.isSignalOn_charging;
                    }
                }
            }
        }
    }
}

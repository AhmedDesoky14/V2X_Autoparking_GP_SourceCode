import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id:gaugeScreen
    visible: true
    width: 1024
    height: 600

    function setGaugeColor(newColor) {
       gaugeScreen.color = newColor
    }
    property string homeBackgroundColor: "#00172D"
    property string rectangleColor: "grey"
    property string rectangleColorBorder: "#C0C0C0"
    property string textColor: "white"
    property real rectangleOpacity: 0.2
    property real textOpacity: 1
    property string rectColor:"#00172D"
    property string rectBorderColor: "#C0C0C0"


    SequentialAnimation {
        id: animationLeftGaugeIntial
        running: stackLayout.currentIndex === 1 && previousScreenIndex === 0 // Run only when transitioning from video
        NumberAnimation {
            id: animationLeftGaugeStartToEnd
            targets: [leftGauge, rightGauge]
            property: "animatedValue"
            duration: 500
            from: 0
            to: 100
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            id: animationLeftGaugeEndToStart
            targets: [leftGauge, rightGauge]
            property: "animatedValue"
            duration: 500
            from: 100
            to: 0
            easing.type: Easing.InOutQuad
        }
        onStopped: {
            //previousScreenIndex=1;
            leftGauge.startGaugeReadTimer();
            rightGauge.startGaugeReadTimer();
        }
    }

    Connections {
        target: animationLeftGaugeIntial
        onRunningChanged: {
            if (firstTime!== 1) {
                animationLeftGaugeIntial.stop();
            }
        }
    }

    SequentialAnimation{
        id :animationBatteryIndicatorIntial
        running: stackLayout.currentIndex === 1 && previousScreenIndex === 0 // Run only when transitioning from video
        NumberAnimation {
            id : animationBatteryIndicatorStartToEnd
            target: batteryIndicator
            property: "animatedValue"
            duration: 1000
            from :0
            to : 10
            easing.type: Easing.InOutQuad
            //running:(reverseState===1)
        }

        NumberAnimation {
            id : animationBatteryIndicatorEndToStart
            targets: batteryIndicator
            property: "animatedValue"
            duration: 1000
            from :10
            to : 0
            easing.type: Easing.InOutQuad
            //running:reverseState===1
        }
        onStopped: {
            // Start the gaugeReadTimer after animationBatteryIndicatorIntial completes
            //previousScreenIndex=1;
            batteryIndicator.startGaugeReadTimer();
        }
    }

    Connections {
        target: animationBatteryIndicatorIntial
        onRunningChanged: {
            if (firstTime !== 1) {
                animationBatteryIndicatorIntial.stop();
            }
        }
    }

    // NumberAnimation {
    //     id : movingCursorAnimationLeftGauge
    //     target: leftGauge
    //     property: "animatedValue"
    //     duration: 1000
    //     from :0
    //     to : 100
    //     easing.type: Easing.InOutQuad
    //     running:reverseState===1

    // }

    // NumberAnimation {
    //     id : animationLeftGaugeToNewValue
    //     targets: leftGauge
    //     property: "animatedValue"
    //     duration: 1000
    //     from :0
    //     to : 100
    //     easing.type: Easing.InOutQuad
    //     running:reverseState===1

    // }

    Component.onCompleted:
    {
        animationLeftGaugeIntial.start();
        animationBatteryIndicatorIntial.start();

    }



    Rectangle {
        id:gaugeScreenBack
        width: parent.width
        height: parent.height
        color:"#00172D"



        Image {
            width: parent.width
            height: parent.height
            anchors.centerIn: parent
            source: "assets/download.svg"
            //fillMode: Image.PreserveAspectCrop
        }
        Rectangle {
            id: topToolBar
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin:20
            height: parent.height / 12
            width: parent.width * 2 / 3
            color: "transparent"
            radius: 20
            Row {
                anchors.centerIn: parent
                spacing: 75

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

        FullCircleGauge {
            id:leftGauge
            gaugeWidth: 400
            gaugeHeight: 400
            color: "transparent"
            minValue: 0
            maxValue: 100
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 50
            currentValue:0
            filePathGauge:"D:\\QT projects\\finalGui_1\\readFiles\\fileTest.txt"

        }

        FullCircleGauge {
            id:rightGauge
            color: "transparent"
            gaugeWidth: 400
            gaugeHeight: 400
            minValue: 0
            maxValue: 100
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 50
            currentValue: 30
            filePathGauge:"D:\\QT projects\\finalGui_1\\readFiles\\fileTest2.txt"

        }

        Row {
            width: parent.width / 3
            height: parent.height / 4
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.margins: 100
            spacing: 70

            Image {
                id: signalImage
                width: 50
                height: 50
                source: "qrc:/FinalGui_all/assets/Left_Signal_Off.png"
                property bool isSignalOn: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalImage.isSignalOn = !signalImage.isSignalOn // Toggle the state
                        signalImage.source = signalImage.isSignalOn ? "qrc:/FinalGui_all/assets/Left_arrow_On.png" : "qrc:/FinalGui_all/assets/Left_Signal_Off.png"
                    }
                }

                NumberAnimation {
                    id: flashAnimation
                    target: signalImage
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                    duration: 1000 // milliseconds
                    running: signalImage.isSignalOn // Start animation only when isSignalOn is true
                    loops: Animation.Infinite // Loop the animation indefinitely when isSignalOn is true
                }

                // Watch for changes in isSignalOn and trigger animation
                onIsSignalOnChanged: {
                    if (isSignalOn) {
                        flashAnimation.running = true; // Start the flashing animation
                    } else {
                        flashAnimation.running = false; // Stop the animation when isSignalOn is false
                        signalImage.opacity = 1.0; // Reset opacity
                    }
                }
            }

            Image {
                id: signalImageMiddle
                width: 100
                height: 100
                source: signalImageMiddle.isSignalOn ? "qrc:/FinalGui_all/assets/turn-signal_On.png" : "qrc:/FinalGui_all/assets/Turn-signal_Off.png"
                property bool isSignalOn: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalImageMiddle.isSignalOn = !signalImageMiddle.isSignalOn; // Toggle the state
                    }
                }

                NumberAnimation {
                    id: flashAnimationMiddle
                    target: signalImageMiddle
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                    duration: 1000 // milliseconds
                    running: signalImageMiddle.isSignalOn // Start animation only when isSignalOn is true
                    loops: Animation.Infinite // Loop the animation indefinitely when isSignalOn is true
                }

                // Watch for changes in isSignalOn and trigger animation
                onIsSignalOnChanged: {
                    if (isSignalOn) {
                        flashAnimationMiddle.running = true; // Start the flashing animation
                    } else {
                        flashAnimationMiddle.running = false; // Stop the animation when isSignalOn is false
                        signalImageMiddle.opacity = 1.0; // Reset opacity
                    }
                }
            }

            Image {
                id: signalImageRight
                width: 50
                height: 50
                source: signalImageRight.isSignalOn ? "qrc:/FinalGui_all/assets/Right_arrow_On.png" : "qrc:/FinalGui_all/assets/Right_signal_Off.png"
                property bool isSignalOn: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        signalImageRight.isSignalOn = !signalImageRight.isSignalOn; // Toggle the state
                    }
                }

                NumberAnimation {
                    id: flashAnimationRight
                    target: signalImageRight
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                    duration: 1000 // milliseconds
                    running: signalImageRight.isSignalOn // Start animation only when isSignalOn is true
                    loops: Animation.Infinite // Loop the animation indefinitely when isSignalOn is true
                }

                onIsSignalOnChanged: {
                    if (isSignalOn) {
                        flashAnimationRight.running = true; // Start the flashing animation
                    } else {
                        flashAnimationRight.running = false; // Stop the animation when isSignalOn is false
                        signalImageRight.opacity = 1.0; // Reset opacity
                    }
                }
            }
        }
    }

    FullCircleGauge {
        id: batteryIndicator
        width: 200 // Adjust as needed
        height: 200
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: 30

        color: "transparent"
        minValue: 0
        maxValue: 10
        spanAngle: 180
        angleOffest: 180
        numofTicks: 3
        filePathGauge:"D:\\QT projects\\finalGui_1\\readFiles\\fileTest3.txt"

    }

    //------------------------Bottom ToolBar-------------------------------------//
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
        anchors.bottomMargin:20
        z: 1
        color: rectangleColor
        height: parent.height * 1 / 12
        width: parent.width * 1 / 3
        radius: 20
        opacity: rectangleOpacity
    }


    Rectangle {
        id: bottomToolBar
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin:20
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
                color: "transparent"
                border.color: "transparent"
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 20
                    height: 20
                    anchors.centerIn: parent
                    source: "qrc:/FinalGui_all/assets/home.png"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            stackLayout.currentIndex=1
                            swipeView.currentIndex=1


                        }
                    }
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
                            //console.log(parkingHelp)
                            if(parkingHelp_Gauge===1)
                            {
                                stackLayout.currentIndex=3
                                stackLayout2.currentIndex=0
                            }
                            else if (parkingHelp_Gauge===2)
                            {
                                stackLayout.currentIndex=3
                                stackLayout2.currentIndex=1
                            }
                            else if (parkingHelp_Gauge===3)
                            {
                                stackLayout.currentIndex=3
                                stackLayout2.currentIndex=2
                            }
                            else if (parkingHelp_Gauge===4)
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
                color: rectColor
                border.color: rectBorderColor
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 20
                    height: 20
                    id: speedometer
                    source: "qrc:/FinalGui_all/assets/speedometer.png"
                    anchors.centerIn: parent
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
}


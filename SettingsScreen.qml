import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import QtMultimedia

Rectangle {

    id:settingsScreen

    width: 1024
    height: 600
    visible: true
    color: backgroundColor

    //--------------------------------
    property string backgroundColorLight:"#90e0ef"


    //-------------------------------
    property real rectangleOpacity: 0.2
    property real textOpacity: 1
    property string rectangleColor: "grey"
    property string textColor: "white"
    property string backgroundColor:"#00172D"
    property string buttonColor: "gray"
    property string fontButtonColor: "white"
    property string fontButtonType: "Helvetica"
    property int  fontButtonSize: 24
    property string backgroundColorRight: "black"
    property string buttonColorRight: "gray"
    property string fontButtonColorRight: "black"
    property string fontButtonTypeRight: "Helvetica"
    property int  fontButtonSizeRight: 24
    property string rectangleColorBorder: "#C0C0C0"
    property string rectColor:"#00172D"
    property string rectBorderColor: "#C0C0C0"
    property string sliderColor:"#00172D"
    property string handlerColor: "#C0C0C0"


    //-----------------------------top ToolBar--------------------------------------//
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
        id: topToolbar
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


    //----------------------------left Screen -------------------------------------//
    Rectangle {
        id: leftScreen
        width: parent.width * 1 / 3
        anchors.top: topToolbar.bottom
        anchors.bottom: bottomToolBar.top
        anchors.left: parent.left
        anchors.margins: 25
        color: rectangleColor
        opacity: rectangleOpacity
        radius: 20
    }
    Rectangle {
        width: parent.width * 1 / 3
        anchors.top: topToolbar.bottom
        anchors.bottom: bottomToolBar.top
        anchors.left: parent.left
        anchors.margins: 25
        color: "transparent"
        radius: 20

        Column {
            id: buttonColumn
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 20
            spacing: 10 // space between elements

            property string selectedText: "Display" // Set the default selected text

            Repeater {
                id: buttonRepeater
                model: ListModel {
                    ListElement { text: "Display" }
                    ListElement { text: "Sounds" }
                    ListElement { text: "Control" }
                    ListElement { text: "Info" }
                    // Add more elements as needed
                }

                delegate: Item {
                    width: buttonColumn.width
                    height: 50 // Adjust this height as needed

                    Text {
                        id: buttonLabel
                        text: model.text
                        font.family: fontButtonType
                        font.pointSize: fontButtonSize
                        color: fontButtonColor
                        font.bold: buttonColumn.selectedText === model.text
                        anchors.centerIn: parent

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                buttonColumn.selectedText = model.text
                                switch (model.text) {
                                case "Display":
                                    displayRectangle.visible = true
                                    soundsRectangle.visible = false
                                    controlRectangle.visible = false
                                    infoRectangle.visible = false
                                    break
                                case "Sounds":
                                    displayRectangle.visible = false
                                    soundsRectangle.visible = true
                                    controlRectangle.visible = false
                                    infoRectangle.visible = false
                                    break
                                case "Control":
                                    displayRectangle.visible = false
                                    soundsRectangle.visible = false
                                    controlRectangle.visible = true
                                    infoRectangle.visible = false
                                    break
                                case "Info":
                                    displayRectangle.visible = false
                                    soundsRectangle.visible = false
                                    controlRectangle.visible = false
                                    infoRectangle.visible = true
                                    break
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Ensure the correct rectangle is visible by default
    Component.onCompleted: {
        displayRectangle.visible = true
        soundsRectangle.visible = false
        controlRectangle.visible = false
        infoRectangle.visible = false
    }

    /*---------------------------------------------Display--------------------------------------------------------------------------*/
    Rectangle {
        //id: displayRectangle
        width: parent.width * 2/3
        anchors.top:topToolbar.bottom
        anchors.bottom:bottomToolBar.top
        color: rectangleColor
        opacity: rectangleOpacity
        visible: true
        anchors.right: parent.right
        anchors.left: leftScreen.right
        anchors.margins:25
        radius:20
        z:1
    }
    Rectangle {
        id: displayRectangle
        width: parent.width * 2/3
        anchors.top:topToolbar.bottom
        anchors.bottom:bottomToolBar.top
        color: "transparent"
        visible: true
        anchors.right: parent.right
        anchors.left: leftScreen.right
        anchors.margins:25
        radius:20
        z:1

        /*----Display Mode----*/
        Text {
            id:displayModeText
            text: qsTr("Display Mode")
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 20
            font.family: fontButtonType
            font.pointSize: fontButtonSize
            color: "white"
        }

        CustomSliderTwoLevels{
            id:displayMode
            anchors.top:displayModeText.bottom
            anchors.left:displayRectangle.left
            anchors.margins:20
            sliderColor: sliderColor
            handlerColor: handlerColor
            onFirstPosition: {
                settingsScreen.color = backgroundColor
                setHomeColor(backgroundColor)
                setgarageSlotsListColor(backgroundColor)
                setgarageIntriorMapColor(backgroundColor)
                setOnSlotParkingColor(backgroundColor)
                setGaugeColor(backgroundColor)


            }
            onSecondPosition: {
                settingsScreen.color = backgroundColorLight
                setHomeColor(backgroundColorLight)
                setgarageSlotsListColor(backgroundColorLight)
                setgarageIntriorMapColor(backgroundColorLight)
                 setOnSlotParkingColor(backgroundColorLight)
                setGaugeColor(backgroundColorLight)
            }
        }
        Text {
            text:"Dark"
            font.pixelSize: 24
            color: "white"
            anchors {
                left: displayMode.left
                verticalCenter: displayMode.verticalCenter
                leftMargin: 10
            }
        }

        Text {
            text:"Light"
            font.pixelSize: 24
            color: "white"
            anchors {
                right: displayMode.right
                verticalCenter: displayMode.verticalCenter
                rightMargin:10
            }
        }


    }
    /*-----------------------------------------------Sounds--------------------------------------------------------------------------*/
    Rectangle {

        width: parent.width * 2/3
        anchors.top:topToolbar.bottom
        anchors.bottom:bottomToolBar.top
        color: rectangleColor
        opacity: rectangleOpacity
        visible: false
        anchors.right: parent.right
        anchors.left: leftScreen.right
        anchors.margins:25
        radius:20
        z:2
    }
    Rectangle {
        id: soundsRectangle
        width: parent.width * 2/3
        anchors.top:topToolbar.bottom
        anchors.bottom:bottomToolBar.top
        visible: false
        anchors.right: parent.right
        anchors.left: leftScreen.right
        anchors.margins:25
        radius:20
        z:2
        color: "transparent"
        /*---- Indictor sounds----*/
        Text {
            id:indictorSoundsText
            text: qsTr("indictor Sound")
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 20
            font.family: fontButtonType
            font.pointSize: fontButtonSize
            color: "white"
        }

        CustomSliderTwoLevels{
            id:indictorSound
            anchors.top:indictorSoundsText.bottom
            anchors.left:soundsRectangle.left
            anchors.margins:20
            sliderColor: sliderColor
            handlerColor: handlerColor
        }
        Text {
            text: "ON"
            font.pixelSize: 24
            color: "white"
            anchors {
                left: indictorSound.left
                verticalCenter: indictorSound.verticalCenter
                leftMargin: 10
            }
        }

        Text {
            text: "OFF"
            font.pixelSize: 24
            color: "white"
            anchors {
                right: indictorSound.right
                verticalCenter: indictorSound.verticalCenter
                rightMargin:10
            }
        }
        /*----Parking Sounds----*/
        Text {
            id:parkingSoundsText
            text: qsTr("Parking Sounds")
            anchors.top: indictorSound.bottom
            anchors.left: parent.left
            anchors.margins: 20
            font.family: fontButtonType
            font.pointSize: fontButtonSize
            color: "white"
        }

        CustomSliderTwoLevels{
            id:parkingSounds
            anchors.top:parkingSoundsText.bottom
            anchors.left:soundsRectangle.left
            anchors.margins:20
            sliderColor: sliderColor
            handlerColor: handlerColor
        }
        Text {
            text: "ON"
            font.pixelSize: 24
            color: "white"
            anchors {
                left: parkingSounds.left
                verticalCenter:parkingSounds.verticalCenter
                leftMargin: 10
            }
        }

        Text {
            text: "OFF"
            font.pixelSize: 24
            color: "white"
            anchors {
                right: parkingSounds.right
                verticalCenter: parkingSounds.verticalCenter
                rightMargin:10
            }
        }

    }
    /*-----------------------------------------------Control--------------------------------------------------------------------------*/
    Rectangle {

        width: parent.width * 2 / 3
        anchors.top:topToolbar.bottom
        anchors.bottom:bottomToolBar.top
        color: rectangleColor
        opacity: rectangleOpacity
        visible: false
        anchors.right: parent.right
        anchors.left: leftScreen.right
        anchors.margins: 25
        radius: 20
        z: 2
    }

    Rectangle {
        id: controlRectangle
        width: parent.width * 2 / 3
        anchors.top:topToolbar.bottom
        anchors.bottom:bottomToolBar.top
        color: "transparent"
        visible: false
        anchors.right: parent.right
        anchors.left: leftScreen.right
        anchors.margins: 25
        radius: 20
        z: 2

        /*----Temp Mode----*/
        Text {
            id:tempModeText
            text: qsTr("Temp Mode")
            anchors.top: controlRectangle.top
            anchors.left: parent.left
            anchors.margins: 20
            font.family: fontButtonType
            font.pointSize: fontButtonSize
            color: "white"
        }

        CustomSliderTwoLevels{
            id:tempMode
            anchors.top:tempModeText.bottom
            anchors.left:controlRectangle.left
            anchors.margins:20
            sliderColor: sliderColor
            handlerColor: handlerColor
        }
        Text {
            text: "C"
            font.pixelSize: 24
            color: "white"
            anchors {
                left: tempMode.left
                verticalCenter:tempMode.verticalCenter
                leftMargin: 10
            }
        }

        Text {
            text: "F"
            font.pixelSize: 24
            color: "white"
            anchors {
                right: tempMode.right
                verticalCenter: tempMode.verticalCenter
                rightMargin:10
            }
        }
        /*----Distance Mode----*/
        Text {
            id:distanceModeText
            text: qsTr("Distance Mode")
            anchors.top: tempMode.bottom
            anchors.left: parent.left
            anchors.margins: 20
            font.family: fontButtonType
            font.pointSize: fontButtonSize
            color: "white"
        }

        CustomSliderTwoLevels{
            id:distanceMode
            anchors.top:distanceModeText.bottom
            anchors.left:controlRectangle.left
            anchors.margins:20
            sliderColor: sliderColor
            handlerColor: handlerColor
        }
        Text {
            text: "Km"
            font.pixelSize: 24
            color: "white"
            anchors {
                left: distanceMode.left
                verticalCenter: distanceMode.verticalCenter
                leftMargin: 10
            }
        }

        Text {
            text: "m"
            font.pixelSize: 24
            color: "white"
            anchors {
                right: distanceMode.right
                verticalCenter: distanceMode.verticalCenter
                rightMargin:10
            }
        }

    }
    /*-----------------------------------------------info--------------------------------------------------------------------------*/
    Rectangle {
        width: parent.width * 2 / 3
        anchors.top:topToolbar.bottom
        anchors.bottom:bottomToolBar.top
        color: rectangleColor
        opacity: rectangleOpacity
        visible: false
        anchors.right: parent.right
        anchors.left: leftScreen.right
        anchors.margins: 25
        radius: 20
    }
    Rectangle {
        id: infoRectangle
        width: parent.width * 2 / 3
        anchors.top:topToolbar.bottom
        anchors.bottom:bottomToolBar.top
        color: "transparent"
        visible: false
        anchors.right: parent.right
        anchors.left: leftScreen.right
        anchors.margins: 25
        radius: 20

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 5

            Text {
                text: qsTr("Car Information: Audi")
                font.family: fontButtonType
                font.pointSize: fontButtonSize + 4
                color: fontButtonColor
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Row {
                spacing: 10
                Text {
                    text: qsTr("Model:")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: fontButtonColor
                }
                Text {
                    text: qsTr("Audi A4")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: "white"
                }
            }

            Row {
                spacing: 10
                Text {
                    text: qsTr("Year:")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: fontButtonColor
                }
                Text {
                    text: qsTr("2024")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: "white"
                }
            }

            Row {
                spacing: 10
                Text {
                    text: qsTr("Engine Type:")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: fontButtonColor
                }
                Text {
                    text: qsTr("2.0L Turbocharged I4")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: "white"
                }
            }

            Row {
                spacing: 10
                Text {
                    text: qsTr("Horsepower:")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: fontButtonColor
                }
                Text {
                    text: qsTr("261 hp")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: "white"
                }
            }

            Row {
                spacing: 10
                Text {
                    text: qsTr("Torque:")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: fontButtonColor
                }
                Text {
                    text: qsTr("273 lb-ft")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: "white"
                }
            }

            Row {
                spacing: 10
                Text {
                    text: qsTr("0-60 mph:")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: fontButtonColor
                }
                Text {
                    text: qsTr("5.2 seconds")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: "white"
                }
            }

            Row {
                spacing: 10
                Text {
                    text: qsTr("Top Speed:")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: fontButtonColor
                }
                Text {
                    text: qsTr("130 mph")
                    font.family: fontButtonType
                    font.pointSize: fontButtonSize
                    color: "white"
                }
            }
        }
    }

    /*-----------------------------------------------bottom ToolBar--------------------------------------------------------------------------*/

    Popup{
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
            home.color = "transparent"
            home.border.color = "transparent"
            parkingRect.color = "transparent"
            parkingRect.border.color = "transparent"
            speedometerRect.color = "transparent"
            speedometerRect.border.color = "transparent"
            settingsRect.color = rectColor
            settingsRect.border.color = rectBorderColor
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
        id: bottomToolBar
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
                            // stackView.pop();
                            // stackView.push(homeScreen);
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
                            onClicked: {
                                if(parkingHelp_Setting===1)
                                {
                                    stackLayout.currentIndex=3
                                    stackLayout2.currentIndex=0
                                }
                                else if (parkingHelp_Setting===2)
                                {
                                    stackLayout.currentIndex=3
                                    stackLayout2.currentIndex=1
                                }
                                else if (parkingHelp_Setting===3)
                                {
                                    stackLayout.currentIndex=3
                                    stackLayout2.currentIndex=2
                                }
                                else if (parkingHelp_Setting==4)
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
                        onClicked: {
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
                color: rectColor
                border.color: rectBorderColor
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 20
                    height: 20
                    id: settings
                    source: "qrc:/FinalGui_all/assets/settings.png"
                    anchors.centerIn: parent
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

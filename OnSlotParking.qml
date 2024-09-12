import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtMultimedia
import Qt3D.Extras
import ReadFile 1.0


Rectangle {
    id:onSlotParking
    width: 1024
    height: 600
    visible: true



    //----------set Color-----------------------

    function setOnSlotParkingColor(newColor) {
        rectMonitorBackground.color=newColor
        // darkBackgroundColor = newColor
        // lightBackgroundColor = "#ade8f4"
    }
    //--------------Help Navigation --------------//
    signal dataChanged(int data)
    signal dataChangedGauge(int dataGauge)
    signal dataChangedSettings(int dataSettings)

    function sendDataGauge()
    {
        dataChangedGauge(4)
    }
    function sendData_onSlotParking() {
        dataChanged(4)
    }
    function sendData_onSlotParking_settings() {
        dataChangedSettings(4)
    }

    property string rectangleColor: "grey"
    property string rectangleColorBorder: "#C0C0C0"
    property string textColor: "white"
    property real rectangleOpacity: 0.2
    property real textOpacity: 1
    property string rectColor:"#00172D"
    property string rectBorderColor: "#C0C0C0"

    //--------------------------------
    // Background Colors
    //--------------------------------
    property string darkBackgroundColor : "#00172D"
    property string lightBackgroundColor : "#02386E"
    //-------------------------------------------
    //------------- Camera Screen ---------------
    //-------------------------------------------
    property real visibilityBackCamera : 0
    property real visibilityFrontCamera : 1
    property real visibilityBackCameraGridlines : 0
    property real splitCameraScreens : 0
    //--------------------------------

    //-------------------------------------------
    property real id_US_TL : 1
    property real id_US_TR : 2
    property real id_US_BL : 3
    property real id_US_BR : 4
    //--------------Data Set-----------------
    property real reading_US_TL : 0.9
    property real reading_US_TR : 0.3
    property real reading_US_BL : 0.6
    property real reading_US_BR : 1.2

    property real state_US_TL : 0 // gray
    property real state_US_TR : 0 // green
    property real state_US_BL : 2 // orange
    property real state_US_BR : 3 // red

    ReadFile {
        id: fileReader
    }
    Timer {
        id: usTimer1
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            var filePath1 = "D://QT projects//FinalGui_all//fileToRead//reading_US_TL.txt";
            var fileContent1 = fileReader.readFileFunc(filePath1);
            reading_US_TL = parseFloat(fileContent1);
        }
    }

    Timer {
        id: usTimer2
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            var filePath2 = "D://QT projects//FinalGui_all//fileToRead//reading_US_TR.txt";
            var fileContent2 = fileReader.readFileFunc(filePath2);
            reading_US_TR = parseFloat(fileContent2);
        }
    }

    Timer {
        id: usTimer3
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            var filePath3 = "D://QT projects//FinalGui_all//fileToRead//reading_US_BL.txt";
            var fileContent3 = fileReader.readFileFunc(filePath3);
            reading_US_BL = parseFloat(fileContent3);
        }
    }

    Timer {
        id: usTimer4
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            var filePath4 = "D://QT projects//FinalGui_all//fileToRead//reading_US_BR.txt";
            var fileContent4 = fileReader.readFileFunc(filePath4);
            reading_US_BR = parseFloat(fileContent4);
        }
    }
    Timer {
        id: usStateTimer1
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            var stateFilePath1 = "D://QT projects//FinalGui_all//fileToRead//state_US_TL.txt";
            var stateFileContent1 = fileReader.readFileFunc(stateFilePath1);
            state_US_TL = parseInt(stateFileContent1);
            console.log(state_US_TL);
            updateColor_US_TL();
            rotatingarc_TL1.requestPaint();
            rotatingarc_TL2.requestPaint();
            rotatingarc_TL3.requestPaint();



        }
    }

    Timer {
        id: usStateTimer2
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            var stateFilePath2 = "D://QT projects//FinalGui_all//fileToRead//state_US_TR.txt";
            var stateFileContent2 = fileReader.readFileFunc(stateFilePath2);
            state_US_TR = parseInt(stateFileContent2);
            updateColor_US_TR();
            rotatingarc_TR1.requestPaint();
            rotatingarc_TR2.requestPaint();
            rotatingarc_TR3.requestPaint();

        }
    }

    Timer {
        id: usStateTimer3
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            var stateFilePath3 = "D://QT projects//FinalGui_all//fileToRead//state_US_BL.txt";
            var stateFileContent3 = fileReader.readFileFunc(stateFilePath3);
            state_US_BL = parseInt(stateFileContent3);
            updateColor_US_BL();
            rotatingarc_BL1.requestPaint();
            rotatingarc_BL2.requestPaint();
            rotatingarc_BL3.requestPaint();
        }
    }

    Timer {
        id: usStateTimer4
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            var stateFilePath4 = "D://QT projects//FinalGui_all//fileToRead//state_US_BR.txt";
            var stateFileContent4 = fileReader.readFileFunc(stateFilePath4);
            state_US_BR = parseInt(stateFileContent4);
            updateColor_US_BR();
            rotatingarc_BR1.requestPaint();
            rotatingarc_BR2.requestPaint();
            rotatingarc_BR3.requestPaint();
        }
    }




    //---------------------------------------
    property real isReadingsShown : 0
    property string readingStateText : "Show"
    property string gridlinesStateText : "Show"

    //--------------------------------------------
    property real warningArcsthicknes : 6
    //----------- Inication Color ------------------
    property string notWorkingColor: "grey"
    property string safeColor :      "green"
    property string warningColor :   "orange"
    property string dangerousColor : "red"
    //------------- Arc Colors ----------------------
    property string warningArc_BR_Color : safeColor
    property string warningArc_BL_Color : safeColor
    property string warningArc_TR_Color : safeColor
    property string warningArc_TL_Color : safeColor
    //------------- Arc Angles ----------------------
    property real warningArc_BR_StartAngle : Math.PI*0
    property real warningArc_BR_EndAngle : Math.PI*0.3

    property real warningArc_BL_StartAngle : Math.PI*0.65
    property real warningArc_BL_EndAngle : Math.PI*0.95

    property real warningArc_TR_StartAngle : Math.PI*1.6
    property real warningArc_TR_EndAngle : Math.PI*1.9

    property real warningArc_TL_StartAngle : Math.PI*1.1
    property real warningArc_TL_EndAngle : Math.PI*1.4
    //------------- Arc Radius ----------------------
    property real warningArc_BR1_radius : 80
    property real warningArc_BR2_radius : 100
    property real warningArc_BR3_radius : 120

    property real warningArc_BL1_radius : 80
    property real warningArc_BL2_radius : 100
    property real warningArc_BL3_radius : 120

    property real warningArc_TR1_radius : 80
    property real warningArc_TR2_radius : 100
    property real warningArc_TR3_radius : 120

    property real warningArc_TL1_radius : 80
    property real warningArc_TL2_radius : 100
    property real warningArc_TL3_radius : 120




    NumberAnimation {
        id : showBackCameraAnimation
        target: backCameraScreen
        property: "opacity"
        from: 0
        to : 1
        duration: 500
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id : hideBackCameraAnimation
        target: backCameraScreen
        property: "opacity"
        from: 1
        to : 0
        duration: 500
        easing.type: Easing.InOutQuad
    }
    NumberAnimation {
        id : showFrontCameraAnimation
        target: frontCameraScreen
        property: "opacity"
        from: 0
        to : 1
        duration: 500
        easing.type: Easing.InOutQuad
    }
    NumberAnimation {
        id : hideFrontCameraAnimation
        target: frontCameraScreen
        property: "opacity"
        from: 1
        to : 0
        duration: 500
        easing.type: Easing.InOutQuad
    }
    //-----------------------------

    //---------------------------------------------------------------
    // Animation To on Camera
    //---------------------------------------------------------------

    NumberAnimation {
        id : splitFrontCameraAnimation_X
        target: frontCameraScreen
        property: "x"
        from: 0
        to : backCameraScreen.width
        duration: 500
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id : splitFrontCameraAnimation_Width
        target: frontCameraScreen
        property: "width"
        from: cameraPart.width* 0.98
        to : cameraPart.width*0.5
        duration: 500
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id : splitBackCameraAnimation_Width
        target: frontCameraScreen
        property: "width"
        from: cameraPart.width* 0.98
        to : cameraPart.width*0.5
        duration: 500
        easing.type: Easing.InOutQuad
    }
    //---------------------------------------------------------------
    // Animation To Appear and Hide the Readings of the Ulterasoincs
    //---------------------------------------------------------------
    NumberAnimation {
        id : showFirstRow_UsReadings
        target: firstRow_UsReadings
        property: "opacity"
        duration: 500
        from : 0
        to : 1
        easing.type: Easing.InOutQuad
    }
    NumberAnimation {
        id : hideFirstRow_UsReadings
        target: firstRow_UsReadings
        property: "opacity"
        duration: 500
        from : 1
        to : 0
        easing.type: Easing.InOutQuad
    }
    //--------------------------------------------
    NumberAnimation {
        id : showSecondRow_UsReadings
        target: secondRow_UsReadings
        property: "opacity"
        duration: 500
        from : 0
        to : 1
        easing.type: Easing.InOutQuad
    }
    NumberAnimation {
        id : hideSecondRow_UsReadings
        target: secondRow_UsReadings
        property: "opacity"
        duration: 500
        from : 1
        to : 0
        easing.type: Easing.InOutQuad
    }
    //----------------Animation on Arcs Flowing ---------------------

    NumberAnimation {
        id : showAnimation_Arcs
        //target: rotatingarc_BR1
        targets: [rotatingarc_BR1,rotatingarc_BR2,rotatingarc_BR3,
            rotatingarc_BL1,rotatingarc_BL2,rotatingarc_BL3,
            rotatingarc_TL1,rotatingarc_TL2,rotatingarc_TL3,
            rotatingarc_TR1,rotatingarc_TR2,rotatingarc_TR3]
        property: "opacity"
        duration: 300
        from : 0
        to : 1
        easing.type: Easing.InOutQuad
        onFinished: hideAnimation_Arcs.start();

    }
    NumberAnimation {
        id : hideAnimation_Arcs
        //target: rotatingarc_BR1
        targets: [rotatingarc_BR1,rotatingarc_BR2,rotatingarc_BR3,
            rotatingarc_BL1,rotatingarc_BL2,rotatingarc_BL3,
            rotatingarc_TL1,rotatingarc_TL2,rotatingarc_TL3,
            rotatingarc_TR1,rotatingarc_TR2,rotatingarc_TR3]
        property: "opacity"
        duration: 300
        from : 1
        to : 0
        easing.type: Easing.InOutQuad
        onFinished: showAnimation_Arcs.start();


    }



    //---------------------------------------------------------------
    //--------------- Color Handling of The Arcs --------------------
    //---------------------------------------------------------------
    Component.onCompleted: {

        updateColor_US_TL();
        updateColor_US_TR();
        updateColor_US_BL();
        updateColor_US_BR();
        showAnimation_Arcs.start();
        // if (showAnimation_Arcs.finished())
        // {
        //     hideAnimation_Arcs.start();
        //     if(hideAnimation_Arcs.finished())
        //     {
        //         showAnimation_Arcs.start();
        //     }
        // }

    }
    //----------  Top Left US -----------------
    function updateColor_US_TL()
    {
        if (state_US_TL === 0)
        {
            warningArc_TL_Color = notWorkingColor;
        }
        else if (state_US_TL === 1)
        {
            warningArc_TL_Color = safeColor
        }
        else if (state_US_TL === 2)
        {
            warningArc_TL_Color = warningColor
        }
        else if (state_US_TL ===3) {
            warningArc_TL_Color = dangerousColor
        }
        else
        {
            warningArc_TL_Color = safeColor
        }
    }
    //--------------Top Right US---------------------
    function updateColor_US_TR()
    {
        if (state_US_TR === 0)
        {
            warningArc_TR_Color = notWorkingColor;
        }
        else if (state_US_TR === 1)
        {
            warningArc_TR_Color = safeColor
        }
        else if (state_US_TR === 2)
        {
            warningArc_TR_Color = warningColor
        }
        else if (state_US_TR ===3) {
            warningArc_TR_Color = dangerousColor
        }
        else
        {
            warningArc_TR_Color = safeColor
        }
    }
    //----------------Botom Left US------------------
    function updateColor_US_BL()
    {
        if (state_US_BL === 0)
        {
            warningArc_BL_Color = notWorkingColor;
        }
        else if (state_US_BL === 1)
        {
            warningArc_BL_Color = safeColor
        }
        else if (state_US_BL === 2)
        {
            warningArc_BL_Color = warningColor
        }
        else if (state_US_BL ===3) {
            warningArc_BL_Color = dangerousColor
        }
        else
        {
            warningArc_BL_Color = safeColor
        }
    }
    //-----------------Bottom Right US---------------------
    function updateColor_US_BR()
    {
        if (state_US_BR === 0)
        {
            warningArc_BR_Color = notWorkingColor;
        }
        else if (state_US_BR === 1)
        {
            warningArc_BR_Color = safeColor
        }
        else if (state_US_BR === 2)
        {
            warningArc_BR_Color = warningColor
        }
        else if (state_US_BR ===3) {
            warningArc_BR_Color = dangerousColor
        }
        else
        {
            warningArc_BR_Color = safeColor
        }
    }
    //--------------------------------------------------------

    NumberAnimation {
        id : showCarParkedSuccessPopUpAnimation
        target: carParkedSuccessfullyPopUp
        property: "opacity"
        duration: 1000
        from : 0
        to : 1


    }




    //-------------------- Pop Up Reached Slot ---------------
    // isReadyToPark PopUp
    Popup {

        id: carParkedSuccessfullyPopUp
        width: 300
        height: 300
        modal: true
        focus: true
        opacity :0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        anchors.centerIn: parent
        background:Rectangle {
            radius : 20
            color: lightBackgroundColor// Set the background color
            width: parent.width
            height: parent.height
            //-----------------------------------------------
            // Popup Headline
            Text {
                text : "Parking Status"
                anchors.horizontalCenter: parent.horizontalCenter
                color : "white"
                font {
                    pixelSize : 24
                    bold : true
                    italic : true
                }
            }
            //-----------------------------------------------
            Text {
                //Popup body
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                bottomPadding: 20
                text: " Car Has Parked \nSuccessfully ! " // Line break inserted here
                horizontalAlignment: Text.AlignHCenter
                color : "white"
                font {
                    pixelSize : 18
                    weight : 500
                    italic : true

                }
            }
            //-----------------------------------------------
            Row {
                bottomPadding: 20
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                spacing:  carParkedPopUp.width/5

                // Confirm Button
                RoundButton {
                    radius : 20
                    width : carParkedPopUp.width /3
                    height : carParkedPopUp.width/9

                    text: "Confirm"
                    font.pointSize: 12

                    palette.button: lightGreenColor
                    palette.buttonText: "black"

                    onClicked: {
                        carParkedPopUp.close()

                    }
                }
            }

        }
    }


    // Background Color
    Rectangle {
        id : rectMonitorBackground
        //color : "#003366"
        color: "#00172D"
        anchors.fill: parent.fill
        width : parent .width
        height : parent.height

        //-----------------------------------------------
        //  Top ToolBar
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
        //-----------------------------------------------
        // Bottom NavigationBar
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
                home.color = "transparent"
                home.border.color = "transparent"
                parkingRect.color = rectColor
                parkingRect.border.color = rectBorderColor
                speedometerRect.color = "transparent"
                speedometerRect.border.color = "transparent"
                settingsRect.color = "transparent"
                settingsRect.border.color = "transparent"
            }
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.top:garagesList.bottom
            anchors.topMargin:5
            color: rectangleColor
            height: parent.height * 1 / 14
            width: parent.width * 1 / 3
            radius: 15
            opacity: rectangleOpacity
        }


        Rectangle {
            id: bottomToolBar
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.top:garagesList.bottom
            color: "transparent"
            height: parent.height * 1 / 14
            width: parent.width * 1 / 3
            radius: 15
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
                                onSlotParking.sendData_onSlotParking()
                                //stackView.pop()
                                stackLayout.currentIndex=1
                                swipeView.currentIndex=1
                                // stackView.push(homeScreen,{parkingHelp:2})
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
                    color: rectColor
                    border.color: rectBorderColor
                    border.width: 2
                    anchors.verticalCenter: parent.verticalCenter

                    Image {
                        width: 20
                        height: 20
                        id: parking
                        source: "qrc:/FinalGui_all/assets/parking.png"
                        anchors.centerIn: parent
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
                                sendDataGauge()
                                //stackView.pop()
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
                            onClicked: {
                                sendData_onSlotParking_settings()
                                stackLayout.currentIndex=2
                            }
                            //stackView.push("SettingsScreen.qml")
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
        //-----------------------------------------------
        // Screen Name
        Rectangle
        {
            id : screenNameRectangle

            width: parent.width*0.3
            height: parent.height *0.1
            anchors.bottom: parent.bottom
            //anchors.left: parent.left
            color : "transparent"
            x: 10
            //Text on the left
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                id : screenName
                text : "On-Slot Parking"
                font.pixelSize: 25
                anchors.left:parent.left
                // width: parent.width
                // height : parent.height

                color : "white"
                font.bold: true
                font.italic: true

            }

        }

        // Rectangle holidng Row
        Rectangle {
            color : "#02386E"
            // color : "#FFE9BC"
            id : monitorScreenRect
            width : parent .width
            height : parent.height * 0.8
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter:  parent.horizontalCenter
            // radius : 20
            Row {
                spacing : monitorScreenRect.width * 0.01
                //---------- Top View of Car Indicator
                Rectangle {
                    id: carTopViewRectangle
                    z:1
                    width: monitorScreenRect.width * 0.3
                    height : monitorScreenRect.height
                    radius : 40
                    // border.color: "blue"
                    // border.width: 3
                    color: "transparent"


                    Image {
                        z:2
                        width : parent.width
                        height: parent.height
                        anchors.centerIn: parent
                        source :  "qrc:/FinalGui_all/assets/CarTopViewRound1.png"
                        clip: true
                        // sourceSize{
                        //     width : 200
                        //     height : 200
                        // }
                        // fillMode: Image.PreserveAspectFit

                        //------- First Row - Top US Readings ------------
                        Row {
                            id : firstRow_UsReadings
                            y: parent.height*0.3
                            z:1
                            width : parent.width
                            height : parent.height*0.1
                            spacing : parent.width*0.6
                            opacity : 0
                            //------- TL US --------
                            Rectangle{
                                width : parent.width*0.2
                                height : parent.height
                                color:  "lightblue"
                                radius : 20

                                Text{
                                    anchors.centerIn: parent
                                    text :reading_US_TL + " M"
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                            //------- TR US --------
                            Rectangle{
                                width : parent.width*0.2
                                height : parent.height
                                color:  "lightblue"
                                radius : 20

                                Text{
                                    anchors.centerIn: parent
                                    text :reading_US_TR + " M"
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                        }
                        //------- Second Row - Bottom US Readings ------------
                        Row {
                            id : secondRow_UsReadings
                            y: parent.height*0.65
                            z:1
                            width : parent.width
                            height : parent.height*0.1
                            spacing : parent.width*0.6
                            opacity : 0
                            //------- BL US --------
                            Rectangle{
                                width : parent.width*0.2
                                height : parent.height
                                color:  "lightblue"
                                radius : 20
                                Text{
                                    anchors.centerIn: parent
                                    text :reading_US_BL + " M"
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                            //------- BR US --------
                            Rectangle{
                                width : parent.width*0.2
                                height : parent.height
                                color:  "lightblue"
                                radius : 20
                                Text{
                                    anchors.centerIn: parent
                                    text :reading_US_BR + " M"
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                            //--------------------------
                            //--------------------------
                        }

                        //---------- Bottom Right -----------------
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_BR1

                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.58
                                var centreY = carTopViewRectangle.height*0.78
                                var thickness =warningArcsthicknes
                                var radius = warningArc_BR1_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_BR_StartAngle
                                var endangle=warningArc_BR_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_BR_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_BR2
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.58
                                var centreY = carTopViewRectangle.height*0.78
                                var thickness =warningArcsthicknes
                                var radius = warningArc_BR2_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_BR_StartAngle
                                var endangle=warningArc_BR_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_BR_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_BR3
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.58
                                var centreY = carTopViewRectangle.height*0.78
                                var thickness =warningArcsthicknes
                                var radius = warningArc_BR3_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_BR_StartAngle
                                var endangle=warningArc_BR_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_BR_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        //---------- Bottom Left -----------------
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_BL1
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.45
                                var centreY = carTopViewRectangle.height*0.75
                                var thickness =warningArcsthicknes
                                var radius = warningArc_BL1_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_BL_StartAngle
                                var endangle=warningArc_BL_EndAngle

                                ctx.beginPath();
                                ctx.fillStyle = warningArc_BL_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_BL2
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.45
                                var centreY = carTopViewRectangle.height*0.75
                                var thickness =warningArcsthicknes
                                var radius = warningArc_BL2_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_BL_StartAngle
                                var endangle=warningArc_BL_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_BL_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_BL3
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.45
                                var centreY = carTopViewRectangle.height*0.75
                                var thickness =warningArcsthicknes
                                var radius = warningArc_BL3_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_BL_StartAngle
                                var endangle=warningArc_BL_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_BL_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        //---------- Top Right Arcs -----------------
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_TR1
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.6
                                var centreY = carTopViewRectangle.height*0.3
                                var thickness =warningArcsthicknes
                                var radius = warningArc_TR1_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_TR_StartAngle
                                var endangle=warningArc_TR_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_TR_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_TR2
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.6
                                var centreY = carTopViewRectangle.height*0.3
                                var thickness =warningArcsthicknes
                                var radius = warningArc_TR2_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_TR_StartAngle
                                var endangle=warningArc_TR_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_TR_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_TR3
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.6
                                var centreY = carTopViewRectangle.height*0.3
                                var thickness =warningArcsthicknes
                                var radius = warningArc_TR3_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_TR_StartAngle
                                var endangle=warningArc_TR_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_TR_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        //----------- Top Left -----------------
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_TL1
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.4
                                var centreY = carTopViewRectangle.height*0.3
                                var thickness =warningArcsthicknes
                                var radius = warningArc_TL1_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_TL_StartAngle
                                var endangle=warningArc_TL_EndAngle

                                ctx.beginPath();
                                ctx.fillStyle = warningArc_TL_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_TL2
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.4
                                var centreY = carTopViewRectangle.height*0.3
                                var thickness =warningArcsthicknes
                                var radius = warningArc_TL2_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_TL_StartAngle
                                var endangle=warningArc_TL_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_TL_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        Canvas {
                            anchors.fill: parent
                            id:rotatingarc_TL3
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.reset();
                                var centreX = carTopViewRectangle.width*0.4
                                var centreY = carTopViewRectangle.height*0.3
                                var thickness =warningArcsthicknes
                                var radius = warningArc_TL3_radius
                                var innerradius = radius-thickness
                                var startangle=warningArc_TL_StartAngle
                                var endangle=warningArc_TL_EndAngle
                                ctx.beginPath();
                                ctx.fillStyle = warningArc_TL_Color;
                                ctx.arc(centreX,centreY,radius,startangle,endangle,false)
                                ctx.arc(centreX,centreY,innerradius,endangle,startangle,true)
                                ctx.fill();
                            }
                        }
                        //---------------------
                    }

                }
                //----------- Camera Part --------------------

                Rectangle {
                    id : cameraPart
                    width: monitorScreenRect.width * 0.7
                    height :monitorScreenRect.height
                    // border.color: "black"
                    // border.width: 3
                    // radius : 20
                    color: "transparent"


                    // ---------- Bottom ToolBar -----------
                    Rectangle
                    {
                        z:1
                        width : parent.width*0.975
                        height: parent.height*0.11
                        //anchors.left : parent.left
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        color : "lightblue"
                        radius : 20
                        border.color: "black"
                        border.width: 3
                        Row
                        {
                            height: parent.height
                            width :parent.width
                            leftPadding: parent.width*0.02
                            topPadding: parent.height*0.1
                            spacing : 12
                            RoundButton {
                                width : parent.width *0.18
                                height : parent.height*0.8
                                radius : 20


                                Label {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    color : "white"
                                    text: readingStateText + " Sensors"
                                    font.bold: true
                                    font.pixelSize: 12
                                }

                                background: Rectangle {
                                    radius : 20
                                    color: lightBackgroundColor

                                }
                                onClicked: {
                                    if (!isReadingsShown)
                                    {
                                        showFirstRow_UsReadings.start();
                                        showSecondRow_UsReadings.start();
                                        readingStateText= "Hide";
                                        isReadingsShown = 1;
                                    }
                                    else
                                    {
                                        hideFirstRow_UsReadings.start();
                                        hideSecondRow_UsReadings.start();
                                        readingStateText= "Show";
                                        isReadingsShown = 0;
                                    }
                                }
                            }
                            RoundButton {
                                width : parent.width *0.18
                                height : parent.height*0.8
                                radius : 20


                                Label {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    color : "white"
                                    text: gridlinesStateText +" Grid-lines"
                                    font.bold: true
                                    font.pixelSize: 12
                                }


                                background: Rectangle {
                                    radius : 20
                                    color: lightBackgroundColor}
                                onClicked: {
                                    if (visibilityBackCameraGridlines !== 1)
                                    {
                                        gridlinesStateText="Hide";
                                        visibilityBackCameraGridlines=1;}
                                    else
                                    {
                                        gridlinesStateText="Show"
                                        visibilityBackCameraGridlines=0
                                    }
                                }
                            }

                            RoundButton {
                                width : parent.width *0.18
                                height : parent.height*0.8
                                radius : 20
                                Label {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    color : "white"
                                    text: "Back Camera"
                                    font.bold: true
                                    font.pixelSize: 12
                                }

                                background: Rectangle {
                                    radius : 20
                                    color: lightBackgroundColor}
                                onClicked:
                                {
                                    splitCameraScreens=0;
                                    hideFrontCameraAnimation.start();
                                    showBackCameraAnimation.start();
                                    visibilityFrontCamera = 0
                                    visibilityBackCamera = 1
                                }
                            }

                            RoundButton {
                                width : parent.width *0.18
                                height : parent.height*0.8
                                radius : 20


                                Label {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    color : "white"
                                    text: "Front Camera"
                                    font.bold: true
                                    font.pixelSize: 12
                                }

                                background: Rectangle {
                                    radius : 20
                                    color: lightBackgroundColor}
                                onClicked:
                                {
                                    splitCameraScreens=0;
                                    hideBackCameraAnimation.start();
                                    showFrontCameraAnimation.start();
                                    visibilityBackCamera = 0
                                    visibilityFrontCamera = 1
                                }
                            }
                            RoundButton {
                                width : parent.width *0.18
                                height : parent.height*0.8
                                radius : 20


                                Label {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.verticalCenter: parent.verticalCenter
                                    color : "white"
                                    text: "Split Cameras"
                                    font.bold: true
                                    font.pixelSize: 12
                                }


                                background: Rectangle {
                                    radius : 20
                                    color: lightBackgroundColor}
                                onClicked:
                                {
                                    splitCameraScreens=1;
                                    visibilityBackCamera = 1 ;
                                    visibilityFrontCamera = 1 ;
                                    backCameraScreen.opacity =1 ;
                                    frontCameraScreen.opacity =1 ;
                                    splitFrontCameraAnimation_X.start();
                                    splitFrontCameraAnimation_Width.start();
                                    splitBackCameraAnimation_Width.start();

                                }
                            }

                        }
                    }
                    //--------------------- Back Camera ----------------------
                    Rectangle
                    {

                        id :backCameraScreen

                        // anchors.horizontalCenter: if (!splitCameraScreens)  parent.horizontalCenter
                        // anchors.verticalCenter: if (!splitCameraScreens)  parent.verticalCenter

                        // anchors.top: parent.top
                        // height :parent.height*0.9
                        anchors.right:if (!splitCameraScreens) parent.right
                        height :parent.height*0.98
                        width : (!splitCameraScreens) ?  parent.width* 0.98 : cameraPart.width*0.5
                        opacity: 0
                        visible: visibilityBackCamera
                        //----------------------------------
                        Rectangle{
                            z:1
                            height : parent.height*0.08
                            width : (!splitCameraScreens) ?  parent.width*0.15 : parent.width*0.3
                            color : "black"

                            anchors.top: parent.top
                            radius: 10
                            opacity : 0.5
                            Text {

                                id: backCameraText
                                text: qsTr("Back Camera")
                                font.pixelSize: 12
                                anchors.centerIn: parent
                                color : "white"
                                font.bold: true
                            }
                        }
                        //-----------------------------------
                        Image {
                            visible : visibilityBackCameraGridlines
                            z:1
                            width : parent.width*0.6
                            height: parent.height*0.6
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            source :  "qrc:/FinalGui_all/assets/reverseCameraGridlines.png"
                            fillMode: Image.PreserveAspectFit
                        }
                        CaptureSession {
                            id: captureSession
                            camera: Camera {}
                            videoOutput: output
                        }
                        VideoOutput {
                            id: output

                            width:  parent.width
                            height : parent.height
                            //anchors.fill : parent.fill
                            fillMode:VideoOutput.Stretch

                        }
                        Component.onCompleted: captureSession.camera.start()
                    }

                    //---------------------Front Cameara----------------------------
                    Rectangle
                    {
                        id: frontCameraScreen
                        //   anchors.horizontalCenter: if (!splitCameraScreens)  parent.horizontalCenter
                        //   anchors.verticalCenter: if (!splitCameraScreens)  parent.verticalCenter
                        anchors.right:if (!splitCameraScreens) parent.right
                        x : (!splitCameraScreens)? 0 : backCameraScreen.width
                        width : (!splitCameraScreens) ?  parent.width* 0.98 : cameraPart.width*0.5
                        height :parent.height* 0.98
                        opacity :1
                        visible: visibilityFrontCamera
                        color: "black"
                        //---------- Top Left Text Indicating Camera Position ------------
                        Rectangle
                        {
                            z:1
                            height : parent.height*0.08
                            width :  (!splitCameraScreens) ?  parent.width*0.15 : parent.width*0.3
                            color : "black"
                            // anchors.left: parent.left
                            anchors.top: parent.top
                            radius: 10
                            opacity : 0.5
                            x: (!splitCameraScreens) ?  0 :5
                            Text {

                                id: frontCameraText
                                text: qsTr("Front Camera")
                                font.pixelSize: 12
                                anchors.centerIn: parent
                                color : "white"
                                font.bold: true
                            }
                        }
                        //------------------------------------------------------------

                        Rectangle{
                            z:1
                            height : parent.height*0.1
                            width : parent.width*0.3
                            color : "black"
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter:  parent.verticalCenter
                            radius: 10

                            //visible: visibilityBackCamera

                            Text {
                                id: frontCameraStatus
                                text: qsTr("Not Responding,\nPlease Check Front Camera")
                                font.pixelSize: 15
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter:  parent.verticalCenter
                                color : "white"
                                font.bold: true
                                horizontalAlignment :Text.AlignHCenter
                                verticalAlignment:Text.AlignVCenter
                            }
                        }

                    }



                }
            }
        }
    }
//----------------pop up --------------------------------//
    Popup {

        id: reachedSlotPopUp
        width: 300
        height: 300
        modal: true
        focus: true
        opacity :0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        anchors.centerIn: parent
        background:Rectangle {
            radius : 20
            color: lightBackgroundColor// Set the background color
            width: parent.width
            height: parent.height
            //-----------------------------------------------
            // Popup Headline
            Text {
                text : "Parking Status"
                anchors.horizontalCenter: parent.horizontalCenter
                color : "white"
                font {
                    pixelSize : 24
                    bold : true
                    italic : true
                }
            }
            //-----------------------------------------------
            Text {
                //Popup body
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                bottomPadding: 20
                text: " You Have Successflly Parked " // Line break inserted here
                horizontalAlignment: Text.AlignHCenter
                color : "white"
                font {
                    pixelSize : 18
                    weight : 500
                    italic : true

                }
            }
            //-----------------------------------------------
            Row {
                bottomPadding: 20
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                spacing:  reachedSlotPopUp.width/5

                // Confirm Button
                RoundButton {
                    radius : 20
                    width : reachedSlotPopUp.width /3
                    height : reachedSlotPopUp.width/9

                    text: "Confirm"
                    font.pointSize: 12

                    palette.button: lightGreenColor
                    palette.buttonText: "black"

                    onClicked: {
                        reachedSlotPopUp.close()
                        // stackView.pop()
                        // stackView.push(onSlotParking)
                        stackLayout.currentIndex=1
                        swipeView.currentIndex=1

                    }
                }
            }

        }
    }





}

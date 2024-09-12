import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import ReadFile 1.0


Rectangle {
    id:garageIntriorMap
    width: 1024
    height: 600
    visible: true

    //--------------------------Navigation Help ------//
    signal dataChanged(int data)
    signal dataChangedGauge(int data)
    signal dataChangedSettings(int data)
    function sendData_intrior() {
        dataChanged(3)
    }
    function sendData_intriorGauge(){
        dataChangedGauge(3)
    }
    function sendDataSettings_intriorGauge(){
        dataChangedSettings(3)
    }

    //----------set Color-----------------------

    function setgarageIntriorMapColor(newColor) {
        rectMonitorBackground.color=newColor
        // darkBackgroundColor = newColor
        // lightBackgroundColor = "#ade8f4"
    }
    //------------------Color-------------------
    property string homeBackgroundColor: "#00172D"
    property string rectangleColor: "grey"
    property string rectangleColorBorder: "#C0C0C0"
    property string textColor: "white"
    property real rectangleOpacity: 0.2
    property real textOpacity: 1
    property string rectColor:"#00172D"
    property string rectBorderColor: "#C0C0C0"



    property string darkBackgroundColor : "#00172D"
    property string lightBackgroundColor : "#02386E"
    property string lightGreenColor :"#5bb450"
    property string lightRedColor :"#FF474C"

    //---------- Map ---------------
    property int mapPointWidth: 1
    property int mapPointHeight: 1
    property string mapPointColor: "black"
    property var mapScale: 10
    //---------- Path ---------------
    property var pathPointWidth: 0.5
    property var pathPointHeight:0.5
    property string pathPointColor: "red"
    //---------- DataSet ---------------
    ReadFile {
        id: fileManager
    }
    property string filePath : "D:\\QT projects\\FinalGui_all\\fileToRead\\pathPointList.txt";
    property var pathPointList :JSON.parse(fileManager.readFileFunc(filePath))

    property string filePath_MapPointList : "D:\\QT projects\\FinalGui_all\\fileToRead\\mapPointList.txt";
    property var mapPointList :JSON.parse(fileManager.readFileFunc(filePath_MapPointList))


    property string filePath_carCurrentLocation : "D:\\QT projects\\FinalGui_all\\fileToRead\\carCurrentLocation.txt";
    property var carCurrentLocation :JSON.parse(fileManager.readFileFunc(filePath_carCurrentLocation))

    property string filePath_slotName : "D:\\QT projects\\FinalGui_all\\fileToRead\\slotName.txt";
    property string slotName :fileManager.readFileFunc(filePath_slotName)

    //---------- Car Data ---------------
    property var currentPosition_X : pathPointList[0].x - (carMoving.width/2)
    property var currentPosition_Y : pathPointList[0].y - (carMoving.height/2)

    property var nextPosition_X : pathPointList[1].x
    property var nextPosition_Y : pathPointList[1].y

    property var currentAngle : pathPointList[0].yaw * 57.2958
    property var nextAngle : pathPointList[1].yaw



    property int index_X : 1
    property int index_Y : 1
    property int index_Angle : 1
    property bool isReachedSpot : false



    // Animation of Car Moving
    NumberAnimation {
        id : animateCarPositionX
        target: carMoving
        property: "x"
        duration: 200
        //easing.type: Easing.InOutQuad
        from :currentPosition_X - (carMoving.width/2)
        to : nextPosition_X - (carMoving.width/2)

        onRunningChanged:
        {
            if(!running)
            {
                // To push for the next Path
                if (index_X === (pathPointList.length-1))
                {
                    index_X = 0; // Reset path index for the current segment
                    //isReachedDestination = true;
                    animateCarPositionX.stop();
                    //PopUpHandling
                    showReachedSlotPopAnimation.start();
                    reachedSlotPopUp.open();
                    //---------------
                    return;
                }
                carMoving.rotation=(pathPointList[index_X].yaw * 57.2958)-90

                console.log("Angle Position", pathPointList[index_Angle].yaw);
                console.log("Path Point Index", index_X);
                console.log("Inital x Position", pathPointList[index_X].x);

                index_X++;
                currentPosition_X=  nextPosition_X ;
                nextPosition_X= pathPointList[index_X].x  ;

                animateCarPositionX.start();
            }
        }


    }
    NumberAnimation {
        id : animateCarPositionY
        target: carMoving
        property: "y"
        duration: 200
        from :currentPosition_Y -(carMoving.height/2)
        to : nextPosition_Y - (carMoving.height/2)
        onRunningChanged:
        {
            if(!running)
            {
                // To push for the next Path
                if (index_Y === (pathPointList.length-1))
                {
                    index_Y = 0; // Reset path index for the current segment
                    isReachedDestination = true;
                    animateCarPositionY.stop();
                    return;
                }
                console.log("Inital y Position", pathPointList[index_Y].y);
                console.log("------------------");
                index_Y++;
                currentPosition_Y=  nextPosition_Y ;
                nextPosition_Y= pathPointList[index_Y].y  ;
                animateCarPositionY.start();
            }
        }
    }


    // Showing Map - Path - Car
    SequentialAnimation{
        id : showInterriorMapDetails

        // Show Map & Path
        NumberAnimation {
            id : showGreyGroundAnimation
            target: greyGroundRepresnetation
            property: "opacity"
            duration: 2000
            from : 0
            to :  1
        }
        // Show Start point
        NumberAnimation{
            id : showStartPointAnimation
            target: startPoint
            property: "opacity"
            duration: 1000
            from : 0
            to :  1
        }
        // Show End Point
        NumberAnimation
        {
            id : showEndPointAnimation
            target: endPoint
            property: "opacity"
            duration: 1000
            from : 0
            to :  1
        }
        // Show Car
        NumberAnimation
        {
            id : showCarAnimation
            target: carMoving
            property: "opacity"
            duration: 1000
            from : 0
            to :  1
        }
        // Flashing End Point
        NumberAnimation
        {
            id : flashingEndPointAnimation
            target: endPoint
            property: "opacity"
            duration: 500
            from : 0
            to :  1
            loops: Animation.Infinite
            running : false
        }


    }


    // PopUpAnimation


    NumberAnimation {
        id : showReachedSlotPopAnimation
        target: reachedSlotPopUp
        property: "opacity"
        duration: 1000
        from : 0
        to : 1

    }


    Component.onCompleted:{
        showInterriorMapDetails.start();

    }





    // Background Rectangle
    Rectangle {
        id : rectMonitorBackground
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
                                garageIntriorMap.sendData_intrior()
                                // stackView.pop()
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
                                // stackView.pop()
                                // stackView.push(gaugeScreen,{parkingHelp:3,reverseState:2});
                                sendData_intriorGauge()
                                // stackView.pop()
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
                                sendDataSettings_intriorGauge()
                                stackLayout.currentIndex=2
                                /*stackView.push("SettingsScreen.qml")*/
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
                text : "Garage Interrior Map"
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
            width : parent.width
            height : parent.height * 0.8
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter:  parent.horizontalCenter
            // radius : 20

            // Row Holding The Middle Screen
            Row {
                spacing : monitorScreenRect.width * 0.02
                //---------- Details of Car (Left third of the car) -----------------
                Rectangle {
                    id: carTopViewRectangle
                    z:1
                    width: monitorScreenRect.width * 0.35
                    height : monitorScreenRect.height
                    radius : 40
                    border.color: "grey"
                    border.width: 3
                    color: "Transparent"
                    //------------- Elements inside the Left Part -----
                    Column {
                        width : parent.width
                        height : parent.height
                        // Left Screen First Block
                        Rectangle
                        {
                            width : parent.width *0.9
                            height : parent.height*0.5
                            anchors.horizontalCenter: parent.horizontalCenter
                            y: parent.height *0.05
                            radius: 20
                            color : "transparent"
                            border.color:  "grey"
                            border.width: 1
                        }
                        // Left Screen Second Block
                        Rectangle
                        {
                            width : parent.width *0.9
                            height : parent.height*0.3
                            anchors.horizontalCenter: parent.horizontalCenter
                            y: parent.height *0.6
                            radius: 20
                            color : "transparent"
                            border.color:  "grey"
                            border.width: 1
                        }
                        // Start Parking Button
                        Rectangle{
                            width : parent.width*0.3
                            height : parent.height*0.1
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom : parent.bottom
                            color:"transparent"
                            RoundButton {
                                width : parent.width*0.8
                                height : parent.height*0.8
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                z: 1
                                radius : 20

                                text: "Start Parking"
                                palette.button: "lightblue"
                                palette.buttonText: "Black"

                                font.pixelSize: 12
                                onClicked: {
                                    animateCarPositionX.start();
                                    animateCarPositionY.start();

                                }
                            }
                        }
                    }



                }
                //------------Big Rectangle holding Grey rectangle---------------
                Rectangle {
                    z:1
                    anchors.right: monitorScreenRect.right
                    anchors.verticalCenter: parent.verticalCenter
                    width : rectMonitorBackground.width*0.615
                    height: rectMonitorBackground.height*0.735
                    color : "grey"
                    //----------------Grey rectangle----------------------
                    Rectangle{
                        id : greyGroundRepresnetation
                        opacity: 0
                        anchors.fill:parent.fill
                        transform: Scale{ xScale: -1  }
                        rotation: 90
                        scale :mapScale
                        //----- Map Representation---------
                        Repeater {
                            id : mapRepresentation
                            opacity: 0


                            model: mapPointList
                            delegate:
                                Rectangle {
                                required property var modelData // variable passing on each element in the map
                                width: mapPointWidth
                                height: mapPointHeight
                                color: mapPointColor
                                x: modelData.x
                                y: modelData.y
                            }
                        }
                        //----- Path Representation---------
                        Repeater {
                            id:pathRepresentation

                            model: pathPointList
                            delegate:
                                Rectangle {
                                z:1
                                required property var modelData // variable passing on each element in the map
                                width: pathPointWidth
                                height: pathPointHeight
                                color: pathPointColor
                                x: modelData.x

                                y: modelData.y
                            }
                        }
                        //----------Car Represnetation -----------
                        Rectangle{
                            opacity: 0
                            z:2
                            id : carMoving
                            //width: parent.width *0.035
                            //height: parent.height*0.1
                            width: 6
                            height: 14
                            x: currentPosition_X
                            y: currentPosition_Y
                            scale : 0.3 // downscale the size of the carmoving
                            border.color: "green"
                            border.width : 0.5
                            color:"transparent"
                            rotation:0
                            transform: Rotation { origin.x: width/2; origin.y: height }

                        }
                        //----------- Start Point -----------
                        Rectangle {
                            z:1
                            id : startPoint
                            width: 2
                            height : 2
                            scale :0.8
                            color : "white"
                            radius : 20
                            opacity : 0
                            x : pathPointList[0].x - (startPoint.width/2)
                            y :pathPointList[0].y  - (startPoint.height/2)
                        }
                        //----------- End Point -----------
                        Rectangle {
                            z:1
                            id : endPoint
                            width: 2
                            height : 2
                            scale :0.8
                            color : "white"
                            radius : 20
                            opacity : 0
                            x : pathPointList[pathPointList.length-1].x - (endPoint.width/2) +1
                            y :(pathPointList[pathPointList.length-1].y  - (endPoint.height/2)) +0.5
                        }

                        Text {
                            transform: Scale{ xScale: -1 }
                            rotation: 90
                            z:1
                            id : slotText
                            text : slotName
                            font.pixelSize: 8
                            width: 2
                            height : 2
                            scale :0.2
                            color : "white"
                            opacity : 1
                            x : pathPointList[pathPointList.length-1].x - (endPoint.width/2) +4
                            y :(pathPointList[pathPointList.length-1].y  - (endPoint.height/2))- 0.2

                        }

                    }
                }
            }
        }

    }
    //-------------------- Pop Up Reached Slot ---------------
    // isReadyToPark PopUp
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
                text: " You Have Reached To\n Your Parking Slot " // Line break inserted here
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
                        stackLayout.currentIndex=3
                        stackLayout2.currentIndex=3

                    }
                }
            }

        }
    }



}


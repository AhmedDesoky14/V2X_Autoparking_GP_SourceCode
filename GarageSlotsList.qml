import QtQuick
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtMultimedia
import QtLocation 6.7
import QtPositioning
import ReadFile 1.0

Rectangle {
    id: garageSlotsList
    width: 1024
    height: 600


    function setgarageSlotsListColor(newColor) {
        garageListScreen.color = newColor
    }
    //--------------Help Navigation --------------//
    signal dataChanged(int data)
    signal dataChangedGauge(int dataGauge)
    signal dataChangedsettings(int dataSettings)
    function sendDataGauge()
    {
        dataChangedGauge(1)
    }
    function sendData() {
        dataChanged(1)
    }
    function sendDatasettings()
    {
        dataChangedsettings(1)
    }
    // Connections {
    //     target: homeScreen // Refers to the sender instance in main.qml

    //     onMyColor: {
    //         garageListScreen.color = color1
    //     }
    // }
    //title: qsTr("Car Dashboard")
    //--------------------------------
    // Colors for Top and Bottom ToolBar
    //--------------------------------
    property string homeBackgroundColor: "#00172D"
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
    //"#003366" Another Blue Color
    //"#FFE9BC" Another Orange Color
    property string darkBackgroundColor : "#00172D"
    property string lightBackgroundColor : "#02386E"
    property string lightGreenColor :"#5bb450"
    property string lightRedColor :"#FF474C"
    //------------------------
    property var choosenSlotLatitude : 0
    property var choosenSlotLongitude : 0
    property string choosenGarageName : ""

    property var myPath_Slots: []

    property var currentDistance: 0
    property var currentTime: 0

    //------------------------
    property int numberOfGarages : 3
    property int currentIndex : 0
    property int indexOnTextChanged:   0
    property bool garagesAroundCarExist: false
    property var incomingGarageList:[]
    property string filePath : "D:\\QT projects\\FinalGui_all\\fileToRead\\GarageList.txt";
    property var incomingGarageList1:[]

    ReadFile  {
        id: garageList
    }
    Component.onCompleted: {

        incomingGarageList1 = garageList.readFileFunc(filePath)
        console.log(incomingGarageList1);

        // Parse the JSON format into an array
        incomingGarageList = JSON.parse(incomingGarageList1);


    }


    //----------------------------------
    property var startPointLatitude : 29.842505054527322
    property var startPointLongitude : 31.37536242982614


    Location
    {
        id : startCoordinate
        coordinate {
            latitude: startPointLatitude
            longitude: startPointLongitude
        }
    }
    Location
    {
        id : listElementCoordinate
        coordinate {
            latitude: endPointLatitude
            longitude: endPointLongitude
        }
    }



    ListModel{
        id: garagelistModelcontainElements
        Component.onCompleted:
        {
            if (incomingGarageList.length ===0)
            {
                garagesAroundCarExist = false;
                garagelistModelcontainElements.clear();
            }
            else
            {
                garagesAroundCarExist = true;
                for (currentIndex ; currentIndex <= incomingGarageList.length; currentIndex++)
                {
                    garagelistModelcontainElements.insert(0,incomingGarageList[currentIndex]);
                }
            }
        }
    }




    NumberAnimation {
        id : animationOpacityPopUp
        target: confirmReserveSlotPopUp
        property: "opacity"
        duration: 600
        easing.type: Easing.InOutQuad
        from : 0
        to : 1
    }


    //-----------------------------------------------
    // Background only
    Rectangle {
        id: garageListScreen
        width  : generalWindow.width
        height : generalWindow.height
        anchors.fill: parent  // Fills the entire Page area
        color: darkBackgroundColor
    }
    //-----------------------------------------------
    // Home Top ToolBar
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
    // Text : Garages List:
    Text {
        id : garagelistTextIndicator
        anchors.top: homeTopToolbar.bottom
        anchors.left: garageListScreen.left
        leftPadding: garagesList.x
        topPadding: 5
        text: "Garages List :"  // Line break inserted here
        color: "white"
        font {
            pixelSize : 35
            italic : true
            bold : true
        }
    }
    //-----------------------------------------------
    // Search Bar

    Rectangle
    {
        id : garagesSearchFieldZone
        width: parent.width*0.7
        height: parent.height *0.08
        // anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: garagelistTextIndicator.bottom

        x:garagesList.x
        anchors.topMargin: 5
        radius : 10
        color : "lightblue"
        TextField {
            id: garagesSearchField
            anchors.fill: garagesSearchFieldZone
            anchors.margins: 10
            placeholderText: qsTr("Choose Garage...")

            background: Rectangle {
                id: textFieldBackground
                color: "lightblue"
                implicitWidth: garagesSearchFieldZone.width
                implicitHeight: garagesSearchFieldZone.height
            }
            onTextChanged: {
                if (garagesSearchField.text === "")
                {
                    console.log("Iam Empty")
                    garagelistModelcontainElements.clear();
                    for (indexOnTextChanged ; indexOnTextChanged <= incomingGarageList.length; indexOnTextChanged++)
                    {
                        garagelistModelcontainElements.insert(0,incomingGarageList[indexOnTextChanged]);
                    }
                    indexOnTextChanged=0;

                }

            }
        }
    }

    //----------- Search Button ----------------
    RoundButton {
        id : searchButton
        anchors.left : garagesSearchFieldZone.right
        anchors.bottom : garagesSearchFieldZone.bottom
        width : parent.width*0.1
        height : parent.height*0.06
        radius : 10
        text: 'Search'
        palette.button: "lightblue"
        palette.buttonText: "Black"
        font.pointSize: 10
        onClicked: {
            //Case Empty

            if (garagesSearchField.text !== "")
            {
                var userInput = garagesSearchField.text.trim().toLowerCase() // Get user input, trim whitespace and convert to lowercase
                // console.log("Search term:", userInput)
                // Loop through incomingGarageList
                for (var i = 0; i < incomingGarageList.length; ++i)
                {
                    var currentGarage = incomingGarageList[i]
                    var garageNameLower = currentGarage.garageName.toLowerCase().trim()
                    //console.log("Current Garage", garageNameLower)

                    // Compare user input with lowercase garage name
                    if (userInput === garageNameLower)
                    {
                        garagelistModelcontainElements.clear()
                        garagelistModelcontainElements.insert(0,incomingGarageList[i]);

                        // Match found! Perform actions based on the matched garage
                        console.log("Matched garage:", currentGarage.garageName)
                        // You can access other properties of the matched garage object here, like currentGarage.cost or currentGarage.location
                        break; // Exit the loop after finding a match
                    }
                    else
                    {
                        garagelistModelcontainElements.clear()

                    }
                }

            }


        }
    }
    //--------- Refresh Button ------------------
    RoundButton {
        id: refreshButton
        anchors.right : garagesList.right
        anchors.bottom : garagesSearchFieldZone.bottom
        width : parent.width*0.07
        height : parent.height*0.06
        radius : 10

        text: "Refresh"
        palette.button: lightGreenColor
        palette.buttonText: "Black"
        font.pointSize: 10
        onClicked: {
            // Re Read The file

            incomingGarageList1 = garageList.readFileFunc(filePath)



            console.log(incomingGarageList1);

            // Parse the JSON format into an array
            incomingGarageList = JSON.parse(incomingGarageList1);
            if (incomingGarageList.length ===0)
            {
                garagesAroundCarExist = false;
                garagelistModelcontainElements.clear();
            }
            else
            {
                garagelistModelcontainElements.clear();
                garagesAroundCarExist = true;

                for (var currentIndex=0 ; currentIndex <= incomingGarageList.length; currentIndex++)
                {
                    garagelistModelcontainElements.insert(0,incomingGarageList[currentIndex]);
                }
            }

            // Access the id property directly from the parsed object
            //console.log("incomingGarageList[1].id:", incomingGarageList[1].id);

        }
    }




    //-----------------------------------------------
    //Garages Scrollable Menu
    Rectangle {
        id: garagesList
        width: parent.width*0.9
        height: parent.height *0.65
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: garagesSearchFieldZone.bottom
        anchors.topMargin: 10
        radius : 20
        color : "lightblue"

        ListView {
            clip: true
            id : garagelist
            width: parent.width
            height: parent.height*0.8
            anchors.fill: parent
            spacing : parent.height*0.05


            populate: Transition {
                NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 500 }
                NumberAnimation { property: "scale"; from: 0; to: 1.; duration: 500 }
            }

            displaced: Transition {
                PropertyAction { properties: "opacity, scale"; value: 1}
                NumberAnimation { properties: "x,y"; duration: 200 }
            }

            model: garagelistModelcontainElements
            delegate:
                Rectangle {
                id : rectDelegate
                width : ListView.view.width
                height : ListView.view.height*0.3
                radius : 20
                border.color: "black"
                // To be checked
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log("Clicked on", garageName)
                        // stackView.push(garageMapScreen)
                    }
                }
                Row {
                    id : row1
                    // ------------------- First Column------------
                    // Includes :
                    // Garage Name
                    // Garage address
                    // Distance
                    // Time
                    //---------------------------------------------
                    spacing: 100
                    Column {
                        id : col1
                        leftPadding: rectDelegate.width*0.01
                        topPadding: rectDelegate.height*0.08
                        //--------------------
                        // Garage Name
                        Text {
                            id : garageNameId
                            text: model.garageName
                            font{
                                pixelSize: 25
                                italic : true
                                bold :true
                            }
                        }
                        //--------------------
                        // Garage Address
                        Row{
                            Text {
                                anchors.fill: parent.fill
                                // width : col1.width
                                topPadding:  rectDelegate.height * 0.05
                                bottomPadding:  rectDelegate.height * 0.1
                                text: location
                                font{
                                    pixelSize: 15
                                    italic : true
                                }
                                wrapMode: Text.Wrap
                            }
                        }
                        Row {
                            topPadding:  rectDelegate.height * 0.05
                            spacing : rectDelegate.width * 0.1
                            //--------------------
                            // Distance
                            Text {
                                text: '<b>Distance :</b> ' + distance + ' K.m'
                                font{
                                    pixelSize: 15
                                }

                            }
                            //--------------------
                            // Time
                            Text {
                                text: '<b>Time :</b> ' + time + ' min'
                                font{
                                    pixelSize: 15
                                }
                            }
                        }
                    }
                    //-----------------------------------------
                    Row{
                        spacing : 5
                        //-------------
                        // Splitter
                        Rectangle
                        {
                            height:  rectDelegate.height*0.9
                            width : 2
                            color: "black"
                            radius : 50
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Column {
                            bottomPadding: rectDelegate.height*0.05
                            topPadding: rectDelegate.height*0.2
                            spacing : 10
                            //-----------------------
                            // Cost
                            Text {

                                leftPadding: rectDelegate.width*0.01
                                text: '<b>Cost :</b> ' + cost + ' L.E'
                                font{
                                    pixelSize: 15
                                }
                            }
                            //-----------------------
                            // Number Of Slots
                            Text {

                                leftPadding: rectDelegate.width*0.01
                                text: '<b> Slots:</b> ' + numSlots
                                font{
                                    pixelSize: 15
                                }
                            }
                            Text {

                                leftPadding: rectDelegate.width*0.01
                                text: '<b> Rate:</b> ' + rate
                                font{
                                    pixelSize: 15
                                }
                            }
                        }
                    }
                    Column
                    {
                        leftPadding : 150
                        topPadding: rectDelegate.height*0.55
                        RoundButton {
                            //anchors.right : rectDelegate.right
                            //anchors.bottom : parent.bottom
                            radius : 20
                            width : 110
                            height : 40
                            text: 'Reserve Slot'
                            palette.button: lightBackgroundColor
                            palette.buttonText: "White"
                            font.pointSize: 12
                            onClicked: {
                                choosenGarageName = garageName ;
                                choosenSlotLatitude  = locationLatitude ;
                                choosenSlotLongitude = locationLongitude ;
                                console.log("Clicked on", choosenSlotLatitude) ;
                                console.log("Clicked on", choosenSlotLongitude) ;

                                confirmReserveSlotPopUp.open();
                                animationOpacityPopUp.start();


                            }
                        }


                    }
                }
            }
        }
    }
    //-----------------------------------------------
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
                            // stackView.pop()
                            //stackView.push(homeScreen,{parkingHelp:1})
                            sendDataGauge()
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
                            // stackView.push(gaugeScreen,{parkingHelp:1,reverseState:2});
                            sendDataGauge()
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
                        onClicked: stackView.push("SettingsScreen.qml")
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                // stackView.pop()
                                // stackView.push(settingsScreen,{parkingHelp:1})
                                sendDatasettings()
                                stackLayout.currentIndex=2

                            }
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
    //------- Pop UP ConfirmationResvervationPopUp---------------

    // isReadyToPark PopUp
    Popup {

        id: confirmReserveSlotPopUp
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
                text : "Confirm Reservation "
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
                text: " Are you sure to reseve a slot\n in " + choosenGarageName.trim() + "?" // Line break inserted here
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
                spacing:  confirmReserveSlotPopUp.width/5
                // Cancel Button
                RoundButton  {
                    radius : 20
                    width : confirmReserveSlotPopUp.width /3
                    height : confirmReserveSlotPopUp.width/9
                    text: "Cancel"

                    palette.button: lightRedColor
                    palette.buttonText: "black"
                    font.pointSize: 12

                    onClicked: {

                        choosenSlotLatitude=0;
                        choosenSlotLongitude=0;
                        choosenGarageName = "";
                        confirmReserveSlotPopUp.close()
                    }
                }
                // Confirm Button
                RoundButton {
                    radius : 20
                    width : confirmReserveSlotPopUp.width /3
                    height : confirmReserveSlotPopUp.width/9

                    text: "Confirm"
                    font.pointSize: 12

                    palette.button: lightGreenColor
                    palette.buttonText: "black"

                    onClicked: {
                        confirmReserveSlotPopUp.close()
                        // stackView.pop()
                        // stackView.push(mapToGarage)
                        stackLayout.currentIndex=3
                        stackLayout2.currentIndex=1


                    }
                }
            }

        }
    }



    //--------------- Distance And Time Handling ----------------
    // New ---------------------------------------

    // function getCurrentDistance (distance)
    //  {
    //      listElementCoordinate.longitude
    //  }
    // function handleDistance(x , y)
    //  {
    //   listElementCoordinate.coordinate.latitude = x
    //   listElementCoordinate.coordinate.longitude = y
    //     routeQuery.clearWaypoints();
    //     routeQuery.addWaypoint(startCoordinate.coordinate) // Start Coordinate
    //     routeQuery.addWaypoint(listElementCoordinate.coordinate) // End Coordinate
    //     routeModel.update();
    // }


    // Component.onCompleted:
    // {


    //     if (incomingGarageList.length ===0)
    //     {
    //         console.log("Array Is Empty");
    //     }
    //     else
    //     {
    //         for (currentIndex ; currentIndex <= incomingGarageList.length; currentIndex++)
    //         {
    //             garagelistModelcontainElements.insert(0,incomingGarageList[currentIndex]);
    //         }
    //     }

    // }

    // RoundButton {
    //     radius : 20
    //     width : 100
    //     height : 100


    //     text: "Confirm"
    //     font.pointSize: 12

    //     palette.button: lightGreenColor
    //     palette.buttonText: "black"

    //     onClicked: {


    //     }
    // }



    // // Data Collector about Routes
    // RouteModel {
    //     id: routeModel
    //     plugin : map.plugin
    //     query: routeQuery
    //     onStatusChanged: {
    //         console.log("------Current Status--------",)
    //         console.log("Current State :", routeModel.status)
    //         console.log("----------------",)
    //         if (status == RouteModel.Ready)
    //         {
    //             console.log("State is Ready",)
    //             console.log("Number of Routes", routeModel.count)
    //             switch (count)
    //             {
    //             case 0:
    //                 // technically not an error
    //                 break
    //             case 1:
    //             {
    //                 currentDistance = routeModel.get(count-1).distance
    //                 currentTime = routeModel.get(count-1).travelTime
    //                 break;
    //             }
    //             }
    //         }
    //         else if (status == RouteModel.Error)
    //         {
    //             console.log("Error State :", routeModel.error)
    //             console.log("Error is : ", routeModel.errorString)

    //             // TO DO Handle Error
    //         }
    //     }
    // }


    // RouteQuery {
    //     id: routeQuery
    //     maneuverDetail : RouteQuery.BasicManeuvers
    //     numberAlternativeRoutes : 3 // int
    //     routeOptimizations : RouteQuery.ShortestRoute
    //     segmentDetail : RouteQuery.BasicSegmentData
    //     travelModes : RouteQuery.CarTravel


    // }







}

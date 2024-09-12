import QtQuick 2.15
import QtQuick.Window 2
import QtLocation 6.7
import QtPositioning
import QtQuick.Controls

// Remember to check the API is needed and check that Mode  Night at mapType QML Type

Rectangle{
    id:mapToGarage
    width: 1024
    height: 600
    visible: true


    //--------------------------Navigation Help ------//
    signal dataChanged(int data)
    signal dataChangedGauge(int dataGauge)
    signal dataChangedSettings(int dataSettings)

    function sendData() {
        dataChanged(2)
    }
    function sendDataGauge()
    {
    dataChangedGauge(2)
    }
    function sendDataSettings()
    {
    dataChangedSettings(2)
    }
    //------------------------------------------------


    property string darkBackgroundColor : "#00172D"
    property string lightBackgroundColor : "#02386E"
    property string lightGreenColor :"#5bb450"
    property string lightRedColor : "#FF474C"
    property string lightBlueColor : "#d8e9f7"
    property string rectColor:"#00172D"
    property string rectBorderColor: "#C0C0C0"

    property bool centerLockOnAnimation : true
    //----------------------------------------------------
    property var startPointLatitude : 29.842505054527322
    property var startPointLongitude : 31.37536242982614

    property var endPointLatitude : 29.84150536713111
    property var endPointLongitude : 31.365029558635143

    property var currentLatitude : startPointLatitude
    property var currentLongitude : startPointLongitude
    //----------------------------------------------------
    property var myPath: []

    property bool isReachedDestination : false
    property var centerLat : 0
    property var centerLong : 0

    property var currentSegmentIndex_Lat : 0
    property var currentPathIndex_Lat : 0

    property var currentSegmentIndex_Long : 0
    property var currentPathIndex_Long : 0

    property var currentlat : routeModel.status == RouteModel.Ready ?  myPath.segments[currentSegmentIndex_Lat].path[currentPathIndex_Lat].latitude :  0
    property var currentlong : routeModel.status == RouteModel.Ready ?  myPath.segments[currentSegmentIndex_Long].path[currentPathIndex_Long].longitude :  0

    property var nextlat : routeModel.status == RouteModel.Ready ?  myPath.segments[currentSegmentIndex_Lat].path[currentPathIndex_Lat+1].latitude :  0
    property var nextlong : routeModel.status == RouteModel.Ready ?  myPath.segments[currentSegmentIndex_Long].path[currentPathIndex_Long+1].longitude :  0
    //------------------------------------------------------
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
        id : cursorlocation
        coordinate {
            latitude: 0
            longitude: 0
        }
    }
    Location
    {
        id : endCoordinate
        coordinate {
            latitude: endPointLatitude
            longitude: endPointLongitude
        }
    }
    //-------------- Animation for Moving Car -----------------
    //---------------------------------------------------------
    NumberAnimation {
        id : latitudeAnimation
        target: movingCursor
        property: "center.latitude"
        running : false
        duration: 1000
        from : currentlat
        to : nextlat
        onRunningChanged:
        {
            if(!running)
            {
                // To push for the next Path
                if (currentPathIndex_Lat >= myPath.segments[currentSegmentIndex_Lat].path.length)
                {
                    currentPathIndex_Lat = 0; // Reset path index for the current segment
                    currentSegmentIndex_Lat++; // Move to the next segment (if applicable)

                    if (currentSegmentIndex_Lat === (myPath.segments.length-1))
                    {
                        latitudeAnimation.stop(); // Stop animation if all segments are covered
                        currentSegmentIndex_Lat=0;
                        return;
                    }
                }
                var currentPath = myPath.segments[currentSegmentIndex_Lat].path[currentPathIndex_Lat];
                currentlat = nextlat ;
                nextlat = currentPath.latitude;
                currentPathIndex_Lat++;
                latitudeAnimation.start();
            }
        }
    }
    //---------------------------------------------------------
    NumberAnimation  {
        id : longitudeAnimation
        target : movingCursor
        property: "center.longitude"
        running : false
        duration: 1000
        from : currentlong
        to : nextlong
        onRunningChanged:
        {
            if(!running)
            {
                // To push for the next Path
                if (currentPathIndex_Long >= myPath.segments[currentSegmentIndex_Long].path.length)
                {
                    currentPathIndex_Long = 0; // Reset path index for the current segment
                    currentSegmentIndex_Long++; // Move to the next segment (if applicable)
                    if (currentSegmentIndex_Long === (myPath.segments.length-1))
                    {
                        isReachedDestination = true;
                        longitudeAnimation.stop(); // Stop animation if all segments are covered
                        currentSegmentIndex_Long=0;
                        //----- Open Pop Up Reached Your distination ------
                        destinationReachedPopUp.open()
                        destinationReachedPopUpTimer.start()
                        //------------------------------------------------
                        return;
                    }
                }
                var currentPath = myPath.segments[currentSegmentIndex_Long].path[currentPathIndex_Long];
                currentlong = nextlong ;
                nextlong = currentPath.longitude;
                currentPathIndex_Long++;
                console.log("--------------------------");
                console.log("Current Long path :",currentPathIndex_Long)
                console.log("Current long : ",currentlong );
                console.log("Next    Long : ",nextlong );
                console.log("Number of Segments : ",myPath.segments.length);
                console.log("Current Segement: ",currentSegmentIndex_Long);
                console.log("--------------------------" );
                movedRouteLine.addCoordinate(currentPath); // Adding the Green Path
                longitudeAnimation.start();
            }
        }
    }
    //--------------------------------------------------------
    // Animation on Start Position Hide and appear
    NumberAnimation {
        id : intialAnimationStartPosition
        target: startLocationMarker
        property: "opacity"
        duration: 800

        running : true
        loops: Animation.Infinite
        from : 0
        to : 1
    }
    //--------------------------------------------
    // Animation On Showing General Path from Start To end and Reback
    SequentialAnimation
    {
        id : sequentialAnimationOnGeneralPath
        //-----------------------------
        // Zoom Animation Out
        NumberAnimation {
            id : mapZoomAnimationOut
            target: map
            property: "zoomLevel"
            duration: 1000
            to : map.zoomLevel-0.5
            running: false
        }
        //-----------------------------
        //From Start To End
        NumberAnimation {
            id : mapCenterAnimationFromStartToEndLatitude
            target: map
            property: "center.latitude"
            duration: 2000
            from : startPointLatitude
            to : endPointLatitude
            running: false
        }
        NumberAnimation {
            id : mapCenterAnimationFromStartToEndLongitude
            target: map
            property: "center.longitude"
            duration: 2000
            from : startPointLongitude
            to : endPointLongitude
            running: false
        }
        //-----------------------------
        // Zoom Animation In
        NumberAnimation {
            id : mapZoomAnimationIn
            target: map
            property: "zoomLevel"
            duration: 1000
            to : map.zoomLevel+0.5
            running: false
        }
        //-----------------------------
        // From End To Start
        NumberAnimation {
            id : mapZoomAnimationOut_Again
            target: map
            property: "zoomLevel"
            duration: 1000
            to : map.zoomLevel-0.5
            running: false
        }
        NumberAnimation {
            id : mapCenterAnimationFromEndToStartLongitude
            target: map
            property: "center.longitude"
            duration: 2000
            from : endPointLongitude
            to : startPointLongitude
            running: false
        }
        NumberAnimation {
            id : mapCenterAnimationFromEndToStartLatitude
            target: map
            property: "center.latitude"
            duration: 2000
            from : endPointLatitude
            to : startPointLatitude
            running: false
        }


    }

    // Timer To Start General Animation
    Timer {
        id: pauseTimerBeforeShowFromStartToEndAnimation
        interval: 500 // Timeout duration in milliseconds (e.g., 5000 for 5 seconds)
        onTriggered:  sequentialAnimationOnGeneralPath.start();
    }
    //--------------------------------------------------------
    Component.onCompleted:
    {
        map.center.latitude = startCoordinate.coordinate.latitude
        map.center.longitude = startCoordinate.coordinate.longitude
        intialAnimationStartPosition.start();

    }





    // Data Collector about Routes
    RouteModel {
        id: routeModel
        plugin : map.plugin
        query: routeQuery

        onStatusChanged: {
            console.log("------Current Status--------",)
            console.log("Current State :", routeModel.status)
            console.log("----------------",)
            if (status == RouteModel.Ready)
            {
                console.log("State is Ready",)
                console.log("Number of Routes", routeModel.count)
                switch (count)
                {
                case 0:
                    // technically not an error
                    break
                case 1:
                {
                    var  currentRouteModel = routeModel.get(count-1); // Get the Route
                    var  currentRouteModelSegments =currentRouteModel.segments; // Get Route Segements
                    myPath = currentRouteModel;
                    // Moving The Start position to Start Point of the Path
                    startLocationMarker.center.latitude = myPath.segments[0].path[0].latitude
                    startLocationMarker.center.longitude = myPath.segments[0].path[0].longitude
                    break;
                }
                }
            }
            else if (status == RouteModel.Error)
            {
                console.log("Error State :", routeModel.error)
                console.log("Error is : ", routeModel.errorString)

                // TO DO Handle Error
            }
        }
    }

    Rectangle{
        z: 1
        id: toolsRect
        width : parent.width*0.05
        height: parent.height*0.4
        //anchors.right: parent.right
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        radius: 20
        border.color: "black"
        color :lightBackgroundColor

        Column {
            width : parent.width
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            spacing : 20
            topPadding: parent.height*0.13
            leftPadding:parent.width*0.2
            //Zoom In Button
            RoundButton {
                id : zoomInButton
                z : 1
                text: qsTr("+")
                width : 30
                height: 30
                radius : 10
                palette.button: lightBlueColor
                palette.buttonText: "black"
                font.pixelSize: 20
                font.bold: true
                onClicked: map.zoomLevel++
            }
            //Zoom out Button
            RoundButton {
                id : zoomOutButton
                z : 1
                text: qsTr("-")
                width : 30
                height: 30
                radius : 10
                palette.button: lightBlueColor
                palette.buttonText: "black"
                font.pixelSize: 20
                onClicked: map.zoomLevel--
            }
            RoundButton {
                id : recenterButton
                z : 1
                text: qsTr("▼")
                width : 30
                height: 30
                radius : 10
                palette.button: lightBlueColor
                palette.buttonText: "black"
                font.pixelSize: 20
                onClicked: {
                    map.center.latitude = currentLatitude
                    map.center.longitude = currentLongitude
                }

            }
            RoundButton {
                id : centerLockButton
                z : 1
                text: centerLockOnAnimation ? qsTr("●") : qsTr("○")
                width : 30
                height: 30
                radius : 10
                palette.button: lightBlueColor
                palette.buttonText: "black"
                font.pixelSize: 20
                onClicked: {
                    if (centerLockOnAnimation === false)
                    {
                        centerLockOnAnimation = true ;
                        mapCenterLatitudeAnimation.start();
                        mapCenterLongitudeAnimation.start();
                    }
                    else
                    {
                        centerLockOnAnimation = false ;
                        mapCenterLatitudeAnimation.stop();
                        mapCenterLongitudeAnimation.stop();
                    }
                }
            }
        }
        //------------------------ Button To get Dirction of end PointInput----------------------------




    }
    //------------ bottomNavigationBar ---------------
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
    Rectangle
    {
        z:1
        id : bottomNavigationBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width*0.4
        height: parent.height *0.06
        anchors.bottom: parent.bottom
        color : lightBackgroundColor
        radius: 10

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
                            sendData()
                            //console.log( i)
                            // stackView.pop()
                            // stackView.push(homeScreen,{parkingHelp:2})
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
                            // stackView.push(gaugeScreen,{parkingHelp:2})
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
                        onClicked: {
                            // stackView.pop()
                            // stackView.push(gaugeScreen,{parkingHelp:2,reverseState:2});
                           sendDataSettings()
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


    //------------ bottomMapButtons ---------------
    Rectangle
    {
        z:1
        id : bottomMapButtons

        width: parent.width*0.25
        height: parent.height *0.06
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        color : lightBackgroundColor
        radius: 10
        Row {
            width : parent.width
            height: parent.height
            spacing :parent.width*0.08
            leftPadding : parent.width*0.09
            topPadding: parent.height * 0.02
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter


            RoundButton {
                id : showDirectionsButton
                z : 1
                text: qsTr("Show Directions")

                palette.button: lightBlueColor
                palette.buttonText: "black"

                onClicked: {
                    console.log("Current State lat :", startCoordinate.coordinate.latitude)
                    console.log("Current State long :", startCoordinate.coordinate.longitude)
                    routeQuery.clearWaypoints();
                    routeQuery.addWaypoint(startCoordinate.coordinate) // Start Coordinate
                    routeQuery.addWaypoint(endCoordinate.coordinate) // End Coordinate
                    routeModel.update();
                    //---------------------------
                    // Center the Map on The Start Position
                    map.center.latitude = startCoordinate.coordinate.latitude
                    map.center.longitude = startCoordinate.coordinate.longitude
                    //----------------------------
                    intialAnimationStartPosition.stop(); // Stop Animating The Start Position
                    //---------------------------
                    // Show End Location
                    endLocationMarker.center.latitude = endCoordinate.coordinate.latitude
                    endLocationMarker.center.longitude = endCoordinate.coordinate.longitude
                    //General Animation Along Path before Navigation
                    pauseTimerBeforeShowFromStartToEndAnimation.start()

                }
            }

            //--------------------------Start Animate button----------------------
            RoundButton {
                id : startAnimateButton
                z : 1
                text: qsTr("Start Navigation ")
                palette.button: lightBlueColor
                palette.buttonText: "black"

                onClicked: {
                    console.log("----------------",)
                    console.log("Check The Path",myPath.path)
                    console.log("Intial Latitude ",myPath.segments[0].path[0].latitude)
                    console.log("Intial longitude",myPath.segments[0].path[0].longitude)
                    console.log("----------------",)

                    maximumFieldOfViewAnimation.start()
                    minimumFieldOfViewAnimation.start ()
                    minimumTiltAnimation.start()
                    maximumTiltAnimation.start()
                    zoomLevelOutAniamtion.start()
                    mapCenterLatitudeAnimation.start()
                    mapCenterLongitudeAnimation.start()
                    zoomLevelInAniamtion.start()
                    latitudeAnimation.start();
                    longitudeAnimation.start();
                }
            }
        }
    }


    //--------- Rectangele Showing The Paths Data---------------

    Rectangle
    {
        z: 1
        id: currentSegmentData
        width : parent.width*0.8
        height: parent.height*0.12
        color: lightBackgroundColor
        border.color: "black"
        border.width: 3
        radius: 8
        anchors.horizontalCenter:  parent.horizontalCenter
        anchors.top:  parent.top
        // anchors.bottom:  parent.bottom
        //------------- Upper Data Showing Instruction ------------------
        Row
        {
            width : parent.width
            height : parent.height
            id:navigationDetailsBar
            z: 1
            anchors.horizontalCenter: currentSegmentData.horizontalCenter
            anchors.verticalCenter: currentSegmentData.verticalCenter
            topPadding: 15
            spacing: 20
            leftPadding: 25
            property bool hasManeuver : myPath.segments[currentSegmentIndex_Lat].maneuver && myPath.segments[currentSegmentIndex_Lat].maneuver.valid

            //------------- Distance Left Block --------------
            Rectangle {
                width : navigationDetailsBar.width*0.18
                height : navigationDetailsBar.height*0.5
                radius : 8
                border.color: "blue"
                border.width: 1.5
                color: "#d8e9f7"
                Text {
                    id : distanceText
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    // anchors.fill: parent
                    wrapMode: Text.WordWrap
                    text: {

                        if (myPath.segments[currentSegmentIndex_Lat].maneuver && myPath.segments[currentSegmentIndex_Lat].maneuver.valid)
                        {
                            if (isReachedDestination)
                            {
                                "Distance Left : " + myPath.segments[myPath.segment.length].maneuver.distanceToNextInstruction + "M"
                            }
                            else
                            {
                                "Distance Left : " + myPath.segments[currentSegmentIndex_Long].maneuver.distanceToNextInstruction + "M"
                            }

                        }
                        else
                        {
                            ""
                        }

                    }
                    color : "black"
                }
            }
            //-------------- Instruction Data ----------------
            Rectangle {
                width : navigationDetailsBar.width*0.5
                height : navigationDetailsBar.height*0.5
                color: "#d8e9f7"
                border.color: "blue"
                border.width: 1.5
                radius : 8


                Text {
                    id : instructionText
                    font.pixelSize:
                    { if (instructionText.text.length >= 20)
                        {
                            13
                        }
                        else
                        {
                            15
                        }

                    }
                    font.bold: true
                    font.italic: true
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    //anchors.fill: parent
                    wrapMode: Text.WordWrap
                    text: {


                        if (myPath.segments[currentSegmentIndex_Lat].maneuver && myPath.segments[currentSegmentIndex_Lat].maneuver.valid)
                        {
                            if (isReachedDestination)
                            {
                                myPath.segments[myPath.segment.length].maneuver.instructionText
                            }
                            else
                            {
                                myPath.segments[currentSegmentIndex_Long].maneuver.instructionText
                            }
                        }
                        else
                        {
                            ""
                        }
                    }
                    color : "black"
                }
            }
            //------------- Time Left Data ----------------
            Rectangle {

                color: "#d8e9f7"
                width : navigationDetailsBar.width*0.18
                height : navigationDetailsBar.height*0.5
                border.color: "blue"
                border.width: 1.5
                radius : 8
                Text {
                    id : timeLeftText
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    // anchors.fill: parent
                    wrapMode: Text.WordWrap
                    text: {


                        if (myPath.segments[currentSegmentIndex_Lat].maneuver && myPath.segments[currentSegmentIndex_Lat].maneuver.valid)
                        {
                            if (isReachedDestination)
                            {
                                "Time Left : " +    myPath.segments[myPath.segment.length].maneuver.timeToNextInstruction + " Sec"
                            }
                            else
                            {
                                "Time Left : " +  (myPath.segments[currentSegmentIndex_Long].maneuver.timeToNextInstruction) + " Sec"

                            }

                        }
                        else
                        {
                            ""
                        }

                    }
                    color : "black"
                }
            }


        }
    }


    RouteQuery {
        id: routeQuery
        //----------------------------------------------------------
        // Initation
        //  clear away any old data in the query
        //  routeQuery.clearWaypoints:
        // // add the start and end coords as waypoints on the route

        // routeQuery.travelModes = RouteQuery.CarTravel
        // routeQuery.routeOptimizations = RouteQuery.FastestRoute
        // routeModel.update();
        //----------------------------------------------------------
        // Paramters
        // departureTime : date
        // excludedAreas : list<georectangle>
        //-----------------
        // featureTypes : QList<FeatureType>
        // RouteQuery.NoFeature             No features will be taken into account when planning the route
        // RouteQuery.TollFeature           Consider tollways when planning the route
        // RouteQuery.HighwayFeature        Consider highways when planning the route
        // RouteQuery.PublicTransitFeature  Consider public transit when planning the route
        // RouteQuery.FerryFeature          Consider ferries when planning the route
        // RouteQuery.TunnelFeature         Consider tunnels when planning the route
        // RouteQuery.DirtRoadFeature       Consider dirt roads when planning the route
        // RouteQuery.ParksFeature          Consider parks when planning the route
        // RouteQuery.MotorPoolLaneFeature  Consider motor pool lanes when planning the route
        // RouteQuery.TrafficFeature        Consider traffic when planning the route
        //-----------------
        maneuverDetail : RouteQuery.BasicManeuvers
        // RouteQuery.NoManeuvers
        // RouteQuery.BasicManeuvers
        //-----------------
        numberAlternativeRoutes : 3 // int
        //-----------------
        routeOptimizations : RouteQuery.ShortestRoute
        // RouteQuery.ShortestRoute      Minimize the length of the journey
        // RouteQuery.FastestRoute       Minimize the traveling time for the journey
        // RouteQuery.MostEconomicRoute	Minimize the cost of the journey
        // RouteQuery.MostScenicRoute	Maximize the scenic potential of the journey
        //-----------------
        segmentDetail : RouteQuery.BasicSegmentData
        // RouteQuery.NoSegmentData	No segment data should be included with the route
        //RouteQuery.BasicSegmentData	Basic segment data will be included with the route
        //-----------------
        travelModes : RouteQuery.CarTravel
        // RouteQuery.CarTravel         The route will be optimized for someone who is driving a car
        // RouteQuery.PedestrianTravel	The route will be optimized for someone who is walking
        // RouteQuery.BicycleTravel 	The route will be optimized for someone who is riding a bicycle
        // RouteQuery.PublicTransit 	Travel The route will be optimized for someone who is making use of public transit
        // RouteQuery.TruckTravel       The route will be optimized for someone who is driving a truck
        //-----------------
        // waypoints : list<coordinate>
        //-----------------
        // Methods
        // void addExcludedArea(georectangle area)
        // void addWaypoint(coordinate)
        // void clearExcludedAreas()
        // void clearWaypoints()
        // FeatureWeight featureWeight(FeatureType featureType)
        // void removeExcludedArea(georectangle area)
        // void removeWaypoint(coordinate)
        // void resetFeatureWeights()
        // void setFeatureWeight(FeatureType feature, FeatureWeight weight)

    }

    // Map Plugin
    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        zoomLevel: 18
        // Center of the Map
        center: QtPositioning.coordinate(startCoordinate.coordinate.latitude, startCoordinate.coordinate.longitude) // Oilibya
        maximumFieldOfView : 0
        minimumFieldOfView : 0
        minimumTilt: 0
        maximumTilt:  0
        copyrightsVisible :false
        visibleRegion :startLocationMarker
        //-------- Animation on Map Itself when start Navigation ------------


        NumberAnimation on maximumFieldOfView
        {
            id : maximumFieldOfViewAnimation
            running : false
            duration: 500
            to : 180
        }
        NumberAnimation on minimumFieldOfView
        {
            id : minimumFieldOfViewAnimation
            running : false
            duration: 500
            to : 60
        }
        NumberAnimation on minimumTilt
        {
            id : minimumTiltAnimation
            running : false
            duration: 500
            to : 30
        }
        NumberAnimation on maximumTilt
        {
            id : maximumTiltAnimation
            running : false
            duration: 500
            to : 80
        }
        NumberAnimation on zoomLevel
        {
            id : zoomLevelOutAniamtion
            running : false
            duration: 500
            to : 17
        }

        NumberAnimation on zoomLevel
        {
            id : zoomLevelInAniamtion
            running : false
            duration: 500
            to : 17.5
        }

        NumberAnimation on center.latitude
        {
            id : mapCenterLatitudeAnimation
            running : false
            duration: 500
            to : currentlat

            onRunningChanged :
            {
                if (!running && centerLockOnAnimation)
                {

                    if (currentSegmentIndex_Lat <= myPath.segments.length)
                    {
                        mapCenterLatitudeAnimation.restart();
                    }
                    else
                    {
                        mapCenterLatitudeAnimation.stop();
                    }
                }

            }
        }
        NumberAnimation on center.longitude
        {
            id : mapCenterLongitudeAnimation
            running : false
            duration: 500
            to : currentlong

            onRunningChanged :
            {
                if (!running && centerLockOnAnimation)
                {

                    if (currentSegmentIndex_Long <= myPath.segments.length)
                    {
                        mapCenterLongitudeAnimation.restart();
                    }
                    else
                    {
                        mapCenterLongitudeAnimation.stop();
                    }
                }

            }
        }

        //-------------- Blue Route Path Display------------------
        MapPolyline {
            z:1
            id: routeLine
            visible: true
            path : routeModel.status == RouteModel.Ready ?  myPath.path : [{ latitude: 0, longitude: 0 }]

            line.color: "blue"
            line.width: 3
            autoFadeIn: true
        }

        MapPolyline {
            z:1
            id: movedRouteLine
            visible: true
            //path : routeModel.status == RouteModel.Ready ?  myPath.segements.path[currentPathIndex_Lat] : [{ latitude: 0, longitude: 0 }]

            line.color: "black"
            line.width: 3
            autoFadeIn: true
        }



        //-------------- Inital Position Green Circle  -----------------
        MapCircle {
            id : startLocationMarker
            center {
                latitude: startCoordinate.coordinate.latitude
                longitude: startCoordinate.coordinate.longitude
            }
            radius: 5.0
            color: 'green'
            border.width: 3
            smooth: true
            opacity:1
        }
        //-------------- End Position Green Circle  -----------------
        MapCircle
        {
            id: endLocationMarker
            radius: 5
            color: "#46a2da"
            border.color: "#190a33"
            border.width: 2
            smooth: true
            opacity:1
            //center: locationData.coordinate
        }
        //------------- Red Cursor ----------------------
        MapCircle {
            z:1
            id : movingCursor
            center {
                latitude: routeModel.status == RouteModel.Ready ?  myPath.segments[0].path[0].latitude :  0
                longitude: routeModel.status == RouteModel.Ready ?  myPath.segments[0].path[0].longitude :  0
            }
            radius: 5.0
            color: 'red'
            border.width: 1.5
            //autoFadeIn: false
            smooth: true
            opacity:1
        }


        //------------------------------------------------------------
        //------------------Map Handling------------------------------
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
        //------------------------------------------------------------
    }

    //--------------------------------------------------------------------------
    // -------------------- Arrived to your Desstination Pop Up ----------------
    Popup {
        id: destinationReachedPopUp
        z:1
        width: 300
        height: 300
        modal: true
        focus: true
        opacity : 1
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        anchors.centerIn: parent
        background:
            Rectangle {
            radius : 20
            color: "lightblue" // Set the background color
            width: parent.width
            height: parent.height
            //-----------------------------------------------
            // Popup Headline
            Text {
                text : " Your Current Status "
                anchors.horizontalCenter: parent.horizontalCenter
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
                text: " You Have Reached To Your\n Destination "  // Line break inserted here
                horizontalAlignment: Text.AlignHCenter
                font {
                    pixelSize : 16
                    weight : 500
                    italic : true

                }
                color: "black"
            }
            //-----------------------------------------------
            Row {
                bottomPadding: 20
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                spacing:  destinationReachedPopUp.width/5

                // Confirm Button
                RoundButton {
                    radius : 20
                    width : destinationReachedPopUp.width /3
                    height : destinationReachedPopUp.width/9
                    background:
                        Rectangle {
                        radius: autoModeButton.radius
                        color: lightGreenColor
                    }
                    text: "Confirm"
                    onClicked: {
                        // Close the current window

                        // stackView.push(garageListScreen)
                        destinationReachedPopUp.close()
                        parkModePopUp.open()

                    }
                }
            }

        }
    }
    Timer {
        id: destinationReachedPopUpTimer
        interval: 3000 // Timeout duration in milliseconds (e.g., 5000 for 5 seconds)
        onTriggered: {
            destinationReachedPopUp.close()
            destinationReachedPopUpTimer.stop()
            parkModePopUp.open();

        }
        repeat: false
    }
    //-------------------- Manual Or Automatic Parking Pop Pop -----------------
    Popup {
        id: parkModePopUp
        z:1
        width: 300
        height: 300
        modal: true
        focus: true
        opacity : 1
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        anchors.centerIn: parent
        background:
            Rectangle {
            radius : 20
            color: "lightblue" // Set the background color
            width: parent.width
            height: parent.height
            //-----------------------------------------------
            // Popup Headline
            Text {
                text : "Parking System"
                anchors.horizontalCenter: parent.horizontalCenter
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
                text: " Please choose your prefered \n Parking Mode  "  // Line break inserted here
                horizontalAlignment: Text.AlignHCenter
                font {
                    pixelSize : 16
                    weight : 500
                    italic : true

                }
                color: "black"
            }
            //-----------------------------------------------
            Row {
                bottomPadding: 20
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                spacing:  parkModePopUp.width/5

                // Manual Mode Button
                RoundButton {
                    id :manualModeButton
                    width : parkModePopUp.width /2.5
                    height : parkModePopUp.width/9
                    text: "Manual Mode"
                    palette.buttonText: "black"
                    background:
                        Rectangle {
                        radius: manualModeButton.radius
                        color: lightRedColor
                    }
                    radius : 20

                    font.pixelSize: 10

                    onClicked: {
                        // Close the current window

                        // stackView.push(garageListScreen)
                        parkModePopUp.close()
                        // stackView.pop();
                        // stackView.push(garageIntriorMap);
                        stackLayout2.currentIndex=2

                    }
                }

                // Automatic Mode Button
                RoundButton {
                    id :autoModeButton
                    width : parkModePopUp.width /2.5
                    height : parkModePopUp.width/9
                    text: "Automatic Mode"
                    background:
                        Rectangle {
                        radius: autoModeButton.radius
                        color: lightGreenColor
                    }
                    radius : 20
                    font.pixelSize: 10

                    palette.buttonText: "black"
                    onClicked: {
                        // Close the current window

                        // stackView.push(garageListScreen)
                        parkModePopUp.close()
                        // stackView.pop();
                        // stackView.push(garageIntriorMap);
                        stackLayout2.currentIndex=2


                    }
                }
            }

        }
    }
    //--------------------------------------------------------------------------
}

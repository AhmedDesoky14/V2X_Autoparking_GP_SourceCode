import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import QtMultimedia

ApplicationWindow {
    visible: true
    width: 1024
    height: 600
    title: "Dashboard"
    color: "black"

    flags: Qt.FramelessWindowHint

    Rectangle {
        width: 40
        height: 40
        color: "red"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        Text {
            text: "X"
            color: "white"
            anchors.centerIn: parent
        }
        MouseArea {
            anchors.fill: parent
            onClicked: Qt.quit()
        }
    }


    property int parkingHelp: 1
    property int parkingHelp_Gauge: 1
    property int parkingHelp_Setting:1
    property int previousScreenIndex: -1 // To track the previous screen index


    property int firstTime:0


    StackLayout {
        id: stackLayout
        anchors.fill: parent
        currentIndex: 0
        onCurrentIndexChanged: {
            if (currentIndex === 1 && previousScreenIndex === 0) {
                firstTime = 1
            }
        }


        VideoScreen {
            onDataChangedVideo:handleDataAfterVideo(data);

        }


        SwipeView
        {
            id:swipeView
            GaugeScreen {
                id:gaugeScreen

            }
            HomeScreen {
                id:homeScreen
            }


        }

        SettingsScreen {
            id:settingsScreen
        }

        StackLayout
        {

            id:stackLayout2
            currentIndex: 0

            GarageSlotsList {
                id: garageSlotsList
                onDataChanged: {
                    handleDataList(data);
                }
                onDataChangedGauge:
                {
                    handleDataGaugeList(dataGauge);
                }
                onDataChangedsettings:
                {
                    handleDataSettingList(dataSettings);
                }
            }


            MapToGarage {
                id: mapToGarage
                onDataChanged: {
                    handleDataMapToGarage(data);
                }
                onDataChangedGauge:
                {
                    handleDataGaugeMapToGarage(dataGauge);
                }
                onDataChangedSettings:
                {
                    parkingHelp_Setting=2;
                    handleDataSettingMapToGarage(dataSettings);
                }
            }


            GarageIntriorMap{
                id: garageInteriorMap
                onDataChanged: handleDataInterior(data);
                onDataChangedGauge:
                {
                    parkingHelp_Gauge=3;

                    //handleDataGaugeIntrior(dataGauge);
                }
                onDataChangedSettings:
                {
                    parkingHelp_Setting=3
                    handleDataSettingIntrior(dataSettings);
                }

            }


            OnSlotParking {
                id: onSlotParking
                onDataChanged: {
                    handleDataSlot(data);
                }
                onDataChangedGauge:
                {
                    handleDataGaugeSlot(dataGauge);
                }
                onDataChangedSettings:{
                    parkingHelp_Setting=4
                    handleDataSettingSlot(dataSettings);
                }

            }

        }
    }

    function handleDataAfterVideo(data)
    {
        firstTime=1;
    }

    function handleDataList(data) {
        parkingHelp = 1;
    }
    function handleDataMapToGarage(data) {
        parkingHelp = 2;
    }

    function handleDataInterior(data) {
        parkingHelp = 3;
    }

    function handleDataSlot(data) {
        parkingHelp = 4;
    }


    function handleDataGaugeList(dataGauge)
    {
        parkingHelp_Gauge=1;
    }
    function handleDataGaugeMapToGarage(dataGauge)
    {
        parkingHelp_Gauge=2;
    }
    function handleDataGaugeIntrior(dataGauge)
    {
        parkingHelp_Gauge=3;
    }

    function handleDataGaugeSlot(dataGauge)
    {
        parkingHelp_Gauge=4;
    }


    function handleDataSettingList(dataSettings)
    {
        parkingHelp_Setting=1;
    }
    function handleDataSettingMapToGarage(dataSettings)
    {
        parkingHelp_Setting=2;
    }
    function handleDataSettingIntrior(dataSettings)
    {
        parkingHelp_Setting=3;
    }

    function handleDataSettingSlot(dataSettings)
    {
        parkingHelp_Setting=4;
    }


    function setGaugeColor(newColor) {
        gaugeScreen.setGaugeColor(newColor)
    }
    function setHomeColor(newColor) {
        homeScreen.setHomeColor(newColor)
    }
    function setgarageSlotsListColor(newColor) {
        garageSlotsList.setgarageSlotsListColor(newColor)
    }

    function setgarageIntriorMapColor(newColor) {
        garageInteriorMap.setgarageIntriorMapColor(newColor)
    }
    function setOnSlotParkingColor(newColor) {
        onSlotParking.setOnSlotParkingColor(newColor)
    }
    Component.onCompleted: {
        // Example: Start with video screen
        stackLayout.currentIndex = 0;
        // stackLayout2.currentIndex=1;
    }
}


import QtQuick


Rectangle {
    id: mySlider

    property int sliderWidth: 180
    property int sliderHeight: 30
    property string sliderColor: "blue"
    property int handlerWidth: 60
    property int handlerHeight: 30
    property string handlerColor: "red"

    width: sliderWidth
    height:sliderHeight
    color: sliderColor
    radius: 20
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter

    Rectangle {
        id: myHandler
        width: handlerWidth
        height: handlerHeight
        color: handlerColor
        radius: 20
        // Initial position (middle of the slider)
        x: (mySlider.width - width) / 2
        anchors.verticalCenter: parent.verticalCenter

        // Mouse area to handle dragging
        MouseArea {
            id: dragArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor

            property bool pressed: false
            property int dragOffset: 0

            onPressed: {
                pressed = true
                dragOffset = mouse.x - myHandler.x
            }

            onPositionChanged: {
                if (pressed) {
                    var newX = mouse.x - dragOffset
                    newX = Math.max(0, Math.min(mySlider.width - myHandler.width, newX))
                    myHandler.x = newX

                    // Calculate the percentage of the handler position within the slider
                    var sliderWidth = mySlider.width - myHandler.width
                    var positionPercentage = newX / sliderWidth

                    // Emit a signal or perform an action based on the positionPercentage
                    if (positionPercentage <= 0.25) {
                        console.log("Slider at first quarter")
                    } else if (positionPercentage <= 0.75) {
                        console.log("Slider at second half")
                    } else {
                        console.log("Slider at last quarter")
                    }
                }
            }

            onReleased: {
                pressed = false
            }
        }
    }
}

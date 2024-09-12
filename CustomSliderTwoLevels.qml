// CustomSlider.qml
import QtQuick 2.15

Rectangle {
    id: mySlider

    property int sliderWidth: 180
    property int sliderHeight: 30
    property string sliderColor: "#00172D"
    property int handlerWidth: 90
    property int handlerHeight: 30
    property string handlerColor: "#C0C0C0"

    signal firstPosition()
    signal secondPosition()

    width: sliderWidth
    height: sliderHeight
    color: sliderColor
    radius: 20

    Rectangle {
        id: myHandler
        width: handlerWidth
        height: handlerHeight
        color: handlerColor
        radius: 20
        x: 0 // Initial position
        anchors.verticalCenter: parent.verticalCenter

        // Mouse area to handle dragging
        MouseArea {
            id: dragArea
            anchors.fill: parent
            //cursorShape: Qt.PointingHandCursor

            property bool pressed: false
            property int dragOffset: 0

            onPressed: {
                pressed = true
                dragOffset = mouse.x - myHandler.x
            }

            onPositionChanged: {
                if (pressed) {
                    var newX = mouse.x - dragOffset

                    // Calculate the valid range for the handler's x position
                    var minX = 0
                    var maxX = mySlider.width - myHandler.width

                    // Snap the handler to the nearest end based on its movement
                    if (newX <= (minX + maxX) / 2) {
                        // Snap to the start (left) of the slider
                        myHandler.x = minX
                        console.log("Handler snapped to the start")
                        mySlider.firstPosition()
                    } else {
                        // Snap to the end (right) of the slider
                        myHandler.x = maxX
                        console.log("Handler snapped to the end")
                        mySlider.secondPosition()
                    }
                }
            }

            onReleased: {
                pressed = false
            }
        }
    }
}

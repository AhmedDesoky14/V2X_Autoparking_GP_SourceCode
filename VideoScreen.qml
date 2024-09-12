import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia


Rectangle {
    visible: true
    width: 1024
    height: 600
    color: "black"
    //--------------------------Navigation Help ------//
    signal dataChangedVideo(int data)

    function sendData() {
        dataChangedVideo(1)
    }
    //------------------------------------------------
    Video {
        id: videoPlayer

        anchors.fill: parent
        source: "file:///C:/Users/user/Downloads/Audi Logo Animation.mp4"
        autoPlay: true
        //fillMode: VideoOutput.Stretch // Uncomment if needed

        onPositionChanged: {
            console.log("Position changed: " + videoPlayer.position) // Debug output
            if (videoPlayer.position === videoPlayer.duration) {
                sendData()
                previousScreenIndex = 0
                stackLayout.currentIndex=1
                console.log("Video finished, navigating to next screen") // Debug output
                //stackView.push(swipeViewComponent) // Navigate to the next screen when the video ends
            }
        }
    }
}


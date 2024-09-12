import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtWebView 1.2 // Import the QtWebView module

Rectangle{
    visible: true
    width: 600
    height: 400
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#00172D" } // Dark blue at the top
        GradientStop { position: 1.0; color: "#0A4C6F" } // Lighter blue at the bottom
    }


  property string textColor: "white"

    GridView {
        id: gridView
        width: parent.width
        height: parent.height
        cellWidth: 200
        cellHeight: 200
        model: ListModel {
            ListElement { name: "YouTube"; iconSource: "qrc:/FinalGui_all/assets/youtube.png"; url: "https://www.youtube.com/" }
            ListElement { name: "Compass"; iconSource: "qrc:/FinalGui_all/assets/compass.png"; url: "https://www.example.com/compass" }
            ListElement { name: "Chrome"; iconSource: "qrc:/FinalGui_all/assets/chrome.png"; url: "https://www.google.com/chrome" }
            ListElement { name: "Music"; iconSource: "qrc:/FinalGui_all/assets/music-note.png"; url: "https://www.example.com/music" }
            ListElement { name: "Spotify"; iconSource: "qrc:/FinalGui_all/assets/spotify.png"; url: "https://www.spotify.com/" }
            ListElement { name: "Netflix"; iconSource: "qrc:/FinalGui_all/assets/netflix.png"; url: "https://www.netflix.com/" }
            // Add more items as needed
        }

        delegate: Item {
            width: gridView.cellWidth
            height: gridView.cellHeight

            Column {
                anchors.centerIn: parent
                spacing: 10

                Image {
                    id: icon
                    source: iconSource
                    width: 100
                    height: 100
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            webView.url = url
                            webView.visible = true // Show the WebView
                        }
                    }
                }

                Text {
                    text: name
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 20
                    color:textColor
                }
            }
        }
    }

    WebView {
        id: webView
        anchors.fill: parent
        visible: false // Initially hide the WebView
    }
}

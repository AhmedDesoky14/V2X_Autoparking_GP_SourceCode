import QtQuick 2.15
import QtQuick.Controls
import QtMultimedia

Rectangle {
    id: musicPlayer
    width: 500
    height: 300
    color: "transparent"
    border.color: "transparent"
    border.width: 1

    property int currentIndex: 0
    property var playlist: [
        "file:///D:/filter/maryam.mp3",
        "file:///D:/filter/qatami.mp3",
        "file:///D:/filter/title.mp3",
        "file:///D:/filter/maryam.mp3",
        "file:///D:/filter/qatami.mp3",
        "file:///D:/filter/title.mp3",
        "file:///D:/filter/maryam.mp3",
        "file:///D:/filter/qatami.mp3",
        "file:///D:/filter/title.mp3"
    ]

    MediaPlayer {
        id: player
        source: musicPlayer.playlist[musicPlayer.currentIndex]
        audioOutput: audioOutput
        onPlaybackStateChanged: {
            if (playbackState === MediaPlayer.EndOfMedia) {
                musicPlayer.nextTrack()
            }
        }
    }

    AudioOutput {
        id: audioOutput
    }

    function getFileName(filePath) {
        var parts = filePath.split("/")
        return parts[parts.length - 1]
    }

    function nextTrack() {
        musicPlayer.currentIndex = (musicPlayer.currentIndex + 1) % musicPlayer.playlist.length
        player.source = musicPlayer.playlist[musicPlayer.currentIndex]
        player.play()
    }

    function previousTrack() {
        musicPlayer.currentIndex = (musicPlayer.currentIndex - 1 + musicPlayer.playlist.length) % musicPlayer.playlist.length
        player.source = musicPlayer.playlist[musicPlayer.currentIndex]
        player.play()
    }



    Column {
        anchors.fill: parent
        spacing: 10
        anchors.margins: 20


        Text {
            id: trackName
            text: musicPlayer.getFileName(musicPlayer.playlist[musicPlayer.currentIndex])
            font.pointSize: 24
            anchors.left: parent.left
            color:"white"

        }

        Slider {
            id: progressSlider
            width: parent.width
            enabled: player.seekable
            value: player.duration > 0 ? player.position / player.duration : 0
            background: Rectangle {
                implicitHeight: 8
                color: "#00172D"
                radius: 3
                Rectangle {
                    width: progressSlider.visualPosition * parent.width
                    height: parent.height
                    color: "grey"
                    radius: 3
                }
            }
            handle: Item {}
            onMoved: function () {
                player.position = player.duration * progressSlider.position
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20

            Image {
                source: "qrc:/FinalGui_all/assets/previous.png" // Replace with your previous icon path
                width: 40
                height: 40
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    onClicked: previousTrack()
                }
            }

            Image {
                source: player.playbackState === MediaPlayer.PlayingState ? "qrc:/FinalGui_all/assets/pause.png" : "qrc:/FinalGui_all/assets/play-button.png" // Replace with your play/pause paths
                width: 40
                height: 40
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (player.playbackState === MediaPlayer.PlayingState) {
                            player.pause()
                        } else {
                            player.play()
                        }
                    }
                }
            }

            Image {
                source: "qrc:/FinalGui_all/assets/next-button.png"
                width: 40
                height: 40
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    onClicked: nextTrack()
                }
            }
        }

    }

    Connections {
        target: musicPlayer
        onCurrentIndexChanged: {
            trackName.text = musicPlayer.getFileName(musicPlayer.playlist[musicPlayer.currentIndex])
        }
    }

    Component.onCompleted: {
        player.pause()
        trackName.text = musicPlayer.getFileName(musicPlayer.playlist[musicPlayer.currentIndex])
    }
}

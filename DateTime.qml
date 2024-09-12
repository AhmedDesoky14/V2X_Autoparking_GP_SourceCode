// DateTimeDisplay.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: 200
    height: 50

    property bool showDate: true // Property to control whether to show the date
    property bool showTime: true // Property to control whether to show the time
    property string currentDate: "" // Property for current date
    property string currentTime: "" // Property for current time

    Timer {
        interval: 1000 // Update every second
        running: true
        repeat: true
        onTriggered: {
            updateDateTime(); // Call function to update date and time
        }
    }

    function updateDateTime() {
        var now = new Date();
        var hours = now.getHours();
        var minutes = now.getMinutes();
        var seconds = now.getSeconds();
        var day = now.getDate();
        var month = now.getMonth() + 1; // Months are zero-based
        var year = now.getFullYear();

        // Format hours, minutes, and seconds to ensure two-digit display
        hours = (hours < 10 ? "0" : "") + hours;
        minutes = (minutes < 10 ? "0" : "") + minutes;
        seconds = (seconds < 10 ? "0" : "") + seconds;

        // Format day, month, and year to ensure two-digit display
        day = (day < 10 ? "0" : "") + day;
        month = (month < 10 ? "0" : "") + month;

        // Update the currentDate and currentTime properties with formatted values
        currentDate = day + "/" + month + "/" + year;
        currentTime = hours + ":" + minutes + ":" + seconds;
    }

    Column {
        anchors.centerIn: parent

        Text {
            id: currentDateTimeText
            visible: showDate
            font {
                family: "Helvetica" // Set the font family
                pixelSize: 24 // Set the font size in pixels
                bold: true // Make the text bold
                italic: false // Do not italicize the text
            }
            text: currentDate // Display formatted current date
            color:"white"

        }

        Text {
            id: currentTimeText
            visible: showTime
            font {
                family: "Helvetica" // Set the font family
                pixelSize: 24 // Set the font size in pixels
                bold: true // Make the text bold
                italic: false // Do not italicize the text
            }
            text:  currentTime // Display formatted current time
            color:"white"

        }
    }
}

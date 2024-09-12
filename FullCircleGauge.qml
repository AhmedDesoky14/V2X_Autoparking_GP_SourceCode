// // FullCircleGauge.qml

// import QtQuick 2.15
// import QtQuick.Controls 2.15
// import QtQuick.Layouts 1.15
// import ReadFile 1.0

// Rectangle {
//     width: gaugeWidth
//     height: gaugeHeight
//     property int gaugeWidth: 0
//     property int gaugeHeight: 200
//     property int minValue: 0
//     property int maxValue: 200
//     property int spanAngle: 270
//     property int angleOffest: 135
//     property int currentValue: 70
//     property int numofTicks: 10
//     property string filePathGauge: "D:\\QT projects\\gaugeDial_2\\gauge1.txt"
//     property bool runningTimer: false

//     property int animatedValue: currentValue
//     // Optionally add a setter function to control how the animatedValue modifies the currentValue
//     onAnimatedValueChanged: {
//         currentValue = animatedValue;
//         gaugeCanvas.requestPaint();
//     }

//     function calculateNeedleAngle(value, minValue, maxValue) {
//         var gaugeRange = spanAngle;
//         var angleOffset = angleOffest;
//         return angleOffset + (gaugeRange * (value - minValue) / (maxValue - minValue));
//     }
//     function startGaugeReadTimer() {
//         gaugeReadTimer.running = true;
//     }

//     Rectangle {
//         width: parent.width
//         height: parent.height
//         color: "transparent"

//         Canvas {
//             id: gaugeCanvas
//             anchors.fill: parent
//             onPaint: {
//                 var ctx = getContext("2d");
//                 var centerX = width / 2;
//                 var centerY = height / 2;
//                 var radius = Math.min(width, height) / 2 * 0.8;
//                 var startAngle = angleOffest * Math.PI / 180;
//                 var gaugeRange = spanAngle;
//                 var endAngle = startAngle + gaugeRange * Math.PI / 180;

//                 // Clear the canvas
//                 ctx.clearRect(0, 0, width, height);

//                 // Draw the outer arc
//                 ctx.beginPath();
//                 ctx.arc(centerX, centerY, radius, startAngle, endAngle, false);
//                 ctx.lineWidth = 5;
//                 ctx.strokeStyle = 'white';
//                 ctx.stroke();

//                 // Draw the major ticks and labels
//                 var numTicks = numofTicks;
//                 var numMinorTicks = numTicks * 5;
//                 for (var i = 0; i <= numTicks; i++) {
//                     var angle = startAngle + (endAngle - startAngle) / numTicks * i;
//                     var xStart = centerX + radius * Math.cos(angle);
//                     var yStart = centerY + radius * Math.sin(angle);
//                     var xEnd = centerX + (radius - 20) * Math.cos(angle);
//                     var yEnd = centerY + (radius - 20) * Math.sin(angle);
//                     ctx.beginPath();
//                     ctx.moveTo(xStart, yStart);
//                     ctx.lineTo(xEnd, yEnd);
//                     ctx.lineWidth = 2;
//                     ctx.strokeStyle = 'white';
//                     ctx.stroke();

//                     // Draw labels
//                     var tickValue = 0 + (maxValue - minValue) / numTicks * i;
//                     var xLabel = centerX + (radius - 40) * Math.cos(angle);
//                     var yLabel = centerY + (radius - 40) * Math.sin(angle);
//                     ctx.font = '14px Arial';
//                     ctx.fillStyle = 'white';
//                     ctx.textAlign = 'center';
//                     ctx.fillText(tickValue.toFixed(0), xLabel, yLabel);
//                 }

//                 // Draw the minor ticks
//                 for (i = 0; i < numMinorTicks; i++) {
//                     if (i % (numMinorTicks / numTicks) === 0) {
//                         continue; // Skip where major ticks are drawn
//                     }
//                     angle = startAngle + (endAngle - startAngle) / numMinorTicks * i;
//                     xStart = centerX + radius * Math.cos(angle);
//                     yStart = centerY + radius * Math.sin(angle);
//                     xEnd = centerX + (radius - 10) * Math.cos(angle);
//                     yEnd = centerY + (radius - 10) * Math.sin(angle);
//                     ctx.beginPath();
//                     ctx.moveTo(xStart, yStart);
//                     ctx.lineTo(xEnd, yEnd);
//                     ctx.lineWidth = 1;
//                     ctx.strokeStyle = 'white';
//                     ctx.stroke();
//                 }
//                 // Draw the needle
//                 var needleAngle =calculateNeedleAngle(currentValue, minValue, maxValue) * Math.PI / 180;
//                 var needleX = centerX + Math.cos(needleAngle) * (radius - 20);
//                 var needleY = centerY + Math.sin(needleAngle) * (radius - 20);
//                 ctx.beginPath();
//                 ctx.moveTo(centerX, centerY);
//                 ctx.lineTo(needleX, needleY);
//                 ctx.lineWidth = 3;
//                 ctx.strokeStyle = "#FF0000";
//                 ctx.stroke();
//             }
//         }

//         Rectangle {
//             id: centerDot
//             width: 10
//             height: 10
//             color: "white"
//             radius: 5
//             anchors.centerIn: parent
//             z: 2
//         }
//     }

//     ReadFile {
//         id: fileManager
//     }

//     Timer {
//         id:gaugeReadTimer
//         interval: 100 // 1 second interval
//         running: false
//         repeat: true
//         onTriggered: {
//             var filePath = filePathGauge;
//             var fileContent = fileManager.readFileFunc(filePath);
//             var newValue = parseInt(fileContent);
//             if (!isNaN(newValue) && newValue >= minValue && newValue <= maxValue) {
//                 currentValue = newValue;
//                 gaugeCanvas.requestPaint();

//             }
//         }
//    }
// }
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ReadFile 1.0

Rectangle {
    id: gaugeContainer
    width: gaugeWidth
    height: gaugeHeight
    property int gaugeWidth: 0
    property int gaugeHeight: 200
    property int minValue: 0
    property int maxValue: 200
    property int spanAngle: 270
    property int angleOffest: 135
    property int currentValue: 70
    property int numofTicks: 10
    property string filePathGauge: "D:\\QT projects\\gaugeDial_2\\gauge1.txt"
    property bool runningTimer: false

    property int animatedValue: currentValue
    onAnimatedValueChanged: {
        gaugeCanvas.requestPaint();
    }

    function calculateNeedleAngle(value, minValue, maxValue) {
        var gaugeRange = spanAngle;
        var angleOffset = angleOffest;
        return angleOffset + (gaugeRange * (value - minValue) / (maxValue - minValue));
    }

    function startGaugeReadTimer() {
        gaugeReadTimer.running = true;
    }

    Rectangle {
        width: parent.width
        height: parent.height
        color: "transparent"

        Canvas {
            id: gaugeCanvas
            anchors.fill: parent
            onPaint: {
                var ctx = getContext("2d");
                var centerX = width / 2;
                var centerY = height / 2;
                var radius = Math.min(width, height) / 2 * 0.8;
                var startAngle = angleOffest * Math.PI / 180;
                var gaugeRange = spanAngle;
                var endAngle = startAngle + gaugeRange * Math.PI / 180;

                // Clear the canvas
                ctx.clearRect(0, 0, width, height);

                // Draw the outer arc
                ctx.beginPath();
                ctx.arc(centerX, centerY, radius, startAngle, endAngle, false);
                ctx.lineWidth = 5;
                ctx.strokeStyle = 'white';
                ctx.stroke();

                // Draw the major ticks and labels
                var numTicks = numofTicks;
                var numMinorTicks = numTicks * 5;
                for (var i = 0; i <= numTicks; i++) {
                    var angle = startAngle + (endAngle - startAngle) / numTicks * i;
                    var xStart = centerX + radius * Math.cos(angle);
                    var yStart = centerY + radius * Math.sin(angle);
                    var xEnd = centerX + (radius - 20) * Math.cos(angle);
                    var yEnd = centerY + (radius - 20) * Math.sin(angle);
                    ctx.beginPath();
                    ctx.moveTo(xStart, yStart);
                    ctx.lineTo(xEnd, yEnd);
                    ctx.lineWidth = 2;
                    ctx.strokeStyle = 'white';
                    ctx.stroke();

                    // Draw labels
                    var tickValue = 0 + (maxValue - minValue) / numTicks * i;
                    var xLabel = centerX + (radius - 40) * Math.cos(angle);
                    var yLabel = centerY + (radius - 40) * Math.sin(angle);
                    ctx.font = '14px Arial';
                    ctx.fillStyle = 'white';
                    ctx.textAlign = 'center';
                    ctx.fillText(tickValue.toFixed(0), xLabel, yLabel);
                }

                // Draw the minor ticks
                for (i = 0; i < numMinorTicks; i++) {
                    if (i % (numMinorTicks / numTicks) === 0) {
                        continue; // Skip where major ticks are drawn
                    }
                    angle = startAngle + (endAngle - startAngle) / numMinorTicks * i;
                    xStart = centerX + radius * Math.cos(angle);
                    yStart = centerY + radius * Math.sin(angle);
                    xEnd = centerX + (radius - 10) * Math.cos(angle);
                    yEnd = centerY + (radius - 10) * Math.sin(angle);
                    ctx.beginPath();
                    ctx.moveTo(xStart, yStart);
                    ctx.lineTo(xEnd, yEnd);
                    ctx.lineWidth = 1;
                    ctx.strokeStyle = 'white';
                    ctx.stroke();
                }

                // Draw the needle
                var needleAngle = calculateNeedleAngle(animatedValue, minValue, maxValue) * Math.PI / 180;
                var needleX = centerX + Math.cos(needleAngle) * (radius - 20);
                var needleY = centerY + Math.sin(needleAngle) * (radius - 20);
                ctx.beginPath();
                ctx.moveTo(centerX, centerY);
                ctx.lineTo(needleX, needleY);
                ctx.lineWidth = 3;
                ctx.strokeStyle = "#FF0000";
                ctx.stroke();
            }
        }

        Rectangle {
            id: centerDot
            width: 10
            height: 10
            color: "white"
            radius: 5
            anchors.centerIn: parent
            z: 2
        }
    }

    ReadFile {
        id: fileManager
    }

    Timer {
        id: gaugeReadTimer
        interval: 1000 // 1 second interval
        running: false
        repeat: true
        onTriggered: {
            var filePath = filePathGauge;
            var fileContent = fileManager.readFileFunc(filePath);
            var newValue = parseInt(fileContent);
            if (!isNaN(newValue) && newValue >= minValue && newValue <= maxValue) {
                valueAnimator.from = animatedValue;
                valueAnimator.to = newValue;
                valueAnimator.restart();
            }
        }
    }

    NumberAnimation {
        id: valueAnimator
        target: gaugeContainer
        property: "animatedValue"
        from: currentValue
        to: currentValue
        duration: 1000 // 1 second duration
        onStopped: {
            currentValue = animatedValue;
        }
    }
}

import QtQuick 2.0

Canvas {
    id: root
    property real lastX: 0
    property real lastY: 0
    property color sliceColor: "blue"

    property real manualFirstX: 0
    property real manualFirstY: 0
    property real manualLastX: 0
    property real manualLastY: 0

    function clear() {
        var ctx = getContext("2d");
        ctx.reset();
        root.requestPaint()
    }

    function eraseRect(x, y, width, height) {
        var ctx = getContext("2d");
        ctx.clearRect(x, y, width, height)
        root.requestPaint()
    }

    function drawLine(x_start, y_start, x_end, y_end) {
        var ctx = getContext("2d")
        ctx.globalCompositeOperation="source-over";
        ctx.lineWidth = 2
        ctx.strokeStyle = root.sliceColor
        ctx.beginPath()
        ctx.moveTo(x_start, y_start)
        ctx.lineTo(x_end, y_end)
        ctx.stroke()
        ctx.closePath()
        root.requestPaint()
    }

    onPaint: {
        var ctx = getContext("2d")
        ctx.beginPath()
        if (pen.checked || armRemover.checked) {
            ctx.globalCompositeOperation="source-over";
            ctx.lineWidth = 2
            ctx.strokeStyle = root.sliceColor
            ctx.beginPath()
            ctx.moveTo(lastX, lastY)
            ctx.lineTo(area.mouseX, area.mouseY)
            ctx.stroke()
        }
        else if (eraser.checked) {
            ctx.globalCompositeOperation="destination-out";
            ctx.arc(lastX,lastY,8,0,Math.PI*2,false);
            ctx.fill();
        }
        ctx.closePath()
        lastX = area.mouseX
        lastY = area.mouseY
    }

    MouseArea {
        id: area
        anchors.fill: parent
        onPressed: {
            if (armRemover.checked) {
                manualFirstX = mouseX
                manualFirstY = mouseY
            }

            root.lastX = mouseX
            root.lastY = mouseY
        }
        onPositionChanged: {
            root.requestPaint()
        }
        onReleased: {
            if (armRemover.checked) {
                manualLastX = mouseX
                manualLastY = mouseY
                myProcessor.manualRemoveConnectedArms(manualFirstX, manualFirstY, manualLastX, manualLastY, sliceCanvas.width, sliceCanvas.height)
            }
        }
    }
}

import QtQuick 2.0

Canvas {
    id: root
    property int lastX: 0
    property int lastY: 0

    function clear() {
        var ctx = getContext("2d");
        ctx.reset();
        root.requestPaint()
    }

    function drawLine(x_start, y_start, x_end, y_end) {
        var ctx = getContext("2d")
        ctx.lineWidth = 2
        ctx.strokeStyle = color.red
        ctx.beginPath()
        ctx.moveTo(x_start, y_start)
        ctx.lineTo(x_end, y_end)
        ctx.stroke()
        root.requestPaint()
    }

    onPaint: {
        var ctx = getContext("2d")
        ctx.lineWidth = 2
        ctx.strokeStyle = "blue"
        ctx.beginPath()
        ctx.moveTo(lastX, lastY)
        lastX = area.mouseX
        lastY = area.mouseY
        ctx.lineTo(lastX, lastY)
        ctx.stroke()
    }

    MouseArea {
        id: area
        anchors.fill: parent
        onPressed: {
            root.lastX = mouseX
            root.lastY = mouseY
        }
        onPositionChanged: {
            root.requestPaint()
        }
    }
}

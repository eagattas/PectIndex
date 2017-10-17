import QtQuick 2.0

Canvas {
    id: root
    property int lastX: 0
    property int lastY: 0
    property int mode: 0
    property color sliceColor: "blue"

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
        if (mode == 0) {
            ctx.globalCompositeOperation="source-over";
            ctx.lineWidth = 2
            ctx.strokeStyle = root.sliceColor
            ctx.beginPath()
            ctx.moveTo(lastX, lastY)
            ctx.lineTo(area.mouseX, area.mouseY)
            ctx.stroke()
        }
        else if (mode == 1) {
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
            root.lastX = mouseX
            root.lastY = mouseY
            console.log("mouseX: " + mouseX + ", mouseY: " + mouseY)
        }
        onPositionChanged: {
            root.requestPaint()
        }
    }
}

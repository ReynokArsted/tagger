import untitled
import QtQuick
import QtQuick.Controls

Item {
    id: control
    property alias text: label.text
    property int fontSize: 14
    property bool showBorder: true
    signal clicked()

    implicitWidth: label.implicitWidth + 24
    implicitHeight: 32
    height: implicitHeight
    width: implicitWidth

    Rectangle {
        anchors.fill: parent
        radius: 4
        border.width: control.showBorder ? 1 : 0
        border.color: Theme.borderColor
        color: control.pressed ? Theme.fieldBackground : Theme.backgroundColor
    }

    Text {
        id: label
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: control.fontSize
        color: Theme.textColor
        elide: Text.ElideRight
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPressed: control.pressed = true
        onReleased: { control.pressed = false; control.clicked() }
        onCanceled: control.pressed = false
    }

    property bool pressed: false
}

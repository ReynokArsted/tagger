import untitled
import QtQuick
import QtQuick.Controls

Button {
    id: control

    background: Rectangle {
        color: control.pressed ? Theme.fieldBackground : Theme.backgroundColor
        border.color: Theme.borderColor
        border.width: 1
        radius: 4
    }

    contentItem: Text {
        text: control.text
        color: Theme.textColor
        font.pixelSize: 14
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
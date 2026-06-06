import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("tagger")
    
    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 20
        
        Label {
            text: "Enter your text:"
            font.pixelSize: 14
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        TextField {
            id: input
            width: 300
            height: 40
    
            text: "Change this!"
            color: "steelblue"
            font.pixelSize: 16
            padding: 12
    
            background: Rectangle {
                color: "white"
                border.color: input.focus ? "#007acc" : "steelblue"
                border.width: input.focus ? 3 : 2
                radius: 8
            }
            onTextChanged: console.log(`Text has changed to: ${text}`)
        }

        Label {
            text: `Current: ${input.text}`
            font.pixelSize: 12
            color: "gray"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
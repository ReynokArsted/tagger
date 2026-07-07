import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Window
import QtQuick.Layouts

import untitled 1.0
import untitled.files 1.0
import "./" as Example

ApplicationWindow {
    id: win
    width: 800
    height: 450
    maximumWidth: 800
    maximumHeight: 450
    minimumWidth: 800
    minimumHeight: 450

    flags: Qt.Window
        | Qt.WindowTitleHint
        | Qt.WindowSystemMenuHint
        | Qt.WindowMinimizeButtonHint
        | Qt.FramelessWindowHint

    visible: true
    title: qsTr("tagger")
    color: "transparent"
    
    Item {
        id: content
        anchors.fill: parent
        property int r: 8
        property int frame: 2
        property string tagText: input.text
        clip: true
        layer.enabled: true
        layer.smooth: true

        Rectangle {
            anchors.fill: parent
            radius: content.r
            color: Theme.borderColor
        }

        Rectangle {
            anchors.fill: parent
            anchors.margins: content.frame
            radius: content.r - content.frame
            color: Theme.backgroundColor
        }

        Item {
            anchors.fill: parent

            Rectangle {
                id: topBar
                height: 36
                width: parent.width
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: content.frame
                color: Theme.backgroundColor
                radius: content.r - content.frame
                clip: true

                MouseArea { anchors.fill: parent; onPressed: win.startSystemMove() }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 8

                    Text { text: win.title; color: Theme.textColor }
                    Item { Layout.fillWidth: true }
                    Example.ThemedButton { text: "—"; showBorder: false; onClicked: win.showMinimized() }
                    Example.ThemedButton { text: "x"; showBorder: false; onClicked: win.close() }
                }
            }

            ColumnLayout {
                anchors.margins: 10
                anchors.fill: parent
                anchors.topMargin: topBar.height

                Row {
                    spacing: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Column {
                        spacing: 10

                        Label {
                            text: qsTr("Вводи теги:")
                            color: Theme.textColor
                            font.pixelSize: 14
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        RowLayout {
                            spacing: 10
                            Layout.fillWidth: true

                            Rectangle {
                                Layout.preferredWidth: 200
                                Layout.preferredHeight: 40
                                color: Theme.fieldBackground
                                border.color: Theme.borderColor
                                border.width: 1
                                radius: 4

                                TextInput {
                                    id: input
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    text: qsTr("Поле ввода")
                                    color: Theme.textColor
                                    font.pixelSize: 16

                                    horizontalAlignment: Text.AlignLeft
                                    clip: true

                                    onTextChanged: console.log(`Text has changed to: ${text}`)
                                    onAccepted: fileModel.setFolder(input.text)
                                }
                            }

                            Example.ThemedButton {
                                text: qsTr("..")
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 40
                                onClicked: {
                                    const p = fileModel.parentFolder()
                                    if (p !== "" && fileModel.hasFolder) {
                                        fileModel.setFolder(p)
                                        input.text = p
                                    }
                                }
                            }
                        }

                        Label {
                            text: qsTr("Текущий ввод: %1").arg(input.text)
                            font.pixelSize: 12
                            color: "gray"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Button {
                            text: qsTr("Add")
                            onClicked: ThingModel.listOfThingies.addThing(input.text)
                        }

                        Example.ThingGrid {
                            id: g
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        Example.ThemedButton {
                            text: Theme.isDarkMode ? qsTr("Установить светлую тему") : qsTr("Установить тёмную тему")
                            onClicked: Theme.isDarkMode = !Theme.isDarkMode
                        }

                        Example.ThemedButton {
                            text: Translator.language === "en" ? qsTr("Изменить язык на русский") : qsTr("Изменить язык на английский")
                            onClicked: Translator.language = Translator.language === "en" ? "ru" : "en"
                        }
                    }

                    FileListModel { id: fileModel }

                    Column {
                        width: 450
                        height: 450

                        ListView {
                            id: fileListView
                            anchors.fill: parent
                            anchors.margins: 8
                            clip: true

                            property alias tagInput: input

                            spacing: 6
                            model: fileModel

                            delegate: Rectangle {
                                id: card
                                width: fileListView.width
                                height: 36

                                radius: 8
                                color: Theme.fieldBackground
                                border.color: Theme.borderColor
                                border.width: 1

                                property bool pressed: false
                                scale: pressed ? 0.98 : 1.0
                                Behavior on scale { NumberAnimation { duration: 120; easing.type: Easing.OutQuad } }

                                Text {
                                    anchors.fill: parent
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 12
                                    verticalAlignment: Text.AlignVCenter
                                    text: isDir ? (name + "/") : name
                                    color: Theme.textColor
                                    elide: Text.ElideRight
                                }

                                Menu {
                                    id: ctxMenu
                                    MenuItem { 
                                        onTriggered: console.log("Open:", path)
                                        contentItem: Text {
                                            text: qsTr("Открыть")
                                            color: Theme.textColor
                                        } 
                                    }
                                    MenuItem 
                                    {  
                                        onTriggered: console.log("Rename:", path)
                                        contentItem: Text {
                                            text: qsTr("Переименовать")
                                            color: Theme.textColor
                                        }  
                                    }
                                    MenuItem { 
                                        onTriggered: console.log("Delete:", path)
                                        contentItem: Text {
                                            text: qsTr("Удалить")
                                            color: Theme.textColor
                                        }   
                                    }
                                    background: Rectangle {
                                        implicitWidth: 200
                                        implicitHeight: 40
                                        color: Theme.fieldBackground
                                        radius: 8
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                                    onPressed: {
                                        if (mouse.button === Qt.RightButton) {
                                            card.pressed = false
                                            ctxMenu.x = mouse.x
                                            ctxMenu.y = mouse.y + 6
                                            ctxMenu.open()
                                        } else if (mouse.button === Qt.LeftButton) {
                                            card.pressed = true
                                        }
 
                                    }

                                    onReleased: if (mouse.button === Qt.LeftButton) 
                                    {
                                        card.pressed = false
                                        fileListView.tagInput.text = fileListView.tagInput.text + name + "/"
                                        fileModel.setFolder(fileListView.tagInput.text)
                                    }
                                    onCanceled: card.pressed = false

                                    onClicked: if (mouse.button === Qt.LeftButton) console.log("Selected:", path)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
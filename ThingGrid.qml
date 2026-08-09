import QtQuick 
import QtQml.Models
import QtQuick.Controls
import QtQuick.Effects

import untitled 1.0
import "./" as Example

GridView {
    id: root

    property bool highlightBorders: false
    property bool isTagSelectionMode: false
    property QtObject hub
    property var selectedTagIds: [] 

    property Item dragOverlay 

    width: 600
    height: 200
    cellWidth: 100
    cellHeight: 100
    clip: true

    displaced: Transition {
        NumberAnimation {
            properties: "x,y"
            easing.type: Easing.OutQuad
        }
    }

    model: DelegateModel {
        id: visualModel

        model: ThingModel.listOfThingies

        delegate: DropArea {
            id: delegateRoot

            required property color color
            required property string name
            required property int id
            property int modelIndex
            property int visualIndex: DelegateModel.itemsIndex

            readonly property bool isSelected:
                root.isTagSelectionMode && root.selectedTagIds.indexOf(id) !== -1

            width: root.cellWidth
            height: root.cellHeight

            onEntered: function (drag) {
                var from = (drag.source as Example.ThingTile).visualIndex
                var to = thingTile.visualIndex
                visualModel.items.move(from, to)
            }

            onDropped: function (drag) {
                var from = modelIndex
                var to = (drag.source as Example.ThingTile).visualIndex
                ThingModel.listOfThingies.move(from, to)
            }

            Example.ThingTile {
                id: thingTile

                dragParent: root.dragOverlay 

                width: root.cellWidth * 0.8
                height: root.cellHeight * 0.8
                //dragParent: root
                visualIndex: delegateRoot.visualIndex
                color: delegateRoot.color
                borderHighlight: delegateRoot.isSelected 

    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#40000000"
        shadowBlur: 0.5
        shadowHorizontalOffset: 0
        shadowVerticalOffset: 2
    }

                onPressed: {
                    delegateRoot.modelIndex = visualIndex 
                }

                onClicked: {
                    root.hub.selected(delegateRoot.id, delegateRoot.name)
                    ThingModel.listOfThingies.move(delegateRoot.visualIndex, 0)  
                }
                
                onRightClicked: tagMenu.popup()
                Menu {
                    id: tagMenu

                    MenuItem {
                        text: qsTr("Показать файлы по метке")
                        onTriggered: 
                        {
                            root.hub.tagFilesRequested(delegateRoot.id, delegateRoot.name)
                            ThingModel.listOfThingies.move(delegateRoot.visualIndex, 0) 
                        }
                    }
                    MenuItem {
                        text: qsTr("Удалить метку")
                        onTriggered: root.hub.tagDeleteRequested(delegateRoot.id, delegateRoot.name)
                    }
                }

                Text {
                    anchors.fill: parent
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "white"
                    anchors.margins: 5
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 10
                    font.pixelSize: 30
                    text: delegateRoot.name
                }
            }
        }
    }
}
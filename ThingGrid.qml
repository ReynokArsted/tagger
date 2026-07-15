import QtQuick 
import QtQml.Models
import untitled 1.0

import "./" as Example

GridView {
    id: root

    property bool highlightBorders: false
    property bool isTagSelectionMode: false
    property QtObject hub

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

                width: root.cellWidth * 0.8
                height: root.cellHeight * 0.8
                dragParent: root
                visualIndex: delegateRoot.visualIndex
                color: delegateRoot.color
                borderHighlight: false

                onPressed: {
                    delegateRoot.modelIndex = visualIndex
                    root.hub.selected(delegateRoot.id)
                    borderHighlight = true
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
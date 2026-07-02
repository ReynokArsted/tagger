/* Author: Remy van Elst, https://raymii.org
 * License: GNU AGPLv3
 */

import QtQuick 
import QtQml.Models
import untitled 1.0

import "./" as Example

GridView {
    id: root
    width: 600
    height: 200

    cellWidth: 100
    cellHeight: 100

    displaced: Transition {
        NumberAnimation {
            properties: "x,y"
            easing.type: Easing.OutQuad
        }
    }

    model: DelegateModel {
        id: visualModel
        model: ThingModel.listOfThingies

        // each square is both a drag-able item as well as a droparea (to drop items in).
        delegate: DropArea {
            id: delegateRoot
            required property color color
            required property string name

            property int modelIndex

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

            property int visualIndex: DelegateModel.itemsIndex

            Example.ThingTile {
                id: thingTile
                width: root.cellWidth * 0.8
                height: root.cellHeight * 0.8
                dragParent: root
                visualIndex: delegateRoot.visualIndex
                color: delegateRoot.color
                onPressed: delegateRoot.modelIndex = visualIndex

                // content of the draggable square
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
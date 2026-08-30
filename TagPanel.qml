import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import untitled 1.0

ColumnLayout 
{
    id: tag_panel

    spacing: 10
    Layout.preferredWidth: 320
    Layout.minimumWidth: 320
    Layout.maximumWidth: 320
    Layout.fillHeight: true
    clip: true
    
    property var fileModel
    property var thingModel
    property var hub
    property var dragOverlay
    property bool addTagHighlight: false
    property bool tagSelectMode: false
    property var selectedTagIds: []
    property Window tpparentWin: null

    property alias tagInput: search_line.tagInput
    property alias pathText: search_line.text

    signal settingsClicked()
    signal helpClicked()
    signal taggingConfirmed(string path)
    signal taggingCancelled()
    signal thingAdded(string path)
    signal folderChanged(string path)

    Label 
    {
        //text: qsTr("Вводи метки:")
        color: Theme.textColor
        font.pixelSize: 14
        anchors.horizontalCenter: parent.horizontalCenter
    }

    SearchLine 
    {
        id: search_line

        Layout.fillWidth: true

        fileModel: tag_panel.fileModel
        parentWin: tpparentWin

        onFolderChanged: fileModel.setFolder(path)
    }

    Label 
    {
        text: qsTr("Текущий ввод: %1").arg(tagInput.text)
        font.pixelSize: 12
        color: "gray"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ThingGrid 
    {
        id: grid

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: 0
        highlightBorders: tag_panel.addTagHighlight
        isTagSelectionMode: tag_panel.tagSelectMode
        selectedTagIds: tag_panel.selectedTagIds
        hub: tag_panel.hub
        dragOverlay: tag_panel.dragOverlay
    }
                        
    RowLayout 
    {
        Layout.fillWidth: true
        Layout.preferredHeight: 52
                            
        ThemedButton 
        {
            text: "⚙"
            onClicked: tag_panel.settingsClicked()
        }
        ThemedButton 
        {
            text: "?"
            onClicked: tag_panel.helpClicked()
        }
    }
}

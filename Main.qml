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
    minimumWidth: 500
    minimumHeight: 350
    visible: true
    title: qsTr("tagger")
    color: "transparent"

    flags: Qt.Window
        | Qt.WindowTitleHint
        | Qt.WindowSystemMenuHint
        | Qt.WindowMinimizeButtonHint
        | Qt.FramelessWindowHint

    property string editingPath: ""
    property string editingName: ""
    property bool renameMode: false
    property bool addTagHighlight: false

    property bool tagSelectMode: false
    property string tagTargetPath: ""
    property string tagTargetName: ""
    property var selectedTagIds: []

    function startTagging(path, name) {
        tagTargetPath = path
        tagTargetName = name
        selectedTagIds = ThingModel.listOfThingies.tagIdsForFile(path)
        tagSelectMode = true
    }

    function toggleTagSelection(tagId) {
        const idx = selectedTagIds.indexOf(tagId)
        const arr = selectedTagIds.slice()
        if (idx >= 0) arr.splice(idx, 1)
        else arr.push(tagId)
        selectedTagIds = arr
    }

    function removeTagFromSelection(tagId) {
        const idx = selectedTagIds.indexOf(tagId)
        if (idx >= 0) {
            const arr = selectedTagIds.slice()
            arr.splice(idx, 1)
            selectedTagIds = arr
        }
    }

    function confirmTagging() {
        if (tagTargetPath !== "") {
            const ok = ThingModel.listOfThingies.assignTagsToFile(tagTargetPath, selectedTagIds)
            if (!ok) console.warn("Не удалось сохранить метки для файла: ", tagTargetPath)
        }
        cancelTagging()
    }

    function cancelTagging() {
        tagSelectMode = false
        tagTargetPath = ""
        tagTargetName = ""
        selectedTagIds = []
    }

    // onTagSelectModeChanged: {
    //     if (tagSelectMode) {
    //         tagSelectDialog.open()
    //     } else if (tagSelectDialog.visible) {
    //         tagSelectDialog.close()
    //     }
    // }

    Example.SignalHub { id: hub }
    Connections 
    {
        target: hub
        function onSelected(id, name) 
        {
            if (win.tagSelectMode) 
            {
                console.log("selected tag id: ", id)
                win.toggleTagSelection(id)
            }
            else
                if (input.text.indexOf(name) === -1) 
                {
                    const sep = input.text.length > 0 && !input.text.endsWith(" ") ? " " : ""
                    input.text += sep + name
                }
        }

        function onTagDeleteRequested(id, name) {
            console.log("Delete tag:", id, name)
            const ok = ThingModel.listOfThingies.removeThing(id)
            if (!ok) console.warn("Error removing tag:", name)
            else win.removeTagFromSelection(id)
        }

        function onTagFilesRequested(id, name) {
            console.log("Show files by tag:", id, name)
            const query = name
            input.text = query 
            fileModel.setFolder(query) 
        }
    }

    function startRename(path, name) {
        editingPath = path
        editingName = name
        renameMode = true
    }

    function commitRename(newName) {
        const trimmed = newName.trim()
        if (trimmed !== "" && editingPath !== "") {
            console.log("Rename: ", editingPath, " -> ", trimmed)
            // fileModel.rename(editingPath, trimmed)
        }
        renameMode = false
        editingPath = ""
        editingName = ""
    }

    function cancelRename() {
        renameMode = false
        editingPath = ""
        editingName = ""
    }
    
    Item {
        id: content

        property int r: 8
        property int frame: 2
        property string tagText: input.text

        anchors.fill: parent
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

                RowLayout {
                    spacing: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        spacing: 10
                        Layout.preferredWidth: 320
                        Layout.fillHeight: true

                        Label {
                            text: qsTr("Вводи метки:")
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

                            Example.ThemedButton {
                                text: qsTr("+")
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 40
                                onClicked: ThingModel.listOfThingies.addThing(input.text)
                            }

                            Example.ThemedButton {
                                visible: win.tagSelectMode ? true : false
                                text: "V"
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 40
                                onClicked: win.confirmTagging()
                            }

                            Example.ThemedButton {
                                visible: win.tagSelectMode ? true : false
                                text: qsTr("X")
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 40
                                onClicked: win.cancelTagging()
                            }
                        }

                        Label {
                            text: qsTr("Текущий ввод: %1").arg(input.text)
                            font.pixelSize: 12
                            color: "gray"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Example.ThingGrid {
                            id: g

                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            highlightBorders: win.addTagHighlight
                            isTagSelectionMode: win.tagSelectMode
                            selectedTagIds: win.selectedTagIds  
                            hub: hub
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

                    // Dialog {
                    //     id: renameDialog
                    //     modal: true
                    //     title: qsTr("Переименовать")
                    //     standardButtons: Dialog.Ok | Dialog.Cancel

                    //     property string targetPath: ""
                    //     property string targetName: ""

                    //     x: (win.width - width) / 2
                    //     y: (win.height - height) / 2
                    //     width: 320

                    //     contentItem: ColumnLayout {
                    //         spacing: 10

                    //         Label {
                    //             text: qsTr("Новое имя:")
                    //             color: Theme.textColor
                    //         }

                    //         TextField {
                    //             id: renameField
                    //             Layout.fillWidth: true
                    //             text: renameDialog.targetName
                    //             selectByMouse: true
                    //         }
                    //     }

                    //     onOpened: renameField.forceActiveFocus()
                    //     onAccepted: {
                    //         const newName = renameField.text.trim()
                    //         if (newName !== "") {
                    //             console.log("Rename:", renameDialog.targetPath, "->", newName)
                    //             // fileModel.rename(renameDialog.targetPath, newName)
                    //         }
                    //     }
                    // }

                    // Dialog {
                    //     id: tagSelectDialog

                    //     modal: true
                    //     title: qsTr("Теги для: %1").arg(win.tagTargetName)
                    //     standardButtons: Dialog.Ok | Dialog.Cancel
                    //     x: (win.width - width) / 2
                    //     y: (win.height - height) / 2
                    //     width: 340

                    //     onAccepted: win.confirmTagging()
                    //     onRejected: win.cancelTagging()
                    //     onClosed: if (win.tagSelectMode) win.cancelTagging()

                    //     contentItem: ColumnLayout {
                    //         spacing: 10

                    //         Label {
                    //             text: qsTr("Выберите один или несколько тегов:")
                    //             color: Theme.textColor
                    //             wrapMode: Text.WordWrap
                    //             Layout.fillWidth: true
                    //         }

                    //         Flow {
                    //             Layout.fillWidth: true
                    //             Layout.preferredHeight: 160
                    //             spacing: 6

                    //             Repeater {
                    //                 model: ThingModel.listOfThingies

                    //                 delegate: Rectangle {
                    //                     property bool selected: win.selectedTagIds.indexOf(model.id) !== -1
                    //                     width: tagLabel.implicitWidth + 24
                    //                     height: 30
                    //                     radius: 15
                    //                     color: selected ? Theme.accentColor : Theme.fieldBackground
                    //                     border.color: Theme.borderColor
                    //                     border.width: 1

                    //                     Behavior on color { ColorAnimation { duration: 100 } }

                    //                     Text {
                    //                         id: tagLabel
                    //                         anchors.centerIn: parent
                    //                         text: model.name
                    //                         color: selected ? "white" : Theme.textColor
                    //                     }

                    //                     MouseArea {
                    //                         anchors.fill: parent
                    //                         onClicked: win.toggleTagSelection(model.id)
                    //                     }
                    //                 }
                    //             }
                    //         }

                    //         Label {
                    //             text: qsTr("Выбрано тегов: %1").arg(win.selectedTagIds.length)
                    //             font.pixelSize: 12
                    //             color: "gray"
                    //         }
                    //     }
                    // }

                    ColumnLayout {
                        //Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: 300

                        ListView {
                            id: fileListView

                            property alias tagInput: input

                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.margins: 8 
                            clip: true
                            spacing: 6
                            model: fileModel

                            delegate: Rectangle {
                                id: card

                                property bool pressed: false

                                width: ListView.view.width
                                height: 36
                                radius: 8
                                color: ListView.isCurrentItem ? Theme.selectionColor : Theme.fieldBackground
                                border.color: ListView.isCurrentItem ? Theme.accentColor : Theme.borderColor
                                border.width: ListView.isCurrentItem ? 2 : 1
                                scale: pressed ? 0.98 : 1.0

                                Behavior on scale { NumberAnimation { duration: 120; easing.type: Easing.OutQuad } }
                                Behavior on color { ColorAnimation { duration: 100 } }

                                Row {
                                    anchors.fill: parent
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 12
                                    spacing: 8

                                    TextInput {
                                        id: renameInput

                                        visible: win.renameMode && win.editingPath === path
                                        text: win.editingName
                                        color: Theme.textColor
                                        font.pixelSize: 16
                                        selectByMouse: true
                                        activeFocusOnPress: true
                                        horizontalAlignment: Text.AlignLeft
                                        verticalAlignment: Text.AlignVCenter
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: parent.width

                                        onVisibleChanged: if (visible) {
                                            text = win.editingName
                                            forceActiveFocus()
                                            selectAll()
                                        }

                                        onAccepted: win.commitRename(text)
                                        onEditingFinished: if (visible) win.commitRename(text)
                                        Keys.onEscapePressed: win.cancelRename()
                                    }

                                    Text {
                                        visible: !renameInput.visible
                                        text: isDir ? (name + "/") : name
                                        color: Theme.textColor
                                        elide: Text.ElideRight
                                        verticalAlignment: Text.AlignVCenter
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: parent.width
                                    }
                                }

                                Menu {
                                    id: ctxMenu

                                    property string currentPath: ""
                                    property string currentName: ""
                                    property bool currentIsDir: false

                                    MenuItem {
                                        text: qsTr("Задать метки")
                                        onTriggered: {
                                            console.log("Tag added for: ", ctxMenu.currentPath)
                                            //win.addTagHighlight = true
                                            win.startTagging(ctxMenu.currentPath, ctxMenu.currentName)
                                        }
                                    }
                                    MenuItem {
                                        text: qsTr("Открыть")
                                        onTriggered: {
                                            if (ctxMenu.currentIsDir) {
                                                fileModel.setFolder(ctxMenu.currentPath)
                                                fileListView.tagInput.text = ctxMenu.currentPath
                                            } 
                                            else fileModel.openFile(ctxMenu.currentPath, win)
                                        }
                                    }
                                    MenuItem {
                                        text: qsTr("Открыть с помощью ...")
                                        onTriggered: {
                                            if (ctxMenu.currentIsDir) {
                                                fileModel.setFolder(ctxMenu.currentPath)
                                                fileListView.tagInput.text = ctxMenu.currentPath
                                            } 
                                            else fileModel.openWith(ctxMenu.currentPath, win)
                                        }
                                    }
                                    MenuItem {
                                        text: qsTr("Переименовать")
                                        onTriggered: win.startRename(ctxMenu.currentPath, ctxMenu.currentName)
                                    }
                                    MenuItem {
                                        text: qsTr("Удалить")
                                        onTriggered: console.log("Delete: ", ctxMenu.currentPath)
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                                    onPressed: {
                                        if (mouse.button === Qt.RightButton) {
                                            card.pressed = false
                                            ctxMenu.currentPath = path
                                            ctxMenu.currentName = name
                                            ctxMenu.currentIsDir = isDir
                                            ctxMenu.popup(card, mouse.x, mouse.y + 6)
                                        } 
                                        else if (mouse.button === Qt.LeftButton) card.pressed = true
                                    }

                                    onReleased: if (mouse.button === Qt.LeftButton) card.pressed = false

                                    onClicked: if (mouse.button === Qt.LeftButton) fileListView.currentIndex = index
                                    onDoubleClicked: {
                                        if (mouse.button === Qt.LeftButton) { 
                                            if (isDir) {
                                                fileListView.tagInput.text = fileListView.tagInput.text + name + "/"
                                                fileModel.setFolder(fileListView.tagInput.text)
                                            } 
                                            else fileModel.openFile(path)
                                        }
                                    }
                                    onCanceled: card.pressed = false
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    readonly property int resizeMargin: 6
    MouseArea { // top bond
        height: win.resizeMargin
        anchors { left: parent.left; right: parent.right; top: parent.top }
        cursorShape: Qt.SizeVerCursor
        onPressed: win.startSystemResize(Qt.TopEdge)
    }
    MouseArea { // bot bond
        height: win.resizeMargin
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        cursorShape: Qt.SizeVerCursor
        onPressed: win.startSystemResize(Qt.BottomEdge)
    }
    MouseArea { // left bond
        width: win.resizeMargin
        anchors { top: parent.top; bottom: parent.bottom; left: parent.left }
        cursorShape: Qt.SizeHorCursor
        onPressed: win.startSystemResize(Qt.LeftEdge)
    }
    MouseArea { // rifht bond
        width: win.resizeMargin
        anchors { top: parent.top; bottom: parent.bottom; right: parent.right }
        cursorShape: Qt.SizeHorCursor
        onPressed: win.startSystemResize(Qt.RightEdge)
    }

    // corners
    MouseArea {
        width: win.resizeMargin * 2
        height: win.resizeMargin * 2
        anchors { top: parent.top; left: parent.left }
        cursorShape: Qt.SizeFDiagCursor
        onPressed: win.startSystemResize(Qt.TopEdge | Qt.LeftEdge)
    }
    MouseArea {
        width: win.resizeMargin * 2
        height: win.resizeMargin * 2
        anchors { top: parent.top; right: parent.right }
        cursorShape: Qt.SizeBDiagCursor
        onPressed: win.startSystemResize(Qt.TopEdge | Qt.RightEdge)
    }
    MouseArea {
        width: win.resizeMargin * 2
        height: win.resizeMargin * 2
        anchors { bottom: parent.bottom; left: parent.left }
        cursorShape: Qt.SizeBDiagCursor
        onPressed: win.startSystemResize(Qt.BottomEdge | Qt.LeftEdge)
    }
    MouseArea {
        width: win.resizeMargin * 2
        height: win.resizeMargin * 2
        anchors { bottom: parent.bottom; right: parent.right }
        cursorShape: Qt.SizeFDiagCursor
        onPressed: win.startSystemResize(Qt.BottomEdge | Qt.RightEdge)
    }
}
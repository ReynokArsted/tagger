import QtQuick
import QtQuick.Controls

ApplicationWindow 
{
    width: 640
    height: 480
    visible: true
    title: qsTr("tagger")

    color: Theme.backgroundColor

    Row 
    {
        spacing: 2
        
        Column 
        {
            //anchors.horizontalCenter: parent.horizontalCenter
            //anchors.verticalCenter: parent.verticalCenter
            spacing: 20
        
            Label 
            {
                text: qsTr("Вводи теги:")
                color: Theme.textColor
                font.pixelSize: 14
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                width: 200
                height: 40
                color: Theme.fieldBackground
                border.color: Theme.borderColor
                border.width: 1
                radius: 4          

                TextInput
                {
                    id: input
                    width: 300
                    height: 40
    
                    text: qsTr("Поле ввода")
                    color: Theme.textColor
                    font.pixelSize: 16
                    padding: 12
    
                    onTextChanged: console.log(`Text has changed to: ${text}`)
                }
            }

            Label 
            {
                text: qsTr("Текущий ввод: %1").arg(input.text)
                font.pixelSize: 12
                color: "gray"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ThemedButton {
                text: Theme.isDarkMode ? qsTr("Установить светлую тему") : qsTr("Установить тёмную тему")
                onClicked: Theme.isDarkMode = !Theme.isDarkMode
            }
            ThemedButton {
                text: AppTranslator.language === "en" ? qsTr("Изменить язык на русский") : qsTr("Изменить язык на английский")
                onClicked: AppTranslator.language = AppTranslator.language === "en" ? "ru" : "en"
            }
        }
        Rectangle { color: "red"; width: 50; height: 50 }
        Rectangle { color: "green"; width: 20; height: 50 }
        Rectangle { color: "blue"; width: 50; height: 20 }
    }
}
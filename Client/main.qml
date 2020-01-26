import QtQuick 2.9
import QtQuick.Controls 2.2

ApplicationWindow {
    visible: true
    width: 320
    height: 640
    font.pointSize: 14
    title: qsTr("QtChat")
    header: Rectangle {
        height: 120
        color: "orange"
        Row {
            width: parent.width
            height: 60
            leftPadding: 5
            topPadding: 15
            Image {
                id: avatar
                width: 60
                height: 60
                source: "file:/D://QtProject/chatAll/Client/Client/image/avatar01.jpg"
            }
            Label {
                leftPadding: 15
                text: "昵称"
            }
        }

        Row {
            width: parent.width
            height: 30
            topPadding: 90

            Label {
                leftPadding: 5
                text: qsTr("搜索:")
            }

            TextInput {
                id: search
                maximumLength: 25
                bottomPadding: 10
                font.pointSize: 14
                cursorVisible: true
                text: "search"
            }

        }
    }
    TabBar {
        id: tabs
        width: parent.width
        height: 40
        bottomPadding: parent.bottom
        TabButton{
            text: "消息"

         }
        TabButton{
            text: "联系人"

        }
    }
    ScrollView {
        //anchors.fill: parent
        ListView {
            width: parent.width
            model: 20
            delegate: ItemDelegate {
                text: "Item " + (index + 1)
                width: parent.width
                height: 60
            }
        }
    }
}

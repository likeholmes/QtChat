import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import io.qtchat.model 1.0

Page {
    id: indexPage
    //anchors.fill: parent

    header: Rectangle {
        height: 160
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
                source: "file:D://QtProject/chatAll/Client/image/avatar01.jpg"
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

        TabBar {
            id: tabs
            width: parent.width
            height: 40
            anchors.bottom: parent.bottom
            TabButton{
                text: "消息"
             }
            TabButton{
                text: "联系人"
            }
        }
    }

    StackLayout {
        id: stackLayout
        width: parent.width
        anchors.fill: parent
        currentIndex: tabs.currentIndex

        ScrollView {
            id: messagesScroll
            ListView {
                id: messageslist
                model: 20
                delegate: ItemDelegate {
                    text: "Item" + (index + 1)
                    width: parent.width
                    height: 60
                }
            }
        }

        ScrollView {
            id: contactsScroll
            //anchors.fill: parent
            ListView {
                id: contact_list
                model: SqlContactsModel{}
                delegate: ItemDelegate {
                    width: parent.width
                    height: 60

                    Row {

                        Image {
                            id: contactAvatar
                            source: "file:"+model.avatar
                            width: 60
                            height: 60
                        }
                        Text {
                            id: name
                            font.pixelSize: 24
                            text: model.name
                        }
                    }

                    onClicked: {
                        stackView.push("qrc:/MessagePage.qml", { conversionWithName: model.name,
                                           conversionWithAccount: model.account});

                    }
                }
            }
        }
    }
}

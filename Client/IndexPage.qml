import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import io.qtchat.model 1.0

Page {
    id: indexPage
    anchors.fill: parent
    property bool readySearch : searchInput.length > 0
    property string userName
    property string userAccount
    property string userAvatar

    header: Rectangle {
        height: 140
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
                //source: userAvatar
                source: "file:D://QtProject/chatAll/Client/image/avatar01.jpg"
            }
            Label {
                leftPadding: 15
                text: "昵称"
                //text: userName
            }
        }

        Row {
            width: parent.width
            height: 30
            topPadding: 90
            spacing: 8
            leftPadding: 5

            TextField {
                id: searchInput
                maximumLength: 20
                placeholderText: "search"
            }

            Button {
                id: searchButton
                text: "搜索"
                enabled: searchInput.length > 0
                onClicked:accounts.model = 20
            }

        }

    }

    ListView{
        id: accounts
        //model: 20
        anchors.fill: parent
        anchors.leftMargin: 1
        anchors.rightMargin: 1
        visible: readySearch
        delegate:  Rectangle {
            id: contact
            height: 60
            width: parent.width
            color: "lightblue"
            border.color: "white"
            ItemDelegate{
                width: parent.width
                height: parent.height

                Row {
                    height: parent.height
                    width: parent.width
                    spacing: 10
                    Image {
                        id: contact_avatar
                        height: parent.height
                        width: parent.height
                        //source: model.avatar
                        source: "file:D://QtProject/chatAll/Client/image/avatar01.jpg"
                    }

                    Label {
                        id: contact_account
                        //text: model.name
                        text: index
                    }
                }
                onClicked: dialog.open()

                Dialog {
                    id: dialog
                    //title: ""
                    //头像
                    //昵称
                    //签名

                    Button {
                        id: add
                        text: "添加"
                        //onClicked: model.addFriend()
                    }
                }
            }

        }
        ScrollBar.vertical: ScrollBar{}
    }

    ColumnLayout {
        anchors.fill: parent
        visible: !readySearch

        TabBar {
            id: tabs
            Layout.fillWidth: true

            height: 40
            TabButton{
                text: "消息"
             }
            TabButton{
                text: "联系人"
            }
        }

        StackLayout {
            id: stackLayout

            currentIndex: tabs.currentIndex

            ListView {
                id: messageslist
                model: 20
                delegate: ItemDelegate {
                    text: index
                    width: parent.width
                    height: 60
                }

                ScrollBar.vertical: ScrollBar{}
            }

            ListView {
                id: contact_list
                model: SqlContactsModel{}
                delegate: ItemDelegate {
                    width: parent.width
                    height: 60

                    Row {

                        Image {
                            id: contactAvatar
                            source: "file:"+model.avatar;
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
                        indexPage.StackView.view.push("qrc:/MessagePage.qml", { conversionWithName: model.name,
                                           conversionWithAccount: model.account, user: "laozi", //user: userAccount,
                                                      avatarPath: model.avatar});

                    }
                }
            }

        }
    }
}

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import io.qtchat.model 1.0
import io.qtchat.mytype 1.0

Page {
    id: indexPage
    //anchors.fill: parent

    property bool readySearch : searchInput.length > 0
    property User user

    /*Connections {
        target: client
        onMsgReceived: {
            msg = client.message;
            //如何让全局都访问一个局部对象，并且在任何地方都可以接收【接收消息】信号
            messagelist.model.sendMessage(msg.type, msg.textMsg, msg.authur, msg.recipient)
        }
    }*/

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
                source: "file:///" + user.avatarPath
            }
            Label {
                leftPadding: 15
                text: user.name
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

            Connections{
                target: client
                onSearchSuccess: accounts.model = client.searchContent
            }

            Button {
                id: searchButton
                text: "搜索"
                enabled: searchInput.length > 0
                onClicked:{
                    client.dealSearch(searchInput.text)
                }
            }

        }

    }

    ListView{
        id: accounts
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
                        source: "file:/" + model.avatarPath
                    }

                    Label {
                        id: contact_account
                        text: model.name + model.avatarPath
                    }
                }
                onClicked: dialog.open()
                User{
                    id: newFriend
                    account: model.account
                    name: model.name
                    describe: model.describe
                    avatarPath: model.avatarPath
                    isgroup: model.isgroup
                }

                //待改
                Connections{
                    target: client
                    onAddSuccess: contact_list.model.addFriend(newFriend);//更新联系人信息
                }
                //待改

                Dialog {
                    id: dialog
                    title: "是否添加该用户为好友"
                    standardButtons: Dialog.Ok | Dialog.Cancel

                    onAccepted: {
                        client.dealAddFriend(model.account);
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

            Connections{
                target: client
                onAcceptSuccess: contact_list.model.flush();
            }


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
                            source: "file:///" + model.avatar
                            width: 60
                            height: 60
                        }
                        Text {
                            id: name
                            font.pixelSize: 24
                            text: model.name
                        }
                    }

                    User{
                        id: aContact
                        name: model.name
                        avatarPath: model.avatar
                        isgroup: model.isgroup
                        account: model.account
                    }

                    onClicked: {
                        indexPage.StackView.view.push(
                                    "qrc:/MessagePage.qml", { conversionWith: aContact, user: user});
                    }
                }
            }

        }
    }
}

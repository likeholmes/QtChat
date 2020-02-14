import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0
import io.qtchat.model 1.0
import io.qtchat.mytype 1.0

Page{
    id: messagePage
    //anchors.fill: parent
    property User conversionWith
    property User user
    header: ToolBar{
        id: toolBoor
        Button {
            highlighted: true
            font.pixelSize: 11
            text: qsTr("Back")
            onClicked: messagePage.StackView.view.pop()
        }
        Text {
            font.pixelSize: 24
            anchors.centerIn: parent
            text: conversionWith.name
        }
    }

    ColumnLayout {
        id: messageColumn
        anchors.fill: parent

        ListView {
            id: messagelist
            model: SqlConversationModel{
                recipient: conversionWith.account
            }
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: rowPane.leftPadding

            spacing: 15

            delegate: Column {
                id: itemColumn
                property bool sentByMe: model.sender !== conversionWith.account
                property bool isFileExist: model.type !== Message.Text && model.fileExist(model.content)
                anchors.right: sentByMe ? parent.right : undefined
                Row {
                    id: itemRow
                    anchors.right: sentByMe ? parent.right : undefined
                    spacing: 10

                    Image {
                        width: 40
                        height: 40
                        source: "file:/" + conversionWith.avatarPath
                        visible: !sentByMe
                    }

                    Image {
                        id: img
                        width: 160
                        height: 160
                        visible: model.type !== Message.Text
                        source:  model.type === Message.Picture && isFileExsit ? "file:/" + model.content : "默认缩略图"

                        Button {
                            id: downloadButton
                            width: parent.width
                            height: parent.height
                            opacity: 0
                            enabled: !isFileExist
                            Connections {
                                 target: client
                                 onDownloadSuccess: {
                                     //打开网址对应的文件
                                    isFileExist = true
                                 }
                            }
                            onClicked: {
                                client.dealDownload(model.content)
                            }
                        }
                    }

                    Rectangle {
                         width: 160
                         height: 40
                         color: sentByMe ? "lightgrey" : "steelblue"
                         radius: 4
                         visible: model.type === Message.Text

                         Label {
                             text: model.content
                             color: sentByMe ? "black" : "white"
                             anchors.fill: parent
                             anchors.margins: 10
                         }

                    }

                }

                Label {
                    font.pixelSize: 11
                    text: Qt.formatDateTime(model.time, "MMMd hh:mm")
                    color: "grey"
                    anchors.right: sentByMe ? undefined : parent.right
                }
            }

            ScrollBar.vertical: ScrollBar{}
        }

        Pane {
            id: rowPane

            Layout.fillWidth: true

            RowLayout {

                width: parent.width

                Message {
                    id: msg
                    authur: user.account
                    recipient: conversionWith.account
                }

                Connections {
                    target: client

                    onSendSuccess: {
                        messagelist.model.sendMessage(msg)
                    }
                }

                FileDialog {
                    property bool isPicture
                    id: fileDialog
                    title: qsTr("选择一个文件")
                    selectedNameFilter: "All files (*)"
                    nameFilters: [ "Image files (*.png *.jpg)", "All files (*)" ]
                    onAccepted: {
                        isPicture = fileUrl.toString().indexOf("png") > 0 || fileUrl.toString().indexOf("jpg") > 0;
                        msg.type = isPicture ? Message.Picture : Message.File;
                        msg.filePath = fileUrl.toLocalFile();
                        client.dealSendMsg(msg);
                    }
                }

                Button {
                    id: fileButton
                    text: qsTr("文件")
                    onClicked: {
                        fileDialog.open();
                    }
                }

                TextArea {
                    id: messageText
                    placeholderText: "发送消息"
                    Layout.fillWidth: true
                }

                Button {
                    id: sendButton
                    text: qsTr("发送")
                    enabled: messageText.length > 0
                    onClicked: {
                        msg.type = Message.Text
                        msg.textMsg = messageText.text
                        client.dealSendMsg(msg)
                        messageText.clear();
                    }
                }
            }
        }
    }
}





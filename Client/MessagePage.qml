import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0
import io.qtchat.model 1.0

Page{
    id: messagePage
    //anchors.fill: parent
    property string conversionWithName
    property string conversionWithAccount
    property string avatarPath
    property string user
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
            text: conversionWithName
        }
    }

    ColumnLayout {
        id: messageColumn
        anchors.fill: parent

        ListView {
            id: messagelist
            model: SqlConversationModel{
                recipient: conversionWithAccount
            }
            //model: 50
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: rowPane.leftPadding

            spacing: 15

            delegate: Column {
                id: itemColumn
                property bool sentByMe: model.sender !== conversionWithAccount
                anchors.right: sentByMe ? parent.right : undefined
                Row {
                    id: itemRow
                    anchors.right: sentByMe ? parent.right : undefined
                    spacing: 10

                    Image {
                        width: 40
                        height: 40
                        source: "file:/"+avatarPath
                        visible: !sentByMe
                    }
                    Rectangle {
                         width: 160
                         height: 40
                         color: sentByMe ? "lightgrey" : "steelblue"
                         radius: 4
                         visible: model.type === "text"
                         //图片就之后再想怎么弄

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

                FileDialog {
                    property bool isPicture
                    property string type
                    id: fileDialog
                    title: qsTr("选择一个文件")
                    selectedNameFilter: "All files (*)"
                    nameFilters: [ "Image files (*.png *.jpg)", "All files (*)" ]
                    onAccepted: {
                        /*
                            上传文件，将文件链接添加到数据库
                            判断文件为什么类型，如picture和file
                            显示文件应在对话框中进行
                        */
                        //uploadFile(fileUrl)
                        isPicture = fileUrl.toString().indexOf("png") > 0 || fileUrl.toString().indexOf("jpg") > 0;
                        type = isPicture ? "picture" : "file";
                        messagelist.model.sendMessage(type, fileUrl, user, conversionWithAccount);
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
                        messagelist.model.sendMessage("text", messageText.text,
                                                     user, conversionWithAccount);
                        messageText.clear();
                    }
                }
            }
        }
    }
}





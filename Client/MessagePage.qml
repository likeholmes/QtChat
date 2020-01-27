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
    property string user
    header: ToolBar{
        id: toolBoor
        Button {
            highlighted: true
            font.pixelSize: 11
            text: qsTr("Back")
            onClicked: stackView.pop()
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
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 30
            delegate: Rectangle {
                id: message
                Text {
                    text: model.content
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
                        isPicture = fileUrl.search("png") > 0 || fileUrl.search("jpg") > 0;
                        type = isPicture ? "picture" : "file";
                        messagelist.sendMessage(type, fileUrl, user, conversionWithAccount);
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





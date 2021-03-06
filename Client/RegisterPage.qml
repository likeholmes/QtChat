import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0
import io.qtchat.mytype 1.0

Page {
    id: root
    //anchors.fill: parent

    GridLayout {
        id: gridLayout
        anchors.centerIn: parent
        rowSpacing: 20
        columns: 2

        Label {
            id: nameLabel
            text: "昵称"
        }

        TextField {
            id: nameInput
        }

        Label {
            id: passwordLabel
            text: "密码"
        }

        TextField {
            id: passwordInput
            echoMode: "Password"
        }

        Label {
            id: passwordMoreLabel
            text: "确认密码"
            //保证确认密码与密码一致
        }

        TextField {
            id: passwordMoreInput
            echoMode: "Password"
        }

        Label {
            id: avatarLabel
            text: "头像"
        }

        ColumnLayout {

            Rectangle {
                id: avatarRect
                height: 120
                width: 120
                border.color: "black"

                Image {
                    id: image
                    height: 120
                    width: 120
                }
            }

            FileDialog {
                id: fileDialog

                title: qsTr("选择一个文件")
                selectedNameFilter: "Image files (*.png *.jpg)"
                nameFilters: [ "Image files (*.png *.jpg)", "All files (*)" ]

                onAccepted: {
                    image.source = fileUrl
                }
            }

            Button {
                id: avataButton
                text: "选择图片"
                onClicked: fileDialog.open()
            }
        }

        Label {
            text: "个人签名"
        }

        Rectangle {
            border.color: "black"
            width: 200
            height: 100
            TextArea {
                id: describeInput
                placeholderText: "请输入"
            }
        }

        Button {
            Layout.columnSpan: 2
            id: okButton
            text: "确认"

            User {
                id: user
                name: nameInput.text
                password: passwordInput.text
                describe: describeInput.text
                avatarPath: fileDialog.fileUrl.toString()
            }

            Connections {
                target: client
                onRegisterSuccess: root.StackView.view.push("qrc:/IndexPage.qml", {user: user})
            }

            onClicked: {//将数据传到后台，通过套接字发送到服务器，进行注册
                //线程等待服务器响应，若注册失败则弹窗，需要account,否则stackView.push()
                client.dealRegister(user)
            }
        }
    }
}

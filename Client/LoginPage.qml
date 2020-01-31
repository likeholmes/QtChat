import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Page {
    id: root

    height: 320
    width: 320

    ColumnLayout {
        id: columnLayout
        spacing: 25
        anchors.centerIn: parent

        RowLayout {
            id: accountLayout
            Label {
                id: accountLabel
                text: "账号"
            }

            TextField {
                id: accountInput
                maximumLength: 20
            }

        }

        RowLayout {
            id: passwordLayout
            Label {
                id: passwordLabel
                text: "密码"
            }

            TextField {
                id: passwordInput
                echoMode: "Password"
                maximumLength: 20
            }
        }

        RowLayout {
            id: buttonLayout
            spacing: 20
            Layout.margins: (columnLayout.width - width) / 2

            Button {
                id: loginButton
                text: "登录"
                enabled: accountInput.length > 0 && passwordInput.length > 0
                onClicked: {
                    //发送数据到服务器,accountInput.text, passwordInput.text
                    //线程等待服务器响应，处理完从服务器接收的数据后转化为QML可以接收的格式
                    //根据服务器响应跳转页面,创建一个data为c++object，返回data,if(data.response === "success")root.StackView.view.push(
                    //"qrc:/IndexPage.qml", {userAccount: data.senderAccount, userName: data.sendName,
                    //userAvatar})
                    //userAvatar应在线程中存储到本地后得到的本地链接
                }
                //xx.onXX: StackView.push()
            }

            Button {
                id: registerButton
                text: "注册"
            }
        }

    }


}

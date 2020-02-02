import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import io.qtchat.mytype 1.0

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

                Connections{
                    target: client
                    onLoginSuccess: root.StackView.view.push("qrc:/IndexPage.qml", {user: client.user})
                }

                onClicked: {
                    client.dealLogin(accountInput.text, passwordInput.text);

                }
            }

            Button {
                id: registerButton
                text: "注册"
            }
        }

    }


}

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import io.qtchat.model 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 320
    height: 640
    font.pointSize: 14
    title: qsTr("QtChat")

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: LoginPage{}
    }
}


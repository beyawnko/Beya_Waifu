import QtQuick 2.12
import QtQuick.Controls 2.12
import "../Waifu2x-Extension-QT/qml" as Effects

ApplicationWindow {
    id: window
    width: 640
    height: 360
    visible: true
    title: qsTr("LiquidGlass Demo")
    Material.theme: Material.Dark

    Rectangle {
        id: background
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#404040" }
            GradientStop { position: 1.0; color: "#303030" }
        }
    }

    Text {
        id: label
        anchors.centerIn: parent
        text: qsTr("Liquid Glass")
        color: "#eeeeee"
        font.pixelSize: 32
    }

    Effects.LiquidGlass {
        anchors.centerIn: label
        width: 200
        height: 200
        sourceItem: background
    }
}

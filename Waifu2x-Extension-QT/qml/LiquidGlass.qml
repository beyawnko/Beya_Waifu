import QtQuick 2.12

/*!
    LiquidGlass shader effect.

    Public properties:
    - sourceItem: Item to capture for the effect. Defaults to parent.
    - live: Updates the capture every frame. Defaults to true.
*/
Item {
    id: root
    property Item sourceItem: parent
    property bool live: true

    ShaderEffectSource {
        id: background
        anchors.fill: parent
        sourceItem: root.sourceItem
        live: root.live
        recursive: true
        hideSource: true
        visible: false
    }

    ShaderEffect {
        anchors.fill: parent
        fragmentShader: "qrc:/shaders/liquidglass.frag.qsb"
        property var sourceTexture: background
        property var environmentTexture: background
    }
}

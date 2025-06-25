import QtQuick 6.0 // Or your Qt Quick version
import QtQuick.Window 2.15
import com.waifu2x.effects 1.0 // The module registered in main.cpp

Window {
    width: 800
    height: 600
    visible: true
    title: "RHI Liquid Glass Test"

    RhiLiquidGlass {
        id: liquidGlassEffect
        anchors.fill: parent
        ior: 1.3
        borderRadius: 25
        chromaticAberrationOffset: 0.01

        // To test background, you'd need a way to load an image.
        // For a quick test, one could expose a source property in RhiLiquidGlassItem
        // that takes a URL or path, or set it from C++.
        // For now, it will render without a background texture or with whatever default
        // the RhiLiquidGlassItem/Node initializes if background is null.

        Component.onCompleted: {
            console.log("RhiLiquidGlass item created.")
            // Try to load a test image if possible, or set a default color background in item
            // For example, if RhiLiquidGlassItem had a 'loadBackgroundImage(url)' slot:
            // liquidGlassEffect.loadBackgroundImage("qrc:/icon/icon.png") // Assuming icon.png is in resources
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true // Ensure hover events are captured by MouseArea for the item
            // RhiLiquidGlassItem handles mouseMoveEvent/hoverMoveEvent internally
        }
    }

    Image {
        id: testImage
        source: "qrc:/icon/icon.png" // Make sure this path is valid in your resources
        visible: false // Not displayed, just to load it for the effect if needed
        Component.onCompleted: {
            if (status === Image.Ready) {
                // Pass the QImage to the effect item
                // This requires a method or property in RhiLiquidGlassItem to accept a QImage
                // For example: liquidGlassEffect.backgroundImage = testImage.grabToImage();
                // Or more directly if RhiLiquidGlassItem could take a source URL:
                // liquidGlassEffect.backgroundSource = "qrc:/icon/icon.png"
                // For now, this is a placeholder to show how one might set it.
                // The current RhiLiquidGlassItem takes QImage via setBackground.
                // This needs to be called from C++ or a more complex QML setup.
                // A simple way for this test is to have RhiLiquidGlassItem load a default image in its constructor.
            } else if (status === Image.Error) {
                console.log("Test image failed to load:", source)
            }
        }
    }
}

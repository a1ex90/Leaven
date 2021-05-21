import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import "../colors.js" as Color
import "../layout.js" as Layout


Button {
    property alias iconSource: icon.source
    z: 5

    Image {
        id: icon
        anchors.centerIn: parent
        height: btn_load.background.height - 6
        fillMode: Image.PreserveAspectFit
    }
    ColorOverlay {
        anchors.fill: icon
        source: icon
        color: parent.hovered ? Color.background : Color.main
        opacity: enabled ? 1.0 : 0.3
    }

    hoverEnabled: true

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.color: parent.hovered ? Color.second : Color.main
        color: parent.hovered ? Color.second : Color.background
        border.width: 1
        radius: 2
    }

    width: 60
    height: 25
}

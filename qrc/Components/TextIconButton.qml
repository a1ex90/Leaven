import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import "../colors.js" as Color
import "../layout.js" as Layout

Button {
    property alias iconSource: icon.source

    hoverEnabled: true
    Image {
        id: icon
        x: parent.width - this.width - 6
        y: 3
        height: btn_vSample.background.height - 6
        fillMode: Image.PreserveAspectFit
    }
    ColorOverlay {
        anchors.fill: icon
        source: icon
        opacity: enabled ? 1.0 : 0.3
        color: parent.hovered ? Color.background : Color.main
    }
    contentItem: Text {
        text: parent.text
        font: parent.font
        opacity: enabled ? 1.0 : 0.3
        color: parent.hovered ? Color.background : Color.main
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.color: parent.hovered ? Color.second : Color.main
        color: parent.hovered ? Color.second : Color.background
        border.width: 1
        radius: 2
    }
    width: 129
    height: 25
}

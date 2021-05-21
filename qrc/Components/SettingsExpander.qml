import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import "../colors.js" as Color
import "../layout.js" as Layout

Button {
    property bool expanded: false
    Image {
        id: icon
        x: 6
        y: 9
        height: bg.height - 18
        fillMode: Image.PreserveAspectFit
        source: expanded ? "../minus.svg" : "../plus.svg"
    }
    ColorOverlay {
        anchors.fill: icon
        source: icon
        color: parent.hovered ? Color.second : Color.main
        opacity: enabled ? 1.0 : 0.3
    }

    contentItem: Text {
        text: parent.text
        font: parent.font
        opacity: enabled ? 1.0 : 0.3
        color: parent.hovered ? Color.second : Color.main
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        id: bg
        opacity: enabled ? 1 : 0.3
        color: Color.background
    }
    width: 129
    height: 25
    text: qsTr("Expand")
    onClicked: {
        expanded = !expanded
    }
}

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import "../colors.js" as Color
import "../layout.js" as Layout

Button {
    property bool on: false
    property string txtOn: "ON"
    property string txtOff: "OFF"
    text: on ? txtOn : txtOff
    height: Layout.label_entry_height
    x: Layout.padding_left + Layout.label_entry_width + Layout.label_padding_right
    z: 5

    contentItem: Text {
        text: parent.text
        font: parent.font
        leftPadding: -8
        color: parent.hovered ? Color.second : Color.third
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        opacity: enabled ? 1 : 0.3
        color: Color.background
    }
    onClicked: {
       on = !on;
    }
}

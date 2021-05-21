import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import "../colors.js" as Color
import "../layout.js" as Layout

TextInput {
    property var number: 0
    x: Layout.padding_left + Layout.label_entry_width + Layout.label_padding_right
    z: 5
    text: "0"
    color: activeFocus || ma.containsMouse ? Color.second : Color.third
    validator: DoubleValidator{}
    onFocusChanged: {
        if(!activeFocus) {
            if(text == "") {
                text = validator.bottom;
            }
            number = Number.fromLocaleString(Qt.locale("en_US"), text);
        }
    }

    MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        onClicked: parent.forceActiveFocus(Qt.MouseFocusReason)
    }
}

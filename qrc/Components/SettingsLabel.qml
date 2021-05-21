import QtQuick 2.7
import QtQuick.Controls 2.0
import "../colors.js" as Color

Item{
    // expose properties and signals
    property alias text: lbl.text
    property string tooltip: ""
    property bool ttShown: false

    z: 5
    Text{
        id: lbl
        color: Color.main
        ToolTip {
            id: tt
            contentItem: Text {
                text: tooltip
                color: Color.third
            }
            background: Rectangle {
                color: Color.bgOpenGl
                border.color: Color.third
            }

            delay: Qt.styleHints.mousePressAndHoldInterval
            visible: ttShown
        }

        MouseArea {
            id: ma
            anchors.fill: lbl
            hoverEnabled: true
            onHoveredChanged: {
                ttShown = !ttShown
            }
        }
    }
}

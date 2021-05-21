/******************************************************
 *
 *   #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *  %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *  %    %## #    CC        V    V  MM M  M MM RR    RR
 *   ,%      %    CC        VV  VV  MM  MM  MM RRRRRR
 *   (%      %,   CC    CC   VVVV   MM      MM RR   RR
 *     #%    %*    CCCCCC     VV    MM      MM RR    RR
 *    .%    %/
 *       (%.      Computer Vision & Mixed Reality Group
 *
 *****************************************************/
/** @copyright:   Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author:   Alex Sommer
 *    @version:   1.0
 *       @date:   19.11.20
 *****************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0

import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.0

import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import "colors.js" as Color
import "layout.js" as Layout

import "Components"
import "ext/QTOpenGLWindow/qrc"
import opengl.qt.viewer 1.0
import sampler.qt.backend 1.0

ApplicationWindow
{
    id: applicationWindow
    visible: true
    width: 1000
    height: 625
    color: Color.bgOpenGl
    title: qsTr("LEAVEN")

    /******************************************************
     * Viewer (OpenGL)
     *****************************************************/
    Item {
        id: openGLViewer
        height: parent.height
        width: parent.width - bg_left_bar.width
        x: bg_left_bar.width
        y: 0
        z: 0
        visible: backend.meshLoaded && !backend.idle
        focus: true
        Keys.onPressed: {
            handleKeyPress(event.key);
        }
        QTOpenGLWindow {
            id:openGLWindow
        }
    }

    /******************************************************
     * Loading GIF
     *****************************************************/
    // TODO: This is not showing when working (app is frozen when idle)
    Rectangle {
        id: loadingSpinner
        width: animation.width
        height: animation.height
        x: parent.width / 2 - animation.width / 2
        y: parent.height / 2 - animation.height / 2
        color: "transparent"
        visible: backend.idle
        AnimatedImage { id: animation; source: "spinner.gif" }
    }

    /******************************************************
     * Controls
     *****************************************************/

    SamplerBackend {
        id: backend
        objectName: "bcknd_id"
        onSampleChanged: {
            vSamples.text = backend.samples;
            sSamples.text = backend.samples;
        }
    }

    LeavenIconButton {
        id: btn_load
        x: 10
        y: 10
        iconSource: "load.svg"
        onClicked: {
            openGLViewer.forceActiveFocus();
            fd_loadMesh.open();
        }
    }

    LeavenIconButton {
        id: btn_save
        x: 79
        y: 10
        iconSource: "save.svg"
        enabled: backend.sampled
        onClicked: {
            openGLViewer.forceActiveFocus();
            fd_saveFile.open();
        }
    }

    /******************************************************
     * General Settings
     *****************************************************/

    Item {
        id: generalSettings
        x: Layout.settingsPaddingLeft
        y: btn_load.y + btn_load.height + 15
        z: 5
        width: bg_left_bar.width
        visible: true

        SettingsLabel {
            id: lbl_meshNormalization
            text: qsTr("Normalize:")
            tooltip: qsTr("When ON: The longest side of the bounding box will be scaled to unit length 1.")
        }

        SwitchButton {
            id: meshNormalization
            on: true
            y: lbl_meshNormalization.y
            onClicked: {
                backend.settings.meshNormalization = meshNormalization.on ? 1 : 0;
                backend.reloadMesh();
            }
        }

        Text{
            id: lbl_scaling
            text: "Scale:"
            color: ma_lblscaling.containsMouse ? Color.second : Color.main
            y: lbl_meshNormalization.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
            z: 5
            property bool expanded: false
            MouseArea {
                id: ma_lblscaling
                anchors.fill: parent
                hoverEnabled: true
                onClicked: lbl_scaling.expanded = !lbl_scaling.expanded
            }
        }

        SettingsInput {
            id: scaling
            y: lbl_scaling.y
            text: "1.0"
            number: 1.0
            enabled: !lbl_scaling.expanded
            opacity: enabled ? 1.0 : 0.3
            validator: DoubleValidator{bottom: 0.00001; decimals: 10000}
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.scaleX = number;
                    backend.settings.scaleY = number;
                    backend.settings.scaleZ = number;
                    scaleX.text = scaling.text;
                    scaleY.text = scaling.text;
                    scaleZ.text = scaling.text;
                    backend.reloadMesh();
                    openGLViewer.forceActiveFocus();
                }
            }
        }

        SettingsLabel {
            id: lbl_scaleX
            text: "Scale X:"
            visible: lbl_scaling.expanded
            y: lbl_scaling.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SettingsInput {
             id: scaleX
             y: lbl_scaleX.y
             visible: lbl_scaling.expanded
             text: "1.0"
             number: 1.0
             validator: scaling.validator
             onFocusChanged: {
                 if(!activeFocus) {
                     backend.settings.scaleX = number;
                     var d2 = Number.fromLocaleString(Qt.locale("en_US"), scaling.text);
                     if(number > d2) {
                         scaling.text = scaleX.text;
                     }
                     backend.reloadMesh();
                 }
             }

        }

        SettingsLabel {
            id: lbl_scaleY
            text: "Scale Y:"
            visible: lbl_scaling.expanded
            y: lbl_scaleX.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SettingsInput {
            id: scaleY
            y: lbl_scaleY.y
            visible: lbl_scaling.expanded
            text: "1.0"
            number: 1.0
            validator: scaling.validator
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.scaleY = number;
                    var d2 = Number.fromLocaleString(Qt.locale("en_US"), scaling.text);
                    if(number > d2) {
                        scaling.text = scaleY.text;
                    }
                    backend.reloadMesh();
                }
            }
        }

        SettingsLabel {
            id: lbl_scaleZ
            text: "Scale Z:"
            visible: lbl_scaling.expanded
            y: lbl_scaleY.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SettingsInput {
            id: scaleZ
            y: lbl_scaleZ.y
            visible: lbl_scaling.expanded
            text: "1.0"
            number: 1.0
            validator: scaling.validator
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.scaleZ = number;
                    var d2 = Number.fromLocaleString(Qt.locale("en_US"), scaling.text);
                    if(number > d2) {
                        scaling.text = scaleZ.text;
                    }
                    backend.reloadMesh();
                }
            }
        }
    }

    /******************************************************
     * Volume Settings Expand
     *****************************************************/

    SettingsExpander {
        id: btn_volumeS
        text: qsTr("Volume")
        x: 10
        y: lbl_scaling.expanded ? generalSettings.y + lbl_scaleZ.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical : generalSettings.y + lbl_scaling.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        z: 5
        expanded: true
        onClicked: {
            openGLViewer.forceActiveFocus();
            btn_surfaceS.expanded = false;
        }
    }

    /******************************************************
     * Volume Sampling
     *****************************************************/

    Item {
        id: volumeSettings
        x: Layout.settingsPaddingLeft
        y: btn_volumeS.y + 30
        z: 5
        width: bg_left_bar.width
        height: btn_vSample.y + btn_vSample.height + Layout.settingsPaddingVertical
        visible: btn_volumeS.expanded

        SettingsLabel {
            id: lbl_vRadius
            text: qsTr("Radius:")
            tooltip: qsTr("Sampling Particle Radius")
        }

        SettingsInput {
            id: vRadius
            text: "0.02"
            number: 0.02
            validator: DoubleValidator{bottom: 0.000001}
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.radius = number;
                    sRadius.text = vRadius.text;
                }
            }
        }

        SettingsLabel {
            id: lbl_vInvert
            text: qsTr("Invert:")
            tooltip: qsTr("When ON: The volume between mesh and its bounding box will be sampled.")
            y: lbl_vRadius.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SwitchButton {
            id: vInvert
            y: lbl_vInvert.y
            onClicked: {
                backend.settings.vInvert = vInvert.on ? 1 : 0;
            }

        }

        SettingsLabel {
            id: lbl_vSDF
            text: qsTr("SDF Res:")
            tooltip: qsTr("3D resolution of the SDF representation. Higher values lead to more accuracy but higher computational cost.")
            y: lbl_vInvert.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SettingsInput {
            id: sdfX
            y: lbl_vSDF.y
            text: "20"
            number: 20
            validator: IntValidator{bottom: 1; top: 999}
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.sdfX = number;
                }
            }
        }

        SettingsInput {
            id: sdfY
            x: sdfX.x + sdfX.width + 5
            y: lbl_vSDF.y
            text: "20"
            number: 20
            validator: sdfX.validator
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.sdfY = number;
                }
            }
        }

        SettingsInput {
            id: sdfZ
            x: sdfY.x + sdfY.width + 5
            y: lbl_vSDF.y
            text: "20"
            number: 20
            validator: sdfX.validator
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.sdfZ = number;
                }
            }
        }

        SettingsLabel {
            id: lbl_vMode
            text: qsTr("Mode:")
            tooltip: qsTr("Sampling Mode Selection")
            y: lbl_vSDF.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SwitchButton {
            id: vMode
            txtOn: "Grid"
            txtOff: "Random"
            y: lbl_vMode.y
            on: true
            onClicked: {
                backend.settings.vMode = vMode.on ? 0 : 1;
            }
        }

        SettingsLabel {
            id: lbl_vDensity
            text: qsTr("Density:")
            tooltip: qsTr("Possible initial sample points density parameter")
            visible: !vMode.on
            y: lbl_vMode.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SettingsInput {
            id: vDensity
            y: lbl_vDensity.y
            visible: !vMode.on
            text: "10"
            number: 10
            validator: DoubleValidator{bottom: 1.0}
            onFocusChanged: {
                backend.settings.vDensity = number;
            }
        }

        SettingsLabel {
            id: lbl_vTrials
            text: qsTr("Trials:")
            tooltip: qsTr("Number of trial iterations used to find samples in each valid cell")
            visible: !vMode.on
            y: vDensity.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SettingsInput {
            id: vTrials
            y: lbl_vTrials.y
            visible: !vMode.on
            text: "10"
            number: 10
            validator: IntValidator{bottom: 1; top: 1001}
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.vTrials = number;
                }
            }
        }

        SettingsLabel {
            id: lbl_vSamples
            text: qsTr("Samples:")
            tooltip: qsTr("Number of Sampling Particles")
            y: vMode.on ? lbl_vMode.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical: lbl_vTrials.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        Text {
            id: vSamples
            x: Layout.padding_left + Layout.label_entry_width + Layout.label_padding_right
            y: lbl_vSamples.y
            color: Color.third
            text: ""
            visible: backend.sampled
        }

        TextIconButton {
            id: btn_vSample
            y: lbl_vSamples.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
            z: 5
            enabled: backend.meshLoaded
            iconSource: "work.svg"
            text: qsTr("Sample")
            onClicked: {
                openGLViewer.forceActiveFocus();
                backend.volumeSample();
            }
        }
    }

    /******************************************************
     * Surface Settings Expand
     *****************************************************/

    SettingsExpander {
        id: btn_surfaceS
        x: 10
        y: volumeSettings.visible ? volumeSettings.height + btn_volumeS.y + btn_volumeS.height : btn_volumeS.height + btn_volumeS.y
        z: 5
        text: qsTr("Surface")
        expanded: false
        onClicked: {
            openGLViewer.forceActiveFocus();
            btn_volumeS.expanded = false;
        }
    }

    /******************************************************
     * Surface Sampling
     *****************************************************/

    Item {
        id: surfaceSettings
        x: Layout.settingsPaddingLeft
        y: btn_surfaceS.y + 30
        z: 5
        width: bg_left_bar.width
        height: 200
        visible: btn_surfaceS.expanded

        SettingsLabel {
            id: lbl_sRadius
            text: qsTr("Radius:")
            tooltip: qsTr("Sampling particle radius")
        }

        SettingsInput {
            id: sRadius
            text: "0.02"
            number: 0.02
            validator: vRadius.validator
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.radius = number;
                    vRadius.text = sRadius.text;
                }
            }
        }

        SettingsLabel {
            id: lbl_sMinDistance
            text: qsTr("Min Dist:")
            tooltip: qsTr("Mininum distance between sampling particles.")
            y: sRadius.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        SettingsInput {
            id: sMinDistance
            y: lbl_sMinDistance.y
            text: "0.02"
            number: 0.02
            validator: DoubleValidator{bottom: 0.000001}
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.sMinDistance = number;
                }
            }
        }

        SettingsLabel {
            id: lbl_sDensity
            text: qsTr("Density:")
            tooltip: qsTr("Possible initial sample points density parameter")
            y: sMinDistance.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical            
        }

        SettingsInput {
            id:sDensity
            y: lbl_sDensity.y
            text: "40"
            number: 40
            validator: DoubleValidator{bottom: 1.0}
            onFocusChanged: {
                backend.settings.sDensity = number;
            }
        }

        SettingsLabel {
            id: lbl_sTrials
            text: qsTr("Trials:")
            tooltip: qsTr("Number of trial iterations used to find samples in each valid cell")
            y: sDensity.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical            
        }

        SettingsInput {
            id: sTrials
            y: lbl_sTrials.y
            text: "10"
            number: 10
            validator: IntValidator{bottom: 1; top: 1001}
            onFocusChanged: {
                if(!activeFocus) {
                    backend.settings.sTrials = number;
                }
            }
        }

        SettingsLabel {
            id: lbl_sNorm
            text: qsTr("Norm:")
            tooltip: qsTr("Distance norm")
            y: sTrials.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical          
        }

        SwitchButton {
            id: sNorm
            txtOn: "Euclidean"
            txtOff: "Geodesic"
            y: lbl_sNorm.y
            onClicked: {
                backend.settings.norm = sNorm.on ? 0 : 1;
            }
        }

        SettingsLabel {
            id: lbl_sSamples
            text: qsTr("Samples:")
            tooltip: qsTr("Number of Sampling Particles")
            y: lbl_sNorm.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
        }

        Text {
            id: sSamples
            x: Layout.padding_left + Layout.label_entry_width + Layout.label_padding_right
            y: lbl_sSamples.y
            color: Color.third
            text: ""
            visible: backend.sampled
        }

        TextIconButton {
            id: btn_sSample
            y: lbl_sSamples.y + Layout.settingsEntryHeight + Layout.settingsPaddingVertical
            z: 5
            enabled: backend.meshLoaded
            iconSource: "work.svg"
            text: qsTr("Sample")
            onClicked: {
                openGLViewer.forceActiveFocus();
                backend.surfaceSample();
            }
        }
    }

    FileDialog {
        id: fd_loadMesh
        nameFilters: ["mesh files (*.obj)"]
        folder: "file:///" + applicationDirPath + "/../"
        onAccepted:
        {
            backend.loadFile(fd_loadMesh.file);
            openGLViewer.forceActiveFocus();
        }
    }

    FileDialog {
        id: fd_saveFile
        title: "Save file"
        folder: "file:///" + applicationDirPath + "/../"
        fileMode: FileDialog.SaveFile
        onAccepted: {
            backend.save(fd_saveFile.file);
            openGLViewer.forceActiveFocus();
        }
    }

    Rectangle {
        id: bg_left_bar
        x: 0
        y: 0
        z: 1
        width: 150
        height: parent.height
        color: Color.background
        Component.onCompleted: {
            openGLViewer.forceActiveFocus();
        }
    }

    /******************************************************
     * Functions
     *****************************************************/

    // Example how to add key bindings
    function handleKeyPress(key) {
        switch(key) {
        case Qt.Key_P:
            console.log("P pressed");
            break;
        }
    }
}

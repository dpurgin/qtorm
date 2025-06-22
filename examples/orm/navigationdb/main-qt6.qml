import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import QtQuick.Controls.Material

import QtOrm.NavigationDb.Domain

import QtLocation
import QtPositioning

ApplicationWindow {
    id: root

    width: 412
    height: 732
    title: qsTr("QtOrm NavigationDB Example")

    visible: true   

    property int forcedUpdate: 0


    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter {
            name: "osm.mapping.providersrepository.disabled"
            value: true
        }
    }


    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr("‹")
                enabled: stackView.depth > 1
                onClicked: stackView.pop()
            }
            Label {
                text: stackView.currentItem.title
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton {
                id: menuButton

                text: qsTr("⋮")
                onClicked: stackView.currentItem.menu.popup(menuButton)
            }
        }
    }

    StackView {
        id: stackView

        initialItem: provinceView
        anchors.fill: parent
    }

    Component {
        id: provinceView

        Page {
            anchors.fill: parent

            title: qsTr("Provinces")
            readonly property Menu menu: Menu {
                MenuItem {
                    text: qsTr("Add...")
                    onClicked: addProvinceDialog.open()
                }
            }

            ListView {
                id: provinceListView

                model: gProvinceListModel

                anchors.fill: parent

                delegate: Item {
                    height: contentColumn.height
                    width: parent.width

                    Column {
                        id: contentColumn
                        width: parent.width
                        padding: 6

                        Label {
                            text: name
                            font.pixelSize: 24
                        }
                        Label {
                            text: root.forcedUpdate,
                                qsTr("Communities: %1").arg(
                                    communityList.length > 0
                                        ? communityList.map(community => community.name).join(', ')
                                        : qsTr("none"))

                            elide: Text.ElideRight
                            width: parent.width - parent.padding
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            const currentProvince = gProvinceListModel.at(index);

                            gCommunityListModel.filter = {
                                "province": currentProvince
                            }

                            stackView.push(communityView, {
                                               "currentProvince": currentProvince
                                           })
                        }
                    }
                }
            }
        }
    }

    Component {
        id: communityView

        Page {
            id: communitiesPage

            title: qsTr("Communities in %1").arg(currentProvince.name)
            property Province currentProvince
            readonly property Menu menu: Menu {
                 id: menu

                 MenuItem {
                     text: qsTr("Select mode")
                     checkable: true
                     checked: communitiesPage.selectMode
                     onClicked: {
                         communitiesPage.selectMode = !communitiesPage.selectMode
                         communitiesPage.selectedIndices = []
                     }
                 }

                 MenuItem {
                     text: qsTr("Remove")
                     enabled: communitiesPage.selectMode && selectedIndices.length > 0
                     onClicked: {
                         communitiesPage.selectedIndices
                            .map(index => gCommunityListModel.at(index))
                            .forEach(instance => gCommunityListModel.remove(instance))
                     }
                 }

                 MenuItem {
                     text: qsTr("Sort by name")

                     checkable: true
                     checked: gCommunityListModel.order.length === 0 ||
                              gCommunityListModel.order[0] === "name"

                     onClicked: {
                         gCommunityListModel.order = ["name", "postCode"];
                     }
                 }

                 MenuItem {
                     text: qsTr("Sort by postcode")

                     checkable: true
                     checked: gCommunityListModel.order.length > 0 &&
                              gCommunityListModel.order[0] === "postCode"

                     onClicked: {
                         gCommunityListModel.order = ["postCode", "name"]
                     }
                 }
            }

            property bool selectMode: false
            property var selectedIndices: []

            ListView {
                id: communityListView

                model: gCommunityListModel

                anchors.fill: parent

                delegate: Item {
                    height: contentItem.height
                    width: parent.width

                    RowLayout {
                        id: contentLayout

                        width: parent.width

                        CheckBox {
                            visible: communitiesPage.selectMode
                            onCheckedChanged: {
                                if (checked) {
                                    communitiesPage.selectedIndices.push(index)
                                    communitiesPage.selectedIndicesChanged()
                                } else {
                                    communitiesPage.selectedIndices =
                                            communitiesPage.selectedIndices.filter(x => x !== index)
                                }
                            }
                        }

                        Item {
                            id: contentItem

                            Layout.fillWidth: true
                            height: contentColumn.height

                            Column {
                                id: contentColumn
                                padding: 6

                                Row {
                                    spacing: 6

                                    Label {
                                        text: model.name
                                        font.pixelSize: 24
                                    }

                                    Label {
                                        text: model.postCode
                                        font.pixelSize: 24
                                        color: Material.color(Material.Grey, Material.Shade400)
                                    }

                                }

                                Label {
                                    text: qsTr("Population: %1").arg(model.population)
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                enabled: !communitiesPage.selectMode
                                onClicked: {
                                    stackView.push(mapView, {
                                        "currentCommunity": gCommunityListModel.at(index),
                                        "currentProvince": communitiesPage.currentProvince
                                    })
                                }
                            }

                        }
                    }
                }
            }
        }
    }

    Component {
        id: mapView

        Page {
            id: mapViewPage

            property Province currentProvince
            property Community currentCommunity
            readonly property Menu menu: Menu {
                MenuItem {
                    text: qsTr("Edit...")
                    onClicked: {
                        communityDialog.editMode = true
                        communityDialog.currentProvince = mapViewPage.currentProvince
                        communityDialog.currentCommunity = mapViewPage.currentCommunity
                        communityDialog.open()

                    }
                }

                MenuItem {
                    text: qsTr("Remove")
                    onClicked: {
                        if (gCommunityListModel.remove(mapViewPage.currentCommunity)) {
                            stackView.pop()
                        }
                    }
                }
            }

            title: currentCommunity.name
            anchors.fill: parent

            Map {
                id: map

                anchors.fill: parent

                plugin: mapPlugin
                zoomLevel: 14

                center {
                    latitude: currentCommunity.latitude
                    longitude: currentCommunity.longitude
                }               

                MouseArea {
                    anchors.fill: parent

                    onDoubleClicked: {
                        communityDialog.editMode = false
                        communityDialog.currentProvince = mapViewPage.currentProvince
                        communityDialog.currentCoordinate =
                                map.toCoordinate(Qt.point(mouse.x, mouse.y))
                        communityDialog.open()
                    }
                }

                MapQuickItem {
                    coordinate {
                        latitude: currentCommunity.latitude
                        longitude: currentCommunity.longitude
                    }

                    anchorPoint {
                        x: mapMarker.width / 4
                        y: mapMarker.height
                    }

                    sourceItem: Image {
                        id: mapMarker
                        source: "qrc:///marker.png"

                        MouseArea {
                            anchors.fill: parent
                            preventStealing: true

                            onClicked: {
                                communityDialog.editMode = true
                                communityDialog.currentCommunity = mapViewPage.currentCommunity
                                communityDialog.open()
                            }
                        }
                    }
                }
            }
        }
    }   

    Dialog {
        id: addProvinceDialog

        anchors.centerIn: parent

        width: parent.width * 2/3

        ColumnLayout {
            anchors.fill: parent

            TextField {
                id: provinceName

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                placeholderText: qsTr("Province name")
            }
        }

        onAccepted: {
            gProvinceListModel.create({"name": provinceName.text})
        }

        standardButtons: Dialog.Ok | Dialog.Cancel
    }

    Dialog {
        id: communityDialog

        property Community currentCommunity
        property Province currentProvince
        property var currentCoordinate
        property bool editMode: false        

        anchors.centerIn: parent
        width: parent.width * 3/4

        ColumnLayout {
            anchors.fill: parent

            TextField {
                id: communityTextField
                Layout.fillWidth: true
                placeholderText: qsTr("Community name")
                text: communityDialog.editMode? communityDialog.currentCommunity.name: ""
            }

            TextField {
                id: postCodeTextField
                Layout.fillWidth: true
                placeholderText: qsTr("Postcode")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 0
                }
                text: communityDialog.editMode? communityDialog.currentCommunity.postCode: ""
            }

            TextField {
                id: populationTextField
                Layout.fillWidth: true
                placeholderText: qsTr("Population")
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator {
                    bottom: 0
                }
                text: communityDialog.editMode? communityDialog.currentCommunity.population: ""
            }

            ComboBox {
                id: provinceComboBox
                Layout.fillWidth: true
                model: gProvinceListModel
                textRole: "name"
                currentIndex:
                    gProvinceListModel.indexOf(communityDialog.editMode?
                                                   communityDialog.currentCommunity.province:
                                                   communityDialog.currentProvince)
            }

            Label {
                text: communityDialog.editMode
                        ? "%1, %2".arg(communityDialog.currentCommunity.latitude)
                                  .arg(communityDialog.currentCommunity.longitude)
                        : "%1, %2".arg(communityDialog.currentCoordinate.latitude)
                                  .arg(communityDialog.currentCoordinate.longitude)
                Layout.alignment: Text.AlignHCenter
            }
        }

        onAccepted: {
            if (!communityDialog.editMode) {
                const instance = gCommunityListModel.create({
                    "name": communityTextField.text,
                    "postCode": postCodeTextField.text,
                    "population": Number(populationTextField.text),
                    "province": gProvinceListModel.at(provinceComboBox.currentIndex),
                    "longitude": currentCoordinate.longitude,
                    "latitude": currentCoordinate.latitude
                })

                if (instance) {
                    stackView.pop();
                    stackView.push(mapView, {
                        "currentCommunity": instance,
                        "currentProvince": communityDialog.currentProvince
                    });
                }
            } else {
                communityDialog.currentCommunity.name = communityTextField.text;
                communityDialog.currentCommunity.postCode = postCodeTextField.text;
                communityDialog.currentCommunity.population = Number(populationTextField.text)
                communityDialog.currentCommunity.province =
                        gProvinceListModel.at(provinceComboBox.currentIndex)

                gCommunityListModel.update(communityDialog.currentCommunity)
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel
    }

    Connections {
        target: gCommunityListModel
        onEntityInstanceRemoved: root.forcedUpdate++
        onEntityInstanceCreated: root.forcedUpdate++
    }
}

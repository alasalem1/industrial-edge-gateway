import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window

    visible: true
    width: 900
    height: 650
    minimumWidth: 760
    minimumHeight: 560
    title: "SentinelEdge | Operations Center"
    property bool darkMode: Theme.dark

    property string activePage: "Overview"

    readonly property color backgroundColor: darkMode ? "#11151C" : "#F3F5F8"
    readonly property color sidebarColor: darkMode ? "#171D26" : "#FFFFFF"
    readonly property color panelColor: darkMode ? "#1B222C" : "#FFFFFF"
    readonly property color panelAltColor: darkMode ? "#252E3A" : "#E8EDF2"
    readonly property color borderColor: darkMode ? "#303B49" : "#DCE3EA"
    readonly property color textColor: darkMode ? "#E8EDF3" : "#172334"
    readonly property color mutedColor: darkMode ? "#9AA8B8" : "#657487"
    readonly property color accentColor: darkMode ? "#36A99A" : "#16877D"
    readonly property color warningColor: darkMode ? "#E6B65C" : "#A96B12"
    readonly property color dangerColor: darkMode ? "#E47777" : "#C33D48"

    readonly property color selectedNavColor: darkMode ? "#263442" : "#E5F0EF"
    readonly property color secondaryTextColor: darkMode ? "#A9B8C8" : "#526174"
    readonly property color statusBackgroundColor: darkMode ? "#20352F" : "#E7F3EF"
    readonly property color statusBorderColor: darkMode ? "#365B50" : "#B9D9CE"
    readonly property color statusTextColor: darkMode ? "#8CE0D5" : "#167568"
    readonly property color noticeBackgroundColor: darkMode ? "#202F35" : "#EAF2F4"
    readonly property color noticeBorderColor: darkMode ? "#3B555B" : "#C9DCE0"
    readonly property color noticeIconColor: darkMode ? "#28564F" : "#D1E8E3"
    readonly property color goodStatusColor: darkMode ? "#75D6C8" : "#16877D"
    readonly property color chartGridColor: darkMode ? "#344252" : "#DCE3EA"
    readonly property color chartLineColor: darkMode ? "#5597D5" : "#2876B8"

    property var navigationItems: [
        "Overview",
        "Machine health",
        "AI diagnostics",
        "System resources",
        "Incidents",
        "Model management"
    ]

    function selectPage(page) {
        activePage = page
    }

    Rectangle {
        anchors.fill: parent
        color: backgroundColor
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // =====================================================
        // SIDEBAR
        // =====================================================

        Rectangle {
            Layout.preferredWidth: 184
            Layout.fillHeight: true
            color: sidebarColor

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                anchors.topMargin: 18
                anchors.bottomMargin: 14
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 9

                    Rectangle {
                        Layout.preferredWidth: 36
                        Layout.preferredHeight: 36
                        radius: 9
                        color: accentColor

                        Text {
                            anchors.centerIn: parent
                            text: "S"
                            color: "white"
                            font.pixelSize: 22
                            font.bold: true
                        }
                    }

                    ColumnLayout {
                        spacing: 2

                        Text {
                            text: "SentinelEdge"
                            color: textColor
                            font.pixelSize: 14
                            font.bold: true
                        }

                        Text {
                            text: "EDGE INTELLIGENCE"
                            color: mutedColor
                            font.pixelSize: 8
                            font.letterSpacing: 1.1
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.topMargin: 19
                    Layout.bottomMargin: 20
                    height: 1
                    color: borderColor
                }

                Text {
                    text: "WORKSPACE"
                    color: mutedColor
                    font.pixelSize: 9
                    font.bold: true
                    font.letterSpacing: 1.4
                    Layout.bottomMargin: 12
                }

                Repeater {
                    model: navigationItems

                    delegate: Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 39
                        Layout.bottomMargin: 5
                        radius: 6

                        color: activePage === modelData
                               ? selectedNavColor : "transparent"

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 9
                            anchors.rightMargin: 7
                            spacing: 9

                            Rectangle {
                                Layout.preferredWidth: 3
                                Layout.preferredHeight: 20
                                radius: 2
                                color: activePage === modelData
                                       ? accentColor : "transparent"
                            }

                            Text {
                                text: "0" + (index + 1)
                                color: mutedColor
                                font.pixelSize: 9
                            }

                            Text {
                                Layout.fillWidth: true
                                text: modelData
                                color: activePage === modelData
                                       ? textColor : secondaryTextColor
                                font.pixelSize: 10
                                font.weight: activePage === modelData
                                             ? Font.DemiBold : Font.Normal
                                elide: Text.ElideRight
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: selectPage(modelData)
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 104
                    radius: 7
                    color: panelAltColor
                    border.color: borderColor

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 7

                        Text {
                            text: "DEVICE STATUS"
                            color: mutedColor
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 1
                        }

                        RowLayout {
                            spacing: 6

                            Rectangle {
                                width: 7
                                height: 7
                                radius: 4
                                color: accentColor
                            }

                            Text {
                                text: "LOCAL PLATFORM"
                                color: textColor
                                font.pixelSize: 9
                                font.bold: true
                            }
                        }

                        Text {
                            text: "Offline-first architecture"
                            color: mutedColor
                            font.pixelSize: 10
                        }

                        Text {
                            text: "SentinelEdge Linux 1.0"
                            color: mutedColor
                            font.pixelSize: 10
                        }
                    }
                }
            }
        }

        // =====================================================
        // MAIN WORKSPACE
        // =====================================================

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // TOP BAR

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 68
                color: sidebarColor
                border.color: borderColor

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 26
                    anchors.rightMargin: 26
                    spacing: 16

                    ColumnLayout {
                        spacing: 4

                        Text {
                            text: activePage
                            color: textColor
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }

                        Text {
                            text: "SentinelEdge  /  Operations platform"
                            color: mutedColor
                            font.pixelSize: 11
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Rectangle {
                        Layout.preferredWidth: 88
                        Layout.preferredHeight: 32
                        Layout.alignment: Qt.AlignVCenter
                        radius: 7
                        color: statusBackgroundColor
                        border.color: statusBorderColor

                        Row {
                            anchors.centerIn: parent
                            spacing: 7

                            Rectangle {
                                width: 7
                                height: 7
                                radius: 4
                                color: accentColor
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: "LOCAL"
                                color: statusTextColor
                                font.pixelSize: 10
                                font.bold: true
                            }
                        }
                    }

                    Button {
                        id: themeButton

                        Layout.preferredWidth: 132
                        Layout.preferredHeight: 36
                        Layout.alignment: Qt.AlignVCenter

                        text: Theme.dark ? "LIGHT THEME" : "DARK THEME"
                        onClicked: Theme.dark = !Theme.dark

                        background: Rectangle {
                            radius: 7
                            color: themeButton.hovered
                                   ? selectedNavColor : panelAltColor
                            border.color: borderColor
                        }

                        contentItem: Text {
                            text: themeButton.text
                            color: textColor
                            font.pixelSize: 10
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }

            // PAGE CONTENT

            ScrollView {
                id: pageScroll

                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                ColumnLayout {
                    width: pageScroll.availableWidth
                    spacing: 14

                    // =================================================
                    // =================================================
                    // OVERVIEW
                    // =================================================

                    ColumnLayout {
                        visible: activePage === "Overview"
                        Layout.fillWidth: true
                        Layout.margins: 18
                        spacing: 14

                        // Page heading

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    text: "Operations overview"
                                    color: Theme.textPrimary
                                    font.pixelSize: 22
                                    font.weight: Font.Bold
                                }

                                Text {
                                    text: "Industrial edge platform status and resource monitoring"
                                    color: Theme.textMuted
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: 126
                                Layout.preferredHeight: 30
                                radius: 6
                                color: Theme.liveSoft
                                border.color: Theme.live

                                RowLayout {
                                    anchors.centerIn: parent
                                    spacing: 7

                                    Rectangle {
                                        width: 7
                                        height: 7
                                        radius: 4
                                        color: Theme.live
                                    }

                                    Text {
                                        text: "HOST MONITOR"
                                        color: Theme.live
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }
                            }
                        }

                        // Subsystem availability

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 3
                            columnSpacing: 9
                            rowSpacing: 9

                            Repeater {
                                model: [
                                    {
                                        name: "LINUX MONITOR",
                                        status: "LIVE",
                                        detail: "Host measurements",
                                        kind: "live"
                                    },
                                    {
                                        name: "MACHINE INPUT",
                                        status: "NOT CONNECTED",
                                        detail: "Sensor acquisition",
                                        kind: "neutral"
                                    },
                                    {
                                        name: "AI ENGINE",
                                        status: "NOT LOADED",
                                        detail: "Local inference",
                                        kind: "neutral"
                                    }
                                ]

                                delegate: Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 78
                                    radius: 7
                                    color: Theme.panel
                                    border.color: Theme.border

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 11
                                        spacing: 5

                                        Text {
                                            text: modelData.name
                                            color: Theme.textMuted
                                            font.pixelSize: 10
                                            font.bold: true
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: modelData.status
                                            color: modelData.kind === "live"
                                                   ? Theme.live : Theme.neutral
                                            font.pixelSize: 12
                                            font.bold: true
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: modelData.detail
                                            color: Theme.textMuted
                                            font.pixelSize: 10
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }
                                    }
                                }
                            }
                        }

                        // Primary host resource metrics

                        Text {
                            text: "HOST RESOURCE UTILIZATION"
                            color: Theme.textSecondary
                            font.pixelSize: 12
                            font.bold: true
                            font.letterSpacing: 0.7
                            Layout.topMargin: 3
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 2
                            columnSpacing: 10
                            rowSpacing: 10

                            // CPU

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 145
                                radius: 8
                                color: Theme.panel
                                border.color: Theme.border

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 14
                                    spacing: 9

                                    RowLayout {
                                        Layout.fillWidth: true

                                        Text {
                                            text: "CPU UTILIZATION"
                                            color: Theme.textSecondary
                                            font.pixelSize: 11
                                            font.bold: true
                                        }

                                        Item {
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "LIVE"
                                            color: Theme.live
                                            font.pixelSize: 10
                                            font.bold: true
                                        }
                                    }

                                    Text {
                                        text: systemMonitor.cpuUsage.toFixed(1) + "%"
                                        color: Theme.textPrimary
                                        font.pixelSize: 32
                                        font.bold: true
                                    }

                                    Rectangle {
                                        Layout.fillWidth: true
                                        height: 7
                                        radius: 4
                                        color: Theme.trackColor

                                        Rectangle {
                                            width: parent.width
                                                   * Math.max(0, Math.min(100, systemMonitor.cpuUsage)) / 100
                                            height: parent.height
                                            radius: 4
                                            color: Theme.accent
                                        }
                                    }

                                    Text {
                                        text: "Current Linux host CPU usage"
                                        color: Theme.textMuted
                                        font.pixelSize: 11
                                    }
                                }
                            }

                            // Memory

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 145
                                radius: 8
                                color: Theme.panel
                                border.color: Theme.border

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 14
                                    spacing: 9

                                    RowLayout {
                                        Layout.fillWidth: true

                                        Text {
                                            text: "MEMORY UTILIZATION"
                                            color: Theme.textSecondary
                                            font.pixelSize: 11
                                            font.bold: true
                                        }

                                        Item {
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: "LIVE"
                                            color: Theme.live
                                            font.pixelSize: 10
                                            font.bold: true
                                        }
                                    }

                                    Text {
                                        text: systemMonitor.memoryUsage.toFixed(1) + "%"
                                        color: Theme.textPrimary
                                        font.pixelSize: 32
                                        font.bold: true
                                    }

                                    Rectangle {
                                        Layout.fillWidth: true
                                        height: 7
                                        radius: 4
                                        color: Theme.trackColor

                                        Rectangle {
                                            width: parent.width
                                                   * Math.max(0, Math.min(100, systemMonitor.memoryUsage)) / 100
                                            height: parent.height
                                            radius: 4
                                            color: Theme.info
                                        }
                                    }

                                    Text {
                                        text: "Current Linux host memory usage"
                                        color: Theme.textMuted
                                        font.pixelSize: 11
                                    }
                                }
                            }
                        }

                        // Industrial subsystem integration status

                        Text {
                            text: "INDUSTRIAL SUBSYSTEMS"
                            color: Theme.textSecondary
                            font.pixelSize: 12
                            font.bold: true
                            font.letterSpacing: 0.7
                            Layout.topMargin: 3
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 2
                            columnSpacing: 10
                            rowSpacing: 10

                            // Machine telemetry

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 190
                                radius: 8
                                color: Theme.panel
                                border.color: Theme.border

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 14
                                    spacing: 9

                                    RowLayout {
                                        Layout.fillWidth: true

                                        Text {
                                            text: "MACHINE TELEMETRY"
                                            color: Theme.textSecondary
                                            font.pixelSize: 11
                                            font.bold: true
                                        }

                                        Item {
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: telemetryMonitor.status
                                            color: telemetryMonitor.connected
                                                   ? Theme.textPrimary
                                                   : Theme.neutral
                                            font.pixelSize: 10
                                            font.bold: true
                                        }
                                    }

                                    GridLayout {
                                        Layout.fillWidth: true
                                        columns: 2
                                        columnSpacing: 12
                                        rowSpacing: 10

                                        Text {
                                            text: "Temperature"
                                            color: Theme.textMuted
                                            font.pixelSize: 11
                                        }

                                        Text {
                                            text: telemetryMonitor.connected
                                                  ? telemetryMonitor.temperature.toFixed(1) + " °C"
                                                  : "—"
                                            color: Theme.textPrimary
                                            font.pixelSize: 13
                                            font.bold: true
                                        }

                                        Text {
                                            text: "Pressure"
                                            color: Theme.textMuted
                                            font.pixelSize: 11
                                        }

                                        Text {
                                            text: telemetryMonitor.connected
                                                  ? telemetryMonitor.pressure.toFixed(1) + " bar"
                                                  : "—"
                                            color: Theme.textPrimary
                                            font.pixelSize: 13
                                            font.bold: true
                                        }

                                        Text {
                                            text: "Vibration"
                                            color: Theme.textMuted
                                            font.pixelSize: 11
                                        }

                                        Text {
                                            text: telemetryMonitor.connected
                                                  ? telemetryMonitor.vibration.toFixed(1) + " mm/s"
                                                  : "—"
                                            color: Theme.textPrimary
                                            font.pixelSize: 13
                                            font.bold: true
                                        }

                                        Text {
                                            text: "Motor speed"
                                            color: Theme.textMuted
                                            font.pixelSize: 11
                                        }

                                        Text {
                                            text: telemetryMonitor.connected
                                                  ? telemetryMonitor.motorRpm + " RPM"
                                                  : "—"
                                            color: Theme.textPrimary
                                            font.pixelSize: 13
                                            font.bold: true
                                        }
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                    }

                                    Text {
                                        text: "SOURCE  /  " + telemetryMonitor.source
                                        color: Theme.textMuted
                                        font.pixelSize: 10
                                        font.bold: true
                                        elide: Text.ElideRight
                                        Layout.fillWidth: true
                                    }
                                }
                            }

                            // Rule-based diagnostics

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 290
                                radius: 8
                                color: Theme.panel
                                border.color: Theme.border

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 14
                                    spacing: 9

                                    Text {
                                        text: "RULE-BASED DIAGNOSTICS"
                                        color: Theme.textSecondary
                                        font.pixelSize: 11
                                        font.bold: true
                                    }

                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 32
                                        radius: 6

                                        property string statusKind:
                                            telemetryMonitor.diagnosticStatus === "NORMAL" ? "live" :
                                            telemetryMonitor.diagnosticStatus === "WARNING" ? "warning" :
                                            telemetryMonitor.diagnosticStatus === "CRITICAL" ? "danger" :
                                            "neutral"

                                        color: Theme.statusSoftColor(statusKind)

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.leftMargin: 10
                                            anchors.rightMargin: 10

                                            Text {
                                                text: telemetryMonitor.diagnosticStatus
                                                color: Theme.statusColor(parent.parent.statusKind)
                                                font.pixelSize: 13
                                                font.bold: true
                                            }

                                            Item {
                                                Layout.fillWidth: true
                                            }

                                            Text {
                                                text: telemetryMonitor.connected
                                                      ? "ENGINE ACTIVE"
                                                      : "WAITING FOR DATA"
                                                color: Theme.textMuted
                                                font.pixelSize: 9
                                                font.bold: true
                                            }
                                        }
                                    }

                                    Text {
                                        text: telemetryMonitor.diagnosticSummary
                                        color: Theme.textPrimary
                                        font.pixelSize: 12
                                        wrapMode: Text.WordWrap
                                        Layout.fillWidth: true
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                    }

                                    Text {
                                        text: "ML ANOMALY DETECTION"
                                        color: Theme.textSecondary
                                        font.pixelSize: 11
                                        font.bold: true
                                    }

                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 32
                                        radius: 6

                                        property string statusKind:
                                            telemetryMonitor.aiStatus === "NORMAL" ? "live" :
                                            telemetryMonitor.aiStatus === "ANOMALY" ? "danger" :
                                            "neutral"

                                        color: Theme.statusSoftColor(statusKind)

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.leftMargin: 10
                                            anchors.rightMargin: 10

                                            Text {
                                                text: telemetryMonitor.aiStatus
                                                color: Theme.statusColor(parent.parent.statusKind)
                                                font.pixelSize: 13
                                                font.bold: true
                                            }

                                            Item {
                                                Layout.fillWidth: true
                                            }

                                            Text {
                                                text: "ISOLATION FOREST / C INFERENCE"
                                                color: Theme.textMuted
                                                font.pixelSize: 9
                                                font.bold: true
                                            }
                                        }
                                    }

                                    Text {
                                        text: telemetryMonitor.aiSummary
                                        color: Theme.textPrimary
                                        font.pixelSize: 12
                                        wrapMode: Text.WordWrap
                                        Layout.fillWidth: true
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                    }

                                    Text {
                                        text: "ENGINE  /  C RULE EVALUATION"
                                        color: Theme.textMuted
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }
                            }

                        }

                        // Platform information

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 86
                            radius: 8
                            color: Theme.panelAlt
                            border.color: Theme.border

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 16

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5

                                    Text {
                                        text: "SENTINELEDGE LINUX"
                                        color: Theme.textPrimary
                                        font.pixelSize: 12
                                        font.bold: true
                                    }

                                    Text {
                                        text: "Version 1.0  /  Development build"
                                        color: Theme.textMuted
                                        font.pixelSize: 11
                                    }
                                }

                                Rectangle {
                                    width: 1
                                    Layout.fillHeight: true
                                    color: Theme.border
                                }

                                ColumnLayout {
                                    spacing: 5

                                    Text {
                                        text: "SYSTEM UPTIME"
                                        color: Theme.textMuted
                                        font.pixelSize: 10
                                        font.bold: true
                                    }

                                    Text {
                                        text: systemMonitor.uptime
                                        color: Theme.textPrimary
                                        font.pixelSize: 13
                                        font.bold: true
                                    }
                                }
                            }
                        }
                    }


                    // MACHINE HEALTH
                    // =================================================

                    ColumnLayout {
                        visible: activePage === "Machine health"
                        Layout.fillWidth: true
                        Layout.margins: 18
                        spacing: 13

                        Text {
                            text: "Machine health"
                            color: textColor
                            font.pixelSize: 21
                            font.bold: true
                        }

                        Text {
                            text: "Industrial sensor monitoring and machine condition assessment"
                            color: mutedColor
                            font.pixelSize: 10
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 175
                            color: panelColor
                            border.color: borderColor
                            radius: 7

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 10

                                Text {
                                    text: "TELEMETRY SOURCE"
                                    color: mutedColor
                                    font.pixelSize: 9
                                    font.bold: true
                                }

                                Text {
                                    text: {
                                        if (!telemetryMonitor.connected)
                                            return "WAITING FOR TELEMETRY"

                                        if (telemetryMonitor.source === "MODBUS-TCP")
                                            return "LIVE MODBUS TELEMETRY"

                                        if (telemetryMonitor.source === "SIMULATED-C-ACQUISITION")
                                            return "LIVE SIMULATED TELEMETRY"

                                        return "LIVE INDUSTRIAL TELEMETRY"
                                    }
                                    color: telemetryMonitor.connected ? textColor : warningColor
                                    font.pixelSize: 17
                                    font.bold: true
                                }

                                Text {
                                    text: telemetryMonitor.connected
                                          ? "Receiving fresh measurements from " + telemetryMonitor.source
                                          : "No fresh telemetry available. Check the acquisition service."
                                    color: mutedColor
                                    font.pixelSize: 10
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        Repeater {
                            model: ["Temperature", "Pressure", "Vibration", "Motor speed"]

                            delegate: Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 48
                                radius: 6
                                color: panelColor
                                border.color: borderColor

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 14
                                    anchors.rightMargin: 14

                                    Text {
                                        text: modelData
                                        color: textColor
                                        font.pixelSize: 10
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                    }

                                    Text {
                                        text: {
                                            if (!telemetryMonitor.connected)
                                                return "NO DATA"

                                            switch (index) {
                                            case 0: return telemetryMonitor.temperature.toFixed(1) + " °C"
                                            case 1: return telemetryMonitor.pressure.toFixed(1) + " bar"
                                            case 2: return telemetryMonitor.vibration.toFixed(1) + " mm/s"
                                            case 3: return telemetryMonitor.motorRpm + " RPM"
                                            default: return "NO DATA"
                                            }
                                        }
                                        color: telemetryMonitor.connected ? textColor : warningColor
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }
                            }
                        }
                    }

                    // =================================================
                    // AI DIAGNOSTICS
                    // =================================================

                    ColumnLayout {
                        visible: activePage === "AI diagnostics"
                        Layout.fillWidth: true
                        Layout.margins: 18
                        spacing: 13

                        Text {
                            text: "AI diagnostics"
                            color: textColor
                            font.pixelSize: 21
                            font.bold: true
                        }

                        Text {
                            text: "Local inference engine and machine anomaly analysis"
                            color: mutedColor
                            font.pixelSize: 10
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 175
                            radius: 7
                            color: panelColor
                            border.color: borderColor

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 10

                                Text {
                                    text: "INFERENCE ENGINE"
                                    color: mutedColor
                                    font.pixelSize: 9
                                    font.bold: true
                                }

                                Text {
                                    text: telemetryMonitor.aiStatus
                                    color: telemetryMonitor.aiStatus === "ANOMALY" ? dangerColor :
                                           telemetryMonitor.aiStatus === "NORMAL" ? Theme.statusColor("live") :
                                           warningColor
                                    font.pixelSize: 18
                                    font.bold: true
                                }

                                Text {
                                    text: telemetryMonitor.connected
                                          ? "Connected to local Isolation Forest inference running in native C."
                                          : "Waiting for valid sensor telemetry."
                                    color: mutedColor
                                    font.pixelSize: 10
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 100
                            radius: 7
                            color: panelColor
                            border.color: borderColor

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 7

                                Text {
                                    text: "DIAGNOSTIC OUTPUT"
                                    color: mutedColor
                                    font.pixelSize: 9
                                    font.bold: true
                                }

                                Text {
                                    text: telemetryMonitor.aiStatus === "ANOMALY"
                                      ? "ANOMALY DETECTED"
                                      : telemetryMonitor.aiStatus === "NORMAL"
                                      ? "NO ANOMALY DETECTED"
                                      : telemetryMonitor.aiStatus
                                    color: textColor
                                    font.pixelSize: 13
                                    font.bold: true
                                }

                                Text {
                                    text: telemetryMonitor.aiSummary
                                    wrapMode: Text.WordWrap
                                    color: mutedColor
                                    font.pixelSize: 9
                                }
                            }
                        }
                    }

                    // =================================================
                    // SYSTEM RESOURCES
                    // =================================================

                    ColumnLayout {
                        visible: activePage === "System resources"
                        Layout.fillWidth: true
                        Layout.margins: 18
                        spacing: 13

                        Text {
                            text: "System resources"
                            color: textColor
                            font.pixelSize: 21
                            font.bold: true
                        }

                        Text {
                            text: "Live operating system resource utilization"
                            color: mutedColor
                            font.pixelSize: 10
                        }

                        Repeater {
                            model: [
                                {
                                    name: "CPU utilization",
                                    value: systemMonitor.cpuUsage
                                },
                                {
                                    name: "Memory utilization",
                                    value: systemMonitor.memoryUsage
                                }
                            ]

                            delegate: Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 100
                                radius: 7
                                color: panelColor
                                border.color: borderColor

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 15
                                    spacing: 10

                                    RowLayout {
                                        Layout.fillWidth: true

                                        Text {
                                            text: modelData.name
                                            color: textColor
                                            font.pixelSize: 11
                                            font.bold: true
                                        }

                                        Item {
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: modelData.value.toFixed(1) + "%"
                                            color: textColor
                                            font.pixelSize: 16
                                            font.bold: true
                                        }
                                    }

                                    Rectangle {
                                        Layout.fillWidth: true
                                        height: 9
                                        radius: 4
                                        color: chartGridColor

                                        Rectangle {
                                            width: parent.width
                                                   * Math.max(0, Math.min(100, modelData.value)) / 100
                                            height: parent.height
                                            radius: 4
                                            color: accentColor
                                        }
                                    }

                                    Text {
                                        text: "LIVE LINUX MEASUREMENT"
                                        color: mutedColor
                                        font.pixelSize: 10
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 70
                            radius: 7
                            color: panelColor
                            border.color: borderColor

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 14
                                spacing: 5

                                Text {
                                    text: "SYSTEM UPTIME"
                                    color: mutedColor
                                    font.pixelSize: 10
                                    font.bold: true
                                }

                                Text {
                                    text: systemMonitor.uptime
                                    color: textColor
                                    font.pixelSize: 18
                                    font.bold: true
                                }
                            }
                        }
                    }

                    // =================================================
                    // INCIDENTS
                    // =================================================

                    ColumnLayout {
                        visible: activePage === "Incidents"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.margins: 18
                        spacing: 13

                        Text {
                            text: "Incident management"
                            color: textColor
                            font.pixelSize: 21
                            font.bold: true
                        }

                        Text {
                            text: "Persistent machine fault history and acknowledgment"
                            color: mutedColor
                            font.pixelSize: 10
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 3
                            columnSpacing: 10

                            Repeater {
                                model: [
                                    { label: "ACTIVE FAULTS", value: faultManager.activeFaultCount, color: dangerColor },
                                    { label: "NEEDS ACKNOWLEDGMENT", value: faultManager.unacknowledgedCount, color: warningColor },
                                    { label: "RECORDED EVENTS", value: faultManager.events.length, color: accentColor }
                                ]

                                delegate: Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 76
                                    radius: 7
                                    color: panelColor
                                    border.color: borderColor

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        spacing: 5

                                        Text {
                                            text: modelData.label
                                            color: mutedColor
                                            font.pixelSize: 8
                                            font.bold: true
                                            wrapMode: Text.WordWrap
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: modelData.value
                                            color: modelData.color
                                            font.pixelSize: 22
                                            font.bold: true
                                        }
                                    }
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Text {
                                text: "Storage: " + faultManager.storageStatus
                                color: mutedColor
                                font.pixelSize: 9
                                Layout.fillWidth: true
                            }

                            Button {
                                text: "Acknowledge all"
                                enabled: faultManager.unacknowledgedCount > 0
                                onClicked: faultManager.acknowledgeAll()
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumHeight: 180
                            radius: 7
                            color: panelColor
                            border.color: borderColor
                            clip: true

                            ListView {
                                id: incidentList
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 8
                                clip: true
                                model: faultManager.events

                                ScrollBar.vertical: ScrollBar {}

                                delegate: Rectangle {
                                    required property var modelData

                                    width: incidentList.width
                                    height: incidentContent.implicitHeight + 22
                                    radius: 6
                                    color: panelAltColor
                                    border.color: borderColor

                                    ColumnLayout {
                                        id: incidentContent
                                        anchors.fill: parent
                                        anchors.margins: 11
                                        spacing: 7

                                        RowLayout {
                                            Layout.fillWidth: true
                                            spacing: 8

                                            Text {
                                                text: modelData.title || "Unknown fault"
                                                color: textColor
                                                font.pixelSize: 12
                                                font.bold: true
                                                Layout.fillWidth: true
                                            }

                                            Text {
                                                text: modelData.severity || "UNKNOWN"
                                                color: modelData.severity === "CRITICAL"
                                                       ? dangerColor
                                                       : modelData.severity === "WARNING"
                                                         ? warningColor : mutedColor
                                                font.pixelSize: 9
                                                font.bold: true
                                            }
                                        }

                                        Text {
                                            text: modelData.message || "No diagnostic details"
                                            color: secondaryTextColor
                                            font.pixelSize: 9
                                            wrapMode: Text.WordWrap
                                            Layout.fillWidth: true
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true

                                            Text {
                                                text: "Detected: " + (modelData.timestamp || "Unknown")
                                                color: mutedColor
                                                font.pixelSize: 8
                                                Layout.fillWidth: true
                                                wrapMode: Text.WrapAnywhere
                                            }

                                            Text {
                                                text: modelData.active
                                                      ? (modelData.acknowledged ? "ACKNOWLEDGED" : "ACTIVE")
                                                      : "CLEARED"
                                                color: modelData.active
                                                       ? (modelData.acknowledged ? accentColor : warningColor)
                                                       : goodStatusColor
                                                font.pixelSize: 8
                                                font.bold: true
                                            }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true

                                            Item {
                                                Layout.fillWidth: true
                                            }

                                            Button {
                                                text: "Acknowledge"
                                                visible: modelData.active && !modelData.acknowledged
                                                onClicked: faultManager.acknowledge(modelData.id)
                                            }
                                        }
                                    }
                                }

                                Text {
                                    anchors.centerIn: parent
                                    visible: incidentList.count === 0
                                    text: "No incidents recorded yet"
                                    color: mutedColor
                                    font.pixelSize: 12
                                }
                            }
                        }
                    }

                    // MODEL MANAGEMENT
                    // =================================================

                    ColumnLayout {
                        visible: activePage === "Model management"
                        Layout.fillWidth: true
                        Layout.margins: 18
                        spacing: 13

                        Text {
                            text: "Model management"
                            color: textColor
                            font.pixelSize: 21
                            font.bold: true
                        }

                        Text {
                            text: "Embedded AI model configuration and runtime status"
                            color: mutedColor
                            font.pixelSize: 10
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 160
                            radius: 7
                            color: panelColor
                            border.color: borderColor

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 8

                                Text {
                                    text: "ACTIVE MODEL"
                                    color: mutedColor
                                    font.pixelSize: 9
                                    font.bold: true
                                }

                                Text {
                                    text: "Embedded Isolation Forest"
                                    color: textColor
                                    font.pixelSize: 17
                                    font.bold: true
                                }

                                RowLayout {
                                    Layout.fillWidth: true

                                    Text {
                                        text: "INFERENCE STATUS"
                                        color: mutedColor
                                        font.pixelSize: 9
                                        font.bold: true
                                    }

                                    Item {
                                        Layout.fillWidth: true
                                    }

                                    Text {
                                        text: telemetryMonitor.aiStatus
                                        color: telemetryMonitor.aiStatus === "NORMAL"
                                               ? goodStatusColor
                                               : telemetryMonitor.aiStatus === "ANOMALY"
                                                 ? dangerColor
                                                 : warningColor
                                        font.pixelSize: 11
                                        font.bold: true
                                    }
                                }

                                Text {
                                    text: telemetryMonitor.aiStatus === "UNKNOWN"
                                          ? "Waiting for valid telemetry before running inference."
                                          : telemetryMonitor.aiSummary
                                    color: mutedColor
                                    font.pixelSize: 9
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }

                                Text {
                                    text: telemetryMonitor.aiStatus === "UNKNOWN"
                                          ? "LATEST SCORE  /  NOT AVAILABLE"
                                          : "LATEST ANOMALY SCORE  /  " +
                                            telemetryMonitor.aiAnomalyScore.toFixed(4)
                                    color: mutedColor
                                    font.pixelSize: 9
                                    font.bold: true
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 135
                            radius: 7
                            color: panelColor
                            border.color: borderColor

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 9

                                Text {
                                    text: "MODEL CONFIGURATION"
                                    color: mutedColor
                                    font.pixelSize: 9
                                    font.bold: true
                                }

                                RowLayout {
                                    Layout.fillWidth: true

                                    Text {
                                        text: "Algorithm"
                                        color: mutedColor
                                        font.pixelSize: 10
                                    }

                                    Item { Layout.fillWidth: true }

                                    Text {
                                        text: "Isolation Forest"
                                        color: textColor
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true

                                    Text {
                                        text: "Number of trees"
                                        color: mutedColor
                                        font.pixelSize: 10
                                    }

                                    Item { Layout.fillWidth: true }

                                    Text {
                                        text: "128"
                                        color: textColor
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true

                                    Text {
                                        text: "Maximum samples"
                                        color: mutedColor
                                        font.pixelSize: 10
                                    }

                                    Item { Layout.fillWidth: true }

                                    Text {
                                        text: "256"
                                        color: textColor
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true

                                    Text {
                                        text: "Anomaly threshold"
                                        color: mutedColor
                                        font.pixelSize: 10
                                    }

                                    Item { Layout.fillWidth: true }

                                    Text {
                                        text: "-0.604171951"
                                        color: textColor
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 105
                            radius: 7
                            color: panelColor
                            border.color: borderColor

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 7

                                Text {
                                    text: "MODEL INTEGRATION"
                                    color: mutedColor
                                    font.pixelSize: 9
                                    font.bold: true
                                }

                                Text {
                                    text: "BUILT INTO APPLICATION"
                                    color: goodStatusColor
                                    font.pixelSize: 12
                                    font.bold: true
                                }

                                Text {
                                    text: "The model is compiled into the native C application. No separate model artifact or cryptographic signature-verification workflow is configured."
                                    color: mutedColor
                                    font.pixelSize: 9
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }

                    Item {
                        Layout.preferredHeight: 10
                    }
                }
            }
        }
    }
}

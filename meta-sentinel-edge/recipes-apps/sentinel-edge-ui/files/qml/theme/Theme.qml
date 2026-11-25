pragma Singleton
import QtQuick

// Central design tokens for SentinelEdge.
// Every colour, font size and spacing value used by the UI lives here.
QtObject {
    id: theme

    // ---- Theme mode (dark by default) --------------------------------
    property bool dark: true

    // ---- Surfaces ----------------------------------------------------
    readonly property color background:  dark ? "#0F1318" : "#F4F6F8"
    readonly property color sidebar:     dark ? "#0B0E12" : "#FFFFFF"
    readonly property color topBar:      dark ? "#12171D" : "#FFFFFF"
    readonly property color panel:       dark ? "#161B22" : "#FFFFFF"
    readonly property color panelAlt:    dark ? "#1C232C" : "#EEF1F5"
    readonly property color border:      dark ? "#262E39" : "#DDE3EA"
    readonly property color navActive:   dark ? "#1A2A2C" : "#E4F3F1"

    // ---- Text --------------------------------------------------------
    readonly property color textPrimary:   dark ? "#E6EBF1" : "#16202C"
    readonly property color textSecondary: dark ? "#A3AFBD" : "#46525F"
    readonly property color textMuted:     dark ? "#7C8998" : "#6B7787"
    readonly property color textOnAccent:  "#FFFFFF"

    // ---- Accent and status colours ----------------------------------
    // live    : data is flowing from a real source
    // neutral : subsystem not connected / not integrated yet (not a fault)
    // warning : real warning (reserved for future live data)
    // danger  : real alarm (reserved for future live data)
    readonly property color accent:  dark ? "#2BB5A6" : "#0E8A7E"
    readonly property color live:    accent
    readonly property color neutral: dark ? "#8794A3" : "#6B7787"
    readonly property color warning: dark ? "#E2A93B" : "#B26F0A"
    readonly property color danger:  dark ? "#E5605F" : "#C0392F"
    readonly property color info:    dark ? "#4C8FD0" : "#2876B8"

    readonly property color liveSoft:    dark ? "#14302D" : "#E2F3F0"
    readonly property color neutralSoft: dark ? "#222B35" : "#EAEEF2"
    readonly property color warningSoft: dark ? "#33291A" : "#FBF1DD"
    readonly property color dangerSoft:  dark ? "#351D1E" : "#FBE6E4"

    readonly property color trackColor: dark ? "#263040" : "#E3E8EE"

    // ---- Typography (minimum 12 px) ---------------------------------
    readonly property int fontCaption: 12
    readonly property int fontLabel:   12
    readonly property int fontBody:    13
    readonly property int fontSection: 15
    readonly property int fontTitle:   22
    readonly property int fontKpi:     32

    // ---- Spacing and shape ------------------------------------------
    readonly property int spaceXs: 4
    readonly property int spaceSm: 8
    readonly property int spaceMd: 12
    readonly property int spaceLg: 16
    readonly property int spaceXl: 24
    readonly property int radius:  6
    readonly property int pageMargin: 24

    // ---- Helpers -----------------------------------------------------
    function statusColor(kind) {
        switch (kind) {
        case "live":    return live
        case "warning": return warning
        case "danger":  return danger
        default:        return neutral
        }
    }

    function statusSoftColor(kind) {
        switch (kind) {
        case "live":    return liveSoft
        case "warning": return warningSoft
        case "danger":  return dangerSoft
        default:        return neutralSoft
        }
    }
}

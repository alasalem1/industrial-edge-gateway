SUMMARY = "SentinelEdge Industrial Data Acquisition"
DESCRIPTION = "Industrial telemetry acquisition and JSON publishing service"
LICENSE = "CLOSED"

SRC_URI = "file://industrial-data-acquisition.c \
           file://industrial-data-acquisition.service \
"

S = "${WORKDIR}"

inherit systemd

SYSTEMD_SERVICE:${PN} = "industrial-data-acquisition.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} \
        ${WORKDIR}/industrial-data-acquisition.c \
        -o industrial-data-acquisition
}

do_install() {
    install -d ${D}${bindir}

    install -m 0755 industrial-data-acquisition \
        ${D}${bindir}/industrial-data-acquisition

    install -d ${D}${systemd_system_unitdir}

    install -m 0644 \
        ${WORKDIR}/industrial-data-acquisition.service \
        ${D}${systemd_system_unitdir}/industrial-data-acquisition.service
}

FILES:${PN} += "${bindir}/industrial-data-acquisition"
FILES:${PN} += "${systemd_system_unitdir}/industrial-data-acquisition.service"

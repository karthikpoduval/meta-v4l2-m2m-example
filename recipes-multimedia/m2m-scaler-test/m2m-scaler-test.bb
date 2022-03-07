FILESEXTRAPATHS:prepend := "${THISDIR}/:"

DESCRIPTION = "M2M Scaler Test"
AUTHOR = "Karthik Poduval <karthik.poduval@gmail.com>"
SECTION = "DEVEL"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit cmake

SRC_URI = "file://src"

S = "${WORKDIR}/src"

DEPENDS += " libcamera pkgconfig-native"


SRC_URI = "gitsm://github.com/karthikpoduval/qemu.git;branch=scaler-bringup;protocol=https \
           file://powerpc_rom.bin \
           file://run-ptest \
"
SRC_URI[sha256sum] = "980d49ab906a3530feea2309d5167e5177e631bd8609b734d8e9d4d657c365a5"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

PACKAGECONFIG[libxml2] = ""
PACKAGECONFIG[xfs] = ""

#EXTRA_OECONF:append = " --enable-sanitizers"
EXTRA_OECONF:append = " --enable-werror"

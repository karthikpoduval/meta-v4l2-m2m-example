inherit core-image

IMAGE_FEATURE += " ssh-server-dropbear debug-tweaks tools-debug dbg-pkgs"

IMAGE_INSTALL += " v4l-utils yavta libcamera"

IMAGE_INSTALL += " v4l2-m2m-scaler-driver" 

IMAGE_INSTALL += " m2m-scaler-test" 

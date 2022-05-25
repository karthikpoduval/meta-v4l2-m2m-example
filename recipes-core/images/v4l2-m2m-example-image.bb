inherit core-image

IMAGE_FEATURE += " ssh-server-dropbear"

IMAGE_INSTALL += " v4l-utils yavta libcamera"

IMAGE_INSTALL += " v4l2-m2m-scaler-driver" 

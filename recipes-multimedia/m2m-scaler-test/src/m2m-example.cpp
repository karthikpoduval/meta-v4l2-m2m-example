/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2019, Google Inc.
 *
 * based on libcamera V4L2 M2M video device tests
 */

#include <iostream>

#include <libcamera/framebuffer.h>

#include <libcamera/base/event_dispatcher.h>
#include <libcamera/base/thread.h>
#include <libcamera/base/timer.h>

#include "libcamera/internal/device_enumerator.h"
#include "libcamera/internal/media_device.h"
#include "libcamera/internal/v4l2_videodevice.h"

using namespace std;
using namespace libcamera;


class M2MScaler {
public:
	M2MScaler(int width, int height) {
		width_ = width;
		height_ = height;
	
		enumerator_ = DeviceEnumerator::create();
		if (!enumerator_) {
			cerr << "Failed to create device enumerator" << endl;
		}

		if (enumerator_->enumerate()) {
			cerr << "Failed to enumerate media devices" << endl;
		}

		DeviceMatch dm("virtual-v4l2-m2m-scaler");
		dm.add("virtual-v4l2-m2m-scaler-source");
		dm.add("virtual-v4l2-m2m-scaler-sink");

		media_ = enumerator_->search(dm);
		if (!media_) {
			cerr << "No vim2m device found" << endl;
		} 

	}
private:
	std::unique_ptr<DeviceEnumerator> enumerator_;
	std::shared_ptr<MediaDevice> media_;
	V4L2M2MDevice *m2mScaler_;

	std::vector<std::unique_ptr<FrameBuffer>> captureBuffers_;
	std::vector<std::unique_ptr<FrameBuffer>> outputBuffers_;

	unsigned int outputFrames_;
	unsigned int captureFrames_;
	int width_;
	int height_;
};

int main()
{
	M2MScaler *s = new M2MScaler(640, 480);
}

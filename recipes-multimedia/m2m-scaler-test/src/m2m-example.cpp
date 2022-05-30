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
#include "libcamera/internal/mapped_framebuffer.h"

#include "bbb_splash.h"
#include "bbb_splash_resized.h"

#define DATA_CHECK

using namespace std;
using namespace libcamera;


class M2MScaler {
public:
	M2MScaler(Size input, Size output) {
		inputSize_ = input;
		outputSize_ = output;
	
		enumerator_ = DeviceEnumerator::create();
		if (!enumerator_) {
			cerr << "Failed to create device enumerator" << endl;
			assert(1);
		}

		if (enumerator_->enumerate()) {
			cerr << "Failed to enumerate media devices" << endl;
			assert(1);
		}

		DeviceMatch dm("m2ms");
		dm.add("m2ms-source");
		dm.add("m2ms-sink");

		media_ = enumerator_->search(dm);
		if (!media_) {
			cerr << "No vim2m device found" << endl;
			assert(1);
		}


	}

	void outputBufferComplete(FrameBuffer *buffer)
	{
		cout << "Received output buffer" << endl;

		outputFrames_++;

		/* Requeue the buffer for further use. */
		m2mScaler_->output()->queueBuffer(buffer);
	}

	void receiveCaptureBuffer(FrameBuffer *buffer)
	{
		cout << "Received capture buffer" << endl;

		captureFrames_++;
#ifdef DATA_CHECK
		int cookie = buffer->cookie();
		assert(memcmp(outputMappedBuffer_[cookie].planes()[0].data(), bbb_splash_resize_rgb, bbb_splash_resize_rgb_len) ==0);
#endif
		/* Requeue the buffer for further use. */
		m2mScaler_->capture()->queueBuffer(buffer);
	}


	int run() {
		constexpr unsigned int bufferCount = 1;

		EventDispatcher *dispatcher = Thread::current()->eventDispatcher();
		int ret;

		MediaEntity *entity = media_->getEntityByName("m2ms-source");
		m2mScaler_ = new V4L2M2MDevice(entity->deviceNode());
		if (m2mScaler_->open()) {
			cerr << "Failed to open M2M Scaler device" << endl;
			return -1;
		}

		V4L2VideoDevice *capture = m2mScaler_->capture();
		V4L2VideoDevice *output = m2mScaler_->output();

		V4L2DeviceFormat format = {};
		if (capture->getFormat(&format)) {
			cerr << "Failed to get capture format" << endl;
			return -1;
		}

		format.size = outputSize_;

		if (capture->setFormat(&format)) {
			cerr << "Failed to set capture format" << endl;
			return -1;
		}

		format.size = inputSize_;

		if (output->setFormat(&format)) {
			cerr << "Failed to set output format" << endl;
			return -1;
		}

		ret = capture->allocateBuffers(bufferCount, &captureBuffers_);
		if (ret < 0) {
			cerr << "Failed to allocate Capture Buffers" << endl;
			return -1;
		}

		ret = output->allocateBuffers(bufferCount, &outputBuffers_);
		if (ret < 0) {
			cerr << "Failed to allocate Output Buffers" << endl;
			return -1;
		}

		capture->bufferReady.connect(this, &M2MScaler::receiveCaptureBuffer);
		output->bufferReady.connect(this, &M2MScaler::outputBufferComplete);

		for (const std::unique_ptr<FrameBuffer> &buffer : captureBuffers_) {
			if (capture->queueBuffer(buffer.get())) {
				std::cout << "Failed to queue capture buffer" << std::endl;
			}
			/* mmap the buffer */
			MappedFrameBuffer map(buffer.get(), MappedFrameBuffer::MapFlag::ReadWrite);
			assert(map.isValid());
			inputMappedBuffer_.push_back(std::move(map));
			memcpy(map.planes()[0].data(), bbb_splash_rgb, bbb_splash_rgb_len);
		}

		int cookie = 0;
		for (const std::unique_ptr<FrameBuffer> &buffer : outputBuffers_) {
			if (output->queueBuffer(buffer.get())) {
				std::cout << "Failed to queue output buffer" << std::endl;
			}
			buffer->setCookie(cookie++);
			/* mmap the buffer */
			MappedFrameBuffer map(buffer.get(), MappedFrameBuffer::MapFlag::ReadWrite);
			assert(map.isValid());
			outputMappedBuffer_.push_back(std::move(map));
		}

		ret = capture->streamOn();
		if (ret) {
			cerr << "Failed to streamOn capture" << endl;
		}

		ret = output->streamOn();
		if (ret) {
			cerr << "Failed to streamOn output" << endl;
		}
	
		Timer timeout;
		timeout.start(50000);
		while (timeout.isRunning()) {
			dispatcher->processEvents();
			if (captureFrames_ > 4)
				break;
		}

		cerr << "Output " << outputFrames_ << " frames" << std::endl;
		cerr << "Captured " << captureFrames_ << " frames" << std::endl;

		if (captureFrames_ < 4) {
			cerr << "Failed to capture 30 frames within timeout." << std::endl;
			return -1;
		}

		ret = capture->streamOff();
		if (ret) {
			cerr << "Failed to StreamOff the capture device." << std::endl;
			return -1;
		}

		ret = output->streamOff();
		if (ret) {
			cerr << "Failed to StreamOff the output device." << std::endl;
			return -1;
		}

		return 0;
	}
private:
	std::unique_ptr<DeviceEnumerator> enumerator_;
	std::shared_ptr<MediaDevice> media_;
	V4L2M2MDevice *m2mScaler_;

	std::vector<std::unique_ptr<FrameBuffer>> captureBuffers_;
	std::vector<std::unique_ptr<FrameBuffer>> outputBuffers_;

	unsigned int outputFrames_;
	unsigned int captureFrames_;
	Size inputSize_;
	Size outputSize_;
	std::vector<MappedBuffer> inputMappedBuffer_;
	std::vector<MappedBuffer> outputMappedBuffer_;
};

int main()
{
	Size input(640, 480);
	Size output(320, 240);
	M2MScaler *s = new M2MScaler(input, output);

	assert(s->run() == 0);
}

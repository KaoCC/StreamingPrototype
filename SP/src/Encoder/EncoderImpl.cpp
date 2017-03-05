#include "EncoderImpl.hpp"


namespace SP {

	EncoderImpl::EncoderImpl(size_t width, size_t height) {
		encoder_init(width, height);
	}

	EncoderImpl::~EncoderImpl() {
		encoder_cleanup();
	}

	void EncoderImpl::startEncoding(uint8_t ** encodedBuf, int * encodedSize) {
		encoder_encode(encodedBuf, encodedSize);
	}

	std::uint8_t * EncoderImpl::getEncoderRawBuffer() {
		return encoder_get_raw_data_buf();
	}


	Encoder* CreateEncoder(int width, int height) {
		return new EncoderImpl(width, height);
	}

}

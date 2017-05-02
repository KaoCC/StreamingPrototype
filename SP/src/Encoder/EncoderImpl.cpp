#include "EncoderImpl.hpp"



namespace SP {

	EncoderImpl::EncoderImpl(size_t width, size_t height):losslessEnabled(false) {
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

	void EncoderImpl::enableLossless(bool enable){
	        losslessEnabled = enable;
        }

	std::unique_ptr<Encoder> CreateEncoder(std::size_t width, std::size_t height) {
		return std::make_unique<EncoderImpl>(width, height);
	}


}

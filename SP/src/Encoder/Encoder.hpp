#ifndef _SP_ENCODER_HPP_
#define _SP_ENCODER_HPP_

#include <memory>

namespace SP {

	class Encoder {
	public:
		virtual ~Encoder() = default;
		virtual  void startEncoding(uint8_t** encodedBuf, int* encodedSize) = 0;
		virtual std::uint8_t* getEncoderRawBuffer() = 0;
		virtual void enableLossless(bool enable) = 0;

	};

	std::unique_ptr<Encoder> CreateEncoder(std::size_t width, std::size_t height);

}

#endif

#ifndef _SP_ENCODER_HPP_
#define _SP_ENCODER_HPP_

namespace SP {

	class Encoder {
	public:
		virtual ~Encoder() = default;
		virtual  void startEncoding(uint8_t** encodedBuf, int* encodedSize) = 0;
		virtual std::uint8_t* getEncoderRawBuffer() = 0;

	};

	Encoder* CreateEncoder(int width, int height);

}

#endif

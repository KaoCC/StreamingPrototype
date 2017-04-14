
#ifndef _SP_ENCODERIMPL_HPP_
#define _SP_ENCODERIMPL_HPP_

#include <cstdint>
#include <cstddef>
#include "x264.h"

#include "Encoder.hpp"

namespace SP {


	class EncoderImpl : public Encoder{


	public:

		// public APIs

		EncoderImpl(std::size_t width, std::size_t height);

		virtual ~EncoderImpl();

		void startEncoding(uint8_t** encodedBuf, int* encodedSize);

		std::uint8_t* getEncoderRawBuffer();

                void enableLossless(bool enable);


	private:

		// helper fuctions

		void encoder_cleanup() {

			if (encoder_h264_encoder) {
				x264_encoder_close(encoder_h264_encoder);
				encoder_h264_encoder = nullptr;
			}
			if (encoder_pic_valid) {
				x264_picture_clean(&encoder_pic);
				encoder_pic_valid = false;
			}
		}

		int encoder_init(int width, int height) {
			x264_param_t param;

			encoder_raw_frame_size = width * height * 3;
			endoder_i_frame = 0;

			/* Get default params for preset/tuning */
			x264_param_default_preset(&param, NULL, "zerolatency");
			/* Configure non-default params */
			// param.i_csp = X264_CSP_I420;
			param.i_csp = X264_CSP_RGB;
			param.i_width = width;
			param.i_height = height;
			param.b_vfr_input = 0;
			param.b_repeat_headers = 1;
			param.b_annexb = 1;
			if(losslessEnabled){
                                param.rc.i_qp_constant = 0;
                                param.rc.i_rc_method = X264_RC_CQP;
                        }
			// alloc picture
			if (x264_picture_alloc(&encoder_pic, param.i_csp, param.i_width, param.i_height) < 0) {
				//printf("Fail to allocate picture buffer\n");
				encoder_cleanup();
				//return -1;

				throw "x264_picture_alloc Error";
			}
			encoder_pic_valid = true;
			// open encoder
			encoder_h264_encoder = x264_encoder_open(&param);
			if (!encoder_h264_encoder) {
				//printf("Fail to allocate open encoder\n");
				encoder_cleanup();
				//return -1;

				throw "encoder_h264_encoder Error";

			}


			return 0;
		}


		uint8_t* encoder_get_raw_data_buf() {
			return encoder_pic.img.plane[0];
		}



		// assume the RGB-raw data already in "encoder_get_raw_data_buf()"
		// param p_encoded_buf[out]: points to the buffer if frame emitted, NULL for no frame emitted
		// param p_encoded_size[out]: the size of encoded data, 0 when no frame emitted
		int encoder_encode(uint8_t** p_encoded_buf, int* p_encoded_size) {
			int i_frame_size;
			encoder_pic.i_pts = endoder_i_frame;
			i_frame_size = x264_encoder_encode(encoder_h264_encoder, &encoder_nal, &encoder_i_nal, &encoder_pic, &encoder_pic_out);
			if (i_frame_size < 0) {
				//printf("Encoding error\n");
				//return -1;


				throw "i_frame_size < 0 Error";

			} else if (i_frame_size) {
				*p_encoded_size = i_frame_size;
				*p_encoded_buf = encoder_nal->p_payload;
			} else {
				*p_encoded_size = 0;
				*p_encoded_buf = nullptr;
			}
			return 0;
		}






		// private members

		int encoder_raw_frame_size = 0;
		x264_picture_t encoder_pic;
		x264_picture_t encoder_pic_out;
		bool encoder_pic_valid = false;
		int endoder_i_frame;
		x264_nal_t *encoder_nal;
		int encoder_i_nal;

		x264_t* encoder_h264_encoder = nullptr;

		bool losslessEnabled;

	};


}










#endif



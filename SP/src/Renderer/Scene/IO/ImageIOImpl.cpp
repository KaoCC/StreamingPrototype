

#include "ImageIO.hpp"

#include "../Texture.hpp"

#include "OpenImageIO/imageio.h"

//#include "radeon_rays.h"
#include "math/float3.h"

namespace SP {


	class OpenImgIO : public ImageIO {
	public:
		Texture* loadImage(std::string const& filename) const override;
		void saveImage(std::string const& filename, Texture const* texture) const override;
	};


	// helper function
	static Texture::Format GetTextureForemat(OIIO_NAMESPACE::ImageSpec const& spec) {
		OIIO_NAMESPACE_USING

		if (spec.format.basetype == TypeDesc::UINT8)
			return Texture::Format::kRGBA8;
		else if (spec.format.basetype == TypeDesc::HALF)
			return Texture::Format::kRGBA16;
		else
			return Texture::Format::kRGBA32;
	}





	// Yet to be done !!!
	Texture * OpenImgIO::loadImage(std::string const & filename) const {

		OIIO_NAMESPACE_USING

			ImageInput* input = ImageInput::open(filename);

		if (!input) {
			throw std::runtime_error("Failed to load image: " + filename);
		}

		const ImageSpec& spec = input->spec();

		Texture::Format format = GetTextureForemat(spec);

		std::vector<std::uint8_t> textureData;


		size_t factor = 0;
		TypeDesc selectedType;

		if (format == Texture::Format::kRGBA8) {
			factor = sizeof(char) * 4;
			selectedType = TypeDesc::UINT8;
		} else if (format == Texture::Format::kRGBA16) {
			factor = sizeof(float) * 2;
			selectedType = TypeDesc::HALF;
		} else {  // kRGBA32
			factor = sizeof(RadeonRays::float3);
			selectedType = TypeDesc::FLOAT;
		}


		// resize the buffer
		size_t size = spec.width * spec.height * spec.depth * factor;
		textureData.resize(size);

		// Read data to storage
		input->read_image(selectedType, textureData.data(), factor);


		// close
		input->close();

		// KAOCC: the current stable version of OIIO dose not support this API
		//ImageInput::destroy(input);
		// cleanup
		delete input;

		// KAOCC: check the move syntax
		return new Texture(std::move(textureData.data()), RadeonRays::int2(spec.width, spec.height), format);
	}

	// KAOCC: Yet to be done !
	void OpenImgIO::saveImage(std::string const & filename, Texture const * texture) const {
		throw std::runtime_error("Yet to be done");
	}



	ImageIO * ImageIO::createImageIO() {
		return new OpenImgIO();
	}

}








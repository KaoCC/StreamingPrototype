

#include "ImageIO.hpp"

#include "../Texture.hpp"

#include "OpenImageIO/imageio.h"

namespace SP {


	class OpenImgIO : public ImageIO {
	public:
		Texture* loadImage(std::string const& filename) const override;
		void saveImage(std::string const& filename, Texture const* texture) const override;
	};








	// Yet to be done !!!
	Texture * OpenImgIO::loadImage(std::string const & filename) const {

		OIIO_NAMESPACE_USING

		ImageInput* input = ImageInput::open(filename);

		if (!input) {
			throw std::runtime_error("Failed to load image: " + filename);
		}

		const ImageSpec& spec = input->spec();




		// ...


		return nullptr;
	}

}








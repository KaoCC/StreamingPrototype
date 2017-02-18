#ifndef _SP_IMAGEIO_HPP_
#define _SP_IMAGEIO_HPP_

#include <string>


namespace SP {

	class Texture;

	class ImageIO {
	public:
		// Create default image IO
		static ImageIO* createImageIO();

		// Constructor
		ImageIO() = default;
		// Destructor
		~ImageIO() = default;

		// Load texture from file
		virtual Texture* loadImage(std::string const& filename) const = 0;
		virtual void saveImage(std::string const& filename, Texture const* texture) const = 0;

		// Disallow copying
		ImageIO(ImageIO const&) = delete;
		ImageIO& operator = (ImageIO const&) = delete;
	};







}












#endif

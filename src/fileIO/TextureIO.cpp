#include "TextureIO.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>

namespace TextureIO
{

ImageData Load(const std::string& path)
{
	ImageData result;
	stbi_set_flip_vertically_on_load(0);

	const bool is_hdr = (path.find(".hdr") != std::string::npos);
	result.is_hdr = is_hdr;

	if (is_hdr)
	{
		float* data = stbi_loadf(path.c_str(), &result.width, &result.height, &result.channels, 4);
		if (data)
		{
			const size_t count = static_cast<size_t>(result.width) * result.height * 4;
			result.pixels_hdr.assign(data, data + count);
			stbi_image_free(data);
#ifdef _DEBUG
			std::cout << "HDR image loaded: " << path << " [" << result.width << ":" << result.height << "]\n";
#endif
		}
		else
		{
			std::cout << "HDR image load FAILED: " << path << "\n";
		}
	}
	else
	{
		uint8_t* data = stbi_load(path.c_str(), &result.width, &result.height, &result.channels, 4);
		if (data)
		{
			const size_t count = static_cast<size_t>(result.width) * result.height * 4;
			result.pixels.assign(data, data + count);
			stbi_image_free(data);
#ifdef _DEBUG
			std::cout << "Image loaded: " << path << " [" << result.width << ":" << result.height << "]\n";
#endif
		}
		else
		{
			std::cout << "Image load FAILED: " << path << "\n";
		}
	}

	return result;
}

void SaveHDR(const std::string& path, int w, int h, const float* data)
{
	stbi_flip_vertically_on_write(0);
	const int status = stbi_write_hdr(path.c_str(), w, h, 4, data);
	if (status == 0)
		std::cout << "SaveHDR failed: " << path << "\n";
}

void SavePNG(const std::string& path, int w, int h, const uint8_t* data)
{
	stbi_flip_vertically_on_write(0);
	const int status = stbi_write_png(path.c_str(), w, h, 4, data, 0);
	if (status == 0)
		std::cout << "SavePNG failed: " << path << "\n";
}

} // namespace TextureIO

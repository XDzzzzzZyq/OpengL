/**
 * @file TextureIO.h
 * @brief CPU-side image file I/O for loading and saving texture data.
 *
 * TextureIO separates raw image file access from GPU resource management.
 * It owns no OpenGL state and carries no GL dependency — it operates purely
 * on CPU-side pixel buffers and the file system via stb_image / stb_image_write.
 *
 * The GPU class Texture delegates file loading and saving here:
 * - Texture(file) constructor calls TextureIO::Load to obtain raw pixels, then
 *   uploads them to the GPU.
 * - Texture::SaveTexture downloads pixels from the GPU, then calls
 *   TextureIO::SaveHDR or TextureIO::SavePNG to write the file.
 *
 * Usage:
 * @code
 * // Load raw pixel data from disk (no GL context required)
 * auto img = TextureIO::Load("res/tex/albedo.png");
 * if (!img.pixels.empty())
 *     // upload img.pixels to GL ...
 *
 * // Save HDR frame to disk
 * std::vector<float> hdr_pixels = ...;  // downloaded from GPU
 * TextureIO::SaveHDR("result/frame.hdr", width, height, hdr_pixels.data());
 * @endcode
 *
 * @note Not thread-safe. stb_image global flip flag is reset before each call.
 */

#pragma once

#include <string>
#include <vector>

/**
 * @brief CPU-side image file I/O (no OpenGL dependency).
 */
namespace TextureIO
{
	/**
	 * @brief Raw CPU-side image data returned by TextureIO::Load.
	 *
	 * Exactly one of @c pixels (LDR) or @c pixels_hdr (HDR) is populated
	 * depending on whether @c is_hdr is false or true.
	 */
	struct ImageData
	{
		int width    = 0; ///< Image width in pixels
		int height   = 0; ///< Image height in pixels
		int channels = 0; ///< Number of channels in source file (before forced 4-channel decode)
		bool is_hdr  = false; ///< True if the source file is HDR (float data)

		std::vector<uint8_t> pixels;     ///< LDR pixel data (RGBA, 4 bytes/pixel) when !is_hdr
		std::vector<float>   pixels_hdr; ///< HDR pixel data (RGBA, 4 floats/pixel) when is_hdr
	};

	/**
	 * @brief Loads image data from a file into CPU memory.
	 *
	 * Infers HDR vs LDR from the file extension (.hdr → HDR, otherwise LDR).
	 * Always decodes to 4 channels (RGBA).
	 *
	 * @param path Full file system path to the image file.
	 * @return Populated ImageData. On failure, width/height are 0 and pixel vectors are empty.
	 * @note stb_image vertical flip is disabled before loading.
	 */
	ImageData Load(const std::string& path);

	/**
	 * @brief Saves 32-bit float RGBA pixel data to an HDR file.
	 *
	 * @param path  Full output path including the .hdr extension (e.g. "result/frame.hdr").
	 * @param w     Image width in pixels.
	 * @param h     Image height in pixels.
	 * @param data  Pointer to RGBA float pixel data (w * h * 4 floats).
	 * @note stb_image_write vertical flip is disabled before writing.
	 */
	void SaveHDR(const std::string& path, int w, int h, const float* data);

	/**
	 * @brief Saves 8-bit RGBA pixel data to a PNG file.
	 *
	 * @param path  Full output path including the .png extension (e.g. "result/frame.png").
	 * @param w     Image width in pixels.
	 * @param h     Image height in pixels.
	 * @param data  Pointer to RGBA uint8 pixel data (w * h * 4 bytes).
	 * @note stb_image_write vertical flip is disabled before writing.
	 */
	void SavePNG(const std::string& path, int w, int h, const uint8_t* data);
}

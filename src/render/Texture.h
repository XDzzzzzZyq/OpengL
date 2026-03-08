/**
 * @file Texture.h
 * @brief Texture wrapper for OpenGL texture objects and texture resource management.
 * 
 * Provides the Texture class for creating, managing, and manipulating OpenGL textures,
 * along with TextureLib for global texture caching and resource sharing. Supports
 * various texture types (2D, cubemap, HDR, depth, etc.) and format conversions.
 * 
 * GPU Resource Ownership:
 * - Texture owns the OpenGL texture object (tex_ID)
 * - Resources released via glDeleteTextures in destructor
 * - Move semantics transfer ownership
 * - TextureLib provides shared_ptr-based resource sharing
 * 
 * Architecture:
 * - RAII wrapper for OpenGL texture objects
 * - Global texture cache (TextureLib) prevents duplicate loading
 * - Supports HDR, LDR, cubemaps, and framebuffer textures
 * - Provides IBL processing (irradiance, specular, BRDF LUT)
 */

#pragma once
#include <iostream>
#include <string>
#include <GL/glew.h>

#include "glm/glm.hpp"

/*
 * How to add a texture:
 * 1. Assign a (GLuint) slot for the texture (slot 0-14 are currently in use)
 * 2. Create a function that allocates a TextureRes in TextureLib
 * 3. Call that function and bind the texture to the slot chosen in the rendering loop
 * 4. Pass the slot number to the shader
 */

/**
 * @brief OpenGL texture object wrapper with RAII semantics.
 * 
 * Texture encapsulates an OpenGL texture object and provides methods for creation,
 * binding, resizing, and format conversion. Supports 2D textures, cubemaps, and
 * various pixel formats (LDR, HDR, depth, etc.).
 * 
 * Constructors:
 * - From file: Loads image file and creates texture
 * - From data: Creates texture from raw pixel data
 * - For framebuffer: Allocates empty texture for render targets
 * - For processing: Creates texture for IBL/processing outputs
 * 
 * Usage:
 * @code
 * // Load from file
 * Texture tex("albedo.png", Texture::RGBA_TEXTURE, GL_REPEAT);
 * tex.Bind(0);  // Bind to texture unit 0
 * 
 * // Create framebuffer texture
 * Texture fb_tex(GL_REPEAT, 1920, 1080);  // For color attachment
 * 
 * // Create from data
 * std::vector<float> data(width * height * 4);
 * Texture gen_tex(width, height, GL_RGBA32F, data.data());
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 * @note Resource ownership: Owns tex_ID, releases via glDeleteTextures in destructor.
 */
class Texture
{
public:

	/**
	 * @brief OpenGL texture format and channel layout.
	 * 
	 * Abstracts the OpenGL pixel format (channels and data type) used when
	 * allocating or uploading texture data. Values also serve as default
	 * texture unit slot bases: e.g. BUFFER_TEXTURE (6) is the base slot
	 * offset for framebuffer textures passed as shader uniforms.
	 * 
	 * @note BUFFER_TEXTURE must be kept as value 6; it is used as an offset
	 *       for framebuffer texture slots (BUFFER_TEXTURE + FBType = slot).
	 */
	enum TextureType
	{
		NONE_TEXTURE,            ///< Invalid/uninitialized texture
		RGBA_TEXTURE,            ///< RGBA 8-bit LDR (GL_RGBA8, GL_UNSIGNED_BYTE)
		RGB_TEXTURE,             ///< RGB 8-bit LDR (GL_RGB8, GL_UNSIGNED_BYTE)
		HDR_TEXTURE = 4,         ///< RGBA 32-bit float HDR (GL_RGBA32F, GL_FLOAT)
		HDR_CUBE_TEXTURE = 5,    ///< HDR cubemap (GL_RGBA32F, GL_FLOAT, GL_TEXTURE_CUBE_MAP)
		BUFFER_TEXTURE = 6,      ///< RGBA 8-bit framebuffer; base slot offset for shader uniforms
		HDR_BUFFER_TEXTURE,      ///< RGBA 32F HDR framebuffer (GL_RGBA32F, GL_FLOAT)
		FLOAT_BUFFER_TEXTURE,    ///< Two-channel RG 16F (GL_RG16F, GL_FLOAT)
		RG_TEXTURE,              ///< Two-channel RG 16F (GL_RG16F, GL_FLOAT)
		LAYERED_TEXTURE,         ///< RGBA 32F texture array (GL_TEXTURE_2D_ARRAY)
		LIGHTING_CACHE = 30,     ///< Single-channel R 16F for light caching (GL_R16F)
		DEPTH_CUBE_TEXTURE = 31, ///< Depth cubemap for shadow mapping (GL_DEPTH_COMPONENT32F)
		DEPTH_TEXTURE            ///< Depth 2D for shadow mapping (GL_DEPTH_COMPONENT32F)
	};
private:

	int im_w = 0, im_h = 0, im_bpp = 0; ///< Image width, height, bytes per pixel
	std::string tex_path;                ///< Texture file path or name
	GLuint tex_ID = 0;                   ///< OpenGL texture object ID

	/**
	 * @brief Copies metadata from another texture (for copy constructor).
	 * @param _tex Source texture to copy from
	 */
	void _cpyInfo(const Texture& _tex);
	
	/**
	 * @brief Deletes the OpenGL texture object.
	 */
	void _delTexture();
	
	/**
	 * @brief Deep-copies texture metadata and GPU storage from another texture.
	 * @param _tex Source texture
	 */
	void _deepCopyFrom(const Texture& _tex);
	
	/**
	 * @brief Resets texture ID, deleting old texture if needed.
	 * @param _ID New texture ID to assign
	 */
	void _resetTexID(GLuint _ID) { if (tex_ID > 0 && tex_ID != _ID)_delTexture(); tex_ID = _ID; }

public:

	TextureType tex_type = NONE_TEXTURE; ///< Texture format type
	GLuint tex_slot_offset = 0;                       ///< Texture unit offset for binding

	/**
	 * @brief Constructs texture from image file.
	 * @param texpath Path to image file
	 * @param tex_type Texture type (RGBA_TEXTURE, HDR_TEXTURE, etc.)
	 * @param tile_type Texture wrapping mode (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
	 * @note Automatically loads file and uploads to GPU
	 */
	Texture(const std::string& texpath, TextureType tex_type, GLuint tile_type);
	
	/**
	 * @brief Constructs empty texture for framebuffer attachment.
	 * @param _w Width in pixels
	 * @param _h Height in pixels
	 * @param tex_type Texture type (RGBA_TEXTURE, HDR_TEXTURE, etc.)
	 * @param tile_type Texture wrapping mode
	 * @note Allocates GPU storage but does not initialize pixel data
	 */
	Texture(int _w, int _h, TextureType tex_type, GLuint tile_type);
	
	/**
	 * @brief Constructs texture from raw pixel data.
	 * @param _w Width in pixels
	 * @param _h Height in pixels
	 * @param _layout Internal format (GL_RGBA8, GL_RGBA32F, etc.)
	 * @param _ptr Pointer to pixel data
	 * @param _min_filter Minification filter (default GL_LINEAR)
	 * @param _mag_filter Magnification filter (default GL_LINEAR)
	 * @param _wrap_s S-axis wrapping mode (default GL_REPEAT)
	 * @param _wrap_t T-axis wrapping mode (default GL_REPEAT)
	 */
	Texture(int _w, int _h, GLuint _layout, const void* _ptr,
		GLint _min_filter = GL_LINEAR, GLint _mag_filter = GL_LINEAR,
		GLint _wrap_s = GL_REPEAT, GLint _wrap_t = GL_REPEAT);
	
	/**
	 * @brief Constructs texture by copying metadata from another texture.
	 * @param _w Width
	 * @param _h Height
	 * @param _ID OpenGL texture ID to wrap
	 * @param _type Texture type
	 * @param _name Texture name
	 * @note Does not create new texture, wraps existing ID
	 */
	Texture(int _w, int _h, GLuint _ID, TextureType _type, std::string _name);
	
	/**
	 * @brief Constructs empty texture with specified type.
	 * @param _w Width
	 * @param _h Height
	 * @param _type Texture type
	 */
	Texture(int _w, int _h, TextureType _type);
	
	/**
	 * @brief Default constructor (creates null texture).
	 */
	Texture();

	/**
	 * @brief Copy constructor (deep copy, creates new texture).
	 * @param tex Source texture to copy
	 */
	Texture(const Texture& tex);
	
	/**
	 * @brief Move constructor (transfers ownership of GPU resource).
	 * @param tex Source texture to move from
	 */
	Texture(Texture&& tex) noexcept;

	/**
	 * @brief Copy assignment operator.
	 * @param tex Source texture
	 * @return Reference to this texture
	 */
	Texture& operator=(const Texture& tex);
	
	/**
	 * @brief Move assignment operator (transfers ownership).
	 * @param tex Source texture
	 * @return Reference to this texture
	 */
	Texture& operator=(Texture&& tex)noexcept;

	/**
	 * @brief Destructor (releases OpenGL texture object).
	 */
	~Texture();

	/**
	 * @brief Resizes the texture.
	 * @param size New size (width, height)
	 * @note Reallocates GPU storage, existing data is lost
	 */
	void Resize(const glm::vec2& size);
	
	/**
	 * @brief Resizes the texture.
	 * @param x New width
	 * @param y New height
	 */
	void Resize(GLuint x, GLuint y);
	
	/**
	 * @brief Offsets the texture unit slot for binding.
	 * @param _offset Slot offset to add
	 */
	void OffsetSlot(GLuint _offset) { tex_slot_offset += _offset; };

public:
	/**
	 * @brief Binds texture to a texture unit for sampling.
	 * @param slot Texture unit (0-31), or -1 to use tex_slot_offset
	 * @note Call before rendering to make texture accessible to shaders
	 */
	void Bind(GLuint slot = -1) const;
	
	/**
	 * @brief Binds texture as compute shader image (read/write).
	 * @param slot Image unit (0-7), or -1 to use tex_slot_offset
	 * @param read_or_write Access mode (GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE)
	 * @param _level Mipmap level (default 0)
	 */
	void BindC(GLuint slot = -1, GLuint read_or_write = GL_READ_WRITE, GLuint _level = 0) const;
	
	/**
	 * @brief Binds texture for shader uniform access (legacy, prefer Bind).
	 * @param slot Texture unit
	 */
	void BindU(GLuint slot = -1) const;
	
	/**
	 * @brief Static method to bind texture by ID.
	 * @param _id OpenGL texture ID
	 * @param _slot Texture unit
	 * @param _type Texture type (determines bind target)
	 */
	static inline void BindM(GLuint _id, GLuint _slot = 0, TextureType _type=RGBA_TEXTURE);
	
	/**
	 * @brief Unbinds texture from compute shader image unit.
	 * @param slot Image unit, or -1 for tex_slot_offset
	 * @param read_or_write Access mode (must match BindC call)
	 * @param _level Mipmap level
	 */
	void UnbindC(GLuint slot = -1, GLuint read_or_write = GL_READ_WRITE, GLuint _level = 0) const;
	
	/**
	 * @brief Unbinds texture from texture unit.
	 */
	void Unbind() const;

public:
	/**
	 * @brief Returns texture width.
	 */
	inline int GetW() const { return im_w; }
	
	/**
	 * @brief Returns texture height.
	 */
	inline int GetH() const { return im_h; }
	
	/**
	 * @brief Returns texture size as vec2.
	 */
	inline glm::vec2 GetSize() const { return { im_w, im_h }; };
	
	/**
	 * @brief Returns bytes per pixel.
	 */
	inline int GetBPP() const { return im_bpp; }
	
	/**
	 * @brief Returns OpenGL texture ID.
	 */
	GLuint GetTexID() const { return tex_ID; }
	
	/**
	 * @brief Returns texture name/path.
	 */
	std::string GetTexName() const { return tex_path; }

public:
	/**
	 * @brief Texture storage format information tuple.
	 * 
	 * Contains OpenGL format constants for texture creation:
	 * - internal_layout: Internal format (e.g., GL_RGBA8, GL_RGBA32F)
	 * - layout: Pixel data format (e.g., GL_RGBA, GL_RGB)
	 * - data_type: Pixel data type (e.g., GL_UNSIGNED_BYTE, GL_FLOAT)
	 * - texture_type: Texture target (e.g., GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP)
	 */
	using TexStorageInfo = const std::tuple<GLuint, GLuint, GLuint, GLuint>;

	/**
	 * @brief Parses TextureType enum to OpenGL format constants.
	 * @param _type Texture type enum
	 * @return Tuple of (internal_layout, layout, data_type, texture_type)
	 */
	inline static TexStorageInfo ParseFormat(TextureType _type);
	
	/**
	 * @brief Sets texture parameters (filtering, wrapping).
	 * @tparam Type Texture target (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, etc.)
	 * @param _id Texture ID
	 * @param _fil_min Minification filter
	 * @param _fil_max Magnification filter
	 * @param _warp_s S-axis wrapping mode
	 * @param _warp_t T-axis wrapping mode
	 * @param _lev_min Minimum mipmap level
	 * @param _lev_max Maximum mipmap level
	 * @param _warp_r R-axis wrapping mode (for 3D/cubemap)
	 */
	template<GLuint Type>
	inline static void SetTexParam(GLuint _id, GLuint _fil_min, GLuint _fil_max, GLuint _warp_s = 0, GLuint _warp_t = 0, GLuint _lev_min = 0, GLuint _lev_max = 0, GLuint _warp_r = 0);

public: /** @name Texture Processing
         * Methods for IBL and format conversion processing.
         */
        ///@{
	
	/**
	 * @brief Generates irradiance convolution map for diffuse IBL.
	 * @param _Tar_Tex Source environment map
	 * @note Convolves environment to low-frequency diffuse lighting
	 */
	void GenIrradiaceConvFrom(const Texture& _Tar_Tex);
	
	/**
	 * @brief Generates specular IBL map with importance sampling.
	 * @param _Tar_Tex Source environment map
	 * @param to_cubemap If true, outputs cubemap; otherwise equirectangular
	 */
	void GenIBLSpecularFrom(const Texture& _Tar_Tex, bool to_cubemap = false);
	
	/**
	 * @brief Generates diffuse IBL map.
	 * @param _Tar_Tex Source environment map
	 * @param to_cubemap If true, outputs cubemap; otherwise equirectangular
	 */
	void GenIBLDiffuseFrom(const Texture& _Tar_Tex, bool to_cubemap = false);
	
	/**
	 * @brief Converts equirectangular map to cubemap.
	 * @param _Tar_Tex Source equirectangular texture
	 * @param res Cubemap face resolution (default 1024)
	 */
	void GenCubeMapFrom(const Texture& _Tar_Tex, int res = 1024);
	
	/**
	 * @brief Converts cubemap to equirectangular map.
	 * @param _Tar_Tex Source cubemap texture
	 * @param _w Output width (default 2048)
	 * @param _h Output height (default 1024)
	 */
	void GenERectMapFrom(const Texture& _Tar_Tex, int _w = 2048, int _h = 1024);
	
	/**
	 * @brief Converts texture to depth format.
	 * @param _Tar_Tex Source texture
	 */
	void ConvertDepthFrom(const Texture& _Tar_Tex);
	
	/**
	 * @brief Converts texture to depth cubemap format.
	 * @param _Tar_Tex Source texture
	 */
	void ConvertDepthCubeFrom(const Texture& _Tar_Tex);
	
	/**
	 * @brief Converts texture to PNG-compatible format (RGBA8).
	 * @param _Tar_Tex Source texture
	 */
	void ConvertPNGFrom(const Texture& _Tar_Tex);

	/**
	 * @brief Fills texture with solid color.
	 * @param col Color to fill (RGBA)
	 */
	void FillColor(const glm::vec4 col);
	///@}

private:
	/** @name Internal Processing Methods
	 * Implementation details for texture processing.
	 */
	///@{
	void GenIrradianceConv(GLuint _tar_ID, int _tar_w, int _tar_h, TextureType _tar_type = HDR_TEXTURE);
	void GenIBLSpecular(GLuint _tar_ID, int _tar_w, int _tar_h, TextureType _tar_type = HDR_TEXTURE, bool to_cubemap = false);
	void GenIBLDiffuse(GLuint _tar_ID, int _tar_w, int _tar_h, TextureType _tar_type = HDR_TEXTURE, bool to_cubemap = false);
	void GenCubeMap(GLuint _tar_ID, int _tar_res, TextureType _tar_type = HDR_TEXTURE);
	void GenERectMap(GLuint _tar_ID, int _w, int _h, TextureType _tar_type = HDR_TEXTURE);
	void ConvertDepth(GLuint _tar_ID, int _w, int _h, TextureType _tar_type = DEPTH_TEXTURE);
	void ConvertDepthCube(GLuint _tar_ID, int _w, int _h, TextureType _tar_type = DEPTH_CUBE_TEXTURE);
	void ConvertPNG(GLuint _tar_ID, int _w, int _h);
	///@}

public:

	/**
	 * @brief Saves texture to file.
	 * @param _path Output file path
	 * @param force_png If true, always save as PNG (default false, uses extension)
	 * @param force_cube If true, save as cubemap faces (6 images)
	 */
	void SaveTexture(std::string _path, bool force_png=false, bool force_cube=false) const;
	
	/**
	 * @brief Prints texture metadata to console.
	 * @note Debug utility for inspecting texture properties
	 */
	void PrintTexture() const;
};

#include <unordered_map>

/**
 * @brief Global texture resource cache and factory.
 * 
 * TextureLib provides centralized texture management with automatic caching
 * to prevent duplicate loading of textures. All textures are reference-counted
 * via shared_ptr, and resources are released when no longer referenced.
 * 
 * Functionality:
 * - Load and cache textures from files
 * - Generate procedural textures (noise, LUTs)
 * - Provide built-in textures (IBL BRDF LUT, LTC tables, etc.)
 * - Manage texture lifetime and sharing
 * 
 * Usage:
 * @code
 * auto tex = TextureLib::LoadTexture("albedo.png");  // Loads or returns cached
 * auto brdf_lut = TextureLib::IBL_LUT();             // Built-in LUT
 * auto noise = TextureLib::Noise_2D_16x16xN(6);      // Procedural noise
 * @endcode
 * 
 * @note All methods are static (global texture pool).
 * @note Thread-safety: Not thread-safe. Access from main thread only.
 */
class TextureLib {


public:

	static std::string root_dir; ///< Root directory for texture files
	
	/**
	 * @brief Infers texture type from file extension.
	 * @param path File path
	 * @return TextureType enum (.png → RGBA_TEXTURE, .jpg → RGB_TEXTURE, .hdr → HDR_TEXTURE)
	 */
	static Texture::TextureType ParseFileEXT(std::string path);

	using TextureRes = std::shared_ptr<Texture>; ///< Shared pointer to Texture (for resource sharing)

public:
	/**
	 * @brief Noise texture types for procedural generation.
	 */
	enum NoiseType
	{
		NONE_NOISE,     ///< No noise
		UNIFORM_NOISE,  ///< Uniform random noise
		GAUSSIAN_NOISE, ///< Gaussian (normal) distribution noise
		UNI_2D_NOISE    ///< 2D uniform noise
	};

public:
	/**
	 * @brief Retrieves a cached texture by name.
	 * @param _name Texture name (file name or procedural name)
	 * @return Shared pointer to Texture, or nullptr if not found
	 */
	static TextureRes GetTexture(const std::string& _name);
	
	/**
	 * @brief Loads texture from file or returns cached instance.
	 * @param _name Texture file name (relative to root_dir)
	 * @return Shared pointer to Texture
	 * @note If texture already loaded, returns cached instance
	 */
	static TextureRes LoadTexture(std::string _name);
	
	/**
	 * @brief Returns OpenGL texture ID for a cached texture.
	 * @param _name Texture name
	 * @return Texture ID, or 0 if not found
	 */
	static GLuint GetTextureID(const std::string& _name);

public:
	/**
	 * @brief Generates or retrieves 4x4 2D noise texture.
	 * @return Shared pointer to noise texture
	 */
	static TextureRes Noise_2D_4x4();
	
	/**
	 * @brief Generates or retrieves 4x4xN layered 2D noise texture.
	 * @param n Number of noise layers (default 6)
	 * @return Shared pointer to layered noise texture
	 */
	static TextureRes Noise_2D_4x4xN(int n=6);
	
	/**
	 * @brief Generates or retrieves 16x16xN layered 2D noise texture.
	 * @param n Number of noise layers (default 6)
	 * @return Shared pointer to layered noise texture
	 */
	static TextureRes Noise_2D_16x16xN(int n=6);

	/**
	 * @brief Generates or retrieves IBL BRDF lookup table.
	 * @return Shared pointer to BRDF LUT texture (RG16F, 512x512)
	 * @note Used for split-sum approximation in PBR IBL
	 */
	static TextureRes IBL_LUT();

	/**
	 * @brief Generates or retrieves LTC matrix 1 lookup table.
	 * @return Shared pointer to LTC1 texture
	 * @note Used for linearly transformed cosines (area lights)
	 */
	static TextureRes LTC1();
	
	/**
	 * @brief Generates or retrieves LTC matrix 2 lookup table.
	 * @return Shared pointer to LTC2 texture
	 * @note Used for linearly transformed cosines (area lights)
	 */
	static TextureRes LTC2();

private:
	/**
	 * @brief Generates a single noise texture.
	 * @param _type Noise type (uniform, Gaussian, etc.)
	 * @param _w Width
	 * @param _h Height
	 */
	static void GenNoiseTexture(NoiseType _type, int _w, int _h);
	
	/**
	 * @brief Generates layered noise textures.
	 * @param _type Noise type
	 * @param _w Width
	 * @param _h Height
	 * @param _n Number of layers
	 */
	static void GenNoiseTextures(NoiseType _type, int _w, int _h, int _n);
};


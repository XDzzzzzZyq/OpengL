/**
 * @file StorageBuffer.h
 * @brief OpenGL shader storage buffer object (SSBO) abstraction.
 * 
 * StorageBuffer provides RAII wrapper around OpenGL SSBO for large read/write
 * data accessible from shaders. Supports template-based operations for type-safe
 * data transfer with CPU-GPU bidirectional communication.
 * 
 * @note All OpenGL resources are released in destructor.
 */

#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <cstring>

/**
 * @brief Storage buffer data type classification.
 */
enum SSBType
{
	NONE_LIST,   ///< Untyped buffer
	FLOAT_LIST,  ///< Float array
	INT_LIST,    ///< Integer array
	VEC2_LIST,   ///< vec2 array
	VEC3_LIST,   ///< vec3 array
	CUSTOM_LIST  ///< Custom structure array
};

/**
 * @brief Type trait to detect non-vector types.
 * @tparam T Type to check
 */
template<typename T>
struct is_not_vector : std::true_type {};

/**
 * @brief Specialization for std::vector (false for vectors).
 * @tparam T Vector element type
 * @tparam C Vector allocator type
 */
template<typename T, typename C>
struct is_not_vector<std::vector<T, C>> : std::false_type {};

/**
 * @brief OpenGL shader storage buffer object wrapper.
 * 
 * StorageBuffer (SSBO) provides read/write access to large buffers from shaders.
 * Unlike UBO, SSBO supports:
 * - Much larger buffers (up to GPU memory limit)
 * - Write access from shaders
 * - Dynamic sizing
 * - Atomic operations
 * 
 * Common use cases:
 * - Light arrays for deferred rendering
 * - Particle systems
 * - Compute shader outputs
 * - GPU sorting and reduction
 * 
 * Usage:
 * @code
 * StorageBuffer ssbo(FLOAT_LIST, 3);
 * std::vector<float> data = {...};
 * ssbo.GenStorageBuffer(data);
 * ssbo.BindBufferBase();
 * 
 * // After compute shader
 * std::vector<float> result;
 * ssbo.ReadStorageBuffer(result);
 * @endcode
 * 
 * @note Thread-safety: Not thread-safe. Must be used from OpenGL context thread.
 * @note Ownership: Owns OpenGL SSBO and releases it in destructor.
 * @note Performance: Reading back from GPU is slow. Minimize CPU readbacks.
 */
class StorageBuffer  //shader storage buffer object SSBO
{
private:
	GLuint ssbo_ID = 0;            ///< OpenGL storage buffer object ID
	GLuint ssbo_base = 3;          ///< Binding point index
	SSBType ssbo_type = NONE_LIST; ///< Buffer data type

	void _cpyInfo(const StorageBuffer& ssbo); ///< Copies SSBO info
	void _delSSB();                            ///< Deletes OpenGL storage buffer
	
	/**
	 * @brief Resets SSBO ID, deleting old buffer if different.
	 * @param _ID New SSBO ID
	 */
	void _resetSSBID(GLuint _ID) { if (ssbo_ID > 0 && ssbo_ID != _ID)_delSSB(); ssbo_ID = _ID; }

public:
	/**
	 * @brief Default constructor (empty buffer).
	 */
	StorageBuffer();
	
	/**
	 * @brief Constructs SSBO with type classification.
	 * @param type Data type classification
	 */
	StorageBuffer(SSBType type);
	
	/**
	 * @brief Constructs SSBO with type and binding point.
	 * @param type Data type classification
	 * @param base Binding point index
	 */
	StorageBuffer(SSBType type, GLuint base);
	
	/**
	 * @brief Destructor. Releases OpenGL SSBO.
	 */
	~StorageBuffer();


	/**
	 * @brief Copy constructor.
	 * @param ssbo Source storage buffer
	 */
	StorageBuffer(const StorageBuffer& ssbo);
	
	/**
	 * @brief Move constructor.
	 * @param ssbo Source storage buffer (invalidated)
	 */
	StorageBuffer(StorageBuffer&& ssbo) noexcept;

	/**
	 * @brief Copy assignment operator.
	 * @param ssbo Source storage buffer
	 * @return Reference to this
	 */
	StorageBuffer& operator=(const StorageBuffer& ssbo);
	
	/**
	 * @brief Move assignment operator.
	 * @param ssbo Source storage buffer (invalidated)
	 * @return Reference to this
	 */
	StorageBuffer& operator=(StorageBuffer&& ssbo) noexcept;

public:

	/**
	 * @brief Binds this SSBO (GL_SHADER_STORAGE_BUFFER target).
	 */
	void BindBuffer() const;
	
	/**
	 * @brief Binds SSBO to its binding point for shader access.
	 * @param _base Binding point override (default: use stored binding)
	 */
	void BindBufferBase(GLuint _base = -1) const;
	
	/**
	 * @brief Unbinds the current SSBO.
	 */
	void UnbindBuffer() const;
	
	/**
	 * @brief Sets the binding point index.
	 * @param base New binding point
	 */
	void SetBufferBase(GLuint base);

public:
	/**
	 * @brief Returns OpenGL buffer object ID.
	 * @return SSBO ID
	 */
	GLuint GetID() const { return ssbo_ID; }
	
	/**
	 * @brief Returns binding point index.
	 * @return Binding point
	 */
	GLuint GetBase() const { return ssbo_base; }
	
	/**
	 * @brief Returns buffer data type classification.
	 * @return SSBType
	 */
	SSBType GetType() const { return ssbo_type; }

public:

	/**
	 * @brief Uploads vector data to SSBO.
	 * @tparam T Element type
	 * @param src Source vector (copied to GPU)
	 * @note Uses GL_STATIC_DRAW. For frequently updated data, consider dynamic approach.
	 */
	template <typename T>
	void GenStorageBuffer(const std::vector<T>& src);

	/**
	 * @brief Reads data from SSBO back to CPU.
	 * @tparam T Element type
	 * @param tar Target vector (resized if empty)
	 * @param _offset Byte offset into buffer
	 * @note Performance: GPU readback is slow. Minimize usage.
	 */
	template <typename T>
	void ReadStorageBuffer(std::vector<T>& tar, GLuint _offset = 0);

	/**
	 * @brief Uploads structured data (header + array) to SSBO.
	 * @tparam _Info Header structure type
	 * @tparam _Ele Array element type
	 * @param _info Header data
	 * @param _data Array data
	 * @note Useful for arrays with metadata (e.g., count, bounds)
	 */
	template <typename _Info, typename _Ele> requires is_not_vector<_Info>::value
	void GenStorageBuffers(const _Info& _info, const std::vector<_Ele>& _data);

};

template <typename _Info, typename _Ele> requires is_not_vector<_Info>	::value
void StorageBuffer::GenStorageBuffers(const _Info& _info, const std::vector<_Ele>& _data)
{
	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(_Info) + _data.size()*sizeof(_Ele), nullptr, GL_STATIC_DRAW);

	void* bufferData = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	std::memcpy(static_cast<char*>(bufferData), &_info, sizeof(_Info));
	std::memcpy(static_cast<char*>(bufferData) + sizeof(_Info), _data.data(), _data.size() * sizeof(_Ele));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	UnbindBuffer();
}

template <typename T>
void StorageBuffer::GenStorageBuffer(const std::vector<T>& list)
{
	if (list.size() == 0) return;

	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, list.size() * sizeof(T), list.data(), GL_STATIC_DRAW);
	UnbindBuffer();
}

template <typename T>
void StorageBuffer::ReadStorageBuffer(std::vector<T>& tar, GLuint _offset)
{
	BindBuffer();
	void* dataPtr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

	if (dataPtr == nullptr)
		return;

	/* | -- Offset -- | -----------  Target  -----------|
	 * |-------------   Whole Data Range   -------------| */

	if (tar.size() == 0) {
		GLint bufferSize;
		glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &bufferSize);
		tar.resize(bufferSize - _offset);
	}

	std::memcpy(tar.data(), static_cast<char*>(dataPtr) + _offset, tar.size() * sizeof(T));
	
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	UnbindBuffer();
}


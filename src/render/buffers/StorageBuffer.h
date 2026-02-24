/**
 * @file StorageBuffer.h
 * @brief OpenGL shader storage buffer object (SSBO) abstraction.
 *
 * StorageBuffer provides a RAII wrapper around an OpenGL SSBO for large
 * read/write data accessible from shaders. Inherits copy, move, and
 * deep-copy semantics from GLBuffer. Supports template-based operations for
 * type-safe CPU↔GPU data transfer.
 *
 * @note All OpenGL resources are released deterministically in the destructor.
 */

#pragma once

#include "Buffers.h"
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
 * StorageBuffer (SSBO) provides read/write access to large buffers from
 * shaders. It inherits RAII resource management from GLBuffer, including
 * GPU-side deep copy via glCopyBufferSubData. buf_size is updated by
 * GenStorageBuffer/GenStorageBuffers so deep copy always reflects the
 * current allocation.
 *
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
 *
 * // Deep copy (new GPU buffer allocated):
 * StorageBuffer copy = ssbo;
 * @endcode
 *
 * @note Thread-safety: Not thread-safe. Must be used from the OpenGL context thread.
 * @note Ownership: Inherits GLBuffer ownership; releases via glDeleteBuffers in destructor.
 * @note Performance: Reading back from GPU is slow. Minimise CPU readbacks.
 */
class StorageBuffer : public GLBuffer
{
private:
	GLuint  ssbo_base = 3;          ///< Binding point index
	SSBType ssbo_type = NONE_LIST;  ///< Buffer data type

public:

	/**
	 * @brief Default constructor (empty buffer).
	 */
	StorageBuffer() = default;

	/**
	 * @brief Constructs SSBO with type classification (binding point defaults to 0).
	 * @param type Data type classification
	 */
	StorageBuffer(SSBType type);

	/**
	 * @brief Constructs SSBO with type and binding point.
	 * @param type  Data type classification
	 * @param base  Binding point index
	 */
	StorageBuffer(SSBType type, GLuint base);

	/**
	 * @brief Destructor. Releases OpenGL SSBO (via GLBuffer).
	 */
	~StorageBuffer() = default;

	/**
	 * @brief Copy constructor. Deep-copies GPU buffer content and metadata.
	 * @param ssbo Source storage buffer.
	 */
	StorageBuffer(const StorageBuffer& ssbo);

	/**
	 * @brief Move constructor. Transfers ownership; @p ssbo is left empty.
	 * @param ssbo Source storage buffer (invalidated after move).
	 */
	StorageBuffer(StorageBuffer&& ssbo) noexcept;

	/**
	 * @brief Copy assignment. Deep-copies GPU buffer content and metadata.
	 * @param ssbo Source storage buffer.
	 * @return Reference to this.
	 */
	StorageBuffer& operator=(const StorageBuffer& ssbo);

	/**
	 * @brief Move assignment. Transfers ownership; @p ssbo is left empty.
	 * @param ssbo Source storage buffer (invalidated after move).
	 * @return Reference to this.
	 */
	StorageBuffer& operator=(StorageBuffer&& ssbo) noexcept;

public:

	/**
	 * @brief Binds this SSBO to GL_SHADER_STORAGE_BUFFER.
	 */
	void BindBuffer() const;

	/**
	 * @brief Binds SSBO to its binding point for shader access.
	 * @param _base Binding point override (-1 to use stored binding).
	 */
	void BindBufferBase(GLuint _base = static_cast<GLuint>(-1)) const;

	/**
	 * @brief Unbinds the current SSBO.
	 */
	void UnbindBuffer() const;

	/**
	 * @brief Sets the binding point index.
	 * @param base New binding point.
	 */
	void SetBufferBase(GLuint base);

public:

	/**
	 * @brief Returns the binding point index.
	 * @return Binding point.
	 */
	GLuint  GetBase() const { return ssbo_base; }

	/**
	 * @brief Returns the buffer data type classification.
	 * @return SSBType.
	 */
	SSBType GetType() const { return ssbo_type; }

public:

	/**
	 * @brief Uploads vector data to SSBO and records the buffer size.
	 * @tparam T Element type
	 * @param src Source vector (copied to GPU)
	 * @note Uses GL_STATIC_DRAW. For frequently updated data, consider dynamic approach.
	 */
	template <typename T>
	void GenStorageBuffer(const std::vector<T>& src);

	/**
	 * @brief Reads data from SSBO back to CPU.
	 * @tparam T Element type
	 * @param tar Target vector (resized to buffer size if empty)
	 * @param _offset Byte offset into buffer
	 * @note Performance: GPU readback is slow. Minimise usage.
	 */
	template <typename T>
	void ReadStorageBuffer(std::vector<T>& tar, GLuint _offset = 0);

	/**
	 * @brief Uploads structured data (header + array) to SSBO and records the buffer size.
	 * @tparam _Info Header structure type
	 * @tparam _Ele  Array element type
	 * @param _info Header data
	 * @param _data Array data
	 * @note Useful for arrays with metadata (e.g., count, bounds).
	 */
	template <typename _Info, typename _Ele> requires is_not_vector<_Info>::value
	void GenStorageBuffers(const _Info& _info, const std::vector<_Ele>& _data);

};

template <typename _Info, typename _Ele> requires is_not_vector<_Info>::value
void StorageBuffer::GenStorageBuffers(const _Info& _info, const std::vector<_Ele>& _data)
{
	buf_size = static_cast<GLuint>(sizeof(_Info) + _data.size() * sizeof(_Ele));
	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, buf_size, nullptr, GL_STATIC_DRAW);

	void* bufferData = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	std::memcpy(static_cast<char*>(bufferData), &_info, sizeof(_Info));
	std::memcpy(static_cast<char*>(bufferData) + sizeof(_Info), _data.data(), _data.size() * sizeof(_Ele));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	UnbindBuffer();
}

template <typename T>
void StorageBuffer::GenStorageBuffer(const std::vector<T>& src)
{
	if (src.size() == 0) return;

	buf_size = static_cast<GLuint>(src.size() * sizeof(T));
	BindBuffer();
	glBufferData(GL_SHADER_STORAGE_BUFFER, buf_size, src.data(), GL_STATIC_DRAW);
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


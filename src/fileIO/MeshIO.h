/**
 * @file MeshIO.h
 * @brief File I/O for Wavefront OBJ mesh files.
 *
 * MeshIO separates OBJ parsing from GPU buffer creation. It produces a
 * MeshData::ByteArray — the raw CPU-side interleaved vertex data — without
 * touching any OpenGL state, making it suitable for offline processing, unit
 * testing without a GL context, and future serialization to other formats.
 *
 * MeshData::LoadObj delegates to MeshIO::Read; callers may also call
 * MeshIO::Read directly to obtain raw vertex data for custom GPU uploads.
 *
 * Vertex layout (11 floats per vertex):
 *   [x, y, z,  u, v,  nx, ny, nz,  snx, sny, snz]
 *    ^position  ^UV   ^normal      ^smooth normal
 *
 * Usage:
 * @code
 * auto data = MeshIO::Read("res/obj/monkey.obj");
 * // data.data_array is ready for glBufferData
 * // data.center is the geometric center
 * @endcode
 */

#pragma once

#include "MeshData.h"

/**
 * @brief File I/O utilities for Wavefront OBJ meshes.
 *
 * All functions are stateless and operate purely on the CPU / file system.
 * No OpenGL calls are made.
 */
namespace MeshIO
{
	/**
	 * @brief Parses a Wavefront OBJ file into interleaved vertex data.
	 *
	 * Reads positions, UV coordinates, and per-face normals from the file.
	 * When @p smooth is true, per-vertex smooth normals are computed by
	 * averaging the face normals of all faces sharing that vertex position.
	 *
	 * @param path     Full file system path to the .obj file.
	 * @param smooth   If true (default), compute smooth normals; otherwise
	 *                 copy the face normal to the smooth-normal slot.
	 * @return         Populated MeshData::ByteArray. On failure (file not found),
	 *                 the byte array is empty and counts are zero.
	 */
	MeshData::ByteArray Read(const std::string& path, bool smooth = true);
}

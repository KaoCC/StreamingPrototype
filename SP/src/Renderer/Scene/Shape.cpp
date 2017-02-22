#include "Shape.hpp"

namespace SP {

	Shape::Shape() : materialPtr(nullptr) {
	}

	void Shape::setMaterial(Material const * matPtr) {
		materialPtr = matPtr;
	}

	Material const * Shape::getMaterial() const {
		return materialPtr;
	}

	void Mesh::setIndices(IndexType const * ind, std::size_t num_indices) {

		// Resize internal array and copy data
		verticeIndices.reset(new std::uint32_t[num_indices]);
		normalIndices.reset(new std::uint32_t[num_indices]);
		texcoordIndices.reset(new std::uint32_t[num_indices]);

		//std::copy(ind, ind + num_indices, indices.get());

		for (size_t i = 0; i < num_indices; ++i) {
			verticeIndices[i] = ind[num_indices].vertexIndex;
			normalIndices[i] = ind[num_indices].normalIndex;
			texcoordIndices[i] = ind[num_indices].texcoordIndex;
		}


		numIndices = num_indices;
		setDirty(true);
	}

	std::size_t Mesh::getNumIndices() const {
		return numIndices;
	}

	std::uint32_t const * Mesh::getVerticeIndices() const {
		return verticeIndices.get();
	}

	void Mesh::setVertices(float const * vert, std::size_t num_vertices) {

		// Resize internal array and copy data
		vertices.reset(new RadeonRays::float3[num_vertices]);

		for (std::size_t i = 0; i < num_vertices; ++i) {
			vertices[i].x = vert[3 * i];
			vertices[i].y = vert[3 * i + 1];
			vertices[i].z = vert[3 * i + 2];
			vertices[i].w = 1;
		}

		numVertices = num_vertices;

		setDirty(true);
	}

	std::size_t Mesh::getNumVertices() const {
		return numVertices;
	}

	void Mesh::setNormals(float const * norm, std::size_t num_normals) {

		// Resize internal array and copy data
		normals.reset(new RadeonRays::float3[num_normals]);

		for (std::size_t i = 0; i < num_normals; ++i) {
			normals[i].x = norm[3 * i];
			normals[i].y = norm[3 * i + 1];
			normals[i].z = norm[3 * i + 2];
			normals[i].w = 0;
		}

		numNormals = num_normals;

		setDirty(true);
	}

	std::size_t Mesh::getNumNormals() const {
		return numNormals;
	}

	void Mesh::setUVs(RadeonRays::float2 const * local_uvs, std::size_t num_uvs) {

		// Resize internal array and copy data
		uvs.reset(new RadeonRays::float2[num_uvs]);

		std::copy(local_uvs, local_uvs + num_uvs, uvs.get());
		numUVs = num_uvs;

		setDirty(true);
	}

	void Mesh::setUVs(float const * local_uvs, std::size_t num_uvs) {

		// Resize internal array and copy data
		uvs.reset(new RadeonRays::float2[num_uvs]);

		for (std::size_t i = 0; i < num_uvs; ++i) {
			uvs[i].x = local_uvs[2 * i];
			uvs[i].y = local_uvs[2 * i + 1];
		}

		numUVs = num_uvs;

		setDirty(true);
	}

	std::size_t Mesh::getNumUVs() const {
		return numUVs;
	}

}

#include "Shape.hpp"

#include <iostream>

namespace SP {

	Shape::Shape() : materialPtr(nullptr) {
	}

	void Shape::setMaterial(Material const * matPtr) {
		materialPtr = matPtr;
	}

	Material const * Shape::getMaterial() const {
		return materialPtr;
	}

	void Mesh::setIndices(std::uint32_t const * ind, std::size_t num_indices) {

		// Resize internal array and copy data
		indices.resize(num_indices);

		std::copy(ind, ind + num_indices, indices.data());

		setDirty(true);
	}

	std::size_t Mesh::getNumIndices() const {
		return indices.size();
	}

	std::uint32_t const * Mesh::getIndices() const {
		return indices.data();
	}

	void Mesh::setVertices(const std::vector<RadeonRays::float3>& inputVertices) {
		vertices = inputVertices;

		setDirty(true);
	}

	void Mesh::setVertices(const RadeonRays::float3 * local_vertices, std::size_t num_vertices) {

		vertices.resize(num_vertices);

		std::copy(local_vertices, local_vertices + num_vertices, vertices.data() );

		setDirty(true);
	}

	void Mesh::setVertices(float const * local_vertices, std::size_t num_vertices) {

		// Resize internal array and copy data
		vertices.resize(num_vertices);

		for (std::size_t i = 0; i < num_vertices; ++i) {
			vertices[i].x = local_vertices[3 * i];
			vertices[i].y = local_vertices[3 * i + 1];
			vertices[i].z = local_vertices[3 * i + 2];
			vertices[i].w = 1;
		}

		setDirty(true);
	}

	std::size_t Mesh::getNumVertices() const {
		return vertices.size();
	}

	RadeonRays::float3 const * Mesh::getVertices() const {
		return vertices.data();
	}

	void Mesh::setNormals(float const * norm, std::size_t num_normals) {

		// Resize internal array and copy data
		normals.resize(num_normals);

		for (std::size_t i = 0; i < num_normals; ++i) {
			normals[i].x = norm[3 * i];
			normals[i].y = norm[3 * i + 1];
			normals[i].z = norm[3 * i + 2];
			normals[i].w = 0;
		}


		setDirty(true);
	}

	std::size_t Mesh::getNumNormals() const {
		return normals.size();
	}

	RadeonRays::float3 const * Mesh::getNormals() const {
		return normals.data();
	}

	void Mesh::setUVs(RadeonRays::float2 const * local_uvs, std::size_t num_uvs) {

		// Resize internal array and copy data
		uvs.resize(num_uvs);

		std::copy(local_uvs, local_uvs + num_uvs, uvs.data());

		setDirty(true);
	}

	void Mesh::setUVs(float const * local_uvs, std::size_t num_uvs) {

		// Resize internal array and copy data
		uvs.resize(num_uvs);

		for (std::size_t i = 0; i < num_uvs; ++i) {
			uvs[i].x = local_uvs[2 * i];
			uvs[i].y = local_uvs[2 * i + 1];
		}


		setDirty(true);
	}

	std::size_t Mesh::getNumUVs() const {
		return uvs.size();
	}

	RadeonRays::float2 const * Mesh::getUVs() const {
		return uvs.data();
	}


	// need to check other data such as normals ... ?
	void Mesh::setTransform(const RadeonRays::matrix & transMat) {

		for (auto i = 0; i < getNumVertices(); ++i) {
			vertices[i] = transMat * vertices[i];
		}

	}

}

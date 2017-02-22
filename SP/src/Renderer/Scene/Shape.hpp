#ifndef _SP_SHAPE_HPP_
#define _SP_SHAPE_HPP_

#include "SceneObject.hpp"

#include "math/float3.h"
#include "math/float2.h"

#include <memory>

namespace SP {


	class Material;

	class Shape : public SceneObject  {
	public:

		// Constructor
		Shape();
		// Destructor
		virtual ~Shape() = 0;

		// Get and set material
		void setMaterial(Material const* matPtr);
		Material const* getMaterial() const;

		// disallow copying
		Shape(Shape const&) = delete;
		Shape& operator = (Shape const&) = delete;

	private:
		Material const* materialPtr;
	};

	inline Shape::~Shape() {
	}


	class Mesh : public Shape {
	public:

		struct IndexType {
			std::int32_t vertexIndex;
			std::int32_t normalIndex;
			std::int32_t texcoordIndex;
		};

		Mesh() = default;

		// Set and get index array
		void setIndices(IndexType const* indices, std::size_t num_indices);
		std::size_t getNumIndices() const;
		IndexType const* getIndices() const;

		// Set and get vertex array
		void setVertices(RadeonRays::float3 const* vertices, std::size_t num_vertices);
		void setVertices(float const* vertices, std::size_t num_vertices);
		std::size_t getNumVertices() const;
		RadeonRays::float3 const* getVertices() const;

		// Set and get normal array
		void setNormals(RadeonRays::float3 const* normals, std::size_t num_normals);
		void setNormals(float const* normals, std::size_t num_normals);
		std::size_t getNumNormals() const;
		RadeonRays::float3 const* getNormals() const;

		// Set and get UV array
		void setUVs(RadeonRays::float2 const* uvs, std::size_t num_uvs);
		void setUVs(float const* uvs, std::size_t num_uvs);
		std::size_t getNumUVs() const;
		RadeonRays::float2 const* getUVs() const;

		// disallow copying
		Mesh(Mesh const&) = delete;
		Mesh& operator = (Mesh const&) = delete;

	private:

		std::unique_ptr<RadeonRays::float3[]> vertices;
		std::unique_ptr<RadeonRays::float3[]> normals;
		std::unique_ptr<RadeonRays::float2[]> uvs;
		std::unique_ptr<IndexType> indices;

		std::size_t numVertices = 0;
		std::size_t numNormals = 0;
		std::size_t numUVs = 0;
		std::size_t numIndices = 0;
	};











}






#endif


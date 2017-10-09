#ifndef _SP_SHAPE_HPP_
#define _SP_SHAPE_HPP_

#include "SceneObject.hpp"

#include "math/float3.h"
#include "math/float2.h"

//#include <memory>

#include <vector>

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

		Mesh() = default;

		// Set and get index array
		void setIndices(const std::uint32_t* ind, std::size_t num_indices);
		std::size_t getNumIndices() const;
		std::uint32_t const* getIndices() const;

		// Set and get vertex array
		void setVertices(const std::vector<RadeonRays::float3>& inputVertices);
		void setVertices(const RadeonRays::float3* local_vertices, std::size_t num_vertices);
		void setVertices(float const* vertices, std::size_t num_vertices);
		std::size_t getNumVertices() const;
		RadeonRays::float3 const* getVertices() const;

		// Set and get normal array
		void setNormals(const RadeonRays::float3* local_normals, std::size_t num_normals);
		void setNormals(float const* local_normals, std::size_t num_normals);
		std::size_t getNumNormals() const;
		RadeonRays::float3 const* getNormals() const;

		// Set and get UV array
		void setUVs(RadeonRays::float2 const* local_uvs, std::size_t num_uvs);
		void setUVs(float const* uvs, std::size_t num_uvs);
		std::size_t getNumUVs() const;
		RadeonRays::float2 const* getUVs() const;

		// disallow copying
		Mesh(Mesh const&) = delete;
		Mesh& operator = (Mesh const&) = delete;

	private:

		std::vector<RadeonRays::float3> vertices;
		std::vector<RadeonRays::float3> normals;
		std::vector<RadeonRays::float2> uvs;
		std::vector<std::uint32_t> indices;

	};











}






#endif


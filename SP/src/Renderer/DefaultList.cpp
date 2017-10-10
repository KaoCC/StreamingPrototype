
#include "DefaultList.hpp"

#include "Scene/Material.hpp"


// test

#include <iostream>

SP::Triangle::Triangle() {

	RadeonRays::float4 vertices[] {
		{ 0.f, 0.f, 0.f, 1.0f },
		{ 0.f, 1.f, 0.f, 1.0f },
		{ 1.f, 0.f, 0.f, 1.0f }
	};

	const unsigned int unitLength = 1;

	size_t numOfVertices = 3;

	uint32_t indices[] { 0, 1, 2 };
	size_t numOfIndices = 3;

	float normals[] {
		1, 0, 0,
		1, 0, 0,
		1, 0, 0
	};
	size_t numOfNormals = 3;


	// set vertices
	setVertices(vertices, numOfVertices);

	// set indices
	setIndices(indices, numOfIndices);

	// set Normals
	setNormals(normals, numOfNormals);

	// Generate Zeros if we do not have UVs
	std::vector<RadeonRays::float2> zero(numOfVertices);
	std::fill(zero.begin(), zero.end(), RadeonRays::float2(0, 0));
	setUVs(&zero[0], numOfVertices);

	setName("DefaultTriangle");

	// memory leak !
	Material* diffuse = new SingleBxDF(SingleBxDF::BxDFType::kLambert);
	diffuse->setInputValue("albedo", RadeonRays::float3(10, 15, 10));

	setMaterial(diffuse);

}


SP::Square::Square() {

	// test
	std::cerr << "Square !!!! " << std::endl;

	RadeonRays::float4 vertices[] {
		{ 0.f, 0.f, 0.f, 1.0f },
		{ 0.f, 1.f, 0.f, 1.0f },
		{ 1.f, 0.f, 0.f, 1.0f },
		{ 1.f, 1.f, 0.f, 1.0f }
	};

	const unsigned int unitLength = 1;

	const size_t numOfVertices = 4;

	uint32_t indices[] { 0, 1, 2, 1, 2, 3 };
	const size_t numOfIndices = 6;

	float normals[] {
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0
	};
	const size_t numOfNormals = 4;


	// set vertices
	setVertices(vertices, numOfVertices);

	// set indices
	setIndices(indices, numOfIndices);

	// set Normals
	setNormals(normals, numOfNormals);

	// Generate Zeros if we do not have UVs
	std::vector<RadeonRays::float2> zero(numOfVertices);
	std::fill(zero.begin(), zero.end(), RadeonRays::float2(0, 0));
	setUVs(zero.data(), numOfVertices);

	setName("DefaultSquare");


	// memory leak !
	Material* diffuse = new SingleBxDF(SingleBxDF::BxDFType::kLambert);
	diffuse->setInputValue("albedo", RadeonRays::float3(15, 10, 10));

	setMaterial(diffuse);

}



SP::Mesh* SP::createDefaultShape(DefaultShapeType type) {

	switch (type) {

	case DefaultShapeType::kTriangle:
		return new Triangle();
		break;
	case DefaultShapeType::kSquare:
		return new Square();
		break;

	default:
		// Error ?
		return new Mesh();
		break;
	}
}



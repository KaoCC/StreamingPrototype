
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


	std::unique_ptr<Material> diffuse = std::make_unique<SingleBxDF>(SingleBxDF::BxDFType::kLambert);
	diffuse->setInputValue("albedo", RadeonRays::float3(10, 15, 10));

	setMaterial(std::move(diffuse));

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

	std::unique_ptr<Material> diffuse = std::make_unique<SingleBxDF>(SingleBxDF::BxDFType::kLambert);
	diffuse->setInputValue("albedo", RadeonRays::float3(15, 10, 10));

	setMaterial(std::move(diffuse));
}


#include "Renderer\Scene\IO\tiny_obj_loader.h"


// Warning: Ultra Ugly and Untested Code below ...
SP::Budda::Budda() {

	// KAOCC: Yet to be done ...

	// load the budda from the scene data ...
	// or hard code it


	// data buffers
	//tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> objShapes;
	std::vector<tinyobj::material_t> objMaterials;

	// load the file

	const std::string defaultPath = "Resources/Buddha";
	const std::string defaultModelName = "buddha.objm";

	std::string basepath = defaultPath;
	basepath += "/";
	std::string filename = basepath + defaultModelName;

	std::string errorString;
	bool status = tinyobj::LoadObj(objShapes, objMaterials, errorString, filename.c_str(), basepath.c_str());


	// tmp
	auto objShape = objShapes[0];			// <------------- CAREFUL

	// vertices
	size_t num_vertices = objShape.mesh.positions.size() / 3;
	setVertices(&objShape.mesh.positions[0], num_vertices);


	// normal
	size_t num_normals = objShape.mesh.normals.size() / 3;
	setNormals(&objShape.mesh.normals[0], num_normals);


	// UV
	size_t num_uvs = objShape.mesh.texcoords.size() / 2;

	if (num_uvs) {
		setUVs(&objShape.mesh.texcoords[0], num_uvs);
	} else {

		// Generate Zeros if we do not have UVs
		std::vector<RadeonRays::float2> zero(num_vertices);
		std::fill(zero.begin(), zero.end(), RadeonRays::float2(0, 0));
		setUVs(&zero[0], num_vertices);
	}

	// Set indices
	size_t num_indices = objShape.mesh.indices.size();
	setIndices(reinterpret_cast<const std::uint32_t*>(&objShape.mesh.indices[0]), num_indices);


	// Set material
	std::unique_ptr<Material> diffuse = std::make_unique<SingleBxDF>(SingleBxDF::BxDFType::kLambert);
	diffuse->setInputValue("albedo", RadeonRays::float3(10, 15, 10));

	setMaterial(std::move(diffuse));

}





std::unique_ptr<SP::Mesh> SP::createDefaultShape(DefaultShapeType type) {

	switch (type) {

	case DefaultShapeType::kTriangle:
		return std::make_unique<Triangle>();
		break;
	case DefaultShapeType::kSquare:
		return std::make_unique<Square>();
		break;

	case DefaultShapeType::kBudda:
		return std::make_unique<Budda>();
		break;

	default:
		// Error ?
		return std::make_unique<Mesh>();
		break;
	}
}






#include "SceneIO.hpp"


#include <vector>
#include <set>

// Note: define this in only *one* .cpp
//#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"


#include "../Shape.hpp"

#include <iostream>

namespace SP {




	class SceneIOImpl : public SceneIO {


	public:
		// Load scene from file
		Scene* loadScene(std::string const& filename, std::string const& basepath) const override;

	private:

	};


	Scene * SceneIOImpl::loadScene(std::string const & filename, std::string const & basepath) const {

		//using namespace tinyobj;

		// Yet To Be Done ! 

		// data buffers
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> objShapes;
		std::vector<tinyobj::material_t> objMaterials;

		// load the file

		std::string errorString;
		bool status = tinyobj::LoadObj(&attrib, &objShapes, &objMaterials, &errorString, filename.c_str(), basepath.c_str());
		if (!errorString.empty()) {
			// may contain warnings
			std::cerr << "Error String: " << errorString << std::endl;
		}

		if (!status) {
			throw std::runtime_error(errorString);
		}

		// Allocate Scene
		Scene* scene = new Scene();

		// Enumerate and translate materials
		// Keep track of emissive subset
		std::set<Material const*> emissives;
		std::vector<Material const*> materials(objMaterials.size());

		for (size_t i = 0; i < objMaterials.size(); ++i) {
			// more here ...
		}

		for (size_t s = 0; s < objShapes.size(); ++s) {

			// KAOCC: temp, we should store unique ptr
			Mesh* mesh = new Mesh();

			// Set vertex and index data

			// KAOCC: need to verify the following 

			size_t num_vertices = objShapes[s].mesh.indices.size() / 3;
			size_t verticesIndex = objShapes[s].mesh.indices[0].vertex_index;
			mesh->setVertices(&attrib.vertices[verticesIndex], num_vertices);

			size_t num_normals = objShapes[s].mesh.indices.size() / 3;
			size_t nornalIndex = objShapes[s].mesh.indices[0].normal_index;
			mesh->setNormals(&attrib.normals[nornalIndex], num_normals);


			size_t num_uvs = objShapes[s].mesh.indices.size() / 2;
			size_t texIndex = objShapes[s].mesh.indices[0].texcoord_index;

			if (num_uvs) {
				mesh->setUVs(&attrib.normals[texIndex], num_uvs);
			} else {
				std::vector<RadeonRays::float2> zero(num_uvs);
				std::fill(zero.begin(), zero.end(), RadeonRays::float2(0, 0));
				mesh->setUVs(&zero[0], num_uvs);
			}

			// KAOCC: need to change !!!!
			// Set indices
			size_t num_indices = objShapes[s].mesh.indices.size();
			mesh->setIndices(reinterpret_cast<Mesh::IndexType*>(&objShapes[s].mesh.indices[0]), num_indices);

			// Set material
			size_t idx = objShapes[s].mesh.material_ids[0];
			mesh->setMaterial(materials[idx]);


			// Attach to the scene
			scene->attachShape(mesh);

			// Keep track of the object
			scene->attachAutoreleaseObject(mesh);

			// yet to be done
		}


		// add Image Based Light ?
		// HDR ?

		return scene;
	}




	SceneIO * SceneIO::createSceneIO() {
		return new SceneIOImpl();
	}
}

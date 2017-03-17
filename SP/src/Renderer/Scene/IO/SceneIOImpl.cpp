

#include "SceneIO.hpp"
#include "ImageIO.hpp"

#include <vector>
#include <set>

// Note: define this in only *one* .cpp
//#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"


#include "../Shape.hpp"

#include "../Material.hpp"

#include <iostream>

namespace SP {




	class SceneIOImpl : public SceneIO {


	public:
		// Load scene from file
		Scene* loadScene(std::string const& filename, std::string const& basepath) const override;

	private:
		Material const* parseMaterial(const ImageIO& imageIO, tinyobj::material_t const& mat, std::string const& basepath, Scene& scene) const;
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
		Scene* scene{ new Scene() };

		// Enumerate and translate materials
		// Keep track of emissive subset
		std::set<Material const*> emissives;
		std::vector<Material const*> materials(objMaterials.size());

		auto imageIO{ ImageIO::createImageIO() };

		for (size_t i = 0; i < objMaterials.size(); ++i) {
			// parse the materials
			materials[i] = parseMaterial(*imageIO, objMaterials[i], basepath, *scene);

			// emission stuff !!!
			if (materials[i]->hasEmission()) {
				emissives.insert(materials[i]);
			}

		}

		for (size_t s = 0; s < objShapes.size(); ++s) {

			// KAOCC: design issue: unique_ptr or autorelease ?
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

			// If the mesh has emissive material, add AreaLight for it
			if (emissives.find(materials[idx]) != emissives.cend()) {

				// Add area light for each polygon (triangle-based) of emissive mesh
				for (int k = 0; k < mesh->getNumIndices() / 3; ++k) {

					// yet to be done

					//AreaLight* light = new AreaLight(mesh, l);

					//scene->attachLight(light);
					//scene->attachAutoreleaseObject(light);
				}

			}

		}


		// add Image Based Light ?
		// HDR ?

		return scene;
	}

	// Yet to be done
	Material const * SceneIOImpl::parseMaterial(const ImageIO & imageIO, tinyobj::material_t const & mat, std::string const & basepath, Scene & scene) const {

		RadeonRays::float3 emission(mat.emission[0], mat.emission[1], mat.emission[2]);

		Material* material = nullptr;

		// check for emissive
		if (emission.sqnorm() > 0) {

			material = new SingleBXDF(SingleBXDF::BXDFType::kEmissive);

			// albedo ?
			if (!mat.diffuse_texname.empty()) {
				Texture* texture{ imageIO.loadImage(basepath + "/" + mat.diffuse_texname) };
				material->setInputValue("albedo", texture);
				scene.attachAutoreleaseObject(texture);

			} else {

				material->setInputValue("albedo", emission);
			}

		} else {

			auto s = RadeonRays::float3(mat.specular[0], mat.specular[1], mat.specular[2]);

			if ((s.sqnorm() > 0 || !mat.specular_texname.empty())) {

				// Yet to be done !
				throw std::runtime_error("MultiBXDF: Yet to be done");

			} else {
				// Otherwise create lambert
				Material* diffuse = new SingleBXDF(SingleBXDF::BXDFType::kLambert);

				// Set albedo
				if (!mat.diffuse_texname.empty()) {
					Texture* texture{ imageIO.loadImage(basepath + "/" + mat.diffuse_texname) };
					diffuse->setInputValue("albedo", texture);
					scene.attachAutoreleaseObject(texture);
				} else {
					diffuse->setInputValue("albedo", RadeonRays::float3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]));
				}

				// Set normal
				if (!mat.normal_texname.empty()) {
					Texture* texture{ imageIO.loadImage(basepath + "/" + mat.normal_texname) };
					diffuse->setInputValue("normal", texture);
					scene.attachAutoreleaseObject(texture);
				}

				material = diffuse;
			}


		}


		// check if mat is not null ...
		// may cause error !

		// Set material name
		material->setName(mat.name);

		// Disable normal flip
		material->setTwoSided(true);

		// add to pool
		scene.attachAutoreleaseObject(material);

		return material;
	}




	SceneIO * SceneIO::createSceneIO() {
		return new SceneIOImpl();
	}
}

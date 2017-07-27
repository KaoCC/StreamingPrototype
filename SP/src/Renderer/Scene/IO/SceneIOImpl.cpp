

#include "SceneIO.hpp"
#include "ImageIO.hpp"

#include <vector>
#include <set>

// Note: define this in only *one* .cpp
//#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"


#include "../Shape.hpp"
#include "../Light.hpp"
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
		//tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> objShapes;
		std::vector<tinyobj::material_t> objMaterials;

		// load the file

		std::string errorString;
		bool status = tinyobj::LoadObj(objShapes, objMaterials, errorString, filename.c_str(), basepath.c_str());
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

		auto imageIO = ImageIO::createImageIO() ;

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
			size_t num_vertices = objShapes[s].mesh.positions.size() / 3;
			mesh->setVertices(&objShapes[s].mesh.positions[0], num_vertices);

			size_t num_normals = objShapes[s].mesh.normals.size() / 3;
			mesh->setNormals(&objShapes[s].mesh.normals[0], num_normals);


			size_t num_uvs = objShapes[s].mesh.texcoords.size() / 2;

			if (num_uvs) {
				mesh->setUVs(&objShapes[s].mesh.texcoords[0], num_uvs);
			} else {

				// Generate Zeros if we do not have UVs
				std::vector<RadeonRays::float2> zero(num_vertices);
				std::fill(zero.begin(), zero.end(), RadeonRays::float2(0, 0));
				mesh->setUVs(&zero[0], num_vertices);
			}

			// Set indices
			size_t num_indices = objShapes[s].mesh.indices.size();
			mesh->setIndices(reinterpret_cast<const std::uint32_t*>(&objShapes[s].mesh.indices[0]), num_indices);

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

					AreaLight* light = new AreaLight(mesh, k);
					scene->attachLight(light);
					scene->attachAutoreleaseObject(light);
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

			material = new SingleBxDF(SingleBxDF::BxDFType::kEmissive);

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
				Material* diffuse = new SingleBxDF(SingleBxDF::BxDFType::kLambert);

				// Set albedo
				if (!mat.diffuse_texname.empty()) {
					Texture* texture{ imageIO.loadImage(basepath + "/" + mat.diffuse_texname) };
					diffuse->setInputValue("albedo", texture);
					scene.attachAutoreleaseObject(texture);
				} else {
					diffuse->setInputValue("albedo", RadeonRays::float3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]));
				}

				// Set normal
				if (!mat.specular_highlight_texname.empty()) {
					Texture* texture{ imageIO.loadImage(basepath + "/" + mat.specular_highlight_texname) };
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

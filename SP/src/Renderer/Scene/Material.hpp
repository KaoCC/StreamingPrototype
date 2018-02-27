#ifndef _SP_MATERIAL_HPP_
#define _SP_MATERIAL_HPP_

#include "SceneObject.hpp"

//#include "Texture.hpp"
#include "math/float3.h"

#include <set>
#include <map>

//#include "../DifferentialGeometry.hpp"

namespace SP {


	class Texture;

	class Material : public SceneObject {


	public:

		enum class InputType {

			kFloat4,
			kTexture,
			kMaterial
		};

		struct InputInfo {

			std::string name;
			std::string description;
			std::set<InputType> supportedTypes;
		};


		struct InputValue {
			InputType type;

			// Based on RR

			// Possible values (use based on type)
			union {
				RadeonRays::float4 floatValue;
				Texture const* texValue;
				Material const* matValue;
			};

		};


		// Input data representation
		struct Input {
			InputInfo information;
			InputValue value;
		};



		// Ctor
		Material();

		// force abstract
		virtual ~Material() = 0;




		// ...

		// check if the material has emissive parts
		virtual bool hasEmission() const;







		// set input value 
		// If specific data type is not supported throws std::runtime_error
		template <typename T>
		void setInputValue(const std::string& name, const T& param) {

			auto input_iter = inputTable.find(name);

			if (input_iter != inputTable.cend()) {
				auto& input = input_iter->second;

				if (!setValueParameter(input, param)) {
					throw std::runtime_error("Input type not supported");
				}

			} else {
				throw std::runtime_error("No such input");
			}
		}


		//void setInputValue(const std::string& name, const RadeonRays::float4& value);
		//void setInputValue(const std::string& name, Texture const* texture);
		//void setInputValue(const std::string& name, Material const* material);

		InputValue getInputValue(const std::string& name) const;


		// two sided ?
		bool isTwoSided() const;
		void setTwoSided(bool twosided);


	protected:
		void registerInput(const std::string& nm, const std::string& desc, std::set<InputType>&& suppTypes);
		void clearInputTable();


	private:

		bool setValueParameter(Input& input, const RadeonRays::float4& value);
		bool setValueParameter(Input& input, Texture const* tex);
		bool setValueParameter(Input& input, Material const* mat);

		using InputMap = std::map<std::string, Input>;

		InputMap inputTable;

		bool twoSidedFlag;
	};

	inline Material::~Material() {
	}



	class SingleBxDF : public Material {

	public:

		// Note: from RR
		enum class BxDFType {
			kZero,
			kLambert,
			kIdealReflect,
			kIdealRefract,
			kMicrofacetBeckmann,
			kMicrofacetGGX,
			kEmissive,
			kPassthrough,
			kTranslucent,
			kMicrofacetRefractionGGX,
			kMicrofacetRefractionBeckmann
		};


		SingleBxDF(BxDFType tp);


		BxDFType getBxDFType() const;
		void setBxDFType(BxDFType type);

		bool hasEmission() const override;

	private:
		BxDFType type;

	};

	class MultiBxDF : public Material {

	public:

		// NOTE: from RR
		enum class MultiType {
			kLayered,
			kFresnelBlend,
			kMix
		};

		MultiBxDF(MultiType t);

		MultiType getType() const;
		void setType(MultiType t);

		bool hasEmission() const override;

	private:
		MultiType type;

	};

}








#endif


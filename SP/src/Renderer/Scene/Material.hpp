#ifndef _SP_MATERIAL_HPP_
#define _SP_MATERIAL_HPP_

#include "SceneObject.hpp"

#include "Texture.hpp"
#include "math/float3.h"

#include <set>
#include <map>

namespace SP {


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

		virtual ~Material() = default;



		// ...

		// check if the material has emmisive parts
		virtual bool hasEmission() const;

		// set input type ?
		// Yet to be done


		// two sided ?
		bool isTwoSided() const;
		void setTwoSided(bool twosided);


	protected:
		void registerInput(const std::string& name, const std::string& desc, std::set<InputType>&& suppTypes);
		void clearInputTable();


	private:

		using InputMap = std::map<std::string, Input>;

		InputMap inputTable;

		bool twoSidedFlag;
	};



	class SingleBXDF : public Material {

	public:

		// Note: from RR
		enum class BXDFType {
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


		SingleBXDF(BXDFType tp);

		BXDFType getBXDFType() const;
		void setBXDFType(BXDFType type);

		bool hasEmission() const override;

	private:
		BXDFType type;

	};

	class MultiBXDF : public Material {

	public:

		// NOTE: from RR
		enum class Type {
			kLayered,
			kFresnelBlend,
			kMix
		};

		MultiBXDF(Type t);

		Type getType() const;
		void setType(Type t);

		bool hasEmission() const override;

	private:
		Type type;

	};

}








#endif


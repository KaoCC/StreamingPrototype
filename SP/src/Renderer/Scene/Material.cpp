
#include "Material.hpp"


namespace SP {

	Material::Material() : twoSidedFlag (false){
	}

	bool Material::hasEmission() const {
		return false;
	}

	bool Material::isTwoSided() const {
		return twoSidedFlag;
	}

	void Material::setTwoSided(bool twosided) {
		twoSidedFlag = twosided;
		setDirty(true);
	}

	void Material::registerInput(const std::string & nm, const std::string & desc, std::set<InputType>&& suppTypes) {

		Input input = { 
			{
				nm, desc, std::move(suppTypes)
			},

			{	// Note: tmp test for init
				InputType::kFloat4, RadeonRays::float4()
			}

		};

		input.value.type = *input.information.supportedTypes.begin();

		// test
		switch (input.value.type) {
		case InputType::kFloat4:
			input.value.floatValue = RadeonRays::float4();
			break;

		// Should check the other two:
		case InputType::kTexture:
			input.value.texValue = nullptr;
			break;
		case InputType::kMaterial:
			input.value.matValue = nullptr;
			break;
		default:
			// Error ?
			break;

		}

		inputTable.emplace(std::make_pair(nm, input));
	}

	void Material::clearInputTable() {
		inputTable.clear();
	}



	SingleBXDF::SingleBXDF(BXDFType tp) : type (tp) {

		registerInput("albedo", "Diffuse color", { InputType::kFloat4, InputType::kTexture });
		registerInput("normal", "Normal map", { InputType::kTexture });
		registerInput("ior", "Index of refraction", { InputType::kFloat4 });
		registerInput("fresnel", "Fresnel flag", { InputType::kFloat4 });
		registerInput("roughness", "Roughness", { InputType::kFloat4, InputType::kTexture });


		// set value ?
		// yet to be done
	}

	SingleBXDF::BXDFType SingleBXDF::getBXDFType() const {
		return type;
	}

	void SingleBXDF::setBXDFType(BXDFType t) {
		type = t;
		setDirty(true);
	}

	bool SingleBXDF::hasEmission() const {
		return type == BXDFType::kEmissive;
	}

}







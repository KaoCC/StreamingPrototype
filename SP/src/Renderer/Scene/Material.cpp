
#include "Material.hpp"


namespace SP {

	Material::Material() : twoSidedFlag (false){
	}


	bool Material::isEmissive(const Material * matPtr) {
		return matPtr->hasEmission();
	}

	bool Material::isSingular(const Material * matPtr) {

		auto sinMatPtr{ dynamic_cast<const SingleBxDF*>(matPtr) };

		if (sinMatPtr) {

			auto type{ sinMatPtr->getBxDFType() };
			return type == SingleBxDF::BxDFType::kIdealReflect || type == SingleBxDF::BxDFType::kIdealRefract || type == SingleBxDF::BxDFType::kPassthrough;

		} else {		// mixed
			return false;
		}


	}

	bool Material::isBTDF(const Material * matPtr) {
		auto sinMatPtr{ dynamic_cast<const SingleBxDF*>(matPtr) };

		if (sinMatPtr) {

			auto type{ sinMatPtr->getBxDFType() };
			return type == SingleBxDF::BxDFType::kIdealRefract || type == SingleBxDF::BxDFType::kPassthrough || type == SingleBxDF::BxDFType::kTranslucent
				|| type == SingleBxDF::BxDFType::kMicrofacetRefractionGGX || type == SingleBxDF::BxDFType::kMicrofacetRefractionBeckmann;

		} else {	// mixed
			return false;
		}
	}


	bool Material::hasEmission() const {
		return false;
	}



	bool Material::setValueParameter(Input & input, const RadeonRays::float4 & value) {
		bool ret = false;

		if (input.information.supportedTypes.find(InputType::kFloat4) != input.information.supportedTypes.cend()) {
			ret = true;
			input.value.type = InputType::kFloat4;
			input.value.floatValue = value;
			setDirty(true);
		}

		return ret;
	}

	bool Material::setValueParameter(Input & input, Texture const * tex) {
		bool ret = false;

		if (input.information.supportedTypes.find(InputType::kTexture) != input.information.supportedTypes.cend()) {
			ret = true;
			input.value.type = InputType::kTexture;
			input.value.texValue = tex;
			setDirty(true);
		}

		return ret;
	}

	bool Material::setValueParameter(Input & input, Material const * mat) {
		bool ret = false;

		if (input.information.supportedTypes.find(InputType::kMaterial) != input.information.supportedTypes.cend()) {
			ret = true;
			input.value.type = InputType::kMaterial;
			input.value.matValue = mat;
			setDirty(true);
		}

		return ret;
	}

	Material::InputValue Material::getInputValue(const std::string & name) const {

		auto inputIter = inputTable.find(name);

		if (inputIter != inputTable.cend()) {

			return inputIter->second.value;
		} else {
			throw std::runtime_error("Mat: input not found");
		}

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




	//--------------


	SingleBxDF::SingleBxDF(BxDFType tp) : type (tp) {

		registerInput("albedo", "Diffuse color", { InputType::kFloat4, InputType::kTexture });
		registerInput("normal", "Normal map", { InputType::kTexture });
		registerInput("ior", "Index of refraction", { InputType::kFloat4 });
		registerInput("fresnel", "Fresnel flag", { InputType::kFloat4 });
		registerInput("roughness", "Roughness", { InputType::kFloat4, InputType::kTexture });


		// set value
		// KAOCC: check the template path
		setInputValue("albedo", RadeonRays::float4(0.7f, 0.7f, 0.7f, 1.f));
		setInputValue("normal", static_cast<Texture const*>(nullptr));

	}

	SingleBxDF::BxDFType SingleBxDF::getBxDFType() const {
		return type;
	}

	void SingleBxDF::setBxDFType(BxDFType t) {
		type = t;
		setDirty(true);
	}

	bool SingleBxDF::hasEmission() const {
		return type == BxDFType::kEmissive;
	}

}







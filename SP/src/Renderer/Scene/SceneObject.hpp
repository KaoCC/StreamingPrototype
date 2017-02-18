#ifndef _SP_SCENEOBJECT_HPP_
#define _SP_SCENEOBJECT_HPP_

#include <string>

namespace SP {



	class SceneObject {
	public:
		// Constructor
		SceneObject();
		// Destructor
		virtual ~SceneObject() = 0;

		// Check if the object has been changed since last reset
		bool isDirty() const;
		// Set dirty flag
		void setDirty(bool dirty) const;

		// Set & get name
		void setName(std::string const& name);
		std::string getName() const;

		SceneObject(SceneObject const&) = delete;
		SceneObject& operator = (SceneObject const&) = delete;

	private:
		mutable bool dirtyFlag;

		std::string name;
	};


}



#endif



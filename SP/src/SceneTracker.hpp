#ifndef _SP_SECENETRACKER_HPP_
#define _SP_SECENETRACKER_HPP_

#include "radeon_rays.h"


namespace SP {

	class SceneTracker {


		SceneTracker(int devidx);

		virtual ~SceneTracker();



	private:


		RadeonRays::IntersectionApi* api;


	};







}















#endif


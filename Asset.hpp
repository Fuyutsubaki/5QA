#pragma once
#include <Siv3D.hpp>

namespace asset{
	struct Font{
		static s3d::Font me15() {
			static s3d::Font font{ 15 };
			return font;
		}
	};

}

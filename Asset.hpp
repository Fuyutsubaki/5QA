#pragma once
#include <Siv3D.hpp>

namespace asset{
	struct Font{
		static s3d::Font me15() {
			static s3d::Font font{ 15 };
			return font;
		}
		static s3d::Font me10() {
			static s3d::Font font{ 10 };
			return font;
		}
		static s3d::Font me12() {
			static s3d::Font font{ 12 };
			return font;
		}
	};
	namespace Palette{
		constexpr s3d::Color  red{ 255,127,127 };
		constexpr s3d::Color  blue{ 127,127,255 };
		constexpr s3d::Color  green{ 127,255,131 };
		constexpr s3d::Color  orange{ 255,195,127 };
		constexpr s3d::Color  base_blue{ 0,174,214 };
	};
}

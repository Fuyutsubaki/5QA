#pragma once
#include<HamFramework.hpp>

namespace scene {
	struct shared_data{};
	using RootScene = s3d::SceneManager<s3d::String, shared_data>;
}

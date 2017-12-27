#pragma once

#include<Siv3D.hpp>
#include"util.hpp"

namespace {

	struct InputInfo {

	};

	class Object {
	public:
		virtual ~Object(){}
		void draw(){}
		s3d::Array<InputInfo> check_input()const { return {};  }
		std::string const& id()const { return id_; }

		int zindex;
	private:
		std::string id_;
		s3d::Rect pos;
	};

	class Viewer {
	public:
		void draw() {
			obj_list.sort_by([](auto const&lhs, auto const&rhs) {return lhs->zindex < rhs->zindex; });
			for (auto&p : obj_list) {
				p->draw();
			}
		}

		void update_inputs() {
			input_list = util::flatmap(obj_list, [](auto &obj) {return obj->check_input(); });	
		}

	private:
		s3d::Array<std::shared_ptr<Object>> obj_list;
		s3d::Array<InputInfo> input_list;
	};


}


// todo 
/*
input
draw
イベント発行
オブジェクト操作
*/
#pragma once
#include<Siv3D.hpp>
#include<optional>
#include"GameSceneMsg.hpp"

class GameCPUPlayer {
	struct Event {
		std::uint64_t firetime_msec;
	};
	std::optional<Event> event_;
	s3d::String player_id;
	
	s3d::Array<gamescene::PlayerActionlMsg> msg_list;
	void apply_msg_impl(gamescene::UpdateTurnPlayer const&);
	template<class T>
	void apply_msg_impl(T const&){}
public:
	GameCPUPlayer(s3d::String const&player_id) 
		:player_id(player_id) 
	{	}
	void init_frame() { msg_list = {}; }
	void apply_msg(s3d::Array<gamescene::GameModelMsg>const&list) {
		for (auto &t:list) {
			std::visit([&](auto const&msg) {apply_msg_impl(msg); }, t);
		}
	}

	s3d::Array<gamescene::PlayerActionlMsg> const& get_msg_list()const {
		return msg_list;
	}
	void update();
};

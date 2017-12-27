#include "GameScene.hpp"

void scene::SceneModel::update() {
	auto player_action_msg_list = util::flatmap(cpu_player_list, [](auto player) {return player->get_msg_list(); });
	for (auto cpu_player : cpu_player_list) {
		cpu_player->init_frame();
	}

	model.apply_msg(player_action_msg_list);
	for (auto cpu_player : cpu_player_list) {
		cpu_player->init_frame();
	}

	for (auto cpu_player : cpu_player_list) {
		cpu_player->apply_msg(model.get_msg_list());
		cpu_player->update();
	}
	if (s3d::KeyR.up()) {
		flag_gameover = true;
	}
}

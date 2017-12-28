#include "GameModel.hpp"

#include "GameModelCPU.hpp"

void GameModel::player_turn_state(std::shared_ptr<PlayerModel>& from, std::shared_ptr<PlayerModel>& to, s3d::String const & card_id)
{
	if (state_gameend) {
		return;
	}

	if (from == turn_player || to != turn_player || played_player_list.include(from)) { // 違法な命令
		//LOG_INFO(L"違法な状態です");
		return;
	}

	move_card(*from, *to, card_id);
	
	checkend();
	if (state_gameend) {
		return;
	}

	{
		played_player_list.push_back(from);
		if (played_player_list.size() == player_list.size()) {
			played_player_list = {};
		}
		
		msg_list.push_back(gamescene::UpdateDrawnPlayerList::ctor(played_player_list.map([](auto const&p) {return p->id; })));
	}

	{
		turn_player = from;
		//msgがcpuのアルゴリズムよりな気はしてる
		auto drawable_card = util::flatmap(
			player_list.filter([&](auto const&p) {return (p != turn_player && !played_player_list.include(p)); }),
			[](auto const&p) {return p->hands; }
		);
		msg_list.push_back(gamescene::UpdateTurnPlayer::ctor(turn_player->id, turn_player->hands, drawable_card));
	}
}

void GameModel::checkend() {
	//誰かの手札が0になった
	auto handless = [&] ()->std::optional<s3d::String>{
		auto p = util::find_if(player_list, [](auto const&p) {return p->hands.size() == 0; });
		if (p)
			return (*p)->id;
		else
			return std::nullopt;
	}();
	// 同じ色の数字カードが出きった
	auto discard_color = [&]()->std::optional<s3d::Array<gamedat::Card::Color>> {
		auto numlist = dat->get_card_list().filter([](gamedat::Card const&c) {
			auto p = std::get_if<gamedat::Card::Number>(&c.type);
			return !!p;
		});
		auto allcard_ = util::group_by(numlist, [](gamedat::Card const&c) {return c.color; });

		auto discard_num_list = discard_list.filter([](gamedat::Card const&c) {
			auto p = std::get_if<gamedat::Card::Number>(&c.type);
			return !!p;
		});
		auto discard_ = util::group_by(discard_num_list, [](gamedat::Card const&c) {return c.color; });
		auto list = util::make_vector(discard_).filter([&](auto const&pair) {return pair.second.size() == allcard_[pair.first].size(); }).map([](auto const&pair) {return pair.first; });
		if (list.size() >= discardable_color_num())
			return list;
		else
			return std::nullopt;
	}();

	// alphaカードがそろった
	auto complate_alpha_card = [&]()->std::optional<s3d::String> {
		auto p = util::find_if(player_list, [&](auto const&p) {
			return dat->get_alphaword_list().all([&](auto const&alpha) {
				return p->hands.include_if([&](gamedat::Card const&c) {return c.type == decltype(c.type)(alpha); });

			});
		});
		if (p) {
			return (*p)->id;
		}
		else {
			return std::nullopt;
		}

	}();


	if (handless || discard_color || complate_alpha_card) {
		for (auto p :player_list) {
			p->point_list.push_back(count_point(p->hands));
		}

		auto kimarite = [&]()-> gamescene::EndGame::Kimarite {
			if (complate_alpha_card) {
				return gamescene::EndGame::Basy{ *complate_alpha_card };
			}
			else if (discard_color) {
				return gamescene::EndGame::AllDiscard{ *discard_color };
			}
			else if (handless) {
				return gamescene::EndGame::Handless{ *handless };
			}
		}();

		if (complate_alpha_card) {
			basy_count += 1;
		}
		auto player = player_list.map([](auto p) {return gamescene::EndGame::Player{ p->id, p->point_list }; });
		bool gameover = player_list.any([](auto const&p) {return p->point_list.sum() >= 500; }) || basy_count >= 3;
		msg_list.push_back(gamescene::EndGame::ctor(player, kimarite, gameover));
		state_gameend = true;
		if (gameover) {
			state_gameover = true;
		}
	}
}

void GameModel::deal_hands(){
	discard_list = {};
	msg_list.push_back(gamescene::StartGame::ctor(discardable_color_num()));

	auto list = dat->get_card_list();
	list.shuffle();
	int idx = 0;
	for (auto hand_card : list.in_groups(player_list.size())) {
		player_list[idx]->hands = hand_card;
		msg_list.append(
			hand_card.map([&](auto card) ->gamescene::GameModelMsg {return gamescene::AddHand::ctor(player_list[idx]->id, card.id); })
		);
		idx += 1;
	}
	for (auto &p: player_list) {
		same_discard(*p);
	}

	turn_player = util::at_model(player_list, L"player");
	checkend();
}

 int count_point(s3d::Array<gamedat::Card> const & list) {
	 return list.map([](auto const&c) {
		 auto p = std::get_if<gamedat::Card::Number>(&c.type);
		 return p ? p->num : 0;
	 }).sum();
 }

 void  GameModel::start_nextgame() {
	 if (state_gameover) {
		 return;
	 }
	 state_gameend = false;
	 played_player_list = {};

	 // 再配布
	 deal_hands();
 }
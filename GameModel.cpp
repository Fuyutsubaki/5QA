#include "GameModel.hpp"

#include "GameModelCPU.hpp"

void GameModel::player_turn_state(std::shared_ptr<PlayerModel>& from, std::shared_ptr<PlayerModel>& to, s3d::String const & card_id)
{
	if (state_gameend) {
		return;
	}

	if (from == turn_player || to != turn_player || played_player_list.include(from)) { // 違法な命令
		LOG_INFO(L"違法な状態です");
		return;
	}

	move_card(*from, *to, card_id);
	
	checkend();
	if (state_gameend) {
		return;
	}

	{
		played_player_list.push_back(from);
		s3d::Print(from->id);
		if (played_player_list.size() == player_list.size()) {
			played_player_list = {};
			s3d::ClearPrint();
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
	auto handless = [&] {return player_list.any([](auto const&p) {return p->hands.size() == 0; }); };
	// 同じ色の数字カードが出きった
	auto discard_all_one_color = [&] {
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
		return util::make_vector(discard_).any([&](auto const&pair) {return pair.second.size() == allcard_[pair.first].size(); });
	};

	// alphaカードがそろった
	auto complate_alpha_card = [&] {
		return player_list.any([&](auto const&p) {
			return dat->get_alphaword_list().all([&](auto const&alpha) {
				return p->hands.include_if([&](gamedat::Card const&c) {return c.type == decltype(c.type)(alpha); });

			});
		}); };


	if (handless() || discard_all_one_color() || complate_alpha_card()) {
		for (auto p :player_list) {
			p->point_list.push_back(count_point(p->hands));
		}

		// gameover-check
		//TODO
		auto a = handless();
		auto b = discard_all_one_color();
		auto c = complate_alpha_card();
		if (player_list.any([](auto const&p) {return p->point_list.sum() >= 500; })) {
			msg_list.push_back(gamescene::EndGame::ctor(
				player_list.map([](auto p) {return gamescene::EndGame::Player{ p->id, p->point_list }; }),
				gamescene::EndGame::AllDiscard{ gamedat::Card::Color::red },
				true
			));
			state_gameover = true;
			state_gameend = true;
		}
		else {
			msg_list.push_back(gamescene::EndGame::ctor(
				player_list.map([](auto p) {return gamescene::EndGame::Player{ p->id, p->point_list }; }),
				gamescene::EndGame::AllDiscard{ gamedat::Card::Color::red },
				false
			));
			state_gameend = true;
		}
	}
}

void GameModel::deal_hands(s3d::Array<gamedat::Card>const &deck, s3d::Array<gamedat::Card>const &discard_init){
	 discard_list = discard_init;
	 msg_list.append(
		 discard_list.map([&](auto card) ->gamescene::GameModelMsg {return gamescene::AddDiscard::ctor(card.id); })
	 );


	auto list = deck;
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


	 // handsからそろっているBASYを廃棄
	 s3d::Array<gamedat::Card> to_discard;
	 for (auto p : player_list) {
		 bool has_all = dat->get_alphaword_list().all([&](auto const&a) {return p->hands.include_if([&](auto const&c) {return c.type == decltype(c.type)(a); }); });
		 if (has_all) {

			 // todo 抽象化
			 for (auto a : dat->get_alphaword_list()) {
				 for (int i = 0; i < p->hands.size(); ++i) {
					 if (p->hands[i].type == decltype(p->hands[i].type)(a)) {
						 to_discard.push_back(p->hands[i]);
						 p->hands.remove_at(i);
						 break;
					 }
				 }
			 }
		 }
	 }
	 // discardの数字カードと手札を回収
	 s3d::Array<gamedat::Card> newdeck;
	 newdeck.append(util::flatmap(player_list, [](auto p) {return p->hands; }));
	 newdeck.append(discard_list.filter([](auto const&c) {return !!std::get_if<gamedat::Card::Number>(&c.type); }));
	 discard_list.remove_if([](auto const&c) {return std::get_if<gamedat::Card::Number>(&c.type); });
	 discard_list.append(to_discard);
	 for (auto p : player_list) {
		 p->hands = {};
	 }

	 // 再配布
	 deal_hands(newdeck, discard_list);
 }
#pragma once
#include<vector>
#include<variant>
#include <memory>
#include<Siv3D.hpp>
#include"util.hpp"
#include "GameSceneMsg.hpp"
#include "Gamedata.hpp"

struct PlayerModel {
	s3d::String id;
	s3d::Array<gamedat::Card> hands;
	s3d::Array<int> point_list;
};

template<class V, class F>
std::pair<V, V> partion(V const&src, F pred) {
	V true_list;
	V false_list;
	std::partition_copy(src.begin(), src.end(),
		std::back_inserter(true_list),
		std::back_inserter(false_list),
		pred);
	return std::make_pair(std::move(true_list), std::move(false_list));
}

template<class Eq>
std::pair<s3d::Array<gamedat::Card>, s3d::Array<gamedat::Card>> partion_duple(s3d::Array<gamedat::Card> const&src, Eq eq) {
	auto list = src.map([](auto const&e) {return std::make_pair(false, e); });

	for (auto it1 = list.begin(); it1 != list.end(); ++it1) {
		for (auto it2 = std::next(it1); it2 != list.end(); ++it2) {
			if (!it1->first && !it2->first &&
				eq(it1->second, it2->second))
			{
				it1->first = true;
				it2->first = true;
				break;
			}
		}
	}

	s3d::Array<gamedat::Card> removed;
	s3d::Array<gamedat::Card> left;
	for (auto &e : list) {
		if (e.first) {
			removed.push_back(e.second);
		}
	}
	for (auto &e : list) {
		if (!e.first) {
			left.push_back(e.second);
		}
	}
	return std::make_pair(std::move(removed), std::move(left));

}

int count_point(s3d::Array<gamedat::Card> const&list);


class GameModel {
	void player_turn_state(std::shared_ptr<PlayerModel>&from, std::shared_ptr<PlayerModel>&to, s3d::String const&card_id);

	void move_card(PlayerModel&from, PlayerModel&to, s3d::String const&card_id) {
		auto[removed, left] = partion(from.hands, [&](auto const&c) {return c.id == card_id; });

		from.hands = std::move(left);
		to.hands.append(removed);
		msg_list.append(removed.map([&](auto const&c) ->gamescene::GameModelMsg {return gamescene::RemoveHand::ctor(from.id, c.id); }));
		msg_list.append(removed.map([&](auto const&c) ->gamescene::GameModelMsg {return gamescene::AddHand::ctor(to.id, c.id); }));
		same_discard(to);
	}

	void same_discard(PlayerModel&player) {
		auto[removed, left] = partion_duple(player.hands, [](auto const&a, auto const&b) {return is_discardable_pair(a, b); });

		msg_list.append(removed.map([&](auto const&c) ->gamescene::GameModelMsg {return gamescene::RemoveHand::ctor(player.id, c.id); }));
		msg_list.append(removed.map([&](auto const&c) ->gamescene::GameModelMsg {return gamescene::AddDiscard::ctor(c.id); }));
		player.hands = std::move(left);
		discard_list.append(removed);
	}

	void checkend();

	s3d::Array<std::shared_ptr<PlayerModel>> player_list;
	std::shared_ptr<PlayerModel> turn_player;
	s3d::Array<std::shared_ptr<PlayerModel>> played_player_list;
	s3d::Array<gamedat::Card> discard_list;
	bool state_gameend = false;
	bool state_gameover = false;
	s3d::Array<gamescene::GameModelMsg> msg_list;

	std::shared_ptr<gamedat::GameData> dat;
	

public:
	s3d::Array<gamescene::GameModelMsg>const& get_msg_list() const {
		return msg_list;
	}

	void setup_update() {
		msg_list = {};
	}

	void apply_msg(s3d::Array<gamescene::CtrlMsg> const& in_msg_list) {
		for (auto&msg : in_msg_list) {
			std::visit([&](auto const&t) {apply_msg_impl(t); }, msg);
		}
	}
	void apply_msg(s3d::Array<gamescene::PlayerActionlMsg> const& in_msg_list) {
		for (auto&msg : in_msg_list) {
			std::visit([&](auto const&t) {apply_msg_impl(t); }, msg);
		}
	}

	void deal_hands(s3d::Array<gamedat::Card>const &deck, s3d::Array<gamedat::Card>const &discard_init);
	void start_nextgame();
	bool gameover()const {
		return state_gameover;
	}


	GameModel(std::shared_ptr<gamedat::GameData> const&dat) 
		:dat(dat)
	{
		player_list = {
			std::make_shared<PlayerModel>(PlayerModel{ L"player",{} }),
			std::make_shared<PlayerModel>(PlayerModel{ L"1",{} }),
			std::make_shared<PlayerModel>(PlayerModel{ L"2",{} }),
			std::make_shared<PlayerModel>(PlayerModel{ L"3",{} }),
		};

		deal_hands(dat->get_card_list(), {});
	}
private:
	void apply_msg_impl(gamescene::ClickObject const&msg) {
		if (msg.info.size() == 3 && msg.info[0] == L"hands") {
			auto player = util::at_model(player_list, msg.info[1]);
			player_turn_state(
				util::at_model(player_list, msg.info[1]),
				util::at_model(player_list, L"player"),
				msg.info[2]
			);
		}
	}

	void apply_msg_impl(gamescene::DrawCard const&msg) {

		auto from_player = util::find_if(player_list, [&](auto const&p) {return p->hands.include_if([&](auto const&c) {return c.id == msg.card_id; }); });
		player_turn_state(
			*from_player,
			util::at_model(player_list, msg.player_id),
			msg.card_id
		);
	}

};

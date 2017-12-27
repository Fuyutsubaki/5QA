#include "GameModelCPU.hpp"
#include"Gamedata.hpp"

#include<HamFramework\Rx.hpp>

void GameCPUPlayer::apply_msg_impl(gamescene::UpdateTurnPlayer const &msg)
{
	if (msg.player_id != player_id) {
		return;
	}
	s3d::Array<gamedat::Card> drawable_card_list = msg.drawable_card_list;
	s3d::Array<gamedat::Card> hand_list = msg.hand_list;
	if (!drawable_card_list || !hand_list) {
		return;
	}

	s3d::HashMap<int, int> m;
	// カードを引いたとき、 手札のカードの中で最も消化できる点数の期待値が高いものを引く
	// 色を決める
	auto groupd_drawable = util::group_by(drawable_card_list, [](auto const&c) {return c.color; });
	auto vec = util::make_vector(groupd_drawable);
	auto rez = vec.map([&](auto const&pair) {
		auto[color, cards] = pair;
		double sum_point =
			cards.filter(
				[&](gamedat::Card const&card) {
			return hand_list.include_if([&](gamedat::Card const&hand) {
				return gamedat::is_discardable_pair(hand, card);
			});
		}).map([](auto const&c) {
			return util::visit(c.type)(
				[](gamedat::Card::Number const&num) {return num.num; },
				[](gamedat::Card::Alpha const&) {return 0; }
			);
		}).sum();
		return std::make_pair(color, sum_point / cards.size());
	});


	auto max_value = util::max_by(rez, [](auto const&pair) {return pair.second; });
	auto card_id = groupd_drawable[max_value.first].shuffled().front().id;
	msg_list.push_back(gamescene::DrawCard::ctor(player_id, card_id));
}

void GameCPUPlayer::update() {
	//0.5s待つ
	//firetime_msec = s3d::Time::GetMillisec() + 500;


}

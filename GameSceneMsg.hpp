#pragma once
#include<Siv3D.hpp>
#include "util.hpp"
#include "Gamedata.hpp"
#include<variant>

namespace gamescene{
	struct ClickObject
		:util::Record<2, ClickObject>
	{
		s3d::String id;
		s3d::Array<s3d::String> info;
	};
	using CtrlMsg = std::variant<ClickObject>;

	struct AddHand
		:util::Record<2, AddHand>
	{
		s3d::String player_id;
		s3d::String card_id;
	};

	struct RemoveHand
		:util::Record<2, RemoveHand>
	{
		s3d::String player_id;
		s3d::String card_id;
	};

	struct AddDiscard
		:util::Record<1, AddDiscard>
	{
		s3d::String card_id;
	};

	struct UpdateDrawnPlayerList
		:util::Record<1, UpdateDrawnPlayerList>
	{
		s3d::Array<s3d::String> playerd_player_id_list;
	};

	struct UpdateTurnPlayer
		:util::Record<3, UpdateTurnPlayer>
	{
		s3d::String player_id;
		s3d::Array<gamedat::Card> hand_list;
		s3d::Array<gamedat::Card> drawable_card_list;
	};

	struct EndGame 
		:util::Record<3, EndGame>
	{
		struct Player {
			s3d::String name;
			s3d::Array<int> point_list;
		};
		struct Basy {
			s3d::String name;
		};
		struct Handless {
			s3d::String name;
		};
		struct AllDiscard {
			gamedat::Card::Color color;
		};

		s3d::Array<Player> player_info_list;
		std::variant<Basy, Handless, AllDiscard> kimarite;
		bool gameover;
	};

	using GameModelMsg = std::variant<AddHand, RemoveHand, AddDiscard, UpdateDrawnPlayerList, UpdateTurnPlayer, EndGame>;

	struct DrawCard 
		:util::Record<2, DrawCard>
	{
		s3d::String player_id;
		s3d::String card_id;
	};
	using PlayerActionlMsg = std::variant<DrawCard>;
}

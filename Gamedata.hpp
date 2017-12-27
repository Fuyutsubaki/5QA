#pragma once
#include<Siv3D.hpp>
#include<variant>
#include "util.hpp"

namespace gamedat{
	
	struct Card {
		s3d::String id;
		enum class Color { red, yellow, green, orange };
		Color color;
		struct Number :util::Record<1, Number>{ int num; };
		struct Alpha :util::Record<1, Alpha> { s3d::String word; };
		std::variant<Number, Alpha> type;

		struct Type 
			:util::Record<2, Type>
		{
			Color color;
			std::variant<Number, Alpha> type;
		};
		Type card_type()const {
			return Type::ctor(color, type);
		}
	};

	bool is_discardable_pair(gamedat::Card const&a, gamedat::Card const&b);

	// scene \’z‚É¶¬‚³‚ê‚éB
	// •ÏX‚Í‚³‚ê‚È‚¢
	class GameData {
		s3d::Array<Card> card_list;
	public:
		s3d::Array<Card> const& get_card_list()const {
			return card_list;
		}
		Card const&get_card(s3d::String const&id)const {
			auto it = std::find_if(card_list.begin(), card_list.end(), [&](auto const&c) {return c.id == id; });;
			return *it;
		}

		s3d::Array<Card::Alpha> get_alphaword_list()const {
			return s3d::Array<s3d::String>({ L">",L"B",L"A" ,L"S" ,L"Y" ,L"!" }).map([](auto const&s) {return Card::Alpha::ctor(s); });
		}

		static std::shared_ptr<GameData> make();
	};
}

#include "Gamedata.hpp"

namespace gamedat {
	s3d::Array<Card> cardlist();
	bool gamedat::is_discardable_pair(gamedat::Card const & a, gamedat::Card const & b)
	{
		return util::visit(a.type, b.type)(
			[](gamedat::Card::Number const&lhs, gamedat::Card::Number const&rhs) {return lhs.num == rhs.num; },
			[](auto...) {return false; }
		);
	}

	std::shared_ptr<GameData> GameData::make() {
		GameData tmp;
		tmp.card_list = cardlist();
		return std::make_shared<GameData>(tmp);
	}

	s3d::Array<Card> cardlist()
	{
		// DSL
		/*
		perl -nale '@s = map {$id="cn${F[1]}_$_";qq/Card{ L"${id}", Card::Color::${F[0]},Card::Number::ctor(${F[1]}) },/ } (1..$F[2]);print for(@s)' <<EOS
		red  101  4
		red  96  4
		red  1  4
		green  3  4
		green  5  4
		green  107  4
		blue  25 4
		blue  50 4
		blue  75 4
		orange 0 2
		orange 4 2
		orange 8 2
		orange 16 4
		orange 32 2
		EOS

		perl -nale '@s = map {$id="ca_${F[0]}_$_";qq/Card{ L"${id}", Card::Color::${F[0]},Card::Alpha::ctor(L"$_") },/ } @F[1..$#F];print for(@s)' <<EOS
		red  B S
		blue B A
		green  S Y
		orange A Y
		EOS


		*/
		return {
			Card{ L"cn101_1", Card::Color::red,Card::Number::ctor(101) },
			Card{ L"cn101_2", Card::Color::red,Card::Number::ctor(101) },
			Card{ L"cn101_3", Card::Color::red,Card::Number::ctor(101) },
			Card{ L"cn101_4", Card::Color::red,Card::Number::ctor(101) },
			Card{ L"cn96_1", Card::Color::red,Card::Number::ctor(96) },
			Card{ L"cn96_2", Card::Color::red,Card::Number::ctor(96) },
			Card{ L"cn96_3", Card::Color::red,Card::Number::ctor(96) },
			Card{ L"cn96_4", Card::Color::red,Card::Number::ctor(96) },
			Card{ L"cn1_1", Card::Color::red,Card::Number::ctor(1) },
			Card{ L"cn1_2", Card::Color::red,Card::Number::ctor(1) },
			Card{ L"cn1_3", Card::Color::red,Card::Number::ctor(1) },
			Card{ L"cn1_4", Card::Color::red,Card::Number::ctor(1) },
			Card{ L"cn3_1", Card::Color::green,Card::Number::ctor(3) },
			Card{ L"cn3_2", Card::Color::green,Card::Number::ctor(3) },
			Card{ L"cn3_3", Card::Color::green,Card::Number::ctor(3) },
			Card{ L"cn3_4", Card::Color::green,Card::Number::ctor(3) },
			Card{ L"cn5_1", Card::Color::green,Card::Number::ctor(5) },
			Card{ L"cn5_2", Card::Color::green,Card::Number::ctor(5) },
			Card{ L"cn5_3", Card::Color::green,Card::Number::ctor(5) },
			Card{ L"cn5_4", Card::Color::green,Card::Number::ctor(5) },
			Card{ L"cn107_1", Card::Color::green,Card::Number::ctor(107) },
			Card{ L"cn107_2", Card::Color::green,Card::Number::ctor(107) },
			Card{ L"cn107_3", Card::Color::green,Card::Number::ctor(107) },
			Card{ L"cn107_4", Card::Color::green,Card::Number::ctor(107) },
			Card{ L"cn25_1", Card::Color::blue,Card::Number::ctor(25) },
			Card{ L"cn25_2", Card::Color::blue,Card::Number::ctor(25) },
			Card{ L"cn25_3", Card::Color::blue,Card::Number::ctor(25) },
			Card{ L"cn25_4", Card::Color::blue,Card::Number::ctor(25) },
			Card{ L"cn50_1", Card::Color::blue,Card::Number::ctor(50) },
			Card{ L"cn50_2", Card::Color::blue,Card::Number::ctor(50) },
			Card{ L"cn50_3", Card::Color::blue,Card::Number::ctor(50) },
			Card{ L"cn50_4", Card::Color::blue,Card::Number::ctor(50) },
			Card{ L"cn75_1", Card::Color::blue,Card::Number::ctor(75) },
			Card{ L"cn75_2", Card::Color::blue,Card::Number::ctor(75) },
			Card{ L"cn75_3", Card::Color::blue,Card::Number::ctor(75) },
			Card{ L"cn75_4", Card::Color::blue,Card::Number::ctor(75) },
			Card{ L"cn0_1", Card::Color::orange,Card::Number::ctor(0) },
			Card{ L"cn0_2", Card::Color::orange,Card::Number::ctor(0) },
			Card{ L"cn4_1", Card::Color::orange,Card::Number::ctor(4) },
			Card{ L"cn4_2", Card::Color::orange,Card::Number::ctor(4) },
			Card{ L"cn8_1", Card::Color::orange,Card::Number::ctor(8) },
			Card{ L"cn8_2", Card::Color::orange,Card::Number::ctor(8) },
			Card{ L"cn16_1", Card::Color::orange,Card::Number::ctor(16) },
			Card{ L"cn16_2", Card::Color::orange,Card::Number::ctor(16) },
			Card{ L"cn16_3", Card::Color::orange,Card::Number::ctor(16) },
			Card{ L"cn16_4", Card::Color::orange,Card::Number::ctor(16) },
			Card{ L"cn32_1", Card::Color::orange,Card::Number::ctor(32) },
			Card{ L"cn32_2", Card::Color::orange,Card::Number::ctor(32) },

			Card{ L"ca_red_B", Card::Color::red,Card::Alpha::ctor(L"B") },
			Card{ L"ca_red_S", Card::Color::red,Card::Alpha::ctor(L"S") },
			Card{ L"ca_blue_B", Card::Color::blue,Card::Alpha::ctor(L"B") },
			Card{ L"ca_blue_A", Card::Color::blue,Card::Alpha::ctor(L"A") },
			Card{ L"ca_green_S", Card::Color::green,Card::Alpha::ctor(L"S") },
			Card{ L"ca_green_Y", Card::Color::green,Card::Alpha::ctor(L"Y") },
			Card{ L"ca_orange_A", Card::Color::orange,Card::Alpha::ctor(L"A") },
			Card{ L"ca_orange_Y", Card::Color::orange,Card::Alpha::ctor(L"Y") },
		};
	}
}
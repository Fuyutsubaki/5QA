#include "GameView.h"

void GameView::init() {
	hand_list.clear();
	discard = nullptr;

	s3d::JSONReader conf{ L"dat/gameview.json" };

	auto list = conf[L"hands"];
	for (auto e : list.arrayView()) {

		auto id = e[L"id"].getString();
		auto w = e[L"wh"].arrayView()[0].get<int>();
		auto h = e[L"wh"].arrayView()[1].get<int>();
		auto x = e[L"at"].arrayView()[0].get<int>();
		auto y = e[L"at"].arrayView()[1].get<int>();
		int kodo = e[L"rot"].get<int>();

		hand_list.push_back(gameview_detail::PlayerHand{ id, s3d::Rect{ x - w / 2,y - h / 2,w,h }, kodo / 360. * 2_pi });
	}
	{
		auto rect = conf[L"discard"][L"rect"].arrayView();
		discard = std::make_unique<gameview_detail::Discard>(L"discard", s3d::Rect{ rect[0].get<int>(),rect[1].get<int>(),rect[2].get<int>(),rect[3].get<int>() });
		discard->setCardList(dat->get_card_list());
	}


	// card size
	{
		auto wh = conf[L"hand-cardsize"].arrayView();
		card_size = {wh[0].get<int>(), wh[1].get<int>()};
	}

	//result window
	{
		auto rect = conf[L"result"][L"rect"].arrayView();
		auto ok = conf[L"result"][L"ok"].arrayView();

		s3d::Rect rect_tmp{ rect[0].get<int>(),rect[1].get<int>(),rect[2].get<int>(),rect[3].get<int>() };
		s3d::Rect ok_tmp{ ok[0].get<int>(),ok[1].get<int>(),ok[2].get<int>(),ok[3].get<int>() };
		result_window = std::make_unique<gameview_detail::ResultWindow>(L"result", rect_tmp, ok_tmp);
	}
}
namespace gameview_detail {
	void PlayerHand::update() {
		int max_w = rect.w;
		int sum_w = hands.map([](Card const&c) {return c.getRect().w; }).sum();
		if (sum_w < max_w) {
			int tmp = 0;
			for (auto &e : hands) {
				e.setPos({ tmp,0 });
				tmp += e.getRect().w;
			}
		}
		else {
			int back_w = hands.back().getRect().w;
			std::size_t n = hands.size() - 1;
			hands.each_index([&](std::size_t idx, auto&e) {
				int x = static_cast<int>(((double) idx / n) * (max_w - back_w));
				e.setPos({ x,0 });
			});
		}
	}

	s3d::Array<gamescene::CtrlMsg> Card::input_impl() const {
		if (localRect().leftClicked()) {
			return { gamescene::ClickObject::ctor(id(), s3d::Array<s3d::String>{ {L"hands",player,id()} }) };
		}
		else {
			return {};
		}
	}

	void Card::draw_impl() const {
		s3d::Rect box{ rect.size };
		box.drawFrame();
		if (player == L"player") {
			util::visit(dat.type)(
				[&](gamedat::Card::Number const&num) {
				asset::Font::me15()(num.num).drawAt(box.center()).drawFrame();
			},
				[&](gamedat::Card::Alpha const&a) {
				asset::Font::me15()(a.word).drawAt(box.center()).drawFrame();
			}
			);
		}
	}

	// 全体のカードについての情報が必要

	void Discard::draw_impl() const {
		localRect().drawFrame();
		{
			auto str = list.map([](auto const&t) {
				return std::visit(util::make_overload(
					[&](gamedat::Card::Number const&n) {return L"{}x{}/{}"_fmt(n.num, t.n, t.max); },
					[&](gamedat::Card::Alpha const&a) ->s3d::String {return  L"{}x{}/{}"_fmt(a.word, t.n, t.max); }
				), t.card.type);
			}
			).join(L"\n", L"", L"");
			asset::Font::me15()(str).drawAt(localRect().center()).drawFrame();
		}
	}

	void Discard::setCardList(s3d::Array<gamedat::Card> const & card_list) {
		list = {};

		auto group = util::group_by(card_list, [](gamedat::Card const&e) {return e.card_type(); });
		for (auto[k, v] : group) {
			list.push_back(Dat{ k , 0,(int) v.size() });
		}
	}

	void Discard::addCard(gamedat::Card const & card) {
		auto it = std::find_if(list.begin(), list.end(), [&](Dat const&c) {return c.card == card.card_type(); });
		if (it != list.end()) {
			it->n += 1;
		}
	}

	void ResultWindow::draw_impl() const {
		localRect().drawFrame();
		ok_button.drawFrame();
		asset::Font::me15()(L"OK").drawAt(ok_button.center());


		auto list = pointlist.map([](auto const& p) {return p.point_list; }).rotated();
		auto result = list.map([](auto const&onegame) {return onegame.join(L"\t", L"", L""); }).join(L"\n", L"", L"");
		asset::Font::me15()(result).draw(localRect().tl()).drawFrame();
	}

	s3d::Array<gamescene::CtrlMsg> ResultWindow::input_impl() const {
		if (ok_button.leftClicked())
			return { gamescene::ClickObject::ctor(L"result", s3d::Array<s3d::String>{L"result", L"OK" }) };
		else
			return {};
	}
}

#include "stdafx.h"
#include "UIPortalTime.h"
#include "UIFormMgr.h"
#include "UIListView.h"

#include <ctime>

Portal::Portal(std::string destination, time_t entryFirst, time_t entryInterval, time_t entryClose, time_t destinationClose)
	: destination(destination), entryFirst(entryFirst), entryInterval(entryInterval), entryClose(entryClose), destinationClose(destinationClose) {
}

using namespace GUI;

bool CPortalTime::Init() {
	frm = CFormMgr::s_Mgr.Find("frmPortalTime");
	if(!frm){
		return false;
	}
	frm->SetIsDrag(true);
	list = dynamic_cast<CListView*>(frm->Find("list"));
	if (!list) {
		return false;
	}
	list->GetList()->GetItemImage()->SetColor(COLOR_BLUE);
	list->GetList()->GetItems()->SetItemMargin(20, 0);
	list->SetMargin(0, 0, 0, 0);
	return true;
}

void CPortalTime::Add(Portal entrance) {
	portals.emplace_back(entrance);
	auto row = list->AddItemRow();

	// Column 1 - Destination
	auto pItem = new CItem;
	pItem->SetString(entrance.Destination().data());
	row->SetIndex(0, pItem);

	// Column 2 - Portal state
	pItem = new CItem;
	pItem->SetString("Status");
	row->SetIndex(1, pItem);

	// Column 3 - Time
	pItem = new CItem;
	pItem->SetString("Time");
	row->SetIndex(2, pItem);

	list->Refresh();
}

void CPortalTime::FrameMove(DWORD dwTime) {
	if (!frm->GetIsShow()) {
		return;
	}

	// Update entrance times
	static tm _tm{};
	const auto timeNow = time(nullptr);
	for (auto row = 0; row < portals.size(); ++row) {
		const auto& portal = portals[row];


		auto timeDiff = timeNow - portal.entryFirst;

		static const char* state_opening = "Opening";
		static const char* state_closing = "Closing";
		static const char* state_unknwon = "Unknown";
		auto state = [&]() -> const char* {
			if (portal.entryInterval) {
				auto time_open = timeDiff % portal.entryInterval;
				return time_open > portal.entryClose ? state_opening : state_closing;
			}
			return state_unknwon;
		}();

		auto time_countdown = [&]() -> time_t {
			if (state == state_opening) {
				return (timeDiff % portal.entryInterval);
			}
			if (state == state_closing) {
				return portal.entryClose - (timeDiff % portal.entryInterval);
			}
			return 0;
		}();

		//Update state
		auto pItem = list->GetItemObj(row, 1);
		pItem->SetString(state);

		//Update time countdown
		std::tm* p = gmtime(&time_countdown);
		static char buf[80];
		strftime(buf, sizeof(buf), "%T", p);
		pItem = list->GetItemObj(row, 2);
		pItem->SetString(buf);
	}
}
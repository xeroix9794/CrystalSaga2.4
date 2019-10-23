#pragma once
#include "UIGlobalVar.h"
// Data only
class Portal {
public:
	Portal(std::string destination, time_t entryFirst, time_t entryInterval, time_t entryClose, time_t destinationClose);

	string Destination() const {
		return destination;
	}

	//private:
	std::string destination;
	time_t entryFirst{ 0 };
	time_t entryInterval{ 0 };
	time_t entryClose{ 0 };

	//NOTE: This may be useless data...
	time_t destinationClose{ 0 };
};

// Interface for UI only
namespace GUI {
	class CPortalTime : CUIInterface {
	public:
		virtual bool Init() override;

		void Add(Portal portal);

	private:
		std::vector<Portal> portals;
		CForm* frm{ nullptr };
		CListView* list{ nullptr };

	protected:
		virtual void FrameMove(DWORD dwTime) override;
	};
} // namespace GUI
#include "EditMapMenu.h"

#include "engine/Engine.h"

#include "../../popup/SaveMap.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

EditMapMenu::EditMapMenu( World* world )
	: Menu( world )
{
	AddItem( "Save Map...", MH( this ) {
		NEWV( popup, popup::SaveMap, m_world );
		popup->Open();
		return true;
	});
	AddItem( "Load Map...", MH() {
		return true;
	});
}

}
}
}
}
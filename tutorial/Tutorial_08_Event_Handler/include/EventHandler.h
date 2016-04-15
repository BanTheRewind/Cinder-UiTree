#pragma once

#include "UiTree.h"

struct UiData
{
	ci::vec2 mScale;
	ci::vec2 mTranslate;
};

typedef UiTreeT<UiData> UiTree;

class EventHandler : public UiTree::EventHandlerInterface
{
public:
	EventHandler();
	
	void	disable( UiTree* node ) override;
	void	enable( UiTree* node ) override;
	void	show( UiTree* node ) override;
	void	update( UiTree* node ) override;

#if defined( CINDER_COCOA_TOUCH )
	void	touchesBegan( UiTree* node, ci::app::TouchEvent& event ) override;
#else
	void	mouseDown( UiTree* node, ci::app::MouseEvent& event ) override;
#endif
private:
	float	mSpeed;
};
 
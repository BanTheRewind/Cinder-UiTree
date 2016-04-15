#pragma once

#include "UiData.h"
#include "UiTree.h"

typedef UiTreeT<UiData> UiTree;

class EventHandler : public UiTree::EventHandlerInterface
{
public:
	EventHandler();
	EventHandler( const EventHandler& rhs );

	EventHandler&	operator=( const EventHandler& rhs );

	/*
	 * The event handler is the preferred place to add
	 * functionality to a UiTree.
	 */
	EventHandler&	removeId( uint64_t id );
	uint64_t		getRemoveId() const;
	void			setRemoveId( uint64_t id );

#if defined( CINDER_COCOA_TOUCH )
	void			touchesBegan( UiTree* node, ci::app::TouchEvent& event ) override;
#else
	void			mouseDown( UiTree* node, ci::app::MouseEvent& event ) override;
#endif
private:
	uint64_t		mRemoveId;
};
 
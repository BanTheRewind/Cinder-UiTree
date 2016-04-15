#include "EventHandler.h"

using namespace ci;
using namespace ci::app;

EventHandler::EventHandler()
: mRemoveId( 0 )
{
}

EventHandler::EventHandler( const EventHandler& rhs )
{
	*this = rhs;
}

EventHandler& EventHandler::operator=( const EventHandler& rhs )
{
	mRemoveId = rhs.mRemoveId;
	return *this;
}

EventHandler& EventHandler::removeId( uint64_t id )
{
	setRemoveId( id );
	return *this;
}

uint64_t EventHandler::getRemoveId() const
{
	return mRemoveId;
}

void EventHandler::setRemoveId( uint64_t id )
{
	mRemoveId = id;
}

#if defined( CINDER_COCOA_TOUCH )
void EventHandler::touchesBegan( UiTree* node, TouchEvent& event )
{
	if ( node->hasTouches() ) {
#else
void EventHandler::mouseDown( UiTree* node, MouseEvent& event )
{
	if ( node->isMouseOver() ) {
#endif
		mRemoveId = node->getId();

		/*
		 * Control bubbling in your event handlers to reduce 
		 * the number of signal calls.
		 */
		event.setHandled( true );
	}
}
 
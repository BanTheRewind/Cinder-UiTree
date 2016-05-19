#include "Control.h"

/*
 * By overriding the connect method, we can 
 * hook up fewer events to make the handler
 * a little more lightweight.
 */
void Control::connect( UiTree& node )
{
	node.connectMouseDownEventHandler(		&Control::mouseDown,	this );
	node.connectMouseDragEventHandler(		&Control::mouseDrag,	this );
	node.connectMouseUpEventHandler(		&Control::mouseUp,		this );
	node.connectTouchesBeganEventHandler(	&Control::touchesBegan, this );
	node.connectTouchesEndedEventHandler(	&Control::touchesEnded,	this );
	node.connectTouchesMovedEventHandler(	&Control::touchesMoved,	this );
}

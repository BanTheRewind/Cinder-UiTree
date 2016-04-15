#include "EventHandler.h"

using namespace ci;
using namespace ci::app;

EventHandler::EventHandler()
: mSpeed( 0.21f )
{
}

void EventHandler::disable( UiTree* node ) 
{
	node->setScale( vec2( 0.0f ), mSpeed );
	node->setTranslate( vec2( 0.0f ), mSpeed );
	for ( auto& iter : node->getChildren() ) {
		iter.second.disable();
	}	
}

void EventHandler::enable( UiTree* node ) 
{
	node->show();
}

void EventHandler::show( UiTree* node ) 
{
	node->setScale( node->getData().mScale, mSpeed );
	node->setTranslate( node->getData().mTranslate, mSpeed );
}

void EventHandler::update( UiTree* node )
{
	if ( node->isVisible() && !node->isEnabled() && node->getScale().x <= 1.0f ) {
		node->hide();
	}
}

#if defined( CINDER_COCOA_TOUCH )
void EventHandler::touchesBegan( UiTree* node, TouchEvent& event )
{
	const vec2& p0 = event.getTouches().begin()->getPos();
#else
void EventHandler::mouseDown( UiTree* node, MouseEvent& event )
{
	const vec2& p0( event.getPos() );
#endif
	vec2 p1( node->calcAbsoluteTranslate() );

	// Perform a radial collision test. Toggle this node's children, if hit.
	float l = glm::distance( p0, p1 );
	if ( l < node->getScale().x ) {
		for ( auto& iter : node->getChildren() ) {
			iter.second.isEnabled() ? iter.second.disable() : iter.second.enable();
		}			
	}
}
 
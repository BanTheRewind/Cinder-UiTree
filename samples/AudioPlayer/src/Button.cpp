#include "Button.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Button::Button()
{
}

Button::Button( const Button& rhs )
{
	*this = rhs;
}

Button& Button::operator=( const Button& rhs )
{
	mBiased		= rhs.mBiased;
	mPressed	= rhs.mPressed;
	return *this;
}

Button&	Button::biased( bool enabled )
{
	setBiased( enabled );
	return *this;
}

Button&	Button::pressed( bool enabled )
{
	setPressed( enabled );
	return *this;
}

bool Button::isBiased() const
{
	return mBiased;
}

bool Button::isPressed() const
{
	return mPressed;
}

void Button::setBiased( bool enabled )
{
	mBiased = enabled;
}

void Button::setPressed( bool enabled )
{
	mPressed = enabled;
}

void Button::mouseDown( UiTree* node, MouseEvent& event ) 
{
	if ( node->isMouseOver() ) {
		if ( mBiased ) {
			mPressed = true;
		} else {
			mPressed = !mPressed;
		}
	}
}

void Button::mouseDrag( UiTree* node, MouseEvent& event ) 
{
	if ( mBiased && !node->isMouseOver() ) {
		mPressed = false;
	}
}

void Button::mouseUp( UiTree* node, MouseEvent& event ) 
{
	if ( mBiased ) {
		mPressed = false;
	}
}

void Button::touchesBegan( UiTree* node, TouchEvent& event ) 
{
	if ( node->hasTouches() ) {
		mPressed = true;
	}
}

void Button::touchesEnded( UiTree* node, TouchEvent& event ) 
{
	if ( !mBiased && !node->hasTouches() ) {
		mPressed = false;
	}
}

void Button::touchesMoved( UiTree* node, TouchEvent& event ) 
{
	if ( !mBiased && !node->hasTouches() ) {
		mPressed = false;
	}
}
 
#include "Slider.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Slider::Slider()
{
}

Slider::Slider( const Slider& rhs )
{
	*this = rhs;
}

Slider& Slider::operator=( const Slider& rhs )
{
	mDragging = rhs.mDragging;
	mPosition = rhs.mPosition;
	return *this;
}

Slider&	Slider::position( float v )
{
	setPosition( v );
	return *this;
}

float Slider::getPosition() const
{
	return mPosition;
}

bool Slider::isDragging() const
{
	return mDragging;
}

void Slider::setPosition( float v )
{
	mPosition = math<float>::clamp( v, 0.0f, 1.0f );
}

void Slider::mouseDown( UiTree* node, MouseEvent& event ) 
{
	if ( node->isMouseOver() ) {
		calcPosition( event.getPos() );
		mDragging = true;
	}
}

void Slider::mouseDrag( UiTree* node, MouseEvent& event ) 
{
	if ( mDragging ) {
		calcPosition( event.getPos() );
	}
}

void Slider::mouseUp( UiTree* node, MouseEvent& event ) 
{
	mDragging = false;
}

void Slider::touchesBegan( UiTree* node, TouchEvent& event ) 
{
	if ( node->hasTouches() ) {
		calcPosition( event.getTouches().begin()->getPos() );
		mDragging = true;
	}
}

void Slider::touchesEnded( UiTree* node, TouchEvent& event ) 
{
	mDragging = false;
}

void Slider::touchesMoved( UiTree* node, TouchEvent& event ) 
{
	if ( mDragging ) {
		calcPosition( event.getTouches().begin()->getPos() );
	}
}

void Slider::calcPosition( const ivec2& v )
{
	mPosition = (float)v.x / (float)App::get()->getWindowWidth();
}
 
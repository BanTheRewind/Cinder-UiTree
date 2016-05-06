#pragma once

#include "Control.h"

class Slider : public Control
{
public:
	Slider();
	Slider( const Slider& rhs );

	Slider&	operator=( const Slider& rhs );

	Slider&	position( float v );
	
	float	getPosition() const;
	bool	isDragging() const;

	void	setPosition( float v );

	void	mouseDown( UiTree* node, ci::app::MouseEvent& event ) override;
	void	mouseDrag( UiTree* node, ci::app::MouseEvent& event ) override;
	void	mouseUp( UiTree* node, ci::app::MouseEvent& event ) override;
	void	touchesBegan( UiTree* node, ci::app::TouchEvent& event ) override;
	void	touchesEnded( UiTree* node, ci::app::TouchEvent& event ) override;
	void	touchesMoved( UiTree* node, ci::app::TouchEvent& event ) override;
private:
	void	calcPosition( const ci::ivec2& v );
	bool	mDragging = false;
	float	mPosition = 0.0f;
};
 
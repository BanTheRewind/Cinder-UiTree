#pragma once

#include "Control.h"

class Slider : public Control
{
public:
	Slider();
	Slider( const Slider& rhs );

	Slider&	operator=( const Slider& rhs );

	// Sets the normalized slider position [ 0.0 - 1.0 ].
	Slider&	position( float v );
	
	// Returns the normalized slider position.
	float	getPosition() const;
	// Returns true if user is dragging slider.
	bool	isDragging() const;

	// Sets the normalized slider position [ 0.0 - 1.0 ].
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
 
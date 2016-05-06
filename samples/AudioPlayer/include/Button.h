#pragma once

#include "Control.h"

class Button : public Control
{
public:
	Button();
	Button( const Button& rhs );

	Button&	operator=( const Button& rhs );

	Button&	biased( bool enabled = true );
	Button&	pressed( bool enabled = true );

	bool	isBiased() const;
	bool	isPressed() const;

	void	setBiased( bool enabled );
	void	setPressed( bool enabled );

	void	mouseDown( UiTree* node, ci::app::MouseEvent& event ) override;
	void	mouseDrag( UiTree* node, ci::app::MouseEvent& event ) override;
	void	mouseUp( UiTree* node, ci::app::MouseEvent& event ) override;
	void	touchesBegan( UiTree* node, ci::app::TouchEvent& event ) override;
	void	touchesEnded( UiTree* node, ci::app::TouchEvent& event ) override;
	void	touchesMoved( UiTree* node, ci::app::TouchEvent& event ) override;
private:
	bool	mBiased		= true;
	bool	mPressed	= false;
};
 
#pragma once

#include "Control.h"

class Button : public Control
{
public:
	Button();
	Button( const Button& rhs );

	Button&	operator=( const Button& rhs );

	/*
	 * Set the button bias. A biased button returns
	 * to its original state when released. An unbiased 
	 * button changes state when pressed, and stays in 
	 * the new state when released. On a keyboard, the 
	 * SHIFT key is biased and CAPS LOCK is unbiased. 
	 * Default is true (biased).
	 */
	Button&	biased( bool enabled = true );
	// Mark the button as pressed.
	Button&	pressed( bool enabled = true );

	// Returns true if the button is biased.
	bool	isBiased() const;
	// Returns true if the button is in a positive state.
	bool	isPressed() const;

	/*
	 * Set the button bias. A biased button returns
	 * to its original state when released. An unbiased
	 * button changes state when pressed, and stays in
	 * the new state when released. On a keyboard, the
	 * SHIFT key is biased and CAPS LOCK is unbiased.
	 * Default is true (biased).
	 */
	void	setBiased( bool enabled );
	// Mark the button as pressed.
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
 
#pragma once

#include "cinder/Color.h"

class UiData
{
public:
	/*
	 * Write copy and assignment constructors to ensure 
	 * the integrity of your data as the UiTree structure 
	 * changes.
	 */
	UiData();
	UiData( const UiData& rhs );

	UiData&				operator=( const UiData& rhs );

	/*
	 * Write chain commands to set up your template 
	 * structure with minimal code.
	 */
	UiData&				color( const ci::Colorf& c );
	UiData&				distance( float v );
	UiData&				speed( float v );

	const ci::Colorf&	getColor() const;
	float				getDistance() const;
	float				getSpeed() const;

	void				setColor( const ci::Colorf& c );
	void				setDistance( float v );
	void				setSpeed( float v );
private:
	ci::Colorf			mColor;
	float				mDistance;
	float				mSpeed;
};
 
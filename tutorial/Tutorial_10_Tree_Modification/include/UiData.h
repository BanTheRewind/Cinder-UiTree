#pragma once

/*
 * Our UiData is a bit more sophisticated this time around.
 * We've now got a full class with move semantics and 
 * default values for its properties.
 */
class UiData
{
public:
	UiData();
	UiData( const UiData& rhs );

	UiData&	operator=( const UiData& rhs );

	UiData&	distance( float v );
	UiData&	speed( float v );

	float	getDistance() const;
	float	getSpeed() const;

	void	setDistance( float v );
	void	setSpeed( float v );
private:
	float 	mDistance;
	float 	mSpeed;
};
 
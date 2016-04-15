#include "UiData.h"

UiData::UiData()
: mDistance( 0.0f ), mSpeed( 0.0f )
{
}

UiData::UiData( const UiData& rhs )
{
	*this = rhs;
}

UiData& UiData::operator=( const UiData& rhs )
{
	mDistance	= rhs.mDistance;
	mSpeed		= rhs.mSpeed;
	return *this;
}

UiData& UiData::distance( float v )
{
	setDistance( v );
	return *this;
}

UiData& UiData::speed( float v )
{
	setSpeed( v );
	return *this;
}

float UiData::getDistance() const
{
	return mDistance;
}

float UiData::getSpeed() const
{
	return mSpeed;
}

void UiData::setDistance( float v )
{
	mDistance = v;
}

void UiData::setSpeed( float v )
{
	mSpeed = v;
}
 
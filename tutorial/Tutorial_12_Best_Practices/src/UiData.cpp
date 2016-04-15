#include "UiData.h"

using namespace ci;

UiData::UiData()
: mColor( Colorf::white() ), mDistance( 0.0f ), mSpeed( 0.0f )
{
}

UiData::UiData( const UiData& rhs )
{
	*this = rhs;
}

UiData& UiData::operator=( const UiData& rhs )
{
	mColor		= rhs.mColor;
	mDistance	= rhs.mDistance;
	mSpeed		= rhs.mSpeed;
	return *this;
}

UiData& UiData::color( const Colorf& c )
{
	setColor( c );
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

const Colorf& UiData::getColor() const
{
	return mColor;
}

float UiData::getDistance() const
{
	return mDistance;
}

float UiData::getSpeed() const
{
	return mSpeed;
}

void UiData::setColor( const Colorf& c )
{
	mColor = c;
}

void UiData::setDistance( float v )
{
	mDistance = v;
}

void UiData::setSpeed( float v )
{
	mSpeed = v;
}
 
/*
 * TUTORIAL 5 - TIMELINE ANIMATION
 *
 * The last tutorial demonstrated how animate a UiTree 
 * by either applying force to any of its tranform parameters, 
 * or by using linear interpolation. But what if you want to 
 * animate based on time? Cinder already has a sophisticated 
 * mechanism for that in the Timeline. This tutorial will show 
 * you an easy way to implement the Timeline into your UiTree. 
 * We'll also learn about the UiTree's registration point.
 */

#include "cinder/app/App.h"
#include "cinder/Timeline.h"
#include "UiTree.h"

/*
 * To keep things simple, this tutorial only uses one UiTree 
 * instance. We'll use our data structure to store Anim<T> objects 
 * for our node's color and transform.
 */
struct UiData
{
	ci::Anim<ci::Colorf>	mColor;
	ci::Anim<float>			mRotation;
	ci::Anim<ci::vec2>		mScale;
	ci::Anim<ci::vec2>		mTranslate;
};

typedef UiTreeT<UiData> UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();
	
	void	draw() override;
	void	update() override;
private:
	float	mSize;
	UiTree	mUiTree;
};

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

TutorialApp::TutorialApp()
{
	mSize = 200.0f;

	/* 
	 * Set up the data structure. We are only using one node, so we'll 
	 * just reference the data it already has.
	 */ 
	UiData& data		= mUiTree.getData();
	data.mColor			= Colorf( randFloat(), randFloat(), randFloat() );
	data.mRotation		= 0.0f;
	data.mScale			= vec2( mSize );
	data.mTranslate		= getWindowCenter();
}

void TutorialApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );
	
	const gl::ScopedColor scopedColor( mUiTree.getData().mColor.value() );
	gl::translate( mUiTree.getTranslate() );
	gl::rotate( mUiTree.getRotation().z * glm::two_pi<float>() );

	// After rotating, subtract the registration to pivot around that point.
	gl::translate( -mUiTree.getRegistration() );

	gl::scale( mUiTree.getScale() );
	gl::drawSolidRect( Rectf( vec2( 0.0f ), vec2( 1.0f ) ) );
}

void TutorialApp::update()
{
	// Set animation duration.
	static const float duration = 1.5f;

	UiData& data = mUiTree.getData();

	/*
		* For each Anim, check if its last operation is complete. If so, 
		* choose a new random value and start the next animation. 
		*/
	if ( data.mColor.isComplete() ) {
		timeline().apply( &data.mColor, Colorf( randFloat(), randFloat(), randFloat() ), duration, EaseInOutQuint() );
	}
	if ( data.mRotation.isComplete() ) {
		timeline().apply( &data.mRotation, randFloat( -1.0f, 1.0f ), duration, EaseInOutQuint() );
	}
	if ( data.mScale.isComplete() ) {
		float x = randFloat( mSize * 0.5f, mSize * 2.0f );
		float y = randFloat( mSize * 0.5f, mSize * 2.0f );
		timeline().apply( &data.mScale, vec2( x, y ), duration, EaseInOutQuint() );
	}
	if ( data.mTranslate.isComplete() ) {
		float x = randFloat( mSize * 0.5f, (float)getWindowWidth() - mSize * 0.5f );
		float y = randFloat( mSize * 0.5f, (float)getWindowHeight() - mSize * 0.5f );
		timeline().apply( &data.mTranslate, vec2( x, y ), duration, EaseInOutQuint() );
	}

	/*
		* Update the transform using the Anim values in the node's data. 
		* Note that we do not specify a speed since the Anim is handling 
		* animation.
		*/
	mUiTree
		.rotate( data.mRotation.value() )
		.scale( data.mScale.value() )
		.translate( data.mTranslate.value() );

	/*
		* The UiTree has an additional "registration" transform property. 
		* Other names for this might be origin, pivot, center, etc. It's a 
		* property you may opt to use to change where [0,0,0] or [0,0] 
		* appear to occur. In this tutorial, we are using the registration 
		* to always rotate around the center of the shape.
		*/
	mUiTree
		.registration( mUiTree.getScale() * 0.5f );

	mUiTree.update();
}

CINDER_APP( TutorialApp, RendererGl( RendererGl::Options().msaa( 16 ) ), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
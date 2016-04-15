/*
 * TUTORIAL 3 - INTERACTION
 *
 * The last tutorial demonstrated how to set up events. In 
 * this application, we will go one step further and make the 
 * UiTree draggable. We'll also learn some animation basics.
 */

#include "cinder/app/App.h"
#include "UiTree.h"

/*
 * To keep things simple, we are only going to use one UiTree
 * in this tutorial. It contains a vec2 representing the last 
 * mouse or touch position that hit the shape.
 */
typedef UiTreeT<ci::vec2> UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();
	
	void	draw() override;
	void	update() override;
private:
	UiTree	mUiTree;
};

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

TutorialApp::TutorialApp()
{
	// Hook up events. Some more clever #ifdef'ing for mouse and touch.
	mUiTree
#if defined( CINDER_COCOA_TOUCH )
		.connectTouchesBeganEventHandler( [ & ]( UiTree* node, TouchEvent event )
		{
			const vec2& p = event.getTouches().begin()->getPos();
#else
		.connectMouseDownEventHandler([ & ]( UiTree* node, MouseEvent event )
		{
			const vec2& p( event.getPos() );
#endif
			/*
			 * On mouse down or initial touch, we check of the mouse is over the 
			 * node, or if there are any active touches. If there's a hit, we 
			 * record the cursor position in the data. Otherwise, we move the 
			 * UiTree towards the cursor.
			 */
			if ( node->isMouseOver() || node->hasTouches() ) {
				node->getData() = p;
			} else {
				/*
				 * Note the second argument in this translate() call. This is the 
				 * interpolation speed. The default speed value is 1, which changes 
				 * the translate value immediately. Setting a value of less than one
				 * will lerp (linear interpolate) towards the target value at this rate.
				 */
				node->translate( p - vec2( node->getScale() ) * 0.5f, 0.21f );
			}
		} )
#if defined( CINDER_COCOA_TOUCH )
		.connectTouchesMovedEventHandler( [ & ]( UiTree* node, TouchEvent event )
		{
			const vec2& p = event.getTouches().begin()->getPos();
#else
		.connectMouseDragEventHandler([ & ]( UiTree* node, MouseEvent event )
		{
			const vec2& p( event.getPos() );
#endif
			/*
			 * If we drag a touch or cursor over the UiTree, we are going to 
			 * set its translate *velocity*. Calling translate() tells the UiTree
			 * exactly where to go. However, calling translateVelocity() adds 
			 * momentum. This is useful when you want to move an object in a 
			 * direction, but don't know exactly where you want it to end up.
			 * The second argument in this call is the decay value. The velocity 
			 * is multiplied by this value on every update. Values less than 1 make 
			 * the velocity slow down. The default value is 1, which means the 
			 * velocity will never be reduced.
			 * 
			 * In this case, we are using the difference between the current touch 
			 * or cursor position and the last one that hit the UiTree to make it 
			 * feel like you are "throwing" the square around the screen.
			 * 
			 * Note that calling translateVelocity() will end any running 
			 * translate() operations; and vice versa.
			 */
			if ( node->isMouseOver() || node->hasTouches() ) {
				node->translateVelocity( ( p - node->getData() ), 0.96f );
				node->getData() = p;
			}
		} )
		.data( vec2( 0.0f ) )
		.enable()
		.scale( vec2( 200.0f ) )
		.translate( getWindowCenter() - vec2( 100.0f ) );
}

void TutorialApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );
	
	const gl::ScopedColor scopedColor( Colorf::white() );
	const vec2& p0 = vec2( mUiTree.getTranslate() );
	const vec2& p1 = p0 + vec2( mUiTree.getScale() );
	gl::drawSolidRect( Rectf( p0, p1 ) );
}

void TutorialApp::update()
{
	/* This is a basic wall collision routine. If the square hits the 
	 * edge of the screen, it will bounce off by swapping the direction 
	 * of appropriate velocity value.
	 */
	static const float epsilon = 0.1f;

	const vec2& s = vec2( mUiTree.getScale() );
	const vec2& t = vec2( mUiTree.getTranslate() );
	const vec2& v = vec2( mUiTree.getTranslateVelocity() );
	const float d = mUiTree.getTranslateVelocityDecay();
			
	if ( t.x <= 0.0f ) {
		mUiTree
			.translate( vec2( epsilon, t.y ) )
			.translateVelocity( v * vec2( -1.0f, 1.0f ), d );
	} else if ( t.x + s.x >= (float)getWindowWidth() ) {
		mUiTree
			.translate( vec2( ( (float)getWindowWidth() - s.x ) - epsilon, t.y ) )
			.translateVelocity( v * vec2( -1.0f, 1.0f ), d );
	}
	if ( t.y <= 0.0f ) {
		mUiTree
			.translate( vec2( t.x, epsilon ) )
			.translateVelocity( v * vec2( 1.0f, -1.0f ), d );
	} else if ( t.y + s.y >= (float)getWindowHeight() ) {
		mUiTree
			.translate( vec2( t.x, ( (float)getWindowHeight() - s.y ) - epsilon ) )
			.translateVelocity( v * vec2( 1.0f, -1.0f ), d );
	}

	/*
	 * Call "update" to animate the UI tree and its children.
	 */
	mUiTree.update();
}

CINDER_APP( TutorialApp, RendererGl(), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
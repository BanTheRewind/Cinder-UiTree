/*
 * TUTORIAL 1 - INTRODUCTION
 *
 * This application introduces the fundamentals of creating
 * your first UiTree. A UiTree is a generic, recursive structure 
 * and event handler; built with user interaction in mind. 
 * It's essentially a miniature application which can carry 
 * arbitrary data around with it and exists in a hierarchy. 
 * It's kind of like the building block of a DOM.
 */

#include "cinder/app/App.h"
#include "UiTree.h"

/*
 * The UiTree has some of its own data for display, animation, 
 * event handling, and its place in the hierarchy. It also has 
 * a "data" member which can hold arbitrary data. In this example, 
 * each UiTree will have its own color. To simplify things, we're 
 * going to create an alias for a UiTreeT<T>, where T is a color.
 * We'll call it UiTree for brevity.
 */
typedef UiTreeT<ci::Colorf> UiTree;

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

// Our rather minimal application class declaration
class TutorialApp : public ci::app::App
{
public:
	TutorialApp();
	
	void	draw() override;
private:
	// The tree's root
	UiTree	mUiTree;
};

using namespace ci;
using namespace ci::app;
using namespace std;

TutorialApp::TutorialApp()
{
	/* 
	 * In this tutorial, we will add nodes to our root. Each node 
	 * will represent a different colored rectangle, forming a 
	 * fullscreen grid.
	 */ 
	static const uint8_t grid = 3;
	vec2 sz( getWindowSize() / 3 );
	
	for ( uint8_t y = 0; y < grid; ++y ) {
		for ( uint8_t x = 0; x < grid; ++x ) {
			
			// Choose a random color
			Colorf color( randFloat(), randFloat(), randFloat() );
			
			/*
			 * This adds a child to our root. By calling 
			 * createAndReturnChild(), we can set the node's data 
			 * (color), size, and position using chained calls.
			 */
			mUiTree.createAndReturnChild()
				.data( color )
				.scale( sz * 0.5f )
				.translate( sz * vec2( x, y ) + sz * 0.25f );
			
		}
	}
}

void TutorialApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );
	
	/* 
	 * UiTree's have children of their own type. They are stored 
	 * in a map where the key is a unique ID (we'll cover this 
	 * later) and the value is the UiTree node. Let's iterate 
	 * the tree.
	 */ 
	for ( const auto& iter : mUiTree.getChildren() ) {
		
		// Get the node
		const UiTree& node = iter.second;

		/* 
		 * Use the node's data (color) and built-in properties to 
		 * draw a rectangle. The UiTree's transform and animation 
		 * members are in 3D, so we need to cast them to 2D here.
		 */ 
		const gl::ScopedColor scopedColor( node.getData() );
		const vec2& p0 = vec2( node.getTranslate() );
		const vec2& p1 = p0 + vec2( node.getScale() );
		gl::drawSolidRect( Rectf( p0, p1 ) );
	}
}

CINDER_APP( TutorialApp, RendererGl(), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
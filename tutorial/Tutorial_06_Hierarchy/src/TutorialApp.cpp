/*
 * TUTORIAL 6 - HIERARCHY
 * 
 * Previous tutorials have only used one instance of a UiTree, 
 * or added a few onto an invisible root. In this exercise, 
 * we will create a tree with multiple levels of depth and leverage 
 * the parent/child relationship to walk the layers. We'll also 
 * explore some recursion strategies to create, configure, and 
 * render a complex tree with minimal code.
 */

#include "cinder/app/App.h"
#include "UiTree.h"

/*
 * Our nodes don't need any extra data in this tutorial, 
 * so our UiTree has a data type of void*.
 */
typedef UiTreeT<void*> UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();

	void	draw() override;
private:
	/* 
	 * Recursive methods for creating, positioning, and rendering 
	 * nodes. See the defintions for notes.
	 */
	void	configTree( UiTree& node, float radius, float distance );
	void	createTree( UiTree& parent, size_t count, size_t depth, size_t level = 0 );
	void	drawTree( const UiTree& node );

	UiTree	mUiTree;
};

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

TutorialApp::TutorialApp()
{
	// Recursively create UI tree.
	createTree( mUiTree, 3, 4 );
	
	// Position the root.
#if defined( CINDER_COCOA_TOUCH )
	mUiTree
	.scale( vec2( 33.0f ) )
	.translate( getWindowCenter() + vec2( 57.0f, 0.0f ) );
#else
	mUiTree
		.scale( vec2( 60.0f ) )
		.translate( getWindowCenter() );
#endif

	/*
	 * Walk through tree and set the position and scale for 
	 * each node. In previous tutorials, we set up the node 
	 * at the time it was created. In most cases, however, it 
	 * will be easier to create the tree structure first, 
	 * then walk through it to set up the nodes.
	 */
#if defined( CINDER_COCOA_TOUCH )
	configTree( mUiTree, mUiTree.getScale().x, 150.0f );
#else
	configTree( mUiTree, mUiTree.getScale().x, 270.0f );
#endif
}

/*
 * This recursive method positions the children of "node" in a circle 
 * using the "distance" and "radius" arguments.
 */
void TutorialApp::configTree( UiTree& node, float radius, float distance )
{
	if ( node.getChildren().empty() ) {
		return;
	}
	
	// We use this value to reduce the size and distance of nodes at each level.
	static const float decay = 0.4f;
	radius *= decay;
	
	/*
	 * Set each child's translate at a step in a circle. Note that we start
	 * the circle at a half step forward. This prevents the lines from overlapping
	 * the circles.
	 */
	const float d	= ( (float)M_PI * 2.0f ) / (float)node.getChildren().size();
	float t			= d * 0.5f;
	for ( auto& iter : node.getChildren() ) {
		t		+= d;
		vec2 p	= vec2( math<float>::cos( t ), math<float>::sin( t ) ) * distance;
		iter.second
			.scale( vec2( radius ) )
			.translate( p );
		configTree( iter.second, radius, distance * decay );
	}
}

/*
 * This recursive method creates the tree. The "parent" is the node 
 * to which "count" children will be added. The "depth" and "level" 
 * arguments control the recursion. Every time we create a child node, 
 * we build another tree onto it if our recursion depth hasn't been hit.
 */
void TutorialApp::createTree( UiTree& parent, size_t count, size_t depth, size_t level )
{
	if ( level < depth ) {
		for ( size_t i = 0; i < count; ++i ) {
			createTree( parent.createAndReturnChild(), count, depth, level + 1 );
		}
	}
}

void TutorialApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );

	drawTree( mUiTree );
}

/*
 * This method recursively draws a tree. When using UiTree to create 
 * a scene graph or similar GUI, it's a good idea to write a recursive 
 * renderer to keep code simple and small.
 */
void TutorialApp::drawTree( const UiTree& node )
{
	/*
	 * Draw a line from this node's center to its parent's. Then draw 
	 * a solid white circle surrounded by a red ring.
	 */
	const vec2 p	= vec2( node.calcAbsoluteTranslate() );
	const float r	= node.getScale().x;
	{
		const gl::ScopedLineWidth scopedLineWidth( 1.0f );
		const gl::ScopedColor scopedColor( Colorf( 0.8f, 0.2f, 0.08f ) );
		gl::drawStrokedCircle( p, r + 3.0f );
	}
	{
		const gl::ScopedColor scopedColor( Colorf::white() );
		gl::drawSolidCircle( p, r );
	}
	if ( node.getParent() != nullptr ) {
		const gl::ScopedLineWidth scopedLineWidth( 0.125f );
		const gl::ScopedColor scopedColor( Colorf::gray( 0.25f ) );
		gl::drawLine( vec2( node.getParent()->calcAbsoluteTranslate() ), p );
	}

	for ( auto& iter : node.getChildren() ) {
		drawTree( iter.second );
	}
}

CINDER_APP( TutorialApp, RendererGl( RendererGl::Options().msaa( 16 ) ), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
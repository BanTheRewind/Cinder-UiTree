/*
 * TUTORIAL 9 - FINDING NODES
 * 
 * This tutorial introduces a few convenient ways to 
 * find nodes anywhere in a UiTree. The exists() and find() 
 * methods help you locate a node by ID. contains() hit 
 * tests nodes in a tree with an option to return the ID 
 * of the child which was hit. query() is a powerful method 
 * which return a list of nodes which meet arbitrary criteria 
 * in a user-defined method.
 * 
 * In this tutorial, we'll see examples of each of these 
 * in action. We'll run a query() to pick out which of our 
 * randomly-colored nodes are green. We'll use contains() to 
 * determine when a node contacts an orbiting cursor, then use 
 * exists() and find() to select and modify it.
 */

#include "cinder/app/App.h"
#include "UiTree.h"

typedef UiTreeT<ci::ColorAf> UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();

	void		draw() override;
	void		update() override;
private:
	void		configTree( UiTree& node, float radius, float distance );
	void		createTree( UiTree& parent, size_t count, size_t depth, size_t level = 0 );
	void		drawTree( const UiTree& node );
	ci::ColorAf	randColor();

	ci::vec2	mCursor;
	uint64_t	mSelectedId;

	UiTree		mUiTree;
};

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

TutorialApp::TutorialApp()
{
	mSelectedId	= 0;

	createTree( mUiTree, 3, 4 );
	
#if defined( CINDER_COCOA_TOUCH )
	mUiTree
	.scale( vec2( 33.0f ) )
	.translate( getWindowCenter() + vec2( 57.0f, 0.0f ) );
#else
	mUiTree
	.scale( vec2( 60.0f ) )
	.translate( getWindowCenter() );
#endif

#if defined( CINDER_COCOA_TOUCH )
	configTree( mUiTree, mUiTree.getScale().x, 130.0f );
#else
	configTree( mUiTree, mUiTree.getScale().x, 260.0f );
#endif

	// Use query to find nodes which are not mostly green.
	list<UiTree*> nonGreenNodes = mUiTree.query( [ & ]( const UiTree& node )
	{
		const ColorAf& c = node.getData();
		if ( ( c.r + c.b ) * 0.5f > c.g - 0.333f ) {
			return true;
		}
		return false;
	} );

	// Set all the non-green nodes' alphas to zero.
	for ( UiTree* node : nonGreenNodes ) {
		node->getData().a = 0.0f;
	}
}

void TutorialApp::configTree( UiTree& node, float radius, float distance )
{
	// Choose a random color.
	node.data( randColor() );

	if ( node.getChildren().empty() ) {
		return;
	}

	static const float decay = 0.5f;
	radius *= decay;
	
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
	gl::enableAlphaBlending();

	drawTree( mUiTree );

	const gl::ScopedLineWidth scopedLineWidth( 1.0f );
	const gl::ScopedColor scopedColor( mSelectedId > 0 ? Colorf( 1.0f, 0.0f, 0.0f ) : Colorf( 0.0f, 0.0f, 1.0f ) );
	gl::drawSolidCircle( mCursor, 3.0f );
}

void TutorialApp::drawTree( const UiTree& node )
{
	const vec2 p	= vec2( node.calcAbsoluteTranslate() );
	const float r	= node.getScale().x;
	{
		const gl::ScopedLineWidth scopedLineWidth( 1.0f );
		const gl::ScopedColor scopedColor( Colorf::white() );
		gl::drawStrokedCircle( p, r + 3.0f );
	}
	{
		const gl::ScopedColor scopedColor( node.getData() );
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

ColorAf TutorialApp::randColor()
{
	return ColorAf( randFloat(), randFloat(), randFloat(), 1.0f );
}

void TutorialApp::update() 
{
	// Update the cursor position
	float e	= (float)getElapsedSeconds() * 0.25f;
#if defined( CINDER_COCOA_TOUCH )
	const float r = 130.0f;
#else
	const float r = 200.0f;
#endif
	mCursor	= getWindowCenter() + vec2( cos( e ), sin( e ) ) * ( r + sin( e * 10.0f ) * r * 0.5f );

	// Hit test the UiTree.
	uint64_t selectedId = mSelectedId;
	mSelectedId			= 0;
	mUiTree.contains( mCursor, UiTree::CollisionType_Circle, &mSelectedId );

	/*
	 * Always call exists() before find(). Calling find() with an 
	 * invalid ID throws an exception. Try this by passing an 
	 * out-of-range number (uncomment the line below). When the 
	 * cursor hits a node, we will change its color.
	 */
	//mSelectedId = numeric_limits<uint64_t>::max();
	if ( selectedId != mSelectedId && 
		mSelectedId > 0 && 
		mUiTree.exists( mSelectedId ) ) {
		try {
			UiTree& node = mUiTree.find( mSelectedId );
			node.data( randColor() );
		} catch ( UiTree::ExcIdNotFound ex ) {
			CI_LOG_EXCEPTION( "Invalid ID", ex );
		}
	}
}

CINDER_APP( TutorialApp, RendererGl( RendererGl::Options().msaa( 16 ) ), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
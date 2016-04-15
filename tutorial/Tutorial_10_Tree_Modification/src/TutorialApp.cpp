/*
 * TUTORIAL 10 - TREE MODIFICATION
 * 
 * In previous tutorials, we've built and configured the 
 * UiTree only when the application started. Now we'll learn
 * how to modify the structure of the tree in real time. We'll 
 * also build a more sophisticated UiData structure.
 * 
 * This tutorial demonstrates how to change the hierachy of 
 * existing nodes, spawn new nodes dynamically, and remove nodes.
 * The lines drawn between nodes represent parent-child 
 * relationships. Note that the child nodes change their parent 
 * based on proximity. Click on any node, other than the center 
 * one, to remove it. This will automatically spawn a new one.
 */

#include "cinder/app/App.h"
#include "UiData.h"
#include "UiTree.h"

typedef UiTreeT<UiData> UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();

	void		draw() override;
	void		update() override;
private:
	void		drawTree( const UiTree& node );
	void		makeNode();

	// ID of clicked node to remove.
	uint64_t	mRemoveId;

	UiTree		mUiTree;
};

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

TutorialApp::TutorialApp()
{
	mRemoveId = 0;

	mUiTree
		.enable()
#if defined( CINDER_COCOA_TOUCH )
		.scale( vec2( 33.0f ) )
#else
		.scale( vec2( 60.0f ) )
#endif
		.translate( getWindowCenter() );

	// Make ten randomly positioned nodes.
	for ( uint8_t i = 0; i < 10; ++i ) {
		makeNode();
	}
}

void TutorialApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );

	drawTree( mUiTree );
}

void TutorialApp::drawTree( const UiTree& node )
{
	const vec2 p	= vec2( node.getTranslate() );
	const float r	= node.getScale().x * 0.5f;
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
		gl::drawLine( vec2( node.getParent()->getTranslate() ), p );
	}

	for ( auto& iter : node.getChildren() ) {
		drawTree( iter.second );
	}
}

void TutorialApp::makeNode()
{
	// Choose a random position.
#if defined( CINDER_COCOA_TOUCH )
	const float dMax	= 220.0f;
	const float d		= randFloat( 28.0f, dMax );
	float r				= 11.0f;
#else
	const float dMax	= 400.0f;
	const float d		= randFloat( 50.0f, dMax );
	float r				= 20.0f;
#endif

	// Base the radius on the position.
	r					+= ( 1.0f - ( d / dMax ) ) * r * 2.0f;

	/*
	 * Create the child on the root. It will be assigned an 
	 * appropriate parent in update(). 
	 */
	mUiTree.createAndReturnChild()
		/*
		 * By setting the collision type to "circle", mouse and 
		 * touch activity will be radial instead of rectangular.
		 */
		.collisionType( UiTree::CollisionType_Circle )
		// Set the distance and pick a random orbit speed.
		.data( UiData()
			.distance( d )
			.speed( randFloat( -1.0f, 1.0f ) )
			)
		.enable()
		.scale( vec2( r ) )
		/*
		 * Mark this node for removal when clicked or tapped. 
		 * Do not remove a node from inside its callback.
		 */
#if defined( CINDER_COCOA_TOUCH )
		.connectTouchesBeganEventHandler( [ & ]( UiTree* node, TouchEvent& event )
		{
			if ( node->hasTouches() ) {
#else
		.connectMouseDownEventHandler( [ & ]( UiTree* node, MouseEvent& event )
		{
			if ( node->isMouseOver() ) {
#endif
				mRemoveId = node->getId();
			}
		} );
}

void TutorialApp::update()
{
	/*
	 * If there is a node to remove, remove it and make a 
	 * new one. Note that you can remove any node by ID from 
	 * anywhere in the hierarchy more "inner" than the target.
	 * By calling removeChild() from the root, you don't have 
	 * to go find it.
	 */
	if ( mRemoveId > 0 && mUiTree.removeChild( mRemoveId ) ) {
		makeNode();
	}
	mRemoveId = 0;

	const float e = (float)getElapsedSeconds();

	/* 
	 * To get a flat list of the entire tree, run a query 
	 * which always returns true.
	 */
	list<UiTree*> nodes = mUiTree.query( []( const UiTree& node )
	{
		return true;
	} );

	const vec2 c = getWindowCenter();

	// Iterate through all nodes except the root.
	for ( UiTree* a : nodes ) {
		if ( a->getParent() != nullptr ) {

			// Update the node's position.
			UiData& data	= a->getData();
			const float s	= data.getSpeed();
			a->setTranslate( c + vec2( cos( e * s ), sin( e * s ) ) * data.getDistance() );

			/*
			 * Find the closest node that is larger than this and 
			 * make it this node's parent.
			 */
			float d0 = numeric_limits<float>::max();
			for ( UiTree* b : nodes ) {
				if ( a != b && b->getScale().x > a->getScale().x ) {
					const float d1 = glm::distance( a->getTranslate(), b->getTranslate() );
					if ( d1 < d0 ) {
						d0 = d1;
						a->setParent( b );
					}
				}
			}
		}
	}
}

CINDER_APP( TutorialApp, RendererGl( RendererGl::Options().msaa( 16 ) ), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
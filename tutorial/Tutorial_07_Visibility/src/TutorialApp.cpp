/*
 * TUTORIAL 7 - VISIBILITY
 * 
 * In addition to the UiTree's App-like events, there are four 
 * special events to help control a node's interactivity and 
 * visibility: enable, disable, hide, and show. Enabling or 
 * disabling a node toggles its mouse, touch, and key events, 
 * but also triggers events for "disable" and "enable". Setting 
 * a node's visibility on or off will fire "hide" and show" events.
 * Making a node visible or not does nothing on its own. It's a 
 * flag you can use in your renderer.
 * 
 * We'll apply these concepts to the previous tutorial to make it
 * animated and interactive.
 */

#include "cinder/app/App.h"
#include "UiTree.h"

// We'll need to store each node's target size and position.
struct UiData
{
	ci::vec2 mScale;
	ci::vec2 mTranslate;
};

typedef UiTreeT<UiData> UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();

	void	draw() override;
	void	update() override;
private:
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
	createTree( mUiTree, 3, 4 );

	UiData data;
#if defined( CINDER_COCOA_TOUCH )
	data.mScale		= vec2( 33.0f );
	data.mTranslate	= getWindowCenter() + vec2( 57.0f );
#else
	data.mScale		= vec2( 60.0f );
	data.mTranslate	= getWindowCenter();
#endif
	mUiTree
		.data( data )
		.scale( vec2( 0.0f ) )
		.translate( data.mTranslate );

#if defined( CINDER_COCOA_TOUCH )
	configTree( mUiTree, data.mScale.x, 150.0f );
#else
	configTree( mUiTree, data.mScale.x, 270.0f );
#endif
	
	// Enable the root to reveal it and make it interactive.
	mUiTree.enable();
}

void TutorialApp::configTree( UiTree& node, float radius, float distance )
{
	static const float decay = 0.4f;
	static const float speed = 0.21f;

	/*
	 * Let's hook up our tree's events. When a tree is disabled (interaction
	 * is turned off), we're going to disable its children and animate its 
	 * scale and position to zero. The update event monitors the scale; 
	 * hiding the tree when it gets below one pixel. Enabling interaction 
	 * calls show() which turns on the visibility and animates the scale 
	 * and position back up. We'll hide the tree by default.
	 */
	node
		.connectDisableEventHandler( []( UiTree* node ) 
		{
			node->setScale( vec2( 0.0f ), speed );
			node->setTranslate( vec2( 0.0f ), speed );
			for ( auto& iter : node->getChildren() ) {
				iter.second.disable();
			}	
		} )
		.connectEnableEventHandler( []( UiTree* node ) 
		{
			node->show();
		} )
		.connectShowEventHandler( []( UiTree* node ) 
		{
			node->setScale( node->getData().mScale, speed );
			node->setTranslate( node->getData().mTranslate, speed );
		} )
		.connectUpdateEventHandler( []( UiTree* node )
		{
			if ( node->isVisible() && !node->isEnabled() && node->getScale().x <= 1.0f ) {
				node->hide();
			}
		} )
		.hide();

	// Connect interaction events to nodes which have children.
	if ( !node.getChildren().empty() ) {
		node
#if defined( CINDER_COCOA_TOUCH )
			.connectTouchesBeganEventHandler( []( UiTree* node, TouchEvent& event )
			{
				const vec2& p0 = event.getTouches().begin()->getPos();
#else
			.connectMouseDownEventHandler( []( UiTree* node, MouseEvent& event )
			{
				const vec2& p0( event.getPos() );
#endif
				vec2 p1( node->calcAbsoluteTranslate() );

				// Perform a radial collision test. Toggle this node's children, if hit.
				float l = glm::distance( p0, p1 );
				if ( l < node->getScale().x ) {
					for ( auto& iter : node->getChildren() ) {
						iter.second.isEnabled() ? iter.second.disable() : iter.second.enable();
					}			
				}
			} );

		radius				*= decay;
		const float d		= ( (float)M_PI * 2.0f ) / (float)node.getChildren().size();
		float t				= d * 0.5f;
		for ( auto& iter : node.getChildren() ) {
			UiTree& node	= iter.second;
		
			t				+= d;
			vec2 p			= vec2( math<float>::cos( t ), math<float>::sin( t ) ) * distance;

			UiData data;
			data.mScale		= vec2( radius );
			data.mTranslate	= p;
			node
				.data( data )
				.scale( vec2( 0.0f ) );
			configTree( iter.second, radius, distance * decay );
		}
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

	drawTree( mUiTree );
}

void TutorialApp::drawTree( const UiTree& node )
{
	// Check if a node is visible before drawing it.
	if ( node.isVisible() ) {
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
}

void TutorialApp::update()
{
	mUiTree.update();
}

CINDER_APP( TutorialApp, RendererGl( RendererGl::Options().msaa( 16 ) ), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
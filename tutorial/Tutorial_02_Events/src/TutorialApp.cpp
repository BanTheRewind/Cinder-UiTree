/*
 * TUTORIAL 2 - EVENTS
 *
 * This application demonstrates how to set up event handlers
 * in a UiTree. We'll build on the first tutorial to make our 
 * grid interactive.
 */

#include "cinder/app/App.h"
#include "UiTree.h"

typedef UiTreeT<ci::ColorAf> UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();
	
	void	draw() override;
private:
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
	static const size_t grid	= 3;
	const vec2& sz				= vec2( getWindowSize() / (int32_t)grid );
	for ( size_t y = 0; y < grid; ++y ) {
		for ( size_t x = 0; x < grid; ++x ) {
			
			/*
			 * We're going to create nodes to represent our grid like 
			 * in the last tutorial, but this time we are going to assign 
			 * some event handlers. By writing anonymous methods, we can 
			 * do all this setup work without having to create separate 
			 * class files.
			 */
			mUiTree.createAndReturnChild()			
				/*
				 * Interaction events are enabled by calling the enable() method.
				 * These events are disabled by default.
				 */
				.enable()
#if !defined( CINDER_COCOA_TOUCH )
				// We're not on iOS here, so we'll add a resize handler.
				.connectResizeEventHandler( [ & ]( UiTree* node )
				{
					// Calculate the new size of the rectangle in the grid.
					const vec2& sz = vec2( getWindowSize() / (int32_t)grid );
					
					/*
					 * Each node has an ID that is unique to the entire tree. 
					 * When creating a child, if you don't specify an ID, the tree 
					 * will automatically assign the next available number. The
					 * root is automatically assigned 0, so we assume that its 
					 * children start at 1. Subtract 1 from the node's ID to get 
					 * its index in the grid. Then use the index to calculate
					 * the rectangle's new position.
					 */
					size_t index	= node->getId() - 1;
					size_t x		= index % grid;
					size_t y		= (size_t)math<float>::floor( (float)( index / grid ) );
					
					node
						->scale( sz * 0.5f )
						.translate( sz * vec2( x, y ) + sz * 0.25f );
				} )

				/*
				 * "Data" in a UiTree is whatever type we assigned in the 
				 * template argument. The data here is a color, so when we 
				 * call getData(), we get the UiTree's color. If a mouse or 
				 * touch occurs, we toggle the alpha value.
				 */
				.connectMouseOverEventHandler([ & ]( UiTree* node )
				{
#else
				.connectTouchOverEventHandler( [ & ]( UiTree* node, uint32_t )
				{
#endif
					node->getData().a = 1.0f;
				} )
#if defined( CINDER_COCOA_TOUCH )
				.connectTouchOutEventHandler( [ & ]( UiTree* node, uint32_t )
				{
#else
				.connectMouseOutEventHandler([ & ]( UiTree* node )
				{
#endif
					node->getData().a = 0.2f;
				} )
				.data( ColorAf( randFloat(), randFloat(), randFloat(), 0.2f ) )
				.scale( sz * 0.5f )
				.translate( sz * vec2( x, y ) + sz * 0.25f );
			
		}
	}

	// Enable the root in order to propagate events
	mUiTree.enable();
}

void TutorialApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );
	gl::enableAlphaBlending();
	
	for ( const auto& iter : mUiTree.getChildren() ) {
		const UiTree& node = iter.second;
		
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
 
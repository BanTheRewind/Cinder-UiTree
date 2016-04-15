/*
 * TUTORIAL 8 - EVENT HANDLER
 * 
 * As convenient as it is to use inline anonymous methods
 * to set up UiTree callbacks, it's not necessarily reasonable
 * for very large projects. This is especially true if you want 
 * nodes in the same tree to have unique behaviors (eg, a 
 * draggable node versus a static one that takes keyboard input). 
 * 
 * Enter the EventHandler. This is an interface inside of the 
 * UiTree which has callbacks you can override and an easy way 
 * to connect to a UiTree instance. This tutorial is identical 
 * to the last one, only we've moved our UiTree declarations 
 * and callbacks out to separate files. The main application 
 * just handles setup and update, with interaction events moved 
 * out into a single, central EventHandler class.
 */

#include "cinder/app/App.h"
#include "EventHandler.h"

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();

	void			draw() override;
	void			update() override;
private:
	void			configTree( UiTree& node, float radius, float distance );
	void			createTree( UiTree& parent, size_t count, size_t depth, size_t level = 0 );
	void			drawTree( const UiTree& node );
	
	// The EventHandler instance.
	EventHandler	mEventHandler;

	UiTree			mUiTree;
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

	mUiTree.enable();
}

void TutorialApp::configTree( UiTree& node, float radius, float distance )
{
	static const float decay = 0.4f;

	/*
	 * Let's hook up our tree's events. Everything works just like 
	 * in the last tutorial, but now all the events live in a 
	 * separate class. This helps us to organize our code a bit. 
	 * We can write different event handlers for different nodes to 
	 * give them each their own unique behaviors. To connect a node 
	 * to an event handler, you may either use the node's connect* 
	 * methods, or call connect() from the event handler, which hooks 
	 * up all the events for you.
	 */
	mEventHandler.connect( node );
	node.hide();

	// Disconnect input event from nodes which do not have children.
	if ( node.getChildren().empty() ) {
#if defined( CINDER_COCOA_TOUCH )
		node.disconnectTouchesBeganEventHandler();
#else
		node.disconnectMouseDownEventHandler();
#endif
	} else {
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
 
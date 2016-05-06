#include "cinder/app/App.h"
#include "cinder/audio/audio.h"
#include "cinder/gl/gl.h"

#include "Button.h"
#include "Slider.h"

typedef std::map<NodeId, ci::gl::Texture2dRef> IdTextureMap;

#include "cinder/app/RendererGl.h"
#include "Resources.h"

class AudioPlayerApp : public ci::app::App
{
public:
	AudioPlayerApp();
	
	void							draw() override;
	void							resize() override;
	void							update() override;
private:
	void							drawTree( const UiTree& node );
	Button							mButtonClose;
	Button							mButtonDrag;
	Button							mButtonForward;
	Button							mButtonPlay;
	Button							mButtonRewind;
	IdTextureMap					mIdTextureMap;
	Slider							mSlider;
	UiTree							mUiTree;
	void							showTree( UiTree& node );

	ci::audio::BufferPlayerNodeRef	mBufferPlayerNode;
	float							mSpeed = 0.0f;
};

using namespace ci;
using namespace ci::app;
using namespace std;

AudioPlayerApp::AudioPlayerApp()
{
	// Create the tree structure and connect event handlers
	mButtonDrag.connect( 
		mUiTree.createAndReturnChild( NodeId_Drag )
			.createChild( NodeId_DragRect )
			.createChild( NodeId_DragImage ) 
			.enable()
			);
	mButtonClose.connect( 
		mUiTree.createAndReturnChild( NodeId_Close )
			.createChild( NodeId_CloseRect )
			.createChild( NodeId_CloseImage ) 
			.enable()
			);
	mUiTree.createChild( NodeId_Display );
	mSlider.connect( 
		mUiTree.createAndReturnChild( NodeId_Scrub )
			.createChild( NodeId_ScrubRect )
			.createChild( NodeId_ScrubImage ) 
			.enable()
			);
	mButtonRewind.connect( 
		mUiTree.createAndReturnChild( NodeId_Rewind )
			.createChild( NodeId_RewindRect )
			.createChild( NodeId_RewindImage ) 
			.enable()
			);
	mButtonPlay
		.biased( false )
		.connect( 
			mUiTree.createAndReturnChild( NodeId_Play )
				.createChild( NodeId_PlayRect )
				.createChild( NodeId_PlayImage )
				.createChild( NodeId_PauseImage ) 
				.enable()
				);
	mButtonForward.connect( 
		mUiTree.createAndReturnChild( NodeId_Forward )
			.createChild( NodeId_ForwardRect )
			.createChild( NodeId_ForwardImage ) 
			.enable()
			);
	mUiTree.enable();

	// Set color for images and background rectangles
	ColorAf colorImage	= ColorAf::black();
	ColorAf colorRect	= ColorAf::gray( 0.87f );
	for ( const NodeId id : { 
		NodeId_CloseImage, NodeId_DragImage, NodeId_ForwardImage, 
		NodeId_PauseImage, NodeId_PlayImage, NodeId_RewindImage, 
		NodeId_ScrubImage
		} ) {
		mUiTree.find( id ).data( colorImage );
	}
	for ( const NodeId id : { 
		NodeId_CloseRect, NodeId_DragRect, NodeId_ForwardRect,
		NodeId_PlayRect, NodeId_RewindRect, NodeId_ScrubRect
		} ) {
		mUiTree.find( id ).data( colorRect );
	}

	// Show all nodes, then hide the pause button
	showTree( mUiTree );
	mUiTree.find( NodeId_PauseImage )
		.hide();

	// Load textures for image nodes
	auto loadTexture = [ & ]( const DataSourceRef& dataSource ) -> gl::Texture2dRef
	{
		return gl::Texture2d::create( loadImage( dataSource ) );
	};
	mIdTextureMap[ NodeId_CloseImage ]		= loadTexture( loadResource( RES_PNG_CLOSE ) );
	mIdTextureMap[ NodeId_DragImage ]		= loadTexture( loadResource( RES_PNG_DRAG ) );
	mIdTextureMap[ NodeId_ForwardImage ]	= loadTexture( loadResource( RES_PNG_FORWARD ) );
	mIdTextureMap[ NodeId_PauseImage ]		= loadTexture( loadResource( RES_PNG_PAUSE ) );
	mIdTextureMap[ NodeId_PlayImage ]		= loadTexture( loadResource( RES_PNG_PLAY ) );
	mIdTextureMap[ NodeId_RewindImage ]		= loadTexture( loadResource( RES_PNG_REWIND ) );
	mIdTextureMap[ NodeId_ScrubImage ]		= loadTexture( loadResource( RES_PNG_SCRUB ) );

	gl::enableAlphaBlending();
	gl::enableVerticalSync();

	// Load audio
	auto ctx = audio::Context::master();
	if ( ctx ) {
		ctx->enable();
		mBufferPlayerNode = ctx->makeNode( 
			new audio::BufferPlayerNode( 
				audio::load( loadResource( RES_SAMPLE ), 
				ctx->getSampleRate() )->loadBuffer() 
				) 
			);
		mBufferPlayerNode >> ctx->getOutput();
		mBufferPlayerNode->setLoopEnabled();
	}
	
	// Call resize to do layout
	resize();
}

void AudioPlayerApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.7f ) );
	
	drawTree( mUiTree );
}

void AudioPlayerApp::drawTree( const UiTree& node )
{
	// Only draw nodes which don't have children
	if ( node.isVisible() && node.getChildren().empty() ) {
		NodeId id = (NodeId)node.getId();
		Rectf rect( vec2( 0.0f ), vec2( node.getScale() ) );

		ColorAf c = node.getData();
		if ( 
			( id == NodeId_DragRect		&& mButtonDrag.isPressed() ) || 
			( id == NodeId_ForwardRect	&& mButtonForward.isPressed() ) || 
			( id == NodeId_RewindRect	&& mButtonRewind.isPressed() ) || 
			( id == NodeId_PlayRect		&& mButtonPlay.isPressed() ) || 
			( id == NodeId_ScrubRect	&& mSlider.isDragging() )
			 ) {
			c = ColorAf::gray( 0.9f );
		}

		const gl::ScopedColor scopedColor( c );
		if ( mIdTextureMap.find( id ) != mIdTextureMap.end() ) {
			const gl::Texture2dRef& texture = mIdTextureMap.at( id );
			gl::draw( texture, texture->getBounds(), rect );
		} else {
			gl::drawSolidRect( rect );
		}
	} else {
		for ( const auto& iter : node.getChildren() ) {
			const gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( iter.second.getTranslate() );
			drawTree( iter.second );
		}
	}
}

void AudioPlayerApp::resize()
{
	float h = (float)getWindowHeight();
	float w = (float)getWindowWidth();

	float c = w / 6.0f;
	float r = h / 6.0f;

	float y = 0.0f;

	// Drag bar
	{
		UiTree& node = mUiTree.find( NodeId_Drag )
			.scale( vec2( c * 5.0f, r ) );
		mUiTree.find( NodeId_DragRect )
			.scale( vec2( node.getScale() ) + vec2( -1.0f ) );
		if ( mIdTextureMap.find( NodeId_DragImage ) != mIdTextureMap.end() ) {
			const vec2 sz( mIdTextureMap.at( NodeId_DragImage )->getSize() );
			mUiTree.find( NodeId_DragImage )
				.scale( sz )
				.translate( ( vec2( c, r ) - sz ) * 0.5f );
		}
	}

	// Close button
	{
		UiTree& node = mUiTree.find( NodeId_Close )
			.scale( vec2( c, r ) )
			.translate( vec2( c * 5.0f, 0.0f ) );
		mUiTree.find( NodeId_CloseRect )
			.scale( vec2( node.getScale() ) + vec2( 0.0f, -1.0f ) );
		if ( mIdTextureMap.find( NodeId_CloseImage ) != mIdTextureMap.end() ) {
			const vec2 sz( mIdTextureMap.at( NodeId_CloseImage )->getSize() );
			mUiTree.find( NodeId_CloseImage )
				.scale( sz )
				.translate( ( vec2( node.getScale() ) - sz ) * 0.5f );
		}
	}
	y += r;

	// Display
	mUiTree.find( NodeId_Display )
		.scale( vec2( w, r * 3.0f ) + vec2( 0.0f, -1.0f ) )
		.translate( vec2( 0.0f, y ) );
	y += r * 3.0f;

	// Scrub
	{
		UiTree& node = mUiTree.find( NodeId_Scrub )
			.scale( vec2( w, r ) )
			.translate( vec2( 0.0f, y ) );
		mUiTree.find( NodeId_ScrubRect )
			.scale( vec2( node.getScale() ) + vec2( 0.0f, -1.0f ) );
		if ( mIdTextureMap.find( NodeId_ScrubImage ) != mIdTextureMap.end() ) {
			const vec2 sz( mIdTextureMap.at( NodeId_ScrubImage )->getSize() );
			mUiTree.find( NodeId_ScrubImage )
				.scale( sz )
				.translate( vec2( 0.0f, ( r - sz.y ) * 0.5f ) );
		}
	}
	y += r;

	// Rewind
	float x = 0.0f;
	{
		UiTree& node = mUiTree.find( NodeId_Rewind )
			.scale( vec2( c * 2.0f, r ) )
			.translate( vec2( x, y ) );
		mUiTree.find( NodeId_RewindRect )
			.scale( vec2( node.getScale() ) + vec2( -1.0f, 0.0f ) );
		if ( mIdTextureMap.find( NodeId_RewindImage ) != mIdTextureMap.end() ) {
			const vec2 sz( mIdTextureMap.at( NodeId_RewindImage )->getSize() );
			mUiTree.find( NodeId_RewindImage )
				.scale( sz )
				.translate( ( vec2( node.getScale() ) - sz ) * 0.5f );
		}
		x += node.getScale().x;
	}

	// Play/pause
	{
		UiTree& node = mUiTree.find( NodeId_Play )
			.scale( vec2( c * 2.0f, r ) )
			.translate( vec2( x, y ) );
		mUiTree.find( NodeId_PlayRect )
			.scale( vec2( node.getScale() ) + vec2( -1.0f, 0.0f ) );
		for ( const NodeId id : { NodeId_PauseImage, NodeId_PlayImage } ) {
			if ( mIdTextureMap.find( id ) != mIdTextureMap.end() ) {
				const vec2 sz( mIdTextureMap.at( id )->getSize() );
				mUiTree.find( id )
					.scale( sz )
					.translate( ( vec2( node.getScale() ) - sz ) * 0.5f );
			}
		}
		x += node.getScale().x;
	}

	// Forward
	{
		UiTree& node = mUiTree.find( NodeId_Forward )
			.scale( vec2( c * 2.0f, r ) )
			.translate( vec2( x, y ) );
		mUiTree.find( NodeId_ForwardRect )
			.scale( vec2( node.getScale() ) );
		if ( mIdTextureMap.find( NodeId_ForwardImage ) != mIdTextureMap.end() ) {
			const vec2 sz( mIdTextureMap.at( NodeId_ForwardImage )->getSize() );
			mUiTree.find( NodeId_ForwardImage )
				.scale( sz )
				.translate( ( vec2( node.getScale() ) - sz ) * 0.5f );
		}
	}
}

void AudioPlayerApp::showTree( UiTree& node )
{
	for ( auto& iter : node.show().getChildren() ) {
		showTree( iter.second );
	}
}

void AudioPlayerApp::update()
{
	if ( mButtonClose.isPressed() ) {
		quit();
		return;
	}

	float speed = mSpeed;
	if ( mButtonPlay.isPressed() ) {
		mSpeed = 1.0f;
	} else {
		mSpeed = 0.0f;
	}

	if ( mButtonRewind.isPressed() ) {
		mSpeed = -2.0f;
	}

	if ( mButtonForward.isPressed() ) {
		mSpeed = 2.0f;
	}

	if ( speed != mSpeed ) {
		mUiTree.find( NodeId_PauseImage ).setVisible( mSpeed != 0.0f );
		mUiTree.find( NodeId_PlayImage ).setVisible( mSpeed == 0.0f );

		if ( mSpeed == 0.0f ) {
			mBufferPlayerNode->disable();
		} else if ( speed == 0.0f ) {
			mBufferPlayerNode->enable();
		}
	}
}

CINDER_APP( AudioPlayerApp, RendererGl(), []( App::Settings* settings )
{
	settings->setBorderless();
	settings->setWindowSize( 1600, 900 );
} )
 
	

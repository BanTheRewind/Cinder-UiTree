#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/audio/audio.h"
#include "cinder/gl/gl.h"

#include "Button.h"
#include "Resources.h"
#include "Slider.h"

// Short hand for mapping node IDs to cached textures.
typedef std::map<NodeId, ci::gl::Texture2dRef> IdTextureMap;


class AudioPlayerApp : public ci::app::App
{
public:
	AudioPlayerApp();
	
	void							draw() override;
	void							resize() override;
	void							update() override;
private:
	// The UI tree.
	UiTree							mUiTree;

	// Recursive methods for walking the tree.
	void							drawTree( const UiTree& node );
	void							showTree( UiTree& node );

	// These controls are UiTree::EventHandler implementations.
	Button							mButtonForward;
	Button							mButtonPlay;
	Button							mButtonRewind;
	Slider							mSlider;

	// Caches textures associated with nodes.
	IdTextureMap					mIdTextureMap;

	// Audio playback.
	ci::audio::BufferPlayerNodeRef	mBufferPlayerNode;
	float							mSpeed;

	// Audio visualization.
	ci::gl::BatchRef				mBatchWaveform;
	std::vector<ci::vec2>			mWaveform;
};

using namespace ci;
using namespace ci::app;
using namespace std;

AudioPlayerApp::AudioPlayerApp()
{
	/*
	 * Set the playback speed to zero to indicate we need 
	 * to pause (disable the buffer player node).
	 */
	mSpeed = 0.0f;

	// Create the tree structure and connect event handlers.
	mUiTree.createChild( NodeId_Waveform );
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
		.biased( false ) // Keep button on after release.
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

	// Set color for images and background rectangles.
	ColorAf colorImage	= ColorAf::black();
	ColorAf colorRect	= ColorAf::gray( 0.87f );
	for ( const NodeId id : { 
		NodeId_ForwardImage, NodeId_PauseImage, NodeId_PlayImage, 
		NodeId_RewindImage, NodeId_ScrubImage
		} ) {
		mUiTree.find( id ).data( colorImage );
	}
	for ( const NodeId id : { 
		NodeId_ForwardRect, NodeId_PlayRect, 
		NodeId_RewindRect, NodeId_ScrubRect
		} ) {
		mUiTree.find( id ).data( colorRect );
	}

	// Show all nodes, then hide the pause button.
	showTree( mUiTree );
	mUiTree.find( NodeId_PauseImage )
		.hide();

	// Load textures for image nodes.
	auto loadTexture = [ & ]( const DataSourceRef& dataSource ) -> gl::Texture2dRef
	{
		return gl::Texture2d::create( loadImage( dataSource ) );
	};
	mIdTextureMap[ NodeId_ForwardImage ]	= loadTexture( loadResource( RES_PNG_FORWARD ) );
	mIdTextureMap[ NodeId_PauseImage ]		= loadTexture( loadResource( RES_PNG_PAUSE ) );
	mIdTextureMap[ NodeId_PlayImage ]		= loadTexture( loadResource( RES_PNG_PLAY ) );
	mIdTextureMap[ NodeId_RewindImage ]		= loadTexture( loadResource( RES_PNG_REWIND ) );
	mIdTextureMap[ NodeId_ScrubImage ]		= loadTexture( loadResource( RES_PNG_SCRUB ) );

	// Load audio.
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

	// Some OpenGL settings
	gl::enableAlphaBlending();
	gl::enableVerticalSync();

	// Call resize to do layout
	resize();
}

void AudioPlayerApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.7f ) );

	// Recursively draw the tree starting with the root.	
	drawTree( mUiTree );
}

void AudioPlayerApp::drawTree( const UiTree& node )
{
	// Only draw nodes which do not have children.
	if ( node.isVisible() && node.getChildren().empty() ) {
		NodeId id = (NodeId)node.getId();

		// Draw the waveform batch if we're on the waveform node.
		if ( id == NodeId_Waveform && mBatchWaveform ) {
			mBatchWaveform->draw();
		} else {

			// Get this node's bounds and color.
			Rectf rect( vec2( 0.0f ), vec2( node.getScale() ) );
			ColorAf c = node.getData();
			if ( 
				( id == NodeId_ForwardRect	&& mButtonForward.isPressed() ) || 
				( id == NodeId_RewindRect	&& mButtonRewind.isPressed() ) || 
				( id == NodeId_PlayRect		&& mButtonPlay.isPressed() ) || 
				( id == NodeId_ScrubRect	&& mSlider.isDragging() )
					) {
				c = ColorAf::gray( 0.9f );
			}
			const gl::ScopedColor scopedColor( c );

			// If we cached a texture for this node, draw it.
			if ( mIdTextureMap.find( id ) != mIdTextureMap.end() ) {
				const gl::Texture2dRef& texture = mIdTextureMap.at( id );
				if ( id == NodeId_ScrubImage ) {

					// Move the scrub handle using the slider's normalized position.
					const float w	= (float)texture->getWidth();
					const float x	= mSlider.getPosition() * (float)getWindowWidth() - w * 0.5f;
					rect.x1			+= x;
					rect.x2			+= x;
				}
				gl::draw( texture, texture->getBounds(), rect );
			} else {

				// Just draw a rectangle if there is no image.
				gl::drawSolidRect( rect );
			}
		}
	} else {

		/*
		 * For node's with children, recursively call this method.
		 * Note that we're using a scoped model matrix and then 
		 * translating to this node's position. This draws everything 
		 * in screen space using transform data relative to each node's
		 * parent.
		 */
		for ( const auto& iter : node.getChildren() ) {
			const gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( iter.second.getTranslate() );
			drawTree( iter.second );
		}
	}
}

void AudioPlayerApp::resize()
{
	// Calculate values for a grid layout.
	float h = (float)getWindowHeight();
	float w = (float)getWindowWidth();
	float c = w / 3.0f;
	float r = h / 5.0f;

	// Columna and row position.
	float x = 0.0f;
	float y = 0.0f;

	{ /* 
	   * Create the waveform. Use the waveform node's transform
	   * data to record points in screen space. We're only going 
	   * to read every 3,000th sample from the left channel to 
	   * keep the mesh simple. Because we're drawing with GL_LINES, 
	   * each point, except the first and last, is recorded twice 
	   * in a row.
	   */
		UiTree& node = mUiTree.find( NodeId_Waveform )
			.scale( vec2( w, r * 3.0f ) + vec2( 0.0f, -1.0f ) );
		if ( mBufferPlayerNode ) {
			const audio::BufferRef& buffer	= mBufferPlayerNode->getBuffer();
			const float* samples			= buffer->getChannel( 0 );
			static const size_t d			= 3000;

			const vec2 s( node.getScale() );
			const vec2 t( node.getTranslate() );
			float x			= t.x;
			const float w	= s.x / (float)( buffer->getNumFrames() / d );
			for ( size_t i = 0; i < buffer->getNumFrames(); i += d ) {
				x			+= w;
				vec2 v( x, ( 1 - ( samples[ i ] * 0.5f + 0.5f ) ) * s.y + t.y );
				mWaveform.push_back( v );
				if ( mWaveform.size() > 1 ) {
					mWaveform.push_back( v );
				}
			}
			mWaveform.pop_back();
		}
		y += node.getScale().y;
	}


	{ // Layout the scrub bar.
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

	{ // Rewind button.
		UiTree& node = mUiTree.find( NodeId_Rewind )
			.scale( vec2( c, r ) )
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

	{ // Play/pause button.
		UiTree& node = mUiTree.find( NodeId_Play )
			.scale( vec2( c, r ) )
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

	{ // Forward button.
		UiTree& node = mUiTree.find( NodeId_Forward )
			.scale( vec2( c, r ) )
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
	/*
	 * Recursively shows all nodes in a tree. In order to 
	 * preserve the visibility state of children, calling 
	 * show on a node does not walk its tree. But as you can 
	 * see by this tiny method, it's pretty easy to roll 
	 * your own.
	 */
	for ( auto& iter : node.show().getChildren() ) {
		showTree( iter.second );
	}
}

void AudioPlayerApp::update()
{
	// Values used in seeking.
	static const double s	= 0.2;
	const double l			= mBufferPlayerNode->getNumSeconds();
	const double e			= mBufferPlayerNode->getReadPositionTime();

	// Update the scrub bar if we're not interacting with it.
	if ( !mSlider.isDragging() && l > 0.0 ) {
		mSlider.setPosition( (float)( e / l ) );
	}

	// Set the speed using the playhead buttons.
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

	/*
	 * Swap the pause / play image visibility according to 
	 * any change in the playback speed.
	 */
	if ( speed != mSpeed ) {
		mUiTree.find( NodeId_PauseImage ).setVisible( mSpeed != 0.0f );
		mUiTree.find( NodeId_PlayImage ).setVisible( mSpeed == 0.0f );
		if ( mSpeed == 0.0f ) {
			mBufferPlayerNode->disable();
		} else if ( speed == 0.0f ) {
			mBufferPlayerNode->enable();
		}
	}

	// Perform seeking operations.
	auto ctx = audio::Context::master();
	if ( ctx ) {
		if ( mSlider.isDragging() ) {
			mBufferPlayerNode->seekToTime( (double)mSlider.getPosition() * l );
		} else if ( mSpeed < 0.0f ) {
			mBufferPlayerNode->seekToTime( max( e - s, 0.0 ) );
		} else if ( mSpeed > 1.0f ) {
			mBufferPlayerNode->seekToTime( min( e + s, l ) );
		}
	}

	/* 
	 * Walk through the waveform and set line colors to 
	 * show progress. Create or update the batch.
	 */
	if ( mBufferPlayerNode && !mWaveform.empty() ) {
		const float x = mSlider.getPosition() * mUiTree.find( NodeId_Waveform ).getScale().x; 
		gl::VertBatch vb( GL_LINES );
		for ( const vec2& v : mWaveform ) {
			Colorf c = v.x > x ? Colorf::gray( 0.5f ) : Colorf( 0.9f, 0.3f, 0.8f );
			vb.color( c );
			vb.vertex( v );
		}
		if ( !mBatchWaveform ) {
			mBatchWaveform = gl::Batch::create( vb, gl::getStockShader( gl::ShaderDef().color() ) );
		} else {
			mBatchWaveform->replaceVboMesh( gl::VboMesh::create( vb ) );
		}
	}
}

CINDER_APP( AudioPlayerApp, RendererGl(), []( App::Settings* settings )
{
	settings->setWindowSize( 540, 960 );
} )
 
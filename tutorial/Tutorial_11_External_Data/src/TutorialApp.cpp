/*
 * TUTORIAL 11 - EXTERNAL DATA
 * 
 * As a general rule, the data attached to your UiTree 
 * should be very lightweight. If a UiTree's data has members
 * which run threads or allocate memory, it is better to keep
 * them decoupled from nodes. This improves stability and 
 * performance. There are several strategies for linking nodes 
 * to external data, as you'll see in the sample applications, 
 * but this tutorial will focus on just one. 
 * 
 * This tutorial is essentially the same as #6 (hierarchy), 
 * only instead of procedurally drawing circles to represent 
 * nodes, we will generate a unique batch (gl::BatchRef) for 
 * each node and assign it a color. Color is the kind of data 
 * you can keep in a UiTree without issue, so we'll keep it there.
 * While you can technically store a batch there, it is better 
 * to keep this more sophisticated data elsewhere. So we create 
 * a map where the key is the ID of the node and the value is 
 * the batch.
 */

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "UiTree.h"

// typedef a map to hold the node ID and a batch.
typedef std::map<uint64_t, ci::gl::BatchRef>	IdBatchMap;

// Node's will keep track of their color.
typedef UiTreeT<ci::Colorf>						UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();

	void				draw() override;
private:
	void				configTree( UiTree& node, float radius, float distance );
	void				createTree( UiTree& parent, size_t count, size_t depth, size_t level = 0 );
	void				drawTree( const UiTree& node );

	// Methods to generate random data.
	ci::gl::BatchRef	randBatch();
	ci::Colorf			randColor();

	// The external data map.
	IdBatchMap			mIdBatchMap;

	UiTree				mUiTree;
};

#include "cinder/app/RendererGl.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

TutorialApp::TutorialApp()
{
	createTree( mUiTree, 3, 4 );
	
	mUiTree
		.data( randColor() )
#if defined( CINDER_COCOA_TOUCH )
		.scale( vec2( 80.0f ) )
		.translate( getWindowCenter() + vec2( 57.0f, 0.0f ) );
#else
		.scale( vec2( 150.0f ) )
		.translate( getWindowCenter() );
#endif

	/*
	 * Create a randomly shaped batch for the UiTree root 
	 * and store it in the map.
	 */
	mIdBatchMap[ 0 ] = randBatch();

#if defined( CINDER_COCOA_TOUCH )
	configTree( mUiTree, mUiTree.getScale().x, 150.0f );
#else
	configTree( mUiTree, mUiTree.getScale().x, 270.0f );
#endif
}

void TutorialApp::configTree( UiTree& node, float radius, float distance )
{
	if ( node.getChildren().empty() ) {
		return;
	}
	
	static const float decay	= 0.4f;
	radius						*= decay;

	const float d	= ( (float)M_PI * 2.0f ) / (float)node.getChildren().size();
	float t			= d * 0.5f;
	for ( auto& iter : node.getChildren() ) {
		t		+= d;
		vec2 p	= vec2( math<float>::cos( t ), math<float>::sin( t ) ) * distance;
		iter.second
			.data( randColor() )
			.scale( vec2( radius ) )
			.translate( p );
		configTree( iter.second, radius, distance * decay );
	}
}

void TutorialApp::createTree( UiTree& parent, size_t count, size_t depth, size_t level )
{
	if ( level < depth ) {
		for ( size_t i = 0; i < count; ++i ) {
			UiTree& node = parent.createAndReturnChild();

			/*
			 * Create a randomly shaped batch for the node 
			 * and store it in the map, using the node's ID
			 * as a key.
			 */
			mIdBatchMap[ node.getId() ] = randBatch();
			createTree( node, count, depth, level + 1 );
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
	const vec2 p = vec2( node.calcAbsoluteTranslate() );
	
	/*
	 * The node's ID acts a the key for our external data map.
	 * Perform a check to make sure it is there.
	 */
	if ( mIdBatchMap.find( node.getId() ) != mIdBatchMap.end() ) {

		// Reference the batch in the external data.
		const gl::BatchRef& batch = mIdBatchMap.at( node.getId() );
		if ( batch != nullptr ) {
			const gl::ScopedMatrices scopedMatrices;

			/* 
			 * Use the UiTree's color and transform data to render 
			 * the batch.
			 */
			const gl::ScopedColor scopedColor( node.getData() );
			gl::translate( p );
			gl::scale( node.getScale() );
			batch->draw();
		}
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

gl::BatchRef TutorialApp::randBatch()
{
	// Simple color shader.
	static const gl::GlslProgRef glsl = gl::getStockShader( gl::ShaderDef().color() );

	// Create a solid or stroked batch with a random number of sides.
	int32_t subdivisions = randInt( 3, 7 );
	if ( randBool() ) {
		return gl::Batch::create( geom::Circle().subdivisions( subdivisions ), glsl );
	}
	return gl::Batch::create( geom::Ring().width( 0.1f ).subdivisions( subdivisions ), glsl );
}

Colorf TutorialApp::randColor()
{
	return Colorf( randFloat(), randFloat(), randFloat() );
}

CINDER_APP( TutorialApp, RendererGl( RendererGl::Options().msaa( 16 ) ), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
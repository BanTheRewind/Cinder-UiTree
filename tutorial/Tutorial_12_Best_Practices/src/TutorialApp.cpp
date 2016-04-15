/*
 * TUTORIAL 12 - BEST PRACTICES
 * 
 * This final tutorial is a combination of the previous 
 * tutorials. The key difference is that the code is a bit 
 * to what you would want to do in production. Read through 
 * the comments in all of this project's code files to pick 
 * up best practices.
 */

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "EventHandler.h"

typedef std::map<uint64_t, ci::gl::BatchRef> IdBatchMap;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();

	void				draw() override;
	void				update() override;
private:
	void				drawTree( const UiTree& node );
	void				makeNode();
	ci::gl::BatchRef	randBatch();
	ci::Colorf			randColor();

	EventHandler		mEventHandler;
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
	mUiTree
		.enable()
#if defined( CINDER_COCOA_TOUCH )
		.scale( vec2( 50.0f ) )
#else
		.scale( vec2( 90.0f ) )
#endif
		.translate( getWindowCenter() );

	/*
	 * It's best to keep simple data in your UiTreeT<T>
	 * class or structure. Anything that allocates memory or 
	 * runs threads is best to keep in an external map. 
	 * Use the node's ID as a key for quick lookup. In this 
	 * example, we keep the node's color attached to the UiTree, 
	 * but its VboMeshRef is stored in a separate map. 
	 * Technically, you could keep this in your template class, 
	 * but it's advisable not to do so.
	 */
	mUiTree.getData().setColor( randColor() );
	mIdBatchMap[ 0 ] = randBatch();

	/*
	 * NOTE: Another strategy is to store IDs in your template 
	 * structure. For example, if you have four sounds that are 
	 * re-usable among a two hundred UiTree instances. Instead 
	 * of storing two hundred copies of the same sounds, make a 
	 * list of the four sounds and store the index of the sound 
	 * in your template structure.
	 *
	 * mySoundMap[ 0 ] = mySound0;
	 * mySoundMap[ 1 ] = mySound1;
	 * ...
	 * myUiTree.getData().audioId( 0 ); // This instance plays sound 0
	 */

	for ( uint8_t i = 0; i < 10; ++i ) {
		makeNode();
	}
}

void TutorialApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );

	/*
	 * Never draw from inside your UiTree. Always write 
	 * a decoupled renderer. This will allow you to easily 
	 * target different graphics environments and write 
	 * a debug renderer.
	 */
	drawTree( mUiTree );
}

void TutorialApp::drawTree( const UiTree& node )
{
	const vec2 p = vec2( node.getTranslate() );
	if ( mIdBatchMap.find( node.getId() ) != mIdBatchMap.end() ) {
		const gl::BatchRef& batch = mIdBatchMap.at( node.getId() );
		if ( batch != nullptr ) {
			const gl::ScopedMatrices scopedMatrices;
			const gl::ScopedColor scopedColor( node.getData().getColor() );
			gl::translate( p );
			gl::scale( node.getScale() );
			batch->draw();
		}
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
#if defined( CINDER_COCOA_TOUCH )
	const float dMax	= 220.0f;
	const float d		= randFloat( 28.0f, dMax );
	float r				= 11.0f;
#else
	const float dMax	= 400.0f;
	const float d		= randFloat( 50.0f, dMax );
	float r				= 20.0f;
#endif
	r					+= ( 1.0f - ( d / dMax ) ) * r * 2.0f;

	UiTree& node = mUiTree.createAndReturnChild()
		.collisionType( UiTree::CollisionType_Circle )
		.data( UiData()
			.color( randColor() )
			.distance( d )
			.speed( randFloat( -1.0f, 1.0f ) )
			)
		.enable()
		.scale( vec2( r ) );

	/*
	 * To improve performance and stability of your app
	 */
	mIdBatchMap[ node.getId() ] = randBatch();

	/*
	 * While inline anonymous functions are useful for 
	 * quick sketching, it's better to use an event handler 
	 * instance for callbacks. It routes everything back into
	 * fewer places and helps organize your code.
	 */
	mEventHandler.connect( node );
}

gl::BatchRef TutorialApp::randBatch()
{
	static const gl::GlslProgRef glsl = gl::getStockShader( gl::ShaderDef().color() );

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

void TutorialApp::update()
{
	/*
	 * An event handler is a good place to add functionality 
	 * to your UiTree. Think of it as a many application for 
	 * your node. In this case, we've added some logic to the 
	 * event handler to record the ID of any node that needs 
	 * to be removed.
	 */
	uint64_t removeId = mEventHandler.getRemoveId();

	/*
	 * When removing a node, be sure to also remove any 
	 * external data it is using to save on resources.
	 */
	if ( removeId > 0 && mUiTree.removeChild( removeId ) ) {
		if ( mIdBatchMap.find( removeId ) != mIdBatchMap.end() ) {
			mIdBatchMap.erase( removeId );
		}
		makeNode();
	}
	mEventHandler.setRemoveId( 0 );

	const float e = (float)getElapsedSeconds();

	/*
	 * If you ever need a flat list of all nodes in your UiTree, 
	 * run a query which returns true.
	 */ 
	list<UiTree*> nodes = mUiTree.query( []( const UiTree& node )
	{
		return true;
	} );

	const vec2 c = getWindowCenter();

	/*
	 * Although there is an update event in the UiTree, it is 
	 * best to keep the quantity of update signals to a minimum.
	 * If you want to organize your code a bit, consider writing 
	 * "controller" nodes. That is, create a node's event handler 
	 * with some more sophisticated callbacks. If you have hundreds 
	 * or thousands of nodes that all have update or mouse events, 
	 * for example, you might get better performance by hit testing 
	 * them all from a single node instead of giving them all 
	 * their own event handlers. In this example, we update the tree 
	 * from the main application's update loop.
	 */
	for ( UiTree* a : nodes ) {
		if ( a->getParent() != nullptr ) {
			UiData& data	= a->getData();
			const float s	= data.getSpeed();
			a->setTranslate( c + vec2( cos( e * s ), sin( e * s ) ) * data.getDistance() );

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
 
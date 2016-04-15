/*
 * TUTORIAL 4 - ANIMATION
 *
 * In the third tutorial, we touched on two different types 
 * of animationode. In one instance, we know what our target is
 * and how fast we want to get there. In the other, we want to 
 * move a value in a direction and see where it ends up. In this 
 * tutorial, you'll learn how to control both types of animation
 * for three types of transform: translate, rotate, and scale.
 * We'll also create a custom structure to hold more complex 
 * data in our UiTree.
 */

#include "cinder/app/App.h"
#include "UiTree.h"

/*
 * Each UiTree will animate one type of transform: translate, 
 * rotate, or scale. Let's define an enumerator to indicate 
 * which one we're animating.
 */
enum : size_t
{
	Transform_None, 
	Transform_Rotate, 
	Transform_Scale, 
	Transform_Translate, 
	Transform_Count
} typedef Transform;

/*
 * In previous tutorials, we only needed to keep track of a color 
 * or vec2 in our UiTree. In this example, we want to keep not 
 * only a color, but the type of transform we're animating and 
 * whether or not we're using velocity to animate. To store all 
 * that information, we need to create a custom data structure.
 */
struct UiData
{
	ci::Colorf	mColor;
	Transform	mTransform;
	bool		mUseVelocity;
};

// Use our UiData structure as the template argument for UiTreeT<T>.
typedef UiTreeT<UiData> UiTree;

class TutorialApp : public ci::app::App
{
public:
	TutorialApp();
	
	void		draw() override;
	void		update() override;
private:
	UiTree		mUiTree;

	// Original size of node
	float		mSize;

	// Calculates animation area for a UiTree
	ci::Rectf	calcBounds( const UiTree& node );
};

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

TutorialApp::TutorialApp()
{
	// Set square size
	mSize = 100.0f;
	const vec2 sz( mSize );

	// We are going to create two UiTrees for each type of transform:
	// one which uses velocity and one that does not.
	for ( size_t i = Transform_None + 1; i < Transform_Count; ++i ) {
		for ( size_t j = 0; j < 2; ++j ) {
			
			// Define the data structure.
			UiData data;
			data.mColor			= Colorf( randFloat(), randFloat(), randFloat() );
			data.mTransform		= (Transform)i;
			data.mUseVelocity	= j == 1;


			// Create the node and set its data.
			UiTree& node = mUiTree.createAndReturnChild()
				.data( data )
				.scale( sz );
			const Rectf bounds	= calcBounds( node );
			const vec2 p		= bounds.getCenter() - sz * 0.5f;
			node.translate( p );
		}
	}
}

Rectf TutorialApp::calcBounds( const UiTree& node )
{
#if defined( CINDER_COCOA_TOUCH )
	const vec2 grid( vec2( getWindowSize() ) / vec2( 2.0f, 3.0f ) );
	float column	= node.getData().mUseVelocity ? 1.0f : 0.0f;
	vec2 p0			= vec2( column, (float)( node.getData().mTransform - 1.0f ) ) * grid;
#else
	const vec2 grid( vec2( getWindowSize() ) / vec2( 3.0f, 2.0f ) );
	float row		= node.getData().mUseVelocity ? 1.0f : 0.0f;
	vec2 p0			= vec2( (float)( node.getData().mTransform - 1.0f ), row ) * grid;
#endif
	const vec2 p1	= p0 + grid;
	Rectf bounds( p0, p1 );
	return bounds;
}

void TutorialApp::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );
	
	for ( const auto& iter : mUiTree.getChildren() ) {
		const UiTree& node = iter.second;
		
		const gl::ScopedColor scopedColor( node.getData().mColor );
		const gl::ScopedModelMatrix scopedModelMatrix;
		gl::translate( node.getTranslate() );
		gl::rotate( node.getRotation().z * glm::two_pi<float>() );
		gl::scale( node.getScale() );
		gl::drawSolidRect( Rectf( vec2( 0.0f ), vec2( 1.0f ) ) );
	}
}

void TutorialApp::update()
{
	// Animation speed, velocity decay, and epsilon (margin) for all tranform types.
	static const float decay	= 0.96f;
	static const float epsilon	= 0.1f;
	static const float speed	= 0.1f;

	for ( auto& iter : mUiTree.getChildren() ) {
		UiTree& node = iter.second;

		// Get the animation bounds for this node
		Rectf bounds = calcBounds( node );

		// Get the node's transform values
		const float r = node.getRotation().z;
		const vec2& s = vec2( node.getScale() );
		const vec2& t = vec2( node.getTranslate() );

		/* 
			* Switch on this node's animation type, then animate using 
			* either velocity or sequencing.
			*/
		switch ( node.getData().mTransform ) {
		case Transform_Rotate:
			if ( node.getData().mUseVelocity ) {

				// Get the velocity values.
				const float v = node.getRotationVelocity().z;

				/*
					* Accelerate the node's rotation CW or CCW randomly when its velocity 
					* gets too low.
					*/
				if ( math<float>::abs( v ) < 0.001f ) {
					node.rotationVelocity( randFloat( -0.1f, 0.1f ), decay );
				}

			} else {

				// If we're not using velocity, rotate the node CW and CCW.
				if ( math<float>::abs( node.getRotationTarget().z ) - math<float>::abs( r ) < epsilon ) {
					const float z = r > 0.0f ? -1.0f : 1.0f;
					node.setRotation( z, speed );
				}

			}
			break;
		case Transform_Scale:
			if ( node.getData().mUseVelocity ) {

				/*
					* Accelerate the node's scale in a random direction when its velocity 
					* gets too low.
					*/
				if ( glm::length( node.getScaleVelocity() ) <= epsilon ) {
					node.scaleVelocity( randVec2() * randFloat( 5.0f, 20.0f ), decay );
				}

				// Get the new scale value.
				const vec2& scale = vec2( node.getScale() );

				// Keep the node centered
				node.setTranslate( bounds.getCenter() - scale * 0.5f );

				// Clamp the node's scale.
				if ( node.getScale().x > mSize * 2.0f ) {
					node.setScale( vec2( mSize * 2.0f, scale.y ) );
				} else if ( node.getScale().x < mSize * 0.5f ) {
					node.setScale( vec2( mSize * 0.5f, scale.y ) );
				}
				if ( node.getScale().y > mSize * 2.0f ) {
					node.setScale( vec2( scale.x, mSize * 2.0f ) );
				} else if ( node.getScale().y < mSize * 0.5f ) {
					node.setScale( vec2( scale.x, mSize * 0.5f ) );
				}

			} else {

				/*
					* If we're not using velocity, scale the node up and down.
					* Keep the node centered in its bounds.
					*/
				if ( glm::length( s - vec2( node.getScaleTarget() ) ) < epsilon ) {
					vec2 scale( s.x > mSize ? mSize * 0.5f : mSize * 2.0f );
					node
						.scale( scale, speed )
						.translate( bounds.getCenter() - scale * 0.5f, speed );
				}
			}
			break;
		case Transform_Translate:
			if ( node.getData().mUseVelocity ) {

				// Get the velocity values.
				const vec2& v = vec2( node.getTranslateVelocity() );
				const float d = node.getTranslateVelocityDecay();

				/*
					* Accelerate the node in a random direction when its velocity 
					* gets too low.
					*/
				if ( glm::length( node.getTranslateVelocity() ) <= epsilon ) {
					node.translateVelocity( randVec2() * randFloat( 5.0f, 20.0f ), decay );
				}

				// Bounce off of bounding box edges.
				if ( t.x <= bounds.getX1() ) {
					node
						.translate( vec2( bounds.getX1() + epsilon, t.y ) )
						.translateVelocity( v * vec2( -1.0f, 1.0f ), d );
				} else if ( t.x + s.x >= bounds.getX2() ) {
					node
						.translate( vec2( ( bounds.getX2() - s.x ) - epsilon, t.y ) )
						.translateVelocity( v * vec2( -1.0f, 1.0f ), d );
				}
				if ( t.y <= bounds.getY1() ) {
					node
						.translate( vec2( t.x, bounds.getY1() + epsilon ) )
						.translateVelocity( v * vec2( 1.0f, -1.0f ), d );
				} else if ( t.y + s.y >= bounds.getY2() ) {
					node
						.translate( vec2( t.x, ( bounds.getY2() - s.y ) - epsilon ) )
						.translateVelocity( v * vec2( 1.0f, -1.0f ), d );
				}
			} else {

				/*
					* If we're not using velocity, animate left to right as the 
					* target position is reached.
					*/
				if ( glm::length( t - vec2( node.getTranslateTarget() ) ) < epsilon ) {
					float x = t.x > bounds.getX1() + epsilon ? bounds.getX1() : bounds.getX2() - s.x;
					node.setTranslate( vec2( x, t.y ), speed );
				}
			}
			break;
		default:
			break;
		}
	}

	mUiTree.update();
}

CINDER_APP( TutorialApp, RendererGl( RendererGl::Options().msaa( 16 ) ), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
#include "cinder/app/App.h"
#include "UiTree.h"

typedef UiTreeT<ci::Colorf> UiTree;

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

class _TBOX_PREFIX_App : public ci::app::App
{
public:
	_TBOX_PREFIX_App();
	
	void	draw() override;
private:
	UiTree	mUiTree;
};

using namespace ci;
using namespace ci::app;
using namespace std;

_TBOX_PREFIX_App::_TBOX_PREFIX_App()
{
	static const uint8_t grid = 3;
	vec2 sz( getWindowSize() / 3 );
	for ( uint8_t y = 0; y < grid; ++y ) {
		for ( uint8_t x = 0; x < grid; ++x ) {
			mUiTree.createAndReturnChild()
				.data( Colorf( randFloat(), randFloat(), randFloat() ) )
				.scale( sz * 0.5f )
				.translate( sz * vec2( x, y ) + sz * 0.25f );
		}
	}
}

void _TBOX_PREFIX_App::draw()
{
	gl::viewport( toPixels( getWindowSize() ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Colorf::gray( 0.8f ) );
	
	for ( const auto& iter : mUiTree.getChildren() ) {
		const UiTree& node = iter.second;
		const gl::ScopedColor scopedColor( node.getData() );
		const vec2& p0 = vec2( node.getTranslate() );
		const vec2& p1 = p0 + vec2( node.getScale() );
		gl::drawSolidRect( Rectf( p0, p1 ) );
	}
}

CINDER_APP( _TBOX_PREFIX_App, RendererGl(), []( App::Settings* settings )
{
	settings->setWindowSize( 1600, 900 );
} )
 
	
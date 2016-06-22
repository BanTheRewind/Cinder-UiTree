#pragma once

#include "cinder/app/App.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Quaternion.h"
#include "cinder/Vector.h"

#include "cinder/Log.h"
#include "cinder/Timer.h"

/////////////////////////////////////////////////////////////////////////////////

template<typename T>
class UiTreeT
{
public:
	class EventHandlerInterface
	{
	public:
		inline virtual void disable( UiTreeT<T>* ) {}
		inline virtual void enable( UiTreeT<T>* ) {}
		inline virtual void hide( UiTreeT<T>* ) {}
		inline virtual void keyDown( UiTreeT<T>*, ci::app::KeyEvent& event ) {}
		inline virtual void keyUp( UiTreeT<T>*, ci::app::KeyEvent& event ) {}
		inline virtual void mouseDown( UiTreeT<T>*, ci::app::MouseEvent& event ) {}
		inline virtual void mouseDrag( UiTreeT<T>*, ci::app::MouseEvent& event ) {}
		inline virtual void mouseMove( UiTreeT<T>*, ci::app::MouseEvent& event ) {}
		inline virtual void mouseOut( UiTreeT<T>* ) {}
		inline virtual void mouseOver( UiTreeT<T>* ) {}
		inline virtual void mouseUp( UiTreeT<T>*, ci::app::MouseEvent& event ) {}
		inline virtual void mouseWheel( UiTreeT<T>*, ci::app::MouseEvent& event ) {}
		inline virtual void resize( UiTreeT<T>* ) {}
		inline virtual void show( UiTreeT<T>* ) {}
		inline virtual void touchesBegan( UiTreeT<T>*, ci::app::TouchEvent& event ) {}
		inline virtual void touchesEnded( UiTreeT<T>*, ci::app::TouchEvent& event ) {}
		inline virtual void touchesMoved( UiTreeT<T>*, ci::app::TouchEvent& event ) {}
		inline virtual void touchOut( UiTreeT<T>*, uint32_t ) {}
		inline virtual void touchOver( UiTreeT<T>*, uint32_t ) {}
		inline virtual void update( UiTreeT<T>* ) {}

		inline virtual void	connect( UiTreeT<T>& node )
		{
			node.connectDisableEventHandler(		&EventHandlerInterface::disable,		this );
			node.connectEnableEventHandler(			&EventHandlerInterface::enable,			this );
			node.connectHideEventHandler(			&EventHandlerInterface::hide,			this );
			node.connectKeyDownEventHandler(		&EventHandlerInterface::keyDown,		this );
			node.connectKeyUpEventHandler(			&EventHandlerInterface::keyUp,			this );
			node.connectMouseDownEventHandler(		&EventHandlerInterface::mouseDown,		this );
			node.connectMouseDragEventHandler(		&EventHandlerInterface::mouseDrag,		this );
			node.connectMouseMoveEventHandler(		&EventHandlerInterface::mouseMove,		this );
			node.connectMouseOutEventHandler(		&EventHandlerInterface::mouseOut,		this );
			node.connectMouseOverEventHandler(		&EventHandlerInterface::mouseOver,		this );
			node.connectMouseUpEventHandler(		&EventHandlerInterface::mouseUp,		this );
			node.connectMouseWheelEventHandler(		&EventHandlerInterface::mouseWheel,		this );
			node.connectResizeEventHandler(			&EventHandlerInterface::resize,			this );
			node.connectShowEventHandler(			&EventHandlerInterface::show,			this );
			node.connectTouchesBeganEventHandler(	&EventHandlerInterface::touchesBegan, 	this );
			node.connectTouchesEndedEventHandler(	&EventHandlerInterface::touchesEnded,	this );
			node.connectTouchesMovedEventHandler(	&EventHandlerInterface::touchesMoved,	this );
			node.connectTouchOutEventHandler(		&EventHandlerInterface::touchOut,		this );
			node.connectTouchOverEventHandler(		&EventHandlerInterface::touchOver,		this );
			node.connectUpdateEventHandler(			&EventHandlerInterface::update,			this );
		} 
	};

	enum : uint8_t
	{
		CollisionType_Circle, 
		CollisionType_Cube, 
		CollisionType_Rect, 
		CollisionType_Sphere
	} typedef CollisionType;

	UiTreeT()
	: mCollisionType( CollisionType_Rect ), mEnabled( false ), 
	mEventHandlerDisable( nullptr ), mEventHandlerEnable( nullptr ), 
	mEventHandlerHide( nullptr ), mEventHandlerKeyDown( nullptr ), 
	mEventHandlerKeyUp( nullptr ), mEventHandlerMouseDown( nullptr ), 
	mEventHandlerMouseDrag( nullptr ), mEventHandlerMouseMove( nullptr ), 
	mEventHandlerMouseOut( nullptr ), mEventHandlerMouseOver( nullptr ), 
	mEventHandlerMouseUp( nullptr ), mEventHandlerMouseWheel( nullptr ), 
	mEventHandlerResize( nullptr ), mEventHandlerShow( nullptr ), 
	mEventHandlerTouchesBegan( nullptr ), mEventHandlerTouchesEnded( nullptr ), 
	mEventHandlerTouchesMoved( nullptr ), mEventHandlerTouchOut( nullptr ), 
	mEventHandlerTouchOver( nullptr ), mEventHandlerUpdate( nullptr ), 
	mId( 0 ), mMouseOver( false ), mParent( nullptr ), mRegistration( ci::vec3( 0.0f ) ), 
	mRegistrationSpeed( 0.0f ), mRegistrationTarget( ci::vec3( 0.0f ) ), 
	mRegistrationVelocity( ci::vec3( 0.0f ) ), mRegistrationVelocityDecay( 0.0f ), 
	mRotationSpeed( 1.0f ), mRotationVelocityDecay( 0.0f ), 
	mScale( ci::vec3( 1.0f ) ), mScaleSpeed( 1.0f ), mScaleTarget( ci::vec3( 1.0f ) ), 
	mScaleVelocity( ci::vec3( 0.0f ) ), mScaleVelocityDecay( 0.0f ), 
	mTranslate( ci::vec3( 0.0f ) ), mTranslateSpeed( 1.0f ), 
	mTranslateTarget( ci::vec3( 0.0f ) ), mTranslateVelocity( ci::vec3( 0.0f ) ), 
	mTranslateVelocityDecay( 0.0f ), mVisible( false )
	{
	}

	UiTreeT( const UiTreeT& rhs )
	{
		*this = rhs;
	}

	UiTreeT& operator=( const UiTreeT<T>& rhs )
	{
		mChildren						= rhs.mChildren;
		mCollisionType					= rhs.mCollisionType;
		mConnectionKeyDown				= rhs.mConnectionKeyDown;
		mConnectionKeyUp				= rhs.mConnectionKeyUp;
		mConnectionMouseDown			= rhs.mConnectionMouseDown;
		mConnectionMouseDrag			= rhs.mConnectionMouseDrag;
		mConnectionMouseMove			= rhs.mConnectionMouseMove;
		mConnectionMouseUp				= rhs.mConnectionMouseUp;
		mConnectionMouseWheel			= rhs.mConnectionMouseWheel;
		mConnectionResize				= rhs.mConnectionResize;
		mConnectionTouchesBegan			= rhs.mConnectionTouchesBegan;
		mConnectionTouchesEnded			= rhs.mConnectionTouchesEnded;
		mConnectionTouchesMoved			= rhs.mConnectionTouchesMoved;
		mData							= rhs.mData;
		mEnabled						= rhs.mEnabled;
		mEventHandlerDisable			= rhs.mEventHandlerDisable;
		mEventHandlerEnable				= rhs.mEventHandlerEnable;
		mEventHandlerHide				= rhs.mEventHandlerHide;
		mEventHandlerKeyDown			= rhs.mEventHandlerKeyDown;
		mEventHandlerKeyUp				= rhs.mEventHandlerKeyUp;
		mEventHandlerMouseDown			= rhs.mEventHandlerMouseDown;
		mEventHandlerMouseDrag			= rhs.mEventHandlerMouseDrag;
		mEventHandlerMouseMove			= rhs.mEventHandlerMouseMove;
		mEventHandlerMouseOut			= rhs.mEventHandlerMouseOut;
		mEventHandlerMouseOver			= rhs.mEventHandlerMouseOver;
		mEventHandlerMouseUp			= rhs.mEventHandlerMouseUp;
		mEventHandlerMouseWheel			= rhs.mEventHandlerMouseWheel;
		mEventHandlerResize				= rhs.mEventHandlerResize;
		mEventHandlerShow				= rhs.mEventHandlerShow;
		mEventHandlerTouchesBegan		= rhs.mEventHandlerTouchesBegan;
		mEventHandlerTouchesEnded		= rhs.mEventHandlerTouchesEnded;
		mEventHandlerTouchesMoved		= rhs.mEventHandlerTouchesMoved;
		mEventHandlerTouchOut			= rhs.mEventHandlerTouchOut;
		mEventHandlerTouchOver			= rhs.mEventHandlerTouchOver;
		mEventHandlerUpdate				= rhs.mEventHandlerUpdate;
		mId								= rhs.mId;
		mMouseOver						= rhs.mMouseOver;
		mParent							= rhs.mParent;
		mRegistration					= rhs.mRegistration;
		mRegistrationSpeed				= rhs.mRegistrationSpeed;
		mRegistrationTarget				= rhs.mRegistrationTarget;
		mRegistrationVelocity			= rhs.mRegistrationVelocity;
		mRegistrationVelocityDecay		= rhs.mRegistrationVelocityDecay;
		mRotation						= rhs.mRotation;
		mRotationSpeed					= rhs.mRotationSpeed;
		mRotationTarget					= rhs.mRotationTarget;
		mRotationVelocity				= rhs.mRotationVelocity;
		mRotationVelocityDecay			= rhs.mRotationVelocityDecay;
		mScale							= rhs.mScale;
		mScaleSpeed						= rhs.mScaleSpeed;
		mScaleTarget					= rhs.mScaleTarget;
		mScaleVelocity					= rhs.mScaleVelocity;
		mScaleVelocityDecay				= rhs.mScaleVelocityDecay;
		mTouches						= rhs.mTouches;
		mTranslate						= rhs.mTranslate;
		mTranslateSpeed					= rhs.mTranslateSpeed;
		mTranslateTarget				= rhs.mTranslateTarget;
		mTranslateVelocity				= rhs.mTranslateVelocity;
		mTranslateVelocityDecay			= rhs.mTranslateVelocityDecay;
		mVisible						= rhs.mVisible;

		return *this;
	}

	~UiTreeT()
	{
		setEnabled( false );
	}

	inline uint64_t getNextAvailableId( uint64_t baseId = 0 ) const
	{
		uint64_t id = std::max<uint64_t>( baseId, mId ) + 1;
		for ( const auto& iter : mChildren ) {
			id = std::max<uint64_t>( id, iter.second.getNextAvailableId( id ) );
		}
		return id;
	}

	inline UiTreeT<T>& addChild( const UiTreeT<T>& uiTree )
	{
		return addChild( getRoot()->getNextAvailableId(), uiTree );
	}

	inline UiTreeT<T>& addChild( uint64_t id, const UiTreeT<T>& uiTree )
	{
		if ( getRoot().exists( id ) ) {
			throw ExcDuplicateId( id );
		}
		mChildren[ id ]		= uiTree;
		mChildren[ id ].mId = id;
		mChildren[ id ].parent( this );

		return mChildren.at( id ).parent( this );
	}

	inline UiTreeT<T>& addAndReturnChild( const UiTreeT<T>& uiTree )
	{
		return addAndReturnChild( getRoot().getNextAvailableId(), uiTree );
	}

	inline UiTreeT<T>& addAndReturnChild( uint64_t id, const UiTreeT<T>& uiTree )
	{
		addChild( id, uiTree );
		return mChildren.at( id );
	}

	inline void addChildren( const std::map<uint64_t, UiTreeT<T>>& c )
	{
		for ( auto& iter : c ) {
			addChild( iter.second );
		}
	}

	inline UiTreeT<T>& createChild()
	{
		return createChild( getRoot().getNextAvailableId() );
	}
	
	inline UiTreeT<T>& createChild( uint64_t id )
	{
		if ( getRoot().exists( id ) ) {
			throw ExcDuplicateId( id );
		}
		mChildren[ id ] = UiTreeT().parent( this );
		mChildren[ id ].mId = id;

		return *this;
	}

	inline UiTreeT<T>& createAndReturnChild()
	{
		return createAndReturnChild( getRoot().getNextAvailableId() );
	}
	
	inline UiTreeT<T>& createAndReturnChild( uint64_t id )
	{
		createChild( id );
		return mChildren.at( id );
	}

	inline bool exists( uint64_t id ) const
	{
		if ( mId == id ) {
			return true;
		}
		if ( mChildren.find( id ) != mChildren.end() ) {
			return true;
		}
		for ( const auto& iter : mChildren ) {
			if ( iter.second.exists( id ) ) {
				return true;
			}
		}
		return false;
	}

	inline UiTreeT<T>& find( uint64_t id )
	{
		if ( mId == id ) {
			return *this;
		}
		if ( !mChildren.empty() && mChildren.find( id ) != mChildren.end() ) {
			return mChildren.at( id );
		}
		for ( auto& iter : mChildren ) {
			if ( iter.second.exists( id ) ) {
				return iter.second.find( id );
			}
		}
		throw ExcIdNotFound( id );
	}

	inline const UiTreeT<T>& find( uint64_t id ) const
	{
		if ( mChildren.find( id ) != mChildren.end() ) {
			return mChildren.at( id );
		}
		for ( const auto& iter : mChildren ) {
			if ( iter.second.exists( id ) ) {
				return iter.second.find( id );
			}
		}
		throw ExcIdNotFound( id );
	}

	/* USAGE
	typedef UiTreeT<UiData> UiTree;
	...
	for ( UiTree* iter : node.query( [ & ]( const UiTree& uiTree ) -> bool
	{
		if ( uiTree.getTranslate().y < 500.0f ) {
			return true;
		}
		return false;
	} ) ) {
		CI_LOG_V( iter->getTranslate().y );
	}
	*/
	inline std::list<UiTreeT<T>*> query( const std::function<bool( const UiTreeT<T>& )>& func )
	{
		std::list<UiTreeT<T>*> l;
		if ( func( *this ) ) {
			l.push_back( this );
		}
		for ( auto& iter : mChildren ) {
			l.splice( l.end(), iter.second.query( func ) );
		}
		return l;
	}

	inline std::list<const UiTreeT<T>*> query( const std::function<bool( const UiTreeT<T>& )>& func ) const 
	{
		std::list<UiTreeT<T>*> l;
		for ( const auto& iter : mChildren ) {
			if ( func( iter.second ) ) {
				l.push_back( &iter.second );
			}
			l.splice( l.end(), iter.second.query( func ) );
		}
		return l;
	}

	inline bool removeChild( uint64_t id ) 
	{
		if ( mChildren.find( id ) != mChildren.end() ) {
			mChildren.erase( id );
			return true;
		}
		for ( auto& iter : mChildren ) {
			if ( iter.second.removeChild( id ) ) {
				return true;
			}
		}
		return false;
	}

	inline UiTreeT<T>& children( const std::map<uint64_t, UiTreeT<T>>& c )
	{
		setChildren( c );
		return *this;
	}

	inline UiTreeT<T>& collisionType( CollisionType t )
	{
		setCollisionType( t );
		return *this;
	}

	inline UiTreeT<T>& data( const T& d )
	{
		setData( d );
		return *this;
	}

	inline UiTreeT<T>& disable()
	{
		setEnabled( false );
		return *this;
	}

	inline UiTreeT<T>& enable( bool enabled = true )
	{
		setEnabled( enabled );
		return *this;
	}

	inline UiTreeT<T>& hide()
	{
		setVisible( false );
		return *this;
	}

	inline UiTreeT<T>& parent( UiTreeT<T>* uiTree )
	{
		setParent( uiTree );
		return *this;
	}

	inline UiTreeT<T>& visible( bool isVisible = true )
	{
		setVisible( isVisible );
		return *this;
	}

	inline UiTreeT<T>& show()
	{
		setVisible( true );
		return *this;
	}

	inline ci::mat4 calcModelMatrix() const
	{
		ci::mat4 m( 1.0f );
		m = glm::translate( m, mTranslate - mRegistration );
		m *= glm::toMat4( mRotation );
		m = glm::scale( m, mScale );
		return m;
	}

	inline UiTreeT<T>& registration( const ci::vec2& v, float speed = 1.0f )
	{
		setRegistration( v, speed );
		return *this;
	}

	inline UiTreeT<T>& registration( const ci::vec3& v, float speed = 1.0f )
	{
		setRegistration( v, speed );
		return *this;
	}

	inline UiTreeT<T>& registrationVelocity( const ci::vec2& v, float decay = 1.0f )
	{
		setRegistrationVelocity( v, decay );
		return *this;
	}

	inline UiTreeT<T>& registrationVelocity( const ci::vec3& v, float decay = 1.0f )
	{
		setRegistrationVelocity( v, decay );
		return *this;
	}

	inline UiTreeT<T>& rotate( float z, float speed = 1.0f )
	{
		setRotation( z, speed );
		return *this;
	}

	inline UiTreeT<T>& rotate( const ci::quat& q, float speed = 1.0f )
	{
		setRotation( q, speed );
		return *this;
	}

	inline UiTreeT<T>& rotationVelocity( float z, float decay = 1.0f )
	{
		setRotationVelocity( z, decay );
		return *this;
	}

	inline UiTreeT<T>& rotationVelocity( const ci::quat& q, float decay = 1.0f )
	{
		setRotationVelocity( q, decay );
		return *this;
	}

	inline UiTreeT<T>& scale( const ci::vec2& v, float speed = 1.0f )
	{
		setScale( v, speed );
		return *this;
	}

	inline UiTreeT<T>& scale( const ci::vec3& v, float speed = 1.0f )
	{
		setScale( v, speed );
		return *this;
	}

	inline UiTreeT<T>& scaleVelocity( const ci::vec2& v, float decay = 1.0f )
	{
		setScaleVelocity( v, decay );
		return *this;
	}

	inline UiTreeT<T>& scaleVelocity( const ci::vec3& v, float decay = 1.0f )
	{
		setScaleVelocity( v, decay );
		return *this;
	}

	inline UiTreeT<T>& translate( const ci::vec2& v, float speed = 1.0f )
	{
		setTranslate( v, speed );
		return *this;
	}

	inline UiTreeT<T>& translate( const ci::vec3& v, float speed = 1.0f )
	{
		setTranslate( v, speed );
		return *this;
	}

	inline UiTreeT<T>& translateVelocity( const ci::vec2& v, float decay = 1.0f )
	{
		setTranslateVelocity( v, decay );
		return *this;
	}

	inline UiTreeT<T>& translateVelocity( const ci::vec3& v, float decay = 1.0f )
	{
		setTranslateVelocity( v, decay );
		return *this;
	}

	inline std::map<uint64_t, UiTreeT<T>>& getChildren()
	{
		return mChildren;
	}

	inline const std::map<uint64_t, UiTreeT<T>>& getChildren() const
	{
		return mChildren;
	}

	inline CollisionType getCollisionType() const
	{
		return mCollisionType;
	}

	inline T& getData()
	{
		return mData;
	}

	inline const T& getData() const
	{
		return mData;
	}

	inline uint64_t getId() const
	{
		return mId;
	}
	
	inline UiTreeT<T>* getParent()
	{
		return mParent;
	}

	inline const UiTreeT<T>* getParent() const
	{
		return mParent;
	}

	inline UiTreeT<T>& getRoot()
	{
		return mParent == nullptr ? *this : mParent->getRoot();
	}

	inline const UiTreeT<T>& getRoot() const
	{
		return mParent == nullptr ? *this : mParent->getRoot();
	}

	inline const std::vector<ci::app::TouchEvent::Touch>& getTouches() const
	{
		return mTouches;
	}
	
	inline bool hasTouches() const
	{
		return !mTouches.empty();
	}

	inline bool isEnabled() const
	{
		return mEnabled;
	}

	inline bool isMouseOver() const
	{
		return mMouseOver;
	}

	inline bool isVisible() const
	{
		return mVisible;
	}

	inline bool contains( const ci::vec2& v, CollisionType t = CollisionType_Rect, uint64_t* id = nullptr ) const
	{
		return contains( ci::vec3( v, 0.0f ), t, id );
	}

	inline bool contains( const ci::vec3& v, CollisionType t = CollisionType_Cube, uint64_t* id = nullptr ) const
	{
		const ci::vec3& p = mTranslate - mRegistration;
		bool hit = false;
		switch ( t ) {
		case CollisionType_Circle:
			hit = glm::distance( ci::vec2( p ), ci::vec2( v ) ) < std::min( mScale.x, mScale.y );
			break;
		case CollisionType_Cube:
			hit = ci::AxisAlignedBox( p - mScale * 0.5f, p + mScale * 0.5f ).contains( v );
			break;
		case CollisionType_Rect:
			hit = ci::Rectf( ci::vec2( p ), ci::vec2( p ) + ci::vec2( mScale ) ).contains( ci::vec2( v ) );
			break;
		case CollisionType_Sphere:
			hit = glm::distance( p, v ) < std::min( mScale.x, std::min( mScale.y, mScale.z ) );
			break;
		}
		if ( hit ) {
			if ( id != nullptr ) {
				*id = mId;
			}
			return true;
		}
		for ( const std::pair<uint64_t, UiTreeT<T>>& iter : mChildren ) {
			if ( iter.second.contains( v - p, t, id ) ) {
				return true;
			}
		}
		return false;
	}

	inline const ci::vec3& getRegistration() const
	{
		return mRegistration;
	}

	inline float getRegistrationSpeed() const
	{
		return mRegistrationSpeed;
	}

	inline const ci::vec3& getRegistrationTarget() const
	{
		return mRegistrationTarget;
	}

	inline const ci::vec3& getRegistrationVelocity() const
	{
		return mRegistrationVelocity;
	}

	inline float getRegistrationVelocityDecay() const
	{
		return mRegistrationVelocityDecay;
	}

	inline const ci::quat& getRotation() const
	{
		return mRotation;
	}

	inline float getRotationSpeed() const
	{
		return mRotationSpeed;
	}

	inline const ci::quat& getRotationTarget() const
	{
		return mRotationTarget;
	}

	inline const ci::quat& getRotationVelocity() const
	{
		return mRotationVelocity;
	}

	inline float getRotationVelocityDecay() const
	{
		return mRotationVelocityDecay;
	}

	inline const ci::vec3& getScale() const
	{
		return mScale;
	}

	inline float getScaleSpeed() const
	{
		return mScaleSpeed;
	}

	inline const ci::vec3& getScaleTarget() const
	{
		return mScaleTarget;
	}
	
	inline const ci::vec3& getScaleVelocity() const
	{
		return mScaleVelocity;
	}
	
	inline float getScaleVelocityDecay() const
	{
		return mScaleVelocityDecay;
	}

	inline const ci::vec3& getTranslate() const
	{
		return mTranslate;
	}

	inline float getTranslateSpeed() const
	{
		return mTranslateSpeed;
	}

	inline const ci::vec3& getTranslateTarget() const
	{
		return mTranslateTarget;
	}
	
	inline const ci::vec3& getTranslateVelocity() const
	{
		return mTranslateVelocity;
	}
	
	inline float getTranslateVelocityDecay() const
	{
		return mTranslateVelocityDecay;
	}

	inline ci::vec3 calcAbsoluteTranslate() const
	{
		ci::vec3 p						= mTranslate;
		const UiTreeT<T>* parent	= mParent;
		while ( parent != nullptr ) {
			p						+= parent->getTranslate();
			parent					= parent->getParent();
		}
		return p;
	}

	// Calculates the total number of nodes in this tree and its children.
	inline size_t calcNumNodes() const
	{
		return calcNumNodes( 0 );
	}

	inline void setChildren( const std::map<uint64_t, UiTreeT<T>>& c )
	{
		mChildren.clear();
		addChildren( c );
	}

	inline void setCollisionType( CollisionType t )
	{
		mCollisionType = t;
	}

	inline void setData( const T& d )
	{
		mData = d;
	}

	inline void setEnabled( bool enabled )
	{
		bool prev	= mEnabled;
		mEnabled	= enabled;
		if ( prev != mEnabled ) {
			if ( mEnabled ) {
				ci::app::WindowRef window = ci::app::getWindow();
				if ( mParent == nullptr && window != nullptr ) {
					mConnectionKeyDown = window->getSignalKeyDown().connect( 1, 
						[ this ]( ci::app::KeyEvent& event ) { keyDown( event ); } );
					mConnectionKeyUp = window->getSignalKeyUp().connect( 1, 
						[ this ]( ci::app::KeyEvent& event ) { keyUp( event ); } );
					mConnectionMouseDown = window->getSignalMouseDown().connect( 1, 
						[ this ]( ci::app::MouseEvent& event ) { mouseDown( event ); } );
					mConnectionMouseDrag = window->getSignalMouseDrag().connect( 1, 
						[ this ]( ci::app::MouseEvent& event ) { mouseDrag( event ); } );
					mConnectionMouseMove = window->getSignalMouseMove().connect( 1, 
						[ this ]( ci::app::MouseEvent& event ) { mouseMove( event ); } );
					mConnectionMouseUp = window->getSignalMouseUp().connect( 1, 
						[ this ]( ci::app::MouseEvent& event ) { mouseUp( event ); } );
					mConnectionMouseWheel = window->getSignalMouseWheel().connect( 1, 
						[ this ]( ci::app::MouseEvent& event ) { mouseWheel( event ); } );
					mConnectionResize = window->getSignalResize().connect( 1, 
						[ this ]() { resize(); } );
					mConnectionTouchesBegan = window->getSignalTouchesBegan().connect( 1, 
						[ this ]( ci::app::TouchEvent& event ) { touchesBegan( event ); } );
					mConnectionTouchesEnded = window->getSignalTouchesEnded().connect( 1, 
						[ this ]( ci::app::TouchEvent& event ) { touchesEnded( event ); } );
					mConnectionTouchesMoved = window->getSignalTouchesMoved().connect( 1, 
						[ this ]( ci::app::TouchEvent& event ) { touchesMoved( event ); } );
				}
				if ( mEventHandlerEnable != nullptr ) {
					mEventHandlerEnable( this );
				}
			} else {
				if ( mParent == nullptr ) {
					mConnectionKeyDown.disconnect();
					mConnectionKeyUp.disconnect();
					mConnectionMouseDown.disconnect();
					mConnectionMouseDrag.disconnect();
					mConnectionMouseMove.disconnect();
					mConnectionMouseUp.disconnect();
					mConnectionMouseWheel.disconnect();
					mConnectionResize.disconnect();
					mConnectionTouchesBegan.disconnect();
					mConnectionTouchesEnded.disconnect();
					mConnectionTouchesMoved.disconnect();
				}
				if ( mEventHandlerDisable != nullptr ) {
					mEventHandlerDisable( this );
				}
			}
		}
	}

	inline void setParent( UiTreeT<T>* uiTree )
	{
		mParent = uiTree;
	}

	inline void setVisible( bool visible )
	{
		bool prev	= mVisible;
		mVisible	= visible;
		if ( prev != mVisible ) {
			if ( mVisible && mEventHandlerShow != nullptr ) {
				mEventHandlerShow( this );
			} else if ( !mVisible && mEventHandlerHide != nullptr ) {
				mEventHandlerHide( this );
			}
		}
	}

	inline void setRegistration( const ci::vec2& v, float speed = 1.0f )
	{
		setRegistration( ci::vec3( v, 0.0f ), speed );
	}

	inline void setRegistration( const ci::vec3& v, float speed = 1.0f )
	{
		mRegistrationSpeed		= speed;
		mRegistrationTarget		= v;
		mRegistrationVelocity	= ci::vec3( 0.0f );
		mRegistrationVelocityDecay	= 0.0f;
		if ( speed >= 1.0f ) {
			mRegistration		= mRegistrationTarget;
		}
	}

	inline void setRegistrationVelocity( const ci::vec2& v, float decay = 1.0f )
	{
		setRegistrationVelocity( ci::vec3( v, 0.0f ), decay );
	}

	inline void setRegistrationVelocity( const ci::vec3& v, float decay = 1.0f )
	{
		mRegistrationSpeed			= 1.0f;
		mRegistrationTarget			= mRegistration;
		mRegistrationVelocity		= v;
		mRegistrationVelocityDecay	= decay;
	}

	inline void setRotation( float z, float speed = 1.0f )
	{
		mRotationSpeed	= speed;
		mRotationTarget = glm::angleAxis( z, ci::vec3( 0.0f, 0.0f, 1.0f ) );
	}

	inline void setRotation( const ci::quat& q, float speed = 1.0f )
	{
		mRotationSpeed	= speed;
		mRotationTarget = q;
	}

	inline void setRotationVelocity( float z, float decay = 1.0f )
	{
		setRotationVelocity( glm::angleAxis( z, ci::vec3( 0.0f, 0.0f, 1.0f ) ), decay );
	}

	inline void setRotationVelocity( const ci::quat& q, float decay = 1.0f )
	{
		mRotationSpeed			= 1.0f;
		mRotationTarget			= mRotation;
		mRotationVelocity		= q;
		mRotationVelocityDecay	= decay;
	}

	inline void setScale( const ci::vec2& v, float speed = 1.0f )
	{
		setScale( ci::vec3( v, 0.0f ), speed );
	}

	inline void setScale( const ci::vec3& v, float speed = 1.0f )
	{
		mScaleSpeed		= speed;
		mScaleTarget	= v;
		mScaleVelocity	= ci::vec3( 0.0f );
		mScaleVelocityDecay	= 0.0f;
		if ( speed >= 1.0f ) {
			mScale		= mScaleTarget;
		}
	}

	inline void setScaleVelocity( const ci::vec2& v, float decay = 1.0f )
	{
		setScaleVelocity( ci::vec3( v, 0.0f ), decay );
	}

	inline void setScaleVelocity( const ci::vec3& v, float decay = 1.0f )
	{
		mScaleSpeed			= 1.0f;
		mScaleTarget		= mScale;
		mScaleVelocity		= v;
		mScaleVelocityDecay	= decay;
	}

	inline void setTranslate( const ci::vec2& v, float speed = 1.0f )
	{
		setTranslate( ci::vec3( v, 0.0f ), speed );
	}

	inline void setTranslate( const ci::vec3& v, float speed = 1.0f )
	{
		mTranslateSpeed		= speed;
		mTranslateTarget	= v;
		mTranslateVelocity	= ci::vec3( 0.0f );
		mTranslateVelocityDecay	= 0.0f;
		if ( speed >= 1.0f ) {
			mTranslate		= mTranslateTarget;
		}
	}

	inline void setTranslateVelocity( const ci::vec2& v, float decay = 1.0f )
	{
		setTranslateVelocity( ci::vec3( v, 0.0f ), decay );
	}

	inline void setTranslateVelocity( const ci::vec3& v, float decay = 1.0f )
	{
		mTranslateSpeed			= 1.0f;
		mTranslateTarget		= mTranslate;
		mTranslateVelocity		= v;
		mTranslateVelocityDecay	= decay;
	}

	inline UiTreeT<T>& connectDisableEventHandler( const std::function<void( UiTreeT<T>* )>& eventHandler )
	{
		mEventHandlerDisable = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectDisableEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectDisableEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}

	inline UiTreeT<T>& disconnectDisableEventHandler()
	{
		mEventHandlerDisable = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectEnableEventHandler( const std::function<void( UiTreeT<T>* )>& eventHandler )
	{
		mEventHandlerEnable = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectEnableEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectEnableEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}

	inline UiTreeT<T>& disconnectEnableEventHandler()
	{
		mEventHandlerEnable = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectHideEventHandler( const std::function<void( UiTreeT<T>* )>& eventHandler )
	{
		mEventHandlerHide = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectHideEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectHideEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}

	inline UiTreeT<T>& disconnectHideEventHandler()
	{
		mEventHandlerHide = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectKeyDownEventHandler( const std::function<void( UiTreeT<T>*, ci::app::KeyEvent& )>& eventHandler )
	{
		mEventHandlerKeyDown = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectKeyDownEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectKeyDownEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectKeyDownEventHandler()
	{
		mEventHandlerKeyDown = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectKeyUpEventHandler( const std::function<void( UiTreeT<T>*, ci::app::KeyEvent& )>& eventHandler )
	{
		mEventHandlerKeyUp = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectKeyUpEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectKeyUpEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectKeyUpEventHandler()
	{
		mEventHandlerKeyUp = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectMouseDownEventHandler( const std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>& eventHandler )
	{
		mEventHandlerMouseDown = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectMouseDownEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectMouseDownEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectMouseDownEventHandler()
	{
		mEventHandlerMouseDown = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectMouseDragEventHandler( const std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>& eventHandler )
	{
		mEventHandlerMouseDrag = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectMouseDragEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectMouseDragEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectMouseDragEventHandler()
	{
		mEventHandlerMouseDrag = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectMouseMoveEventHandler( const std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>& eventHandler )
	{
		mEventHandlerMouseMove = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectMouseMoveEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectMouseMoveEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectMouseMoveEventHandler()
	{
		mEventHandlerMouseMove = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectMouseOutEventHandler( const std::function<void( UiTreeT<T>* )>& eventHandler )
	{
		mEventHandlerMouseOut = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectMouseOutEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectMouseOutEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}

	inline UiTreeT<T>& disconnectMouseOutEventHandler()
	{
		mEventHandlerMouseOut = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectMouseOverEventHandler( const std::function<void( UiTreeT<T>* )>& eventHandler )
	{
		mEventHandlerMouseOver = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectMouseOverEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectMouseOverEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}

	inline UiTreeT<T>& disconnectMouseOverEventHandler()
	{
		mEventHandlerMouseOver = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectMouseUpEventHandler( const std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>& eventHandler )
	{
		mEventHandlerMouseUp = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectMouseUpEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectMouseUpEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectMouseUpEventHandler()
	{
		mEventHandlerMouseUp = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectMouseWheelEventHandler( const std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>& eventHandler )
	{
		mEventHandlerMouseWheel = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectMouseWheelEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectMouseWheelEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectMouseWheelEventHandler()
	{
		mEventHandlerMouseWheel = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectShowEventHandler( const std::function<void( UiTreeT<T>* )>& eventHandler )
	{
		mEventHandlerShow = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectShowEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectShowEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}

	inline UiTreeT<T>& disconnectShowEventHandler()
	{
		mEventHandlerShow = nullptr;
		return *this;
	}
	
	inline UiTreeT<T>& connectResizeEventHandler( const std::function<void( UiTreeT<T>* )>& eventHandler )
	{
		mEventHandlerResize = eventHandler;
		return *this;
	}
	
	template<typename V, typename Y>
	inline UiTreeT<T>& connectResizeEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectResizeEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}
	
	inline UiTreeT<T>& disconnectResizeEventHandler()
	{
		mEventHandlerResize = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectTouchesBeganEventHandler( const std::function<void( UiTreeT<T>*, ci::app::TouchEvent& )>& eventHandler )
	{
		mEventHandlerTouchesBegan = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectTouchesBeganEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectTouchesBeganEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectTouchesBeganEventHandler()
	{
		mEventHandlerTouchesBegan = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectTouchesEndedEventHandler( const std::function<void( UiTreeT<T>*, ci::app::TouchEvent& )>& eventHandler )
	{
		mEventHandlerTouchesEnded = eventHandler;
		return *this;
	}

	template<typename V, typename Y>
	inline UiTreeT<T>& connectTouchesEndedEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectTouchesEndedEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectTouchesEndedEventHandler()
	{
		mEventHandlerTouchesEnded = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectTouchesMovedEventHandler( const std::function<void( UiTreeT<T>*, ci::app::TouchEvent& )>& eventHandler )
	{
		mEventHandlerTouchesMoved = eventHandler;
		return *this;
	}
	
	template<typename V, typename Y>
	inline UiTreeT<T>& connectTouchesMovedEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectTouchesMovedEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectTouchesMovedEventHandler()
	{
		mEventHandlerTouchesMoved = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectTouchOutEventHandler( const std::function<void( UiTreeT<T>*, uint32_t )>& eventHandler )
	{
		mEventHandlerTouchOut = eventHandler;
		return *this;
	}
	
	template<typename V, typename Y>
	inline UiTreeT<T>& connectTouchOutEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectTouchOutEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectTouchOutEventHandler()
	{
		mEventHandlerTouchOut = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectTouchOverEventHandler( const std::function<void( UiTreeT<T>*, uint32_t )>& eventHandler )
	{
		mEventHandlerTouchOver = eventHandler;
		return *this;
	}
	
	template<typename V, typename Y>
	inline UiTreeT<T>& connectTouchOverEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectTouchOverEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}

	inline UiTreeT<T>& disconnectTouchOverEventHandler()
	{
		mEventHandlerTouchOver = nullptr;
		return *this;
	}

	inline UiTreeT<T>& connectUpdateEventHandler( const std::function<void( UiTreeT<T>* )>& eventHandler )
	{
		mEventHandlerUpdate = eventHandler;
		return *this;
	}
	
	template<typename V, typename Y>
	inline UiTreeT<T>& connectUpdateEventHandler( V eventHandler, Y* eventHandlerObject )
	{
		return connectUpdateEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}

	inline UiTreeT<T>& disconnectUpdateEventHandler()
	{
		mEventHandlerUpdate = nullptr;
		return *this;
	}

	inline void update()
	{
		for ( auto& iter : mChildren ) {
			iter.second.update();
		}

		static const float epsilon = 0.01f;

		{
			float l = (float)glm::length( mRegistrationVelocity );
			if ( l < epsilon ) {
				mRegistrationVelocityDecay = 0.0f;
			}
			if ( l > 0.0f ) {
				mRegistrationTarget		+= mRegistrationVelocity;
				mRegistrationVelocity	*= mRegistrationVelocityDecay;
			}
		}
		{
			float l = (float)glm::length( mRotationVelocity );
			if ( l < epsilon ) {
				mRotationVelocityDecay = 0.0f;
			}
			if ( l > 0.0f ) {
				mRotationTarget		+= mRotationVelocity;
				mRotationVelocity	*= mRotationVelocityDecay;
			}
		}
		{
			float l = (float)glm::length( mScaleVelocity );
			if ( l < epsilon ) {
				mScaleVelocityDecay = 0.0f;
			}
			if ( l > 0.0f ) {
				mScaleTarget	+= mScaleVelocity;
				mScaleVelocity	*= mScaleVelocityDecay;
			}
		}
		{
			float l = (float)glm::length( mTranslateVelocity );
			if ( l < epsilon ) {
				mTranslateVelocityDecay = 0.0f;
			}
			if ( l > 0.0f ) {
				mTranslateTarget	+= mTranslateVelocity;
				mTranslateVelocity	*= mTranslateVelocityDecay;
			}
		}

		mRegistration	= ci::lerp( mRegistration, mRegistrationTarget, mRegistrationSpeed );
		mRotation		= glm::slerp( mRotation, mRotationTarget, mRotationSpeed );
		mScale			= ci::lerp( mScale, mScaleTarget, mScaleSpeed );
		mTranslate		= ci::lerp( mTranslate, mTranslateTarget, mTranslateSpeed );

		if ( mEventHandlerUpdate != nullptr ) {
			mEventHandlerUpdate( this );
		}
	}
protected:
	inline void keyDown( ci::app::KeyEvent& event )
	{
		if ( mEnabled ) {
			for ( auto& iter : mChildren ) {
				iter.second.keyDown( event );
				if ( event.isHandled() ) {
					return;
				}
			}
			if ( mEventHandlerKeyDown != nullptr ) {
				mEventHandlerKeyDown( this, event );
			}
		}
	}

	inline void keyUp( ci::app::KeyEvent& event )
	{
		if ( mEnabled ) {
			for ( auto& iter : mChildren ) {
				iter.second.keyUp( event );
				if ( event.isHandled() ) {
					return;
				}
			}
			if ( mEventHandlerKeyUp != nullptr ) {
				mEventHandlerKeyUp( this, event );
			}
		}
	}

	inline void mouseDown( ci::app::MouseEvent& event )
	{
		if ( mEnabled ) {
			bool handled = false;
			for ( auto& iter : mChildren ) {
				iter.second.mouseDown( event );
				if ( event.isHandled() ) {
					handled = true;
					return;
				}
			}
			mouseOver( event );
			if ( !handled && mEventHandlerMouseDown != nullptr ) {
				mEventHandlerMouseDown( this, event );
			}
		}
	}

	inline void mouseDrag( ci::app::MouseEvent& event )
	{
		if ( mEnabled ) {
			bool handled = false;
			for ( auto& iter : mChildren ) {
				iter.second.mouseDrag( event );
				if ( event.isHandled() ) {
					handled = true;
					return;
				}
			}
			mouseOver( event );
			if ( !handled && mEventHandlerMouseDrag != nullptr ) {
				mEventHandlerMouseDrag( this, event );
			}
		}
	}

	inline void mouseMove( ci::app::MouseEvent& event )
	{
		if ( mEnabled ) {
			bool handled = false;
			for ( auto& iter : mChildren ) {
				iter.second.mouseMove( event );
				if ( event.isHandled() ) {
					handled = true;
					break;
				}
			}
			mouseOver( event );
			if ( !handled && mEventHandlerMouseMove != nullptr ) {
				mEventHandlerMouseMove( this, event );
			}
		}
	}

	inline void mouseOver( ci::app::MouseEvent& event )
	{
		bool mouseOver	= mMouseOver;
		mMouseOver		= contains( ci::vec2( event.getPos() ), mCollisionType );
		if ( mouseOver != mMouseOver ) {
			if ( mMouseOver && mEventHandlerMouseOver != nullptr ) {
				mEventHandlerMouseOver( this );
			} else if ( !mMouseOver && mEventHandlerMouseOut != nullptr ) {
				mEventHandlerMouseOut( this );
			}
		}
	}

	inline void mouseUp( ci::app::MouseEvent& event )
	{
		if ( mEnabled ) {
			bool handled = false;
			for ( auto& iter : mChildren ) {
				iter.second.mouseUp( event );
				if ( event.isHandled() ) {
					handled = true;
					break;
				}
			}
			mouseOver( event );
			if ( !handled && mEventHandlerMouseUp != nullptr ) {
				mEventHandlerMouseUp( this, event );
			}
		}
	}

	inline void mouseWheel( ci::app::MouseEvent& event )
	{
		if ( mEnabled ) {
			for ( auto& iter : mChildren ) {
				iter.second.mouseWheel( event );
				if ( event.isHandled() ) {
					return;
				}
			}
			if ( mEventHandlerMouseWheel != nullptr ) {
				mEventHandlerMouseWheel( this, event );
			}
		}
	}
	
	inline void resize()
	{
		if ( mEnabled ) {
			for ( auto& iter : mChildren ) {
				iter.second.resize();
			}
			if ( mEventHandlerResize != nullptr ) {
				mEventHandlerResize( this );
			}
		}
	}
	
	inline void touchesBegan( ci::app::TouchEvent& event )
	{
		if ( mEnabled ) {
			bool handled = false;
			for ( auto& iter : mChildren ) {
				iter.second.touchesBegan( event );
				if ( event.isHandled() ) {
					handled = true;
					break;
				}
			}
			touchOver( event.getTouches() );
			if ( !handled && mEventHandlerTouchesBegan != nullptr ) {
				mEventHandlerTouchesBegan( this, event );
			}
		}
	}

	inline void touchesEnded( ci::app::TouchEvent& event )
	{
		if ( mEnabled ) {
			bool handled = false;
			for ( auto& iter : mChildren ) {
				iter.second.touchesEnded( event );
				if ( event.isHandled() ) {
					handled = true;
					break;
				}
			}
			auto removeTouch = [ & ]( uint32_t id ) -> bool
			{
				for ( std::vector<ci::app::TouchEvent::Touch>::iterator iter = mTouches.begin(); iter != mTouches.end(); ) {
					if ( iter->getId() == id ) {
						mTouches.erase( iter );
						return true;
					} else {
						++iter;
					}
				}
				return false;
			};
			for ( const ci::app::TouchEvent::Touch& iter : event.getTouches() ) {
				uint32_t id = iter.getId();
				if ( removeTouch( id ) && mEventHandlerTouchOut != nullptr  ) {
					mEventHandlerTouchOut( this, id );
				}
			}
			if ( !handled && mEventHandlerTouchesEnded != nullptr ) {
				mEventHandlerTouchesEnded( this, event );
			}
		}
	}

	inline void touchesMoved( ci::app::TouchEvent& event )
	{
		if ( mEnabled ) {
			bool handled = false;
			for ( auto& iter : mChildren ) {
				iter.second.touchesMoved( event );
				if ( event.isHandled() ) {
					handled = true;
					break;
				}
			}
			if ( !handled && mEventHandlerTouchesMoved != nullptr ) {
				mEventHandlerTouchesMoved( this, event );
			}
			touchOver( event.getTouches() );
		}
	}

	inline void touchOver( const std::vector<ci::app::TouchEvent::Touch>& touches )
	{
		std::vector<ci::app::TouchEvent::Touch> t;
		for ( const ci::app::TouchEvent::Touch& touch : touches ) {
			const uint32_t id	= touch.getId();
			const bool over		= contains( touch.getPos(), mCollisionType );
			const bool prev		= contains( touch.getPrevPos(), mCollisionType );
			if ( mEventHandlerTouchOver != nullptr && over && !prev ) {
				mEventHandlerTouchOver( this, id );
			} else if ( mEventHandlerTouchOut != nullptr && !over && prev ) {
				mEventHandlerTouchOut( this, id );
			}
			if ( over ) {
				t.push_back( touch );
			}
		}
		for ( const ci::app::TouchEvent::Touch& a : t ) {
			bool found	= false;
			uint32_t id = a.getId();
			for ( const ci::app::TouchEvent::Touch& b : mTouches ) {
				if ( id == b.getId() ) {
					found = true;
					break;
				}
			}
			if ( !found ) {
				mTouches.push_back( a );
				if ( mEventHandlerTouchOver != nullptr ) {
					mEventHandlerTouchOver( this, id );
				}
			}
		}
	}

	inline size_t calcNumNodes( size_t count ) const
	{
		for ( auto& iter : mChildren ) {
			count = iter.second.calcNumNodes( count );
		}
		return count + 1;
	}

	std::map<uint64_t, UiTreeT<T>>								mChildren;
	T															mData;
	uint64_t													mId;
	UiTreeT<T>*													mParent;

	CollisionType												mCollisionType;
	bool														mEnabled;
	bool														mMouseOver;
	std::vector<ci::app::TouchEvent::Touch>						mTouches;
	bool														mVisible;

	ci::vec3													mRegistration;
	float														mRegistrationSpeed;
	ci::vec3													mRegistrationTarget;
	ci::vec3													mRegistrationVelocity;
	float 														mRegistrationVelocityDecay;

	ci::quat													mRotation;
	float														mRotationSpeed;
	ci::quat													mRotationTarget;
	ci::quat													mRotationVelocity;
	float														mRotationVelocityDecay;

	ci::vec3													mScale;
	float														mScaleSpeed;
	ci::vec3													mScaleTarget;
	ci::vec3													mScaleVelocity;
	float														mScaleVelocityDecay;

	ci::vec3													mTranslate;
	float														mTranslateSpeed;
	ci::vec3													mTranslateTarget;
	ci::vec3													mTranslateVelocity;
	float														mTranslateVelocityDecay;

	ci::signals::Connection										mConnectionKeyDown;
	ci::signals::Connection										mConnectionKeyUp;
	ci::signals::Connection										mConnectionMouseDown;
	ci::signals::Connection										mConnectionMouseDrag;
	ci::signals::Connection										mConnectionMouseMove;
	ci::signals::Connection										mConnectionMouseUp;
	ci::signals::Connection										mConnectionMouseWheel;
	ci::signals::Connection										mConnectionResize;
	ci::signals::Connection										mConnectionTouchesBegan;
	ci::signals::Connection										mConnectionTouchesEnded;
	ci::signals::Connection										mConnectionTouchesMoved;

	std::function<void( UiTreeT<T>* )>							mEventHandlerDisable;
	std::function<void( UiTreeT<T>* )>							mEventHandlerEnable;
	std::function<void( UiTreeT<T>* )>							mEventHandlerHide;
	std::function<void( UiTreeT<T>*, ci::app::KeyEvent& )>		mEventHandlerKeyDown;
	std::function<void( UiTreeT<T>*, ci::app::KeyEvent& )>		mEventHandlerKeyUp;
	std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>	mEventHandlerMouseDown;
	std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>	mEventHandlerMouseDrag;
	std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>	mEventHandlerMouseMove;
	std::function<void( UiTreeT<T>* )>							mEventHandlerMouseOut;
	std::function<void( UiTreeT<T>* )>							mEventHandlerMouseOver;
	std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>	mEventHandlerMouseUp;
	std::function<void( UiTreeT<T>*, ci::app::MouseEvent& )>	mEventHandlerMouseWheel;
	std::function<void( UiTreeT<T>* )>							mEventHandlerResize;
	std::function<void( UiTreeT<T>* )>							mEventHandlerShow;
	std::function<void( UiTreeT<T>*, ci::app::TouchEvent& )>	mEventHandlerTouchesBegan;
	std::function<void( UiTreeT<T>*, ci::app::TouchEvent& )>	mEventHandlerTouchesEnded;
	std::function<void( UiTreeT<T>*, ci::app::TouchEvent& )>	mEventHandlerTouchesMoved;
	std::function<void( UiTreeT<T>*, uint32_t )>				mEventHandlerTouchOut;
	std::function<void( UiTreeT<T>*, uint32_t )>				mEventHandlerTouchOver;
	std::function<void( UiTreeT<T>* )>							mEventHandlerUpdate;

	/////////////////////////////////////////////////////////////////////////////////

public:
	class Exception : public ci::Exception
	{
	public:
		inline const char* what() const throw()
		{
			return mMessage;
		}
	protected:
		char mMessage[ 2048 ];
	};

	class ExcIdNotFound : public Exception 
	{
	public:
		ExcIdNotFound( uint64_t id ) throw()
		{
			std::sprintf( this->mMessage, "ID '%lu' not found. Call UiTreeT<T>::exists() before finding a node.", (unsigned long)id );
		}
	};

	class ExcDuplicateId : public Exception 
	{
	public:
		ExcDuplicateId( uint64_t id ) throw()
		{
			std::sprintf( this->mMessage, "ID '%lu' already exists in tree.", (unsigned long)id );
		}
	};
};
 
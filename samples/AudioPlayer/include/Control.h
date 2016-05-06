#pragma once

#include "NodeId.h"
#include "UiTree.h"

typedef UiTreeT<ci::ColorAf> UiTree;
 
class Control : public UiTree::EventHandlerInterface
{
public:
	virtual void mouseDown( UiTree* node, ci::app::MouseEvent& event ) = 0;
	virtual void mouseDrag( UiTree* node, ci::app::MouseEvent& event ) = 0;
	virtual void mouseUp( UiTree* node, ci::app::MouseEvent& event ) = 0;
	virtual void touchesBegan( UiTree* node, ci::app::TouchEvent& event ) = 0;
	virtual void touchesEnded( UiTree* node, ci::app::TouchEvent& event ) = 0;
	virtual void touchesMoved( UiTree* node, ci::app::TouchEvent& event ) = 0;
};
 
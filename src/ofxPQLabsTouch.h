#pragma once

#include "ofMain.h"
#include "PQMTClient.h"

using namespace PQ_SDK_MultiTouch;


class ofxPQLabsTouchEventArgs : public ofEventArgs {
public:
    enum Type {
		down,
		up,
		move
	} type;
    
	int id;
    float x; //0-1
    float y; //0-1
    
    TouchPoint touchPoint;
};


class ofxPQLabsTouch {
public:
    
	ofxPQLabsTouch();
	~ofxPQLabsTouch();

	int connect(const char * ipAddress);
    void disconnect();
    void debugDraw();
	
    // easy listener setup- must have onPQLabsTouchAdded, onPQLabsTouchUpdated, onPQLabsTouchRemoved in ofApp
    template <class ListenerClass>
	void addEventListeners(ListenerClass * listener){
        ofAddListener(onTouchAddedEvent,listener,&ListenerClass::onPQLabsTouchAdded);
        ofAddListener(onTouchUpdatedEvent,listener,&ListenerClass::onPQLabsTouchUpdated);
        ofAddListener(onTouchRemovedEvent,listener,&ListenerClass::onPQLabsTouchRemoved);
    };
    template <class ListenerClass>
	void removeEventListeners(ListenerClass * listener){
        ofRemoveListener(onTouchAddedEvent,listener,&ListenerClass::onPQLabsTouchAdded);
        ofRemoveListener(onTouchUpdatedEvent,listener,&ListenerClass::onPQLabsTouchUpdated);
        ofRemoveListener(onTouchRemovedEvent,listener,&ListenerClass::onPQLabsTouchRemoved);
    };
    
    // the 3 main events
    ofEvent<ofxPQLabsTouchEventArgs> onTouchAddedEvent;
    ofEvent<ofxPQLabsTouchEventArgs> onTouchUpdatedEvent;
    ofEvent<ofxPQLabsTouchEventArgs> onTouchRemovedEvent;
    
    
    bool isConnected() { return connected; }
    vector<ofxPQLabsTouchEventArgs>& getActiveTouches() { return activeTouches; }
    int getActiveTouchCount() { return activeTouches.size(); }
    float getServerWidth() { return serverWidth; }
    float getServerHeight() { return serverHeight; }
    
private:
    
    vector<ofxPQLabsTouchEventArgs> activeTouches;
    bool connected;
    float serverWidth, serverHeight;
    
    //////////////////////call back functions///////////////////////
	// OnReceivePointFrame: function to handle when recieve touch point frame
	//	the unmoving touch point won't be sent from server. The new touch point with its pointevent is TP_DOWN
	//	and the leaving touch point with its pointevent will be always sent from server;
	static void OnReceivePointFrame(int frame_id,int time_stamp,int moving_point_count,const TouchPoint * moving_point_array, void * call_back_object);
	// OnReceivePointFrame: function to handle when recieve touch gesture
	static void OnReceiveGesture(const TouchGesture & ges, void * call_back_object);
	// OnServerBreak: function to handle when server break(disconnect or network error)
	static void OnServerBreak(void * param, void * call_back_object);
	// OnReceiveError: function to handle when some errors occur on the process of receiving touch datas.
	static void OnReceiveError(int err_code,void * call_back_object);
	//
	static void OnGetServerResolution(int x, int y, void * call_back_object);
    //////////////////////call back functions end ///////////////////////

	
    //here use function pointer table to handle the different gesture type;
	typedef void (*PFuncOnTouchGesture)(const TouchGesture & tg,void * call_object);
	static void DefaultOnTG(const TouchGesture & tg,void * call_object); // just show the gesture
    
	static void OnTG_TouchStart(const TouchGesture & tg,void * call_object);
	static void OnTG_Down(const TouchGesture & tg,void * call_object);
	static void OnTG_Move(const TouchGesture & tg,void * call_object);
	static void OnTG_Up(const TouchGesture & tg,void * call_object);
    
	//
	static void OnTG_SecondDown(const TouchGesture & tg,void * call_object);
	static void OnTG_SecondUp(const TouchGesture & tg,void * call_object);
    
	//
	static void OnTG_SplitStart(const TouchGesture & tg,void * call_object);
	static void OnTG_SplitApart(const TouchGesture & tg,void * call_object);
	static void OnTG_SplitClose(const TouchGesture & tg,void * call_object);
	static void OnTG_SplitEnd(const TouchGesture & tg,void * call_object);
    
	// OnTG_TouchEnd: to clear what need to clear;
	static void OnTG_TouchEnd(const TouchGesture & tg,void * call_object);
    
    
    // functions to handle TouchGestures, attention the means of the params
	void InitFuncOnTG();
    // set the call back functions while reciving touch data;
	void SetFuncsOnReceiveProc();
    
    // OnTouchPoint: function to handle TouchPoint
	void OnTouchPoint(const TouchPoint & tp);
	// OnTouchGesture: function to handle TouchGesture
	void OnTouchGesture(const TouchGesture & tg);
    
    
    PFuncOnTouchGesture m_pf_on_tges[TG_TOUCH_END + 1];
    
};




#include "ofxPQLabsTouch.h"


ofxPQLabsTouch::ofxPQLabsTouch() {
    memset(m_pf_on_tges,0, sizeof(m_pf_on_tges));
    serverWidth = serverHeight = 0;
    connected = false;
}

ofxPQLabsTouch::~ofxPQLabsTouch() {
    disconnect();
}

void ofxPQLabsTouch::disconnect() {
    DisconnectServer();
    connected = false;
    activeTouches.clear();
}

int ofxPQLabsTouch::connect(const char * ipAddress = "127.0.0.1") {
	   
    connected = false;
    int err_code = PQMTE_SUCCESS;
    
	// initialize the handle functions of gestures;
	InitFuncOnTG();
	// set the functions on server callback
	SetFuncsOnReceiveProc();
	// connect server
	cout << " connect to server..." << endl;
	if((err_code = ConnectServer()) != PQMTE_SUCCESS){
		cout << " connect server fail, socket error code:" << err_code << endl;
		return err_code;
	}
    // send request to server
	cout << " connect success, send request." << endl;
	TouchClientRequest tcq = {0};
	tcq.type = RQST_RAWDATA_ALL | RQST_GESTURE_ALL;
	if((err_code = SendRequest(tcq)) != PQMTE_SUCCESS){
		cout << " send request fail, error code:" << err_code << endl;
		return err_code;
	}
    //////////////you can set the move_threshold when the tcq.type is RQST_RAWDATA_INSIDE;
	//send threshold
	//int move_threshold = 1;// 1 pixel
	//if((err_code = SendThreshold(move_threshold) != PQMTE_SUCCESS){
	//	cout << " send threadhold fail, error code:" << err_code << endl;
	//	return err_code;
	//}
	//
	////////////////////////
	//get server resolution
	if((err_code = GetServerResolution(OnGetServerResolution, this)) != PQMTE_SUCCESS){
		cout << " get server resolution fail,error code:" << err_code << endl;
		return err_code;
	};
	//
	// start receiving
	cout << " send request success, start recv." << endl;
    connected = true;
    return err_code;
}

void ofxPQLabsTouch::debugDraw() {
    
    for(int i = 0; i < activeTouches.size(); i++) {
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(activeTouches[i].x * serverWidth, activeTouches[i].y * serverHeight);
        ofSetColor(0);
        ofCircle(0, 0, 15);
        ofSetColor(255);
        ofDrawBitmapString(ofToString(activeTouches[i].id), 0, 0);
        ofPopStyle();
        ofPopMatrix();
    }
}

void ofxPQLabsTouch::InitFuncOnTG() {
	// initialize the call back functions of toucha gestures;
	m_pf_on_tges[TG_TOUCH_START] = &ofxPQLabsTouch::OnTG_TouchStart;
	m_pf_on_tges[TG_DOWN] = &ofxPQLabsTouch::OnTG_Down;
	m_pf_on_tges[TG_MOVE] = &ofxPQLabsTouch::OnTG_Move;
	m_pf_on_tges[TG_UP] = &ofxPQLabsTouch::OnTG_Up;
    
	m_pf_on_tges[TG_SECOND_DOWN] = &ofxPQLabsTouch::OnTG_SecondDown;
	m_pf_on_tges[TG_SECOND_UP] = &ofxPQLabsTouch::OnTG_SecondUp;
    
	m_pf_on_tges[TG_SPLIT_START] = &ofxPQLabsTouch::OnTG_SplitStart;
	m_pf_on_tges[TG_SPLIT_APART] = &ofxPQLabsTouch::OnTG_SplitApart;
	m_pf_on_tges[TG_SPLIT_CLOSE] = &ofxPQLabsTouch::OnTG_SplitClose;
	m_pf_on_tges[TG_SPLIT_END] = &ofxPQLabsTouch::OnTG_SplitEnd;
    
	m_pf_on_tges[TG_TOUCH_END] = &ofxPQLabsTouch::OnTG_TouchEnd;
    
}

void ofxPQLabsTouch::SetFuncsOnReceiveProc() {
    
	PFuncOnReceivePointFrame old_rf_func = SetOnReceivePointFrame(&ofxPQLabsTouch::OnReceivePointFrame,this);
	//PFuncOnReceiveGesture old_rg_func = SetOnReceiveGesture(&ofxPQLabsTouch::OnReceiveGesture,this);
	PFuncOnServerBreak old_svr_break = SetOnServerBreak(&ofxPQLabsTouch::OnServerBreak,NULL);
	PFuncOnReceiveError old_rcv_err_func = SetOnReceiveError(&ofxPQLabsTouch::OnReceiveError,NULL);
}

void ofxPQLabsTouch:: OnReceivePointFrame(int frame_id, int time_stamp, int moving_point_count, const TouchPoint * moving_point_array, void * call_back_object) {
	ofxPQLabsTouch * sample = static_cast<ofxPQLabsTouch*>(call_back_object);
	assert(sample != NULL);
	/*const char * tp_event[] =
	{
		"down",
		"move",
		"up",
	};*/
    
	//cout << " frame_id:" << frame_id << " time:"  << time_stamp << " ms" << " moving point count:" << moving_point_count << endl;
	for(int i = 0; i < moving_point_count; ++ i){
		TouchPoint tp = moving_point_array[i];
		sample->OnTouchPoint(tp);
	}
}
void ofxPQLabsTouch::OnReceiveGesture(const TouchGesture & ges, void * call_back_object){
	ofxPQLabsTouch * sample = static_cast<ofxPQLabsTouch*>(call_back_object);
	assert(sample != NULL);
	sample->OnTouchGesture(ges);
}

void ofxPQLabsTouch::OnServerBreak(void * param, void * call_back_object){
	// when the server break, disconenct server;
	cout << "server break, disconnect here" << endl;
	DisconnectServer();
}

void ofxPQLabsTouch::OnReceiveError(int err_code, void * call_back_object){
	switch(err_code)
	{
        case PQMTE_RCV_INVALIDATE_DATA:
            cout << " error: receive invalidate data." << endl;
            break;
        case PQMTE_SERVER_VERSION_OLD:
            cout << " error: the multi-touch server is old for this client, please update the multi-touch server." << endl;
            break;
        default:
            cout << " socket error, socket error code:" << err_code << endl;
	}
}

void ofxPQLabsTouch::OnGetServerResolution(int x, int y, void * call_back_object){
	cout << " server resolution:" << x << "," << y << endl;
    ofxPQLabsTouch * sample = static_cast<ofxPQLabsTouch*>(call_back_object);
	assert(sample != NULL);
    sample->serverWidth = x;
    sample->serverHeight = y;
}

// here, just record the position of point,
//	you can do mouse map like "OnTG_Down" etc;
void ofxPQLabsTouch::OnTouchPoint(const TouchPoint & tp){
    
    ofxPQLabsTouchEventArgs args;
    args.id = tp.id;
    args.x = tp.x / serverWidth;
    args.y = tp.y / serverHeight;
    args.type = (ofxPQLabsTouchEventArgs::Type)tp.point_event;
    args.touchPoint = tp;
    
	switch(tp.point_event)
	{
        case TP_DOWN:
            
            //cout << "  point " << tp.id << " come at (" << tp.x << "," << tp.y
			//<< ") width:" << tp.dx << " height:" << tp.dy << endl;
            activeTouches.push_back(args);
            ofNotifyEvent(onTouchAddedEvent, args);
            break;
        case TP_MOVE:
            //cout << "  point " << tp.id << " move at (" << tp.x << "," << tp.y
			//<< ") width:" << tp.dx << " height:" << tp.dy << endl;
            for(int i = 0; i < activeTouches.size(); i++) {
                if (args.id == activeTouches[i].id) {
                    //activeTouches[i] = args;
                    activeTouches[i].x = args.x;
                    activeTouches[i].y = args.y;
                    activeTouches[i].touchPoint = args.touchPoint;
                    break;
                }
            }
            ofNotifyEvent(onTouchUpdatedEvent, args);
            break;
        case TP_UP:
            //cout << "  point " << tp.id << " leave at (" << tp.x << "," << tp.y
			//<< ") width:" << tp.dx << " height:" << tp.dy << endl;
            for(int i = 0; i < activeTouches.size(); i++) {
                if (args.id == activeTouches[i].id) {
                    activeTouches.erase(activeTouches.begin() + i);
                    break;
                }
            }
            ofNotifyEvent(onTouchRemovedEvent, args);
            break;
	}
        
}

void ofxPQLabsTouch::OnTouchGesture(const TouchGesture & tg){
	if(TG_NO_ACTION == tg.type)
		return ;
	
	assert(tg.type <= TG_TOUCH_END);
	DefaultOnTG(tg,this);
	PFuncOnTouchGesture pf = m_pf_on_tges[tg.type];
	if(NULL != pf){
		pf(tg,this);
	}
}


void ofxPQLabsTouch::OnTG_TouchStart(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_TOUCH_START);
	cout << "  here, the touch start, initialize something." << endl;
}
void ofxPQLabsTouch::DefaultOnTG(const TouchGesture & tg,void * call_object) // just show the gesture
{
	cout <<"ges,name:"<< GetGestureName(tg) << " type:" << tg.type << ",param size:" << tg.param_size << " ";
	for(int i = 0; i < tg.param_size; ++ i)
		cout << tg.params[i] << " ";
	cout << endl;
}
void ofxPQLabsTouch::OnTG_Down(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_DOWN && tg.param_size >= 2);
	cout << "  the single finger touching at :( "
    << tg.params[0] << "," << tg.params[1] << " )" << endl;
}
void ofxPQLabsTouch::OnTG_Move(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_MOVE && tg.param_size >= 2);
	cout << "  the single finger moving on the screen at :( "
    << tg.params[0] << "," << tg.params[1] << " )" << endl;
}
void ofxPQLabsTouch::OnTG_Up(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_UP && tg.param_size >= 2);
	cout << " the single finger is leaving the screen at :( "
    << tg.params[0] << "," << tg.params[1] << " )" << endl;
}
//
void ofxPQLabsTouch::OnTG_SecondDown(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_SECOND_DOWN && tg.param_size >= 4);
	cout << "  the second finger touching at :( "
    << tg.params[0] << "," << tg.params[1] << " ),"
    << " after the first finger touched at :( "
    << tg.params[2] << "," << tg.params[3] << " )" << endl;
}
void ofxPQLabsTouch::OnTG_SecondUp(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_SECOND_UP && tg.param_size >= 4);
	cout << "  the second finger is leaving at :( "
    << tg.params[0] << "," << tg.params[1] << " ),"
    << " while the first finger still anchored around :( "
    << tg.params[2] << "," << tg.params[3] << " )" << endl;
}
//
void ofxPQLabsTouch::OnTG_SplitStart(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_SPLIT_START && tg.param_size >= 4);
	cout << "  the two fingers is splitting with one finger at: ( "
    << tg.params[0] << "," << tg.params[1] << " ),"
    << " , the other at :( "
    << tg.params[2] << "," << tg.params[3] << " )" << endl;
}

void ofxPQLabsTouch::OnTG_SplitApart(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_SPLIT_APART && tg.param_size >= 1);
	cout << "  the two fingers is splitting apart with there distance incresed by "
    << tg.params[0]
    << " with a ratio :" << tg.params[1]
    << endl;
}
void ofxPQLabsTouch::OnTG_SplitClose(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_SPLIT_CLOSE && tg.param_size >= 1);
	cout << "  the two fingers is splitting close with there distance decresed by "
    << tg.params[0]
    << " with a ratio :" << tg.params[1]
    << endl;
}
void ofxPQLabsTouch::OnTG_SplitEnd(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_SPLIT_END);
	cout << "  the two splitting fingers with one finger at: ( "
    << tg.params[0] << "," << tg.params[1] << " ),"
    << " , the other at :( "
    << tg.params[2] << "," << tg.params[3] << " )"
    << " will end" << endl;
}
// OnTG_TouchEnd: to clear what need to clear
void ofxPQLabsTouch::OnTG_TouchEnd(const TouchGesture & tg,void * call_object)
{
	assert(tg.type == TG_TOUCH_END);
	cout << "  all the fingers is leaving and there is no fingers on the screen." << endl;
}


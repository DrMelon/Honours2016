#include "ofxFirstPersonCamera.h"

//Filename: ofxFirstPersonCamera.cpp
//Version: N/A
//Author: http://www.github.com/ofnode/ofxFirstPersonCamera
//Date: N/A
//
//Purpose: This is an openFrameworks plugin sourced from the above link, and is used to provide a simple moveable camera control system.
//

ofxFirstPersonCamera::ofxFirstPersonCamera()
	:m_keyState({ 0 })
	, m_isControlled(false)
	, m_isMouseInited(false)
	, upvector(0, 1, 0)
	, movespeed(1.00f)
	, rollspeed(1.00f)
	, sensitivity(0.10f)
	, keyUp(GLFW_KEY_E)
	, keyDown(GLFW_KEY_C)
	, keyLeft(GLFW_KEY_A)
	, keyRight(GLFW_KEY_D)
	, keyForward(GLFW_KEY_W)
	, keyBackward(GLFW_KEY_S)
	, keyRollLeft(GLFW_KEY_Q)
	, keyRollRight(GLFW_KEY_R)
	, keyRollReset(GLFW_KEY_F)
{
	auto &events = ofEvents();
	ofAddListener(events.update, this, &ofxFirstPersonCamera::update, OF_EVENT_ORDER_BEFORE_APP);
	ofAddListener(events.keyPressed, this, &ofxFirstPersonCamera::keyPressed, OF_EVENT_ORDER_BEFORE_APP);
	ofAddListener(events.keyReleased, this, &ofxFirstPersonCamera::keyReleased, OF_EVENT_ORDER_BEFORE_APP);
	ofAddListener(events.mouseMoved, this, &ofxFirstPersonCamera::mouseMoved, OF_EVENT_ORDER_BEFORE_APP);
	ofAddListener(events.mouseDragged, this, &ofxFirstPersonCamera::mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
}

ofxFirstPersonCamera::~ofxFirstPersonCamera()
{
	auto &events = ofEvents();
	ofRemoveListener(events.update, this, &ofxFirstPersonCamera::update, OF_EVENT_ORDER_BEFORE_APP);
	ofRemoveListener(events.keyPressed, this, &ofxFirstPersonCamera::keyPressed, OF_EVENT_ORDER_BEFORE_APP);
	ofRemoveListener(events.keyReleased, this, &ofxFirstPersonCamera::keyReleased, OF_EVENT_ORDER_BEFORE_APP);
	ofRemoveListener(events.mouseMoved, this, &ofxFirstPersonCamera::mouseMoved, OF_EVENT_ORDER_BEFORE_APP);
	ofRemoveListener(events.mouseDragged, this, &ofxFirstPersonCamera::mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
}

bool ofxFirstPersonCamera::isControlled()
{
	return m_isControlled;
}

void ofxFirstPersonCamera::toggleControl()
{
	m_isControlled ? disableControl() : enableControl();
}

void ofxFirstPersonCamera::enableControl()
{
	GLFWwindow *win = static_cast<ofAppGLFWWindow*>(ofGetWindowPtr())->getGLFWWindow();

	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int win_w;
	int win_h;
	glfwGetWindowSize(win, &win_w, &win_h);
	int win_center_x = win_w / 2.0f;
	int win_center_y = win_h / 2.0f;
	glfwSetCursorPos(win, win_center_x, win_center_y);

	m_isControlled = true;
	m_glfwWindow = win;
}

void ofxFirstPersonCamera::disableControl()
{
	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	m_isControlled = false;
}

void ofxFirstPersonCamera::update(ofEventArgs& args)
{
	bool active = m_isControlled;
	if (!active) return;

	Actions key = m_keyState;

	{ // Roll
		bool unroll = key.RollReset;
		int rolldir = key.RollLeft - key.RollRight;

		if (rolldir) {
			float roll = rollspeed;
			float rate = ofGetFrameRate();
			float angle = roll * (60.0f / rate);
			this->roll(rolldir * angle);
			upvector = this->getUpDir();
		}

		if (unroll) {
			this->roll(-this->getRoll());
			upvector = ofVec3f(0, 1, 0);
		}
	}
	{ // Position
		float look = key.Forward - key.Backward;
		float side = key.Right - key.Left;
		float up = key.Up - key.Down;

		if (look != 0 || side != 0 || up != 0)
		{
			ofVec3f lookdir = this->getLookAtDir();
			ofVec3f sidedir = this->getSideDir();
			ofVec3f updir = this->getUpDir();
			float move = movespeed;
			float rate = ofGetFrameRate();
			float speed = move * (60.0f / rate);
			this->move(lookdir * speed * look +
				sidedir * speed * side +
				updir * speed * up);
		}
	}
}

void ofxFirstPersonCamera::nodeRotate(ofMouseEventArgs& mouse)
{
	bool active = m_isControlled;
	if (!active) return;

	int win_w;
	int win_h;
	auto win = m_glfwWindow;
	glfwGetWindowSize(win, &win_w, &win_h);
	int win_center_x = win_w / 2.0f;
	int win_center_y = win_h / 2.0f;

	float mouse_x = mouse.x;
	float mouse_y = mouse.y;
	bool inited = m_isMouseInited;
	if (!inited) {
		// Fixes first mouse move
		mouse_x = win_center_x;
		mouse_y = win_center_y;
		m_isMouseInited = true;
	}

	float sensit = sensitivity;
	float xdiff = (win_center_x - mouse_x) * sensit;
	float ydiff = (win_center_y - mouse_y) * sensit;

	ofVec3f upvec = upvector;
	ofVec3f sidev = this->getSideDir();
	this->rotate(ydiff, sidev);
	this->rotate(xdiff, upvec);

	glfwSetCursorPos(win, win_center_x, win_center_y);
}

void ofxFirstPersonCamera::mouseMoved(ofMouseEventArgs& mouse)
{
	nodeRotate(mouse);
}

void ofxFirstPersonCamera::mouseDragged(ofMouseEventArgs& mouse)
{
	nodeRotate(mouse);
}

void ofxFirstPersonCamera::keyPressed(ofKeyEventArgs& keys)
{
	Actions key = m_keyState;
	int keycode = keys.keycode;

	if (keycode == keyUp) key.Up = 1;
	else if (keycode == keyDown) key.Down = 1;
	else if (keycode == keyLeft) key.Left = 1;
	else if (keycode == keyRight) key.Right = 1;
	else if (keycode == keyForward) key.Forward = 1;
	else if (keycode == keyBackward) key.Backward = 1;

	else if (keycode == keyRollLeft) key.RollLeft = 1;
	else if (keycode == keyRollRight) key.RollRight = 1;
	else if (keycode == keyRollReset) key.RollReset = 1;

	m_keyState = key;
}

void ofxFirstPersonCamera::keyReleased(ofKeyEventArgs& keys)
{
	Actions key = m_keyState;
	int keycode = keys.keycode;

	if (keycode == keyUp) key.Up = 0;
	else if (keycode == keyDown) key.Down = 0;
	else if (keycode == keyLeft) key.Left = 0;
	else if (keycode == keyRight) key.Right = 0;
	else if (keycode == keyForward) key.Forward = 0;
	else if (keycode == keyBackward) key.Backward = 0;

	else if (keycode == keyRollLeft) key.RollLeft = 0;
	else if (keycode == keyRollRight) key.RollRight = 0;
	else if (keycode == keyRollReset) key.RollReset = 0;

	m_keyState = key;
}
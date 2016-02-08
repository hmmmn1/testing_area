#include <emscripten.h>
#include <string.h>
#include <emscripten/html5.h>
#include <stdio.h>
#include <vector.h>

int w; int h;
std::vector<EmscriptenKeyboardEvent> unusued_keys;
int mousex = 0;
int mousey = 0;


static inline const char *emscripten_event_type_to_string(int eventType) {
	const char *events[] = { "(invalid)", "(none)", "keypress", "keydown", "keyup", "click", "mousedown", "mouseup", "dblclick", "mousemove", "wheel", "resize", "scroll", "blur", "focus", "focusin", "focusout", "deviceorientation", "devicemotion", "orientationchange", "fullscreenchange", "pointerlockchange", "visibilitychange", "touchstart", "touchend", "touchmove", "touchcancel", "gamepadconnected", "gamepaddisconnected", "beforeunload", "batterychargingchange", "batterylevelchange", "webglcontextlost", "webglcontextrestored", "mouseenter", "mouseleave", "mouseover", "mouseout", "(invalid)" };
	++eventType;
	if (eventType < 0) eventType = 0;
	if (eventType >= int(sizeof(events)/sizeof(events[0]))) eventType = sizeof(events)/sizeof(events[0])-1;
	return events[eventType];
}

// The event handler functions can return 1 to suppress the event and disable the default action. That calls event.preventDefault();
// Returning 0 signals that the event was not consumed by the code, and will allow the event to pass on and bubble up normally.
EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
{
//	printf("%s, key: \"%s\", code: \"%s\", location: %lu,%s%s%s%s repeat: %d, locale: \"%s\", char: \"%s\", charCode: %lu, keyCode: %lu, which: %lu\n", emscripten_event_type_to_string(eventType), e->key, e->code, e->location, e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "", e->repeat, e->locale, e->charValue, e->charCode, e->keyCode, e->which);

	return 0;
}

EM_BOOL unlocked_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
	if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN && (e->canvasX>=0) && (e->canvasX<=w) && (e->canvasY>=0) && (e->canvasY<=h)){
		EmscriptenPointerlockChangeEvent plce;
		EMSCRIPTEN_RESULT ret = emscripten_get_pointerlock_status(&plce);
		if (!plce.isActive) {
			printf("Requesting pointer lock..\n");
			EMSCRIPTEN_RESULT ret;
			ret = emscripten_request_pointerlock(0, 1);
		}
	}
	return 0;
}
EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
//	printf("%s, screen: (%ld,%ld), client: (%ld,%ld),%s%s%s%s button: %hu, buttons: %hu, movement: (%ld,%ld), canvas: (%ld,%ld)\n", emscripten_event_type_to_string(eventType), e->screenX, e->screenY, e->clientX, e->clientY, e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "", e->button, e->buttons, e->movementX, e->movementY, e->canvasX, e->canvasY);
	return 0;
}

EM_BOOL wheel_callback(int eventType, const EmscriptenWheelEvent *e, void *userData)
{
//	printf("%s, screen: (%ld,%ld), client: (%ld,%ld),%s%s%s%s button: %hu, buttons: %hu, canvas: (%ld,%ld), delta:(%g,%g,%g), deltaMode:%lu\n", emscripten_event_type_to_string(eventType), e->mouse.screenX, e->mouse.screenY, e->mouse.clientX, e->mouse.clientY, e->mouse.ctrlKey ? " CTRL" : "", e->mouse.shiftKey ? " SHIFT" : "", e->mouse.altKey ? " ALT" : "", e->mouse.metaKey ? " META" : "", e->mouse.button, e->mouse.buttons, e->mouse.canvasX, e->mouse.canvasY, (float)e->deltaX, (float)e->deltaY, (float)e->deltaZ, e->deltaMode);

	return 0;
}

EM_BOOL pointerlockchange_callback(int eventType, const EmscriptenPointerlockChangeEvent *e, void *userData)
{
	printf("%s, isActive: %d, pointerlock element nodeName: \"%s\", id: \"%s\"\n",
		emscripten_event_type_to_string(eventType), e->isActive, e->nodeName, e->id);
	return 0;
}

int init_keyboard_and_mouse(int width, int height)
{
	w = width; h = height;
	EMSCRIPTEN_RESULT ret;
// EMSCRIPTEN_RESULT_NOT_SUPPORTED
	ret = emscripten_set_keypress_callback(0, 0, 1, key_callback);
	ret = emscripten_set_keydown_callback(0, 0, 1, key_callback);
	ret = emscripten_set_keyup_callback(0, 0, 1, key_callback);

	EmscriptenPointerlockChangeEvent plce;
	ret = emscripten_set_pointerlockchange_callback(0, 0, 1, pointerlockchange_callback);
	ret = emscripten_get_pointerlock_status(&plce);

	ret = emscripten_set_click_callback(0, 0, 1, mouse_callback);
	ret = emscripten_set_mousedown_callback(0, 0, 1, mouse_callback);
	ret = emscripten_set_mouseup_callback(0, 0, 1, mouse_callback);
	ret = emscripten_set_dblclick_callback(0, 0, 1, mouse_callback);
	ret = emscripten_set_mousemove_callback(0, 0, 1, mouse_callback);

	ret = emscripten_set_mousedown_callback(0, 0, 1, unlocked_callback);

	ret = emscripten_set_wheel_callback(0, 0, 1, wheel_callback);

	return 0;
}

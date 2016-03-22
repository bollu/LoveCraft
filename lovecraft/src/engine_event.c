#include "engine_event.h"
Event new_event_none() {
    Event e;
    e.type = EventNone;
    return e;
}

Event new_event_simple(EventType t) {
    Event e;
    e.type = t;
    return e;
}

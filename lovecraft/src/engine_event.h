//
// Created by Siddharth Bhat on 17/03/16.
//
#pragma once
struct Phase;
typedef struct Phase Phase;

typedef enum {
    EventNone,
    EventQuit,
    EventChangePhase,
} EventType;

typedef struct ChangePhaseData {
    Phase* new_phase;
} ChangePhaseData;

typedef union EventData {
    ChangePhaseData change_phase_data;
} EventData;

typedef union {
    EventType type;
    EventData data;
} Event;

Event new_event_none();
Event new_event_simple(EventType t);
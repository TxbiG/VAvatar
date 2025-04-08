#pragma once

#include <stdbool.h>

typedef struct {
    float elapsedTime;
    float duration;
    bool playing;
    float currentValue;
} Tween;

Tween tween_callback(Tween tween, Callable callback);
Tween tween_interval(Tween tween, float time);
Tween tween_method(Tween tween, Callable method, Variant from, Variant to, float duration);
Tween tween_property(Tween tween, Object* object, NodePath property, Variant final_val, float duration);

// void bind_node(Node* node);

// Tween chain();

// bool custom_step(float delta);

// int get_loops_left(const Tween* tween);

// float get_total_elapsed_time(const Tween* tween);

Variant interpolate_value(Variant initial_value, Variant delta_value, float elapsed_time, float duration, TransitionType trans_type, EaseType ease_type);

// bool is_running(const Tween* tween);

// bool is_valid(const Tween* tween);

// void kill(Tween* tween);

// Tween parallel();

// void pause(Tween* tween);

// void play(Tween* tween);

Tween set_ease(Tween tween, EaseType ease);

// Tween set_loops(Tween tween, int loops);

// Tween set_parallel(Tween tween, bool parallel);

// Tween set_pause_mode(Tween tween, TweenPauseMode pause_mode);

// Tween set_process_mode(Tween tween, TweenProcessMode process_mode);

// Tween set_speed_scale(Tween tween, float speed);

// Tween set_trans(Tween tween, TransitionType trans);

void stop(Tween* tween);

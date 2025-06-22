#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>

void update_signal_sin(float *signal, size_t signal_cap, float freq, float sample_duration) {
        for (size_t i = 0; i < signal_cap; ++i) {
                signal[i] = sinf(2.f * M_PI * freq * sample_duration * (float)i) * M_PI;
        }
}

void update_signal_tan(float *signal, size_t signal_cap, float freq, float sample_duration) {
        for (size_t i = 0; i < signal_cap; ++i) {
                signal[i] = tanf((float)i) * cosf(2.f * M_PI * freq * sample_duration * (float)i) * M_PI;
        }
}

typedef struct {
        Rectangle bounds;
        float min_value;
        float max_value;
        float value;
        bool is_dragging;
} Slider;

float update_slider(Slider *slider, Vector2 mouse_pos) {
        if (CheckCollisionPointRec(mouse_pos, slider->bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                slider->is_dragging = true;
                float x = mouse_pos.x - slider->bounds.x;
                float ratio = x / slider->bounds.width;
                ratio = Clamp(ratio, 0.0f, 1.0f);
                slider->value = slider->min_value + (slider->max_value - slider->min_value) * ratio;
        }

        if (slider->is_dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float x = mouse_pos.x - slider->bounds.x;
                float ratio = x / slider->bounds.width;
                ratio = Clamp(ratio, 0.0f, 1.0f);
                slider->value = slider->min_value + (slider->max_value - slider->min_value) * ratio;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                slider->is_dragging = false;
        }

        return slider->value;
}

void draw_slider(Slider slider) {
        DrawRectangleRec(slider.bounds, GRAY);
        float ratio = (slider.value - slider.min_value) / (slider.max_value - slider.min_value);
        float handle_x = slider.bounds.x + (slider.bounds.width * ratio) - 5;
        Rectangle handle = { handle_x, slider.bounds.y - 5, 10, slider.bounds.height + 10 };
        DrawRectangleRec(handle, slider.is_dragging ? RED : LIGHTGRAY);
}

int main(void) {
        #define screen_width 1920
        #define screen_height 1080

        InitWindow(screen_width, screen_height, "Dots");
        SetTargetFPS(60);

        size_t signal_cap = screen_width;
        float signal_sin[screen_width] = {0};
        float signal_tan[screen_width] = {0};
        float freq = 0.01f;
        unsigned int sample_rate = 10000;
        float sample_duration = 1.f / sample_rate;

        Slider freq_increment_slider = {
                .bounds = { 100, 50, 300, 20 },
                .min_value = 0.001f,
                .max_value = 1.0f,
                .value = freq,
                .is_dragging = false
        };

        Slider sample_rate_slider = {
                .bounds = { 100, 100, 300, 20 },
                .min_value = 0.01f,
                .max_value = 10000.0f,
                .value = (float)sample_rate,
                .is_dragging = false
        };

        while (!WindowShouldClose()) {
                Vector2 mouse_pos = GetMousePosition();

                float freq_increment = update_slider(&freq_increment_slider, mouse_pos);
                sample_rate = (unsigned int)update_slider(&sample_rate_slider, mouse_pos);
                sample_duration = 1.f / sample_rate;

                freq += freq_increment;

                if (freq > 10000.0f) {
                        freq = 0.0f;
                }

                update_signal_sin(signal_sin, signal_cap, freq, sample_duration);
                update_signal_tan(signal_tan, signal_cap, freq, sample_duration);

                // Keep sliders in sync
                sample_rate_slider.value = (float)sample_rate;

                BeginDrawing();
                ClearBackground(BLACK);

                draw_slider(freq_increment_slider);
                draw_slider(sample_rate_slider);

                DrawText(TextFormat("Freq: %.2f", freq), 410, 50, 20, WHITE);
                DrawText(TextFormat("Freq Increment: %.2f", freq_increment), 410, 70, 20, WHITE);
                DrawText(TextFormat("Sample Rate: %u", sample_rate), 410, 100, 20, WHITE);

                for (size_t i = 0; i < signal_cap; ++i) {
                        Color c = i % 2 == 0 ? PINK : BLUE;
                        /* DrawPixel(i * cosf((float)i), (screen_height/2) + (int)(signal_sin[i] * 100), c); */
                        /* DrawPixel(i * cosf((float)i), (screen_height/2) + (int)(signal_tan[i] * 100), c); */
                        DrawPixel(i * tanf((float)i), (screen_height/2) + (int)(signal_sin[i] * 100), c);
                        DrawPixel(i * tanf((float)i), (screen_height/2) + (int)(signal_tan[i] * 100), c);
                }

                EndDrawing();
        }

        CloseWindow();
        return 0;
}

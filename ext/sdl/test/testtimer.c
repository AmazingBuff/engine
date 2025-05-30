/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/

/* Test program to check the resolution of the SDL timer on the current
   platform
*/
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_test.h>

#define DEFAULT_RESOLUTION 1

static int test_sdl_delay_within_bounds(void) {
    const int testDelay = 100;
    const int marginOfError = 25;
    Uint64 result;
    Uint64 result2;
    Sint64 difference;

    SDLTest_ResetAssertSummary();

    /* Get ticks count - should be non-zero by now */
    result = SDL_GetTicks();
    SDLTest_AssertPass("Call to SDL_GetTicks()");
    SDLTest_AssertCheck(result > 0, "Check result value, expected: >0, got: %" SDL_PRIu64, result);

    /* Delay a bit longer and measure ticks and verify difference */
    SDL_Delay(testDelay);
    SDLTest_AssertPass("Call to SDL_Delay(%d)", testDelay);
    result2 = SDL_GetTicks();
    SDLTest_AssertPass("Call to SDL_GetTicks()");
    SDLTest_AssertCheck(result2 > 0, "Check result value, expected: >0, got: %" SDL_PRIu64, result2);
    difference = result2 - result;
    SDLTest_AssertCheck(difference > (testDelay - marginOfError), "Check difference, expected: >%d, got: %" SDL_PRIu64, testDelay - marginOfError, difference);
    /* Disabled because this might fail on non-interactive systems. */
    SDLTest_AssertCheck(difference < (testDelay + marginOfError), "Check difference, expected: <%d, got: %" SDL_PRIu64, testDelay + marginOfError, difference);

    return SDLTest_AssertSummaryToTestResult() == TEST_RESULT_PASSED ? 0 : 1;
}

static int ticks = 0;

static Uint32 SDLCALL
ticktock(void *param, SDL_TimerID timerID, Uint32 interval)
{
    ++ticks;
    return interval;
}

static Uint64 SDLCALL
ticktockNS(void *param, SDL_TimerID timerID, Uint64 interval)
{
    ++ticks;
    return interval;
}

static Uint32 SDLCALL
callback(void *param, SDL_TimerID timerID, Uint32 interval)
{
    int value = (int)(uintptr_t)param;
    SDL_assert( value == 1 || value == 2 || value == 3 );
    SDL_Log("Timer %" SDL_PRIu32 " : param = %d\n", interval, value);
    return interval;
}

int main(int argc, char *argv[])
{
    int i;
    int desired = -1;
    SDL_TimerID t1, t2, t3;
    Uint64 start, now;
    Uint64 start_perf, now_perf;
    SDLTest_CommonState  *state;
    bool run_interactive_tests = true;
    int return_code = 0;

    /* Initialize test framework */
    state = SDLTest_CommonCreateState(argv, 0);
    if (!state) {
        return 1;
    }

    /* Parse commandline */
    for (i = 1; i < argc;) {
        int consumed;

        consumed = SDLTest_CommonArg(state, i);
        if (!consumed) {
            if (SDL_strcmp(argv[i], "--no-interactive") == 0) {
                run_interactive_tests = false;
                consumed = 1;
            } else if (desired < 0) {
                char *endptr;

                desired = SDL_strtoul(argv[i], &endptr, 0);
                if (desired != 0 && endptr != argv[i] && *endptr == '\0') {
                    consumed = 1;
                }
            }
        }
        if (consumed <= 0) {
            static const char *options[] = { "[--no-interactive]", "[interval]", NULL };
            SDLTest_CommonLogUsage(state, argv[0], options);
            return 1;
        }

        i += consumed;
    }

    if (SDL_GetEnvironmentVariable(SDL_GetEnvironment(), "SDL_TESTS_QUICK") != NULL) {
        SDL_Log("Not running slower tests");
        SDL_Quit();
        return 0;
    }

    /* Verify SDL_GetTicks* acts monotonically increasing, and not erratic. */
    SDL_Log("Sanity-checking GetTicks\n");
    for (i = 0; i < 1000; ++i) {
        start = SDL_GetTicks();
        SDL_Delay(1);
        now = SDL_GetTicks() - start;
        if (now > 100) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "testtimer.c: Delta time erratic at iter %d. Delay 1ms = %d ms in ticks\n", i, (int)now);
            SDL_Quit();
            return 1;
        }
    }

    /* Start the millisecond timer */
    if (desired < 0) {
        desired = DEFAULT_RESOLUTION;
    }
    ticks = 0;
    t1 = SDL_AddTimer(desired, ticktock, NULL);

    /* Wait 1 seconds */
    SDL_Log("Waiting 1 seconds for millisecond timer\n");
    SDL_Delay(1 * 1000);

    /* Stop the timer */
    SDL_RemoveTimer(t1);

    /* Print the results */
    if (ticks) {
        SDL_Log("Millisecond timer resolution: desired = %d ms, actual = %f ms\n",
                desired, (double)(10 * 1000) / ticks);
    }

    /* Wait for the results to be seen */
    SDL_Delay(1 * 1000);

    /* Start the nanosecond timer */
    ticks = 0;
    t1 = SDL_AddTimerNS(desired, ticktockNS, NULL);

    /* Wait 1 seconds */
    SDL_Log("Waiting 1 seconds for nanosecond timer\n");
    SDL_Delay(1 * 1000);

    /* Stop the timer */
    SDL_RemoveTimer(t1);

    /* Print the results */
    if (ticks) {
        SDL_Log("Nanosecond timer resolution: desired = %d ns, actual = %f ns\n",
                desired, (double)(10 * 1000000) / ticks);
    }

    /* Wait for the results to be seen */
    SDL_Delay(1 * 1000);

    /* Check accuracy of nanosecond delay */
    {
        Uint64 desired_delay = SDL_NS_PER_SECOND / 60;
        Uint64 actual_delay;
        Uint64 total_overslept = 0;

        start = SDL_GetTicksNS();
        SDL_DelayNS(1);
        now = SDL_GetTicksNS();
        actual_delay = (now - start);
        SDL_Log("Minimum nanosecond delay: %" SDL_PRIu64 " ns\n", actual_delay);

        SDL_Log("Timing 100 frames at 60 FPS\n");
        for (i = 0; i < 100; ++i) {
            start = SDL_GetTicksNS();
            SDL_DelayNS(desired_delay);
            now = SDL_GetTicksNS();
            actual_delay = (now - start);
            if (actual_delay > desired_delay) {
                total_overslept += (actual_delay - desired_delay);
            }
        }
        SDL_Log("Overslept %.2f ms\n", (double)total_overslept / SDL_NS_PER_MS);
    }

    /* Wait for the results to be seen */
    SDL_Delay(1 * 1000);

    /* Check accuracy of precise delay */
    {
        Uint64 desired_delay = SDL_NS_PER_SECOND / 60;
        Uint64 actual_delay;
        Uint64 total_overslept = 0;

        start = SDL_GetTicksNS();
        SDL_DelayPrecise(1);
        now = SDL_GetTicksNS();
        actual_delay = (now - start);
        SDL_Log("Minimum precise delay: %" SDL_PRIu64 " ns\n", actual_delay);

        SDL_Log("Timing 100 frames at 60 FPS\n");
        for (i = 0; i < 100; ++i) {
            start = SDL_GetTicksNS();
            SDL_DelayPrecise(desired_delay);
            now = SDL_GetTicksNS();
            actual_delay = (now - start);
            if (actual_delay > desired_delay) {
                total_overslept += (actual_delay - desired_delay);
            }
        }
        SDL_Log("Overslept %.2f ms\n", (double)total_overslept / SDL_NS_PER_MS);
    }

    /* Wait for the results to be seen */
    SDL_Delay(1 * 1000);

    /* Test multiple timers */
    SDL_Log("Testing multiple timers...\n");
    t1 = SDL_AddTimer(100, callback, (void *)1);
    if (!t1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create timer 1: %s\n", SDL_GetError());
    }
    t2 = SDL_AddTimer(50, callback, (void *)2);
    if (!t2) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create timer 2: %s\n", SDL_GetError());
    }
    t3 = SDL_AddTimer(233, callback, (void *)3);
    if (!t3) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create timer 3: %s\n", SDL_GetError());
    }

    /* Wait 3 seconds */
    SDL_Log("Waiting 3 seconds\n");
    SDL_Delay(3 * 1000);

    SDL_Log("Removing timer 1 and waiting 3 more seconds\n");
    SDL_RemoveTimer(t1);

    SDL_Delay(3 * 1000);

    SDL_RemoveTimer(t2);
    SDL_RemoveTimer(t3);

    ticks = 0;
    start_perf = SDL_GetPerformanceCounter();
    for (i = 0; i < 1000000; ++i) {
        ticktock(NULL, 0, 0);
    }
    now_perf = SDL_GetPerformanceCounter();
    SDL_Log("1 million iterations of ticktock took %f ms\n", (double)((now_perf - start_perf) * 1000) / SDL_GetPerformanceFrequency());

    SDL_Log("Performance counter frequency: %" SDL_PRIu64 "\n", SDL_GetPerformanceFrequency());
    start = SDL_GetTicks();
    start_perf = SDL_GetPerformanceCounter();
    SDL_Delay(1000);
    now_perf = SDL_GetPerformanceCounter();
    now = SDL_GetTicks();
    SDL_Log("Delay 1 second = %d ms in ticks, %f ms according to performance counter\n", (int)(now - start), (double)((now_perf - start_perf) * 1000) / SDL_GetPerformanceFrequency());

    if (run_interactive_tests) {
        return_code = test_sdl_delay_within_bounds();
    }

    SDL_Quit();
    SDLTest_CommonDestroyState(state);
    return return_code;
}

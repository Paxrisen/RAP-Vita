// RAP-Vita debug log (written directly via sceIo): ux0:data/Raptor/log.txt
#ifdef __vita__
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <SDL.h>
#include <psp2/io/stat.h>
#include <psp2/io/fcntl.h>
#ifndef RAPVITA_VERSION
#define RAPVITA_VERSION "?"
#endif
static SceUID fd1 = -1, fd2 = -1;
static SDL_mutex *mtx;
static const char *volatile vlast = "start";
volatile unsigned vita_frames, vita_events;
static void wr(const char *b) {
    int n = strlen(b);
    if (mtx) SDL_LockMutex(mtx);
    if (fd1 >= 0) sceIoWrite(fd1, b, n);
    if (fd2 >= 0) sceIoWrite(fd2, b, n);
    if (mtx) SDL_UnlockMutex(mtx);
}
extern "C" void vita_logf(const char *fmt, ...) {
    if (fd1 < 0) return;
    char b[512]; int o = snprintf(b, sizeof b, "[%u ms] ", SDL_WasInit(0) ? SDL_GetTicks() : 0);
    va_list a; va_start(a, fmt); vsnprintf(b + o, sizeof b - o - 2, fmt, a); va_end(a);
    strcat(b, "\n"); wr(b);
}
static int hb(void *) {
    unsigned lf = 0, le = 0;
    for (;;) {
        SDL_Delay(2000);
        vita_logf("[hb] frames=%u(+%u) events=%u(+%u) zuletzt=%s", vita_frames, vita_frames - lf, vita_events, vita_events - le, vlast);
        lf = vita_frames; le = vita_events;
    }
    return 0;
}
int vita_debug_log = 0;
extern "C" void vita_log_open(void) {
    if (fd1 >= 0) return;
    sceIoMkdir("ux0:data", 0777);
    sceIoMkdir("ux0:data/Raptor", 0777);
    fd1 = sceIoOpen("ux0:data/Raptor/log.txt", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    wr("RAP-Vita " RAPVITA_VERSION " Log\n");
    static int hbs; if (!hbs) { hbs = 1; mtx = SDL_CreateMutex(); SDL_CreateThread(hb, "hb", NULL); }
}
extern "C" void vita_log_init_unused(void) {
    sceIoMkdir("ux0:data", 0777);
    sceIoMkdir("ux0:data/Raptor", 0777);
    fd1 = sceIoOpen("ux0:data/Raptor/log.txt", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    wr("RAP-Vita " RAPVITA_VERSION " Log\n");
}
extern "C" void vita_log_init(void) {}
extern "C" void vita_log_start_hb(void) {}
extern "C" void vita_trace(const char *s) {
    if (fd1 < 0) return;
    if (s == vlast || (vlast && !strcmp(s, vlast))) return;
    vlast = s;
    static int n; if (++n > 3000) return;
    vita_logf("%s", s);
}
#endif

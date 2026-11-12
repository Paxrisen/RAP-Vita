/* RAP-Vita: reliable access() via stat() (newlib access() is unreliable on the Vita) */
#ifndef VITA_COMPAT_H
#define VITA_COMPAT_H
#ifdef __vita__
#include <sys/stat.h>
#include <unistd.h>
static inline int vita_access(const char *p, int m) { (void)m; struct stat st; return stat(p, &st) == 0 ? 0 : -1; }
#define access(p, m) vita_access((p), (m))
#endif
#endif
#ifdef __vita__
#ifdef __cplusplus
extern "C" {
#endif
void vita_log_init(void); void vita_log_start_hb(void); void vita_trace(const char *s); void vita_logf(const char *fmt, ...);
#ifdef __cplusplus
}
#endif
extern volatile unsigned vita_frames, vita_events;
extern int vita_stretch, vita_touch_on, vita_menu_open, vita_lang, vita_rotate;
void vita_menu_run(void);
int vita_keyboard_run(int which, char *buf, int maxlen);
void vita_screen_to_game(float fx, float fy, int *gx, int *gy);
int vita_eff_rot(void);
#define VTRACE(s) vita_trace(s)
#else
#define VTRACE(s)
#endif

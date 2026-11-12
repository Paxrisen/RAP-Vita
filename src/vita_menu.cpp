// RAP-Vita: own menus (settings + button mapping), EN/DE. Open with SELECT.
#ifdef __vita__
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include "prefapi.h"
typedef struct { const char *name; const uint8_t *data; unsigned w, h; } txt_font_t;
#include "../include/textscreen/include/fonts/normal.h"

extern "C" void vita_logf(const char *fmt, ...);
extern int j_lookup[4];
extern void MUS_Poll(void);
void I_VitaGetRender(SDL_Renderer **r, SDL_Texture **t, int *gw, int *gh);

int vita_lang = 0;        // 0 German, 1 English
int vita_ptr_speed = 1;   // 0 slow, 1 medium, 2 fast
int vita_touch_on = 1;    // front touch
int vita_stretch = 0;     // 0 = 4:3, 1 = wide, 2 = fullscreen
int vita_rstick = 1;      // right stick as pointer
int vita_menu_open = 0;
int vita_touch_game = 0;  // 0 off, 1 steer, 2 steer + fire
int vita_dpad_sens = 2;   // 0..4
int vita_rotate = 0;      // 0 landscape, 1 portrait (right), 2 portrait (left)
int vita_rot_level_only = 1; // portrait only during levels
int vita_keys[3][4] = { { 0, 1, 2, 3 }, { 10, 9, 13, 14 }, { 9, 10, 1, 2 } };
int vita_movestick[3] = { 0, 0, 1 };   // 0 left, 1 right
static const int def_keys[3][4] = { { 0, 1, 2, 3 }, { 10, 9, 13, 14 }, { 9, 10, 1, 2 } };
static const int def_stick[3] = { 0, 0, 1 };
static int kset;
// effective orientation: portrait possibly only during a level
int vita_eff_rot(void)
{
    extern int do_game;
    if (!vita_rotate) return 0;
    if (vita_rot_level_only && !do_game) return 0;
    return vita_rotate;
}
// activate the button layout for the effective orientation
void vita_sync_controls(void)
{
    int o = vita_eff_rot();
    for (int i = 0; i < 4; i++) j_lookup[i] = vita_keys[o][i];
}
extern int vita_debug_log;
extern "C" void vita_log_open(void);
int vita_screenshot(void);
void vita_game_rect(SDL_Rect *d, double *ang);
static int shot_msg;

static SDL_Texture *font_tex;

void vita_settings_load(void)
{
    vita_lang = INI_GetPreferenceLong("Vita", "Language", 0);
    vita_ptr_speed = INI_GetPreferenceLong("Vita", "PointerSpeed", 1);
    vita_touch_on = INI_GetPreferenceLong("Vita", "FrontTouch", 1);
    vita_stretch = INI_GetPreferenceLong("Vita", "Fullscreen", 0) % 3;
    vita_rot_level_only = INI_GetPreferenceLong("Vita", "PortraitLevelOnly", 1);
    static const char *sec[3] = { "Keys0", "Keys1", "Keys2" };
    if (INI_GetPreferenceLong("Vita", "KeysVersion", 0) < 1) {
        // v1.0: apply new default portrait layouts once
        for (int o = 1; o < 3; o++) {
            INI_PutPreferenceLong(sec[o], "Fire", def_keys[o][0]); INI_PutPreferenceLong(sec[o], "FireSp", def_keys[o][1]);
            INI_PutPreferenceLong(sec[o], "ChangeSp", def_keys[o][2]); INI_PutPreferenceLong(sec[o], "MegaFire", def_keys[o][3]);
            INI_PutPreferenceLong(sec[o], "MoveStick", def_stick[o]);
        }
        INI_PutPreferenceLong("Vita", "KeysVersion", 1);
    }
    for (int o = 0; o < 3; o++) {
        vita_keys[o][0] = INI_GetPreferenceLong(sec[o], "Fire", def_keys[o][0]);
        vita_keys[o][1] = INI_GetPreferenceLong(sec[o], "FireSp", def_keys[o][1]);
        vita_keys[o][2] = INI_GetPreferenceLong(sec[o], "ChangeSp", def_keys[o][2]);
        vita_keys[o][3] = INI_GetPreferenceLong(sec[o], "MegaFire", def_keys[o][3]);
        vita_movestick[o] = INI_GetPreferenceLong(sec[o], "MoveStick", def_stick[o]);
    }
    vita_rstick = INI_GetPreferenceLong("Vita", "RightStickPointer", 1);
    vita_touch_game = INI_GetPreferenceLong("Vita", "TouchIngame", 0);
    vita_dpad_sens = INI_GetPreferenceLong("Vita", "DPadSensitivity", 2);
    vita_rotate = INI_GetPreferenceLong("Vita", "Orientation", 0);
    vita_debug_log = INI_GetPreferenceLong("Vita", "DebugLog", 0);
    if (vita_debug_log) vita_log_open();
}
static void save(void)
{
    INI_PutPreferenceLong("Vita", "Language", vita_lang);
    INI_PutPreferenceLong("Vita", "PointerSpeed", vita_ptr_speed);
    INI_PutPreferenceLong("Vita", "FrontTouch", vita_touch_on);
    INI_PutPreferenceLong("Vita", "Fullscreen", vita_stretch);
    INI_PutPreferenceLong("Vita", "RightStickPointer", vita_rstick);
    INI_PutPreferenceLong("Vita", "TouchIngame", vita_touch_game);
    INI_PutPreferenceLong("Vita", "DPadSensitivity", vita_dpad_sens);
    INI_PutPreferenceLong("Vita", "Orientation", vita_rotate);
    INI_PutPreferenceLong("Vita", "DebugLog", vita_debug_log);
    INI_PutPreferenceLong("Vita", "PortraitLevelOnly", vita_rot_level_only);
    static const char *sec[3] = { "Keys0", "Keys1", "Keys2" };
    for (int o = 0; o < 3; o++) {
        INI_PutPreferenceLong(sec[o], "Fire", vita_keys[o][0]);
        INI_PutPreferenceLong(sec[o], "FireSp", vita_keys[o][1]);
        INI_PutPreferenceLong(sec[o], "ChangeSp", vita_keys[o][2]);
        INI_PutPreferenceLong(sec[o], "MegaFire", vita_keys[o][3]);
        INI_PutPreferenceLong(sec[o], "MoveStick", vita_movestick[o]);
    }
    vita_sync_controls();
}
int vita_ptr_range(void) { static const int r[3] = { 2, 3, 5 }; return r[vita_ptr_speed % 3]; }

// ---------- drawing ----------
static void make_font(SDL_Renderer *r)
{
    if (font_tex) return;
    static Uint32 px[128 * 256];
    for (int g = 0; g < 256; g++)
        for (int y = 0; y < 16; y++) {
            unsigned char b = normal_font_data[g * 16 + y];
            for (int x = 0; x < 8; x++)
                px[((g / 16) * 16 + y) * 128 + (g % 16) * 8 + x] = (b & (0x01 << x)) ? 0xFFFFFFFF : 0;
        }
    font_tex = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 128, 256);
    SDL_UpdateTexture(font_tex, NULL, px, 128 * 4);
    SDL_SetTextureBlendMode(font_tex, SDL_BLENDMODE_BLEND);
}
// UTF-8 (umlauts only) -> CP437
static int cp(const unsigned char *&s)
{
    unsigned c = *s++;
    if (c == 0xC3) { c = *s++;
        switch (c) { case 0xA4: return 0x84; case 0xB6: return 0x94; case 0xBC: return 0x81; case 0x84: return 0x8E;
                     case 0x96: return 0x99; case 0x9C: return 0x9A; case 0x9F: return 0xE1; } return '?'; }
    return c;
}
static void text(SDL_Renderer *r, int x, int y, const char *t, Uint8 cr, Uint8 cg, Uint8 cb, int sc = 2)
{
    SDL_SetTextureColorMod(font_tex, cr, cg, cb);
    const unsigned char *s = (const unsigned char *)t;
    while (*s) {
        int g = cp(s);
        SDL_Rect src = { (g % 16) * 8, (g / 16) * 16, 8, 16 }, dst = { x, y, 8 * sc, 16 * sc };
        SDL_RenderCopy(r, font_tex, &src, &dst);
        x += 8 * sc;
    }
}
static int tlen(const char *t) { int n = 0; const unsigned char *s = (const unsigned char *)t; while (*s) { cp(s); n++; } return n; }

// ---------- content ----------
#define L(de, en) (vita_lang ? (en) : (de))
static const char *btn_name(int b)
{
    switch (b) {
    case 0: return L("Kreuz", "Cross"); case 1: return L("Kreis", "Circle");
    case 2: return L("Quadrat", "Square"); case 3: return L("Dreieck", "Triangle");
    case 6: return "Start"; case 9: return "L"; case 10: return "R";
    case 11: return "D-Pad \x18"; case 12: return "D-Pad \x19";
    case 13: return "D-Pad \x1b"; case 14: return "D-Pad \x1a";
    }
    static char b2[16]; snprintf(b2, sizeof b2, "#%d", b); return b2;
}

enum { PAGE_MAIN, PAGE_SET, PAGE_KEYS };
struct Item { char label[64]; char value[48]; };
#define MAXI 12

static int build(int page, Item *it)
{
    int n = 0;
    auto add = [&](const char *l, const char *v) { snprintf(it[n].label, 64, "%s", l); snprintf(it[n].value, 48, "%s", v ? v : ""); n++; };
    if (page == PAGE_MAIN) {
        add(L("Einstellungen", "Settings"), ">");
        add(L("Tastenbelegung", "Button mapping"), ">");
        add(L("Screenshot speichern", "Save screenshot"), shot_msg == 1 ? L("gespeichert!", "saved!") : shot_msg == 2 ? L("Fehler", "error") : "R");
        add(L("Zurück zum Spiel", "Back to game"), "");
    } else if (page == PAGE_SET) {
        add(L("Sprache", "Language"), vita_lang ? "English" : "Deutsch");
        const char *sp[3] = { L("langsam", "slow"), L("mittel", "medium"), L("schnell", "fast") };
        add(L("Zeiger-Tempo", "Pointer speed"), sp[vita_ptr_speed % 3]);
        add(L("Rechter Stick als Zeiger", "Right stick as pointer"), vita_rstick ? L("an", "on") : L("aus", "off"));
        add(L("Touch (vorne)", "Touch (front)"), vita_touch_on ? L("an", "on") : L("aus", "off"));
        const char *tg[3] = { L("aus", "off"), L("steuern", "steer"), L("steuern+feuern", "steer+fire") };
        add(L("Touch im Spiel", "Touch in game"), tg[vita_touch_game % 3]);
        char ds[8]; snprintf(ds, sizeof ds, "%d", vita_dpad_sens + 1);
        add(L("D-Pad-Empfindlichkeit", "D-Pad sensitivity"), ds);
        const char *bf[3] = { "4:3", L("breit", "wide"), L("Vollbild", "Fullscreen") };
        add(L("Bildformat", "Screen format"), bf[vita_stretch % 3]);
        const char *ro[3] = { L("quer", "landscape"), L("hochkant R", "portrait R"), L("hochkant L", "portrait L") };
        add(L("Ausrichtung", "Orientation"), ro[vita_rotate % 3]);
        add(L("Hochkant nur im Level", "Portrait only in level"), vita_rot_level_only ? L("ja", "yes") : L("nein", "no"));
        add(L("Debug-Log", "Debug log"), vita_debug_log ? L("an", "on") : L("aus", "off"));
        add(L("Zurück", "Back"), "");
    } else {
        const char *ro[3] = { L("quer", "landscape"), L("hochkant R", "portrait R"), L("hochkant L", "portrait L") };
        add(L("Belegung für", "Mapping for"), ro[kset]);
        add(L("Feuern", "Fire"), btn_name(vita_keys[kset][0]));
        add(L("Spezialwaffe", "Special weapon"), btn_name(vita_keys[kset][1]));
        add(L("Waffe wechseln", "Change weapon"), btn_name(vita_keys[kset][2]));
        add(L("Mega-Bombe", "Mega bomb"), btn_name(vita_keys[kset][3]));
        add(L("Lenken mit", "Steer with"), vita_movestick[kset] ? L("rechter Stick", "right stick") : L("linker Stick", "left stick"));
        add(L("Standard wiederherstellen", "Restore defaults"), "");
        add(L("Zurück", "Back"), "");
    }
    return n;
}

static const int PX = 110, PY = 8, PW = 740, ROWH = 38;

static void draw(SDL_Renderer *r, SDL_Texture *game, int gw, int gh, int page, int sel, int waiting)
{
    Item it[MAXI]; int n = build(page, it);
    int ow, oh; SDL_RenderGetLogicalSize(r, &ow, &oh);
    SDL_RenderSetLogicalSize(r, 960, 544);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);
    SDL_Rect gd; double ga; vita_game_rect(&gd, &ga);
    SDL_RenderCopyEx(r, game, NULL, &gd, ga, NULL, SDL_FLIP_NONE);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 170); SDL_Rect all = { 0, 0, 960, 544 }; SDL_RenderFillRect(r, &all);
    int ph = 70 + n * ROWH + 40;
    SDL_Rect pan = { PX, PY, PW, ph };
    SDL_SetRenderDrawColor(r, 20, 28, 40, 235); SDL_RenderFillRect(r, &pan);
    SDL_SetRenderDrawColor(r, 220, 150, 40, 255); SDL_RenderDrawRect(r, &pan);
    SDL_Rect pan2 = { PX + 2, PY + 2, PW - 4, ph - 4 }; SDL_RenderDrawRect(r, &pan2);
    const char *title = page == PAGE_MAIN ? "RAP-Vita" : page == PAGE_SET ? L("Einstellungen", "Settings") : L("Tastenbelegung", "Button mapping");
    text(r, PX + (PW - tlen(title) * 16) / 2, PY + 14, title, 240, 170, 50);
    for (int i = 0; i < n; i++) {
        int y = PY + 64 + i * ROWH;
        if (i == sel) { SDL_SetRenderDrawColor(r, 220, 150, 40, 90); SDL_Rect h = { PX + 16, y - 3, PW - 32, ROWH - 2 }; SDL_RenderFillRect(r, &h); }
        text(r, PX + 32, y, it[i].label, 255, 255, 255);
        const char *v = (waiting && i == sel) ? L("Taste drücken...", "Press button...") : it[i].value;
        text(r, PX + PW - 32 - tlen(v) * 16, y, v, 140, 220, 255);
    }
    const char *help = L("Kreuz: OK  Kreis: zurück  Links/Rechts: ändern  R: Screenshot", "Cross: OK  Circle: back  Left/Right: change  R: screenshot");
    text(r, PX + (PW - tlen(help) * 8) / 2, PY + ph - 26, help, 170, 170, 170, 1);
    SDL_RenderPresent(r);
    SDL_RenderSetLogicalSize(r, ow, oh);
}

static void change(int page, int sel, int dir)
{
    if (page == PAGE_KEYS) {
        if (sel == 0) kset = (kset + 3 + dir) % 3;
        else if (sel == 5) vita_movestick[kset] ^= 1;
        return;
    }
    if (page != PAGE_SET) return;
    switch (sel) {
    case 0: vita_lang ^= 1; break;
    case 1: vita_ptr_speed = (vita_ptr_speed + 3 + dir) % 3; break;
    case 2: vita_rstick ^= 1; break;
    case 3: vita_touch_on ^= 1; break;
    case 4: vita_touch_game = (vita_touch_game + 3 + dir) % 3; break;
    case 5: vita_dpad_sens = (vita_dpad_sens + 5 + dir) % 5; break;
    case 6: vita_stretch = (vita_stretch + 3 + dir) % 3; break;
    case 7: vita_rotate = (vita_rotate + 3 + dir) % 3; break;
    case 8: vita_rot_level_only ^= 1; break;
    case 9: vita_debug_log ^= 1; if (vita_debug_log) vita_log_open(); break;
    }
}

void vita_menu_run(void)
{
    SDL_Renderer *r; SDL_Texture *game; int gw, gh;
    I_VitaGetRender(&r, &game, &gw, &gh);
    if (!r || !game) return;
    make_font(r);
    vita_menu_open = 1;
    vita_logf("Vita-Menue offen");
    int page = PAGE_MAIN, sel = 0, waiting = 0, done = 0;
    shot_msg = 0;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {}   // discard old events
    while (!done) {
        while (SDL_PollEvent(&e)) {
            int n; { Item it[MAXI]; n = build(page, it); }
            int act = 0;
            if (e.type == SDL_CONTROLLERBUTTONDOWN) {
                int b = e.cbutton.button;
                if (waiting) {
                    if (b != SDL_CONTROLLER_BUTTON_BACK) {
                        int a = sel - 1;   // Aktion 0..3
                        // swap if the button is already used
                        for (int k = 0; k < 4; k++) if (k != a && vita_keys[kset][k] == b) vita_keys[kset][k] = vita_keys[kset][a];
                        vita_keys[kset][a] = b; save();
                    }
                    waiting = 0; continue;
                }
                switch (b) {
                case SDL_CONTROLLER_BUTTON_DPAD_UP: sel = (sel + n - 1) % n; break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: sel = (sel + 1) % n; break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT: change(page, sel, -1); save(); break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: change(page, sel, 1); save(); break;
                case SDL_CONTROLLER_BUTTON_BACK: done = 1; break;
                case SDL_CONTROLLER_BUTTON_B:
                    if (page == PAGE_MAIN) done = 1; else { sel = page == PAGE_SET ? 0 : 1; page = PAGE_MAIN; }
                    break;
                case SDL_CONTROLLER_BUTTON_A: act = 1; break;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: shot_msg = vita_screenshot() ? 1 : 2; break;
                }
            } else if (e.type == SDL_FINGERDOWN && vita_touch_on && SDL_GetTouchDeviceType(e.tfinger.touchId) == SDL_TOUCH_DEVICE_DIRECT) {
                int tx = (int)(e.tfinger.x * 960), ty = (int)(e.tfinger.y * 544);
                int i = (ty - (PY + 64 - 3)) / ROWH;
                if (tx >= PX && tx < PX + PW && ty >= PY + 61 && i >= 0 && i < n && !waiting) { sel = i; act = 1; }
            }
            if (act) {
                if (page == PAGE_MAIN) { if (sel == 0) { page = PAGE_SET; sel = 0; } else if (sel == 1) { page = PAGE_KEYS; sel = 0; kset = vita_rotate; } else if (sel == 2) { shot_msg = vita_screenshot() ? 1 : 2; } else done = 1; }
                else if (page == PAGE_SET) { if (sel == n - 1) { page = PAGE_MAIN; sel = 0; } else { change(page, sel, 1); save(); } }
                else {
                    if (sel == 0 || sel == 5) { change(page, sel, 1); save(); }
                    else if (sel >= 1 && sel <= 4) waiting = 1;
                    else if (sel == 6) { for (int k = 0; k < 4; k++) vita_keys[kset][k] = def_keys[kset][k]; vita_movestick[kset] = def_stick[kset]; save(); }
                    else { page = PAGE_MAIN; sel = 1; }
                }
            }
        }
        draw(r, game, gw, gh, page, sel, waiting);
        MUS_Poll();
        SDL_Delay(16);
    }
    // wait until Select/Circle are released
    SDL_GameController *gc = SDL_GameControllerOpen(0);
    while (gc && (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_BACK) || SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_B) || SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A)))
    { SDL_PumpEvents(); SDL_Delay(10); }
    while (SDL_PollEvent(&e)) {}
    save();
    vita_menu_open = 0;
    vita_logf("Vita-Menue zu: lang=%d speed=%d touch=%d stretch=%d keys=%d,%d,%d,%d", vita_lang, vita_ptr_speed, vita_touch_on, vita_stretch, j_lookup[0], j_lookup[1], j_lookup[2], j_lookup[3]);
}
#endif

#ifdef __vita__
// ---------- own on-screen keyboard (pilot name / callsign) ----------
static const char *KB_ROWS[4] = { "1234567890", "QWERTZUIOP", "ASDFGHJKL-", "YXCVBNM .'" };
// special keys in row 4 (index 0..2): delete, space, OK
static const int KW = 64, KH = 56, KX0 = (960 - 10 * KW) / 2, KY0 = 200;

static void kb_draw(SDL_Renderer *r, SDL_Texture *game, int which, const char *buf, int cx, int cy)
{
    int ow, oh; SDL_RenderGetLogicalSize(r, &ow, &oh);
    SDL_RenderSetLogicalSize(r, 960, 544);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255); SDL_RenderClear(r);
    SDL_Rect gd; double ga; vita_game_rect(&gd, &ga);
    SDL_RenderCopyEx(r, game, NULL, &gd, ga, NULL, SDL_FLIP_NONE);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 170); SDL_Rect all = { 0, 0, 960, 544 }; SDL_RenderFillRect(r, &all);
    SDL_Rect pan = { 60, 30, 840, 490 };
    SDL_SetRenderDrawColor(r, 20, 28, 40, 240); SDL_RenderFillRect(r, &pan);
    SDL_SetRenderDrawColor(r, 220, 150, 40, 255); SDL_RenderDrawRect(r, &pan);
    const char *title = which ? L("Rufzeichen eingeben", "Enter callsign") : L("Pilotenname eingeben", "Enter pilot name");
    text(r, (960 - tlen(title) * 16) / 2, 46, title, 240, 170, 50);
    // input field
    SDL_Rect fld = { 200, 100, 560, 56 };
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255); SDL_RenderFillRect(r, &fld);
    SDL_SetRenderDrawColor(r, 140, 220, 255, 255); SDL_RenderDrawRect(r, &fld);
    char show[40]; snprintf(show, sizeof show, "%s%s", buf, (SDL_GetTicks() / 400) & 1 ? "_" : " ");
    text(r, 216, 112, show, 255, 255, 255);
    // keys
    for (int y = 0; y < 5; y++) {
        int n = y < 4 ? 10 : 3;
        for (int x = 0; x < n; x++) {
            SDL_Rect k;
            const char *lab; char one[2] = { 0, 0 };
            if (y < 4) { k = { KX0 + x * KW + 3, KY0 + y * KH + 3, KW - 6, KH - 6 }; one[0] = KB_ROWS[y][x]; lab = one; if (one[0] == ' ') lab = "_"; }
            else { int w = 10 * KW / 3; k = { KX0 + x * w + 3, KY0 + 4 * KH + 3, w - 6, KH - 6 };
                   lab = x == 0 ? L("Löschen", "Delete") : x == 1 ? L("Leerzeichen", "Space") : "OK"; }
            bool s = (x == cx && y == cy);
            SDL_SetRenderDrawColor(r, s ? 220 : 50, s ? 150 : 60, s ? 40 : 80, 255); SDL_RenderFillRect(r, &k);
            SDL_SetRenderDrawColor(r, 120, 130, 150, 255); SDL_RenderDrawRect(r, &k);
            text(r, k.x + (k.w - tlen(lab) * 16) / 2, k.y + (k.h - 32) / 2, lab, 255, 255, 255);
        }
    }
    const char *help = L("Kreuz: Taste  Quadrat: löschen  Dreieck: Leer  Start: OK  Kreis: Abbruch",
                         "Cross: key  Square: delete  Triangle: space  Start: OK  Circle: cancel");
    text(r, (960 - tlen(help) * 8) / 2, 494, help, 170, 170, 170, 1);
    SDL_RenderPresent(r);
    SDL_RenderSetLogicalSize(r, ow, oh);
}

int vita_keyboard_run(int which, char *buf, int maxlen)
{
    SDL_Renderer *r; SDL_Texture *game; int gw, gh;
    I_VitaGetRender(&r, &game, &gw, &gh);
    if (!r || !game) return 0;
    make_font(r);
    vita_menu_open = 1;
    int cx = 0, cy = 1, done = 0, ok = 0, len = strlen(buf);
    SDL_GameController *gc = SDL_GameControllerOpen(0);
    while (gc && (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A) || SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_Y)))
    { SDL_PumpEvents(); SDL_Delay(10); }
    SDL_Event e; while (SDL_PollEvent(&e)) {}
    unsigned nextmove = 0;
    while (!done) {
        while (SDL_PollEvent(&e)) {
            int press = 0;   // 1 = current key
            if (e.type == SDL_CONTROLLERBUTTONDOWN) {
                switch (e.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_DPAD_UP: cy = (cy + 4) % 5; break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: cy = (cy + 1) % 5; break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT: cx--; break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: cx++; break;
                case SDL_CONTROLLER_BUTTON_A: press = 1; break;
                case SDL_CONTROLLER_BUTTON_X: if (len) buf[--len] = 0; break;
                case SDL_CONTROLLER_BUTTON_Y: if (len < maxlen) { buf[len++] = ' '; buf[len] = 0; } break;
                case SDL_CONTROLLER_BUTTON_START: ok = 1; done = 1; break;
                case SDL_CONTROLLER_BUTTON_B: done = 1; break;
                }
            } else if (e.type == SDL_FINGERDOWN && vita_touch_on && SDL_GetTouchDeviceType(e.tfinger.touchId) == SDL_TOUCH_DEVICE_DIRECT) {
                int tx = (int)(e.tfinger.x * 960), ty = (int)(e.tfinger.y * 544);
                int y = (ty - KY0) / KH;
                if (tx >= KX0 && tx < KX0 + 10 * KW && ty >= KY0 && y >= 0 && y < 5) {
                    cy = y; cx = y < 4 ? (tx - KX0) / KW : (tx - KX0) / (10 * KW / 3); press = 1;
                }
            }
            int n = cy < 4 ? 10 : 3;
            if (cx < 0) cx = n - 1; if (cx >= n) cx = cx % n;
            if (press) {
                if (cy < 4) { char c = KB_ROWS[cy][cx]; if (len < maxlen && !(c == ' ' && len == 0)) { buf[len++] = c; buf[len] = 0; } }
                else if (cx == 0) { if (len) buf[--len] = 0; }
                else if (cx == 1) { if (len && len < maxlen) { buf[len++] = ' '; buf[len] = 0; } }
                else { ok = 1; done = 1; }
            }
        }
        // left stick moves the selection
        if (gc && SDL_GetTicks() >= nextmove) {
            int ax = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX), ay = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
            int moved = 0;
            if (ax > 16000) { cx++; moved = 1; } else if (ax < -16000) { cx--; moved = 1; }
            if (ay > 16000) { cy = (cy + 1) % 5; moved = 1; } else if (ay < -16000) { cy = (cy + 4) % 5; moved = 1; }
            if (moved) { int n = cy < 4 ? 10 : 3; if (cx < 0) cx = n - 1; if (cx >= n) cx = cx % n; nextmove = SDL_GetTicks() + 180; }
        }
        kb_draw(r, game, which, buf, cx, cy);
        MUS_Poll();
        SDL_Delay(16);
    }
    while (gc && (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A) || SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_START) || SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_B)))
    { SDL_PumpEvents(); SDL_Delay(10); }
    while (SDL_PollEvent(&e)) {}
    // strip trailing spaces
    while (len && buf[len - 1] == ' ') buf[--len] = 0;
    vita_menu_open = 0;
    return ok;
}
#endif

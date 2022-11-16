// dear HanaLovesMe, v1.70 WIP
// (demo code)

// Message to the person tempted to delete this file when integrating Dear HanaLovesMe into their code base:
// Do NOT remove this file from your project! Think again! It is the most useful reference code that you and other coders
// will want to refer to and call. Have the HanaLovesMe::ShowDemoWindow() function wired in an always-available debug menu of
// your game/app! Removing this file from your project is hindering access to documentation for everyone in your team,
// likely leading you to poorer usage of the library.
// Everything in this file will be stripped out by the linker if you don't call HanaLovesMe::ShowDemoWindow().
// If you want to link core Dear HanaLovesMe in your shipped builds but want an easy guarantee that the demo will not be linked,
// you can setup your imconfig.h with #define HanaLovesMe_DISABLE_DEMO_WINDOWS and those functions will be empty.
// In other situation, whenever you have Dear HanaLovesMe available you probably want this to be available for reference.
// Thank you,
// -Your beloved friend, HanaLovesMe_demo.cpp (that you won't delete)

// Message to beginner C/C++ programmers about the meaning of the 'static' keyword:
// In this demo code, we frequently we use 'static' variables inside functions. A static variable persist across calls, so it is
// essentially like a global variable but declared inside the scope of the function. We do this as a way to gather code and data
// in the same place, to make the demo source code faster to read, faster to write, and smaller in size.
// It also happens to be a convenient way of storing simple UI related information as long as your function doesn't need to be reentrant
// or used in threads. This might be a pattern you will want to use in your code, but most of the real data you would be editing is
// likely going to be stored outside your functions.

/*

Index of this file:

// [SECTION] Forward Declarations, Helpers
// [SECTION] Demo Window / ShowDemoWindow()
// [SECTION] About Window / ShowAboutWindow()
// [SECTION] Style Editor / ShowStyleEditor()
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "HanaLovesMe.h"
#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"             // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"    // warning : 'xx' is deprecated: The POSIX name for this item.. // for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"   // warning : cast to 'void *' from smaller integer type 'int'
#pragma clang diagnostic ignored "-Wformat-security"            // warning : warning: format string is not a string literal
#pragma clang diagnostic ignored "-Wexit-time-destructors"      // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. HanaLovesMe coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wunused-macros"              // warning : warning: macro is not used                         // we define snprintf/vsnprintf on Windows so they are available, but not always used.
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant                  // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"           // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"          // warning : macro name is a reserved identifier                //
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"          // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat-security"              // warning : format string is not a string literal (potentially insecure)
#pragma GCC diagnostic ignored "-Wdouble-promotion"             // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"                   // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#if (__GNUC__ >= 6)
#pragma GCC diagnostic ignored "-Wmisleading-indentation"       // warning: this 'if' clause does not guard this statement      // GCC 6.0+ only. See #883 on GitHub.
#endif
#endif

// Play it nice with Windows users. Notepad in 2017 still doesn't display text data with Unix-style \n.
#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#define snprintf    _snprintf
#define vsnprintf   _vsnprintf
#else
#define IM_NEWLINE  "\n"
#endif

#define IM_MAX(_A,_B)       (((_A) >= (_B)) ? (_A) : (_B))

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations, Helpers
//-----------------------------------------------------------------------------

#if !defined(HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS) && defined(HanaLovesMe_DISABLE_TEST_WINDOWS) && !defined(HanaLovesMe_DISABLE_DEMO_WINDOWS)   // Obsolete name since 1.53, TEST->DEMO
#define HanaLovesMe_DISABLE_DEMO_WINDOWS
#endif

#if !defined(HanaLovesMe_DISABLE_DEMO_WINDOWS)

// Forward Declarations
static void ShowExampleAppDocuments(bool* p_open);
static void ShowExampleAppMainMenuBar();
static void ShowExampleAppConsole(bool* p_open);
static void ShowExampleAppLog(bool* p_open);
static void ShowExampleAppLayout(bool* p_open);
static void ShowExampleAppPropertyEditor(bool* p_open);
static void ShowExampleAppLongText(bool* p_open);
static void ShowExampleAppAutoResize(bool* p_open);
static void ShowExampleAppConstrainedResize(bool* p_open);
static void ShowExampleAppSimpleOverlay(bool* p_open);
static void ShowExampleAppWindowTitles(bool* p_open);
static void ShowExampleAppCustomRendering(bool* p_open);
static void ShowExampleMenuFile();

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see misc/fonts/README.txt)
static void HelpMarker(const char* desc)
{
    HanaLovesMe::TextDisabled("(?)");
    if (HanaLovesMe::IsItemHovered())
    {
        HanaLovesMe::BeginTooltip();
        HanaLovesMe::PushTextWrapPos(HanaLovesMe::GetFontSize() * 35.0f);
        HanaLovesMe::TextUnformatted(desc);
        HanaLovesMe::PopTextWrapPos();
        HanaLovesMe::EndTooltip();
    }
}

// Helper to display basic user controls.
void HanaLovesMe::ShowUserGuide()
{
    HanaLovesMeIO& io = HanaLovesMe::GetIO();
    HanaLovesMe::BulletText("Double-click on title bar to collapse window.");
    HanaLovesMe::BulletText("Click and drag on lower right corner to resize window\n(double-click to auto fit window to its contents).");
    HanaLovesMe::BulletText("Click and drag on any empty space to move window.");
    HanaLovesMe::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    HanaLovesMe::BulletText("CTRL+Click on a slider or drag box to input value as text.");
    if (io.FontAllowUserScaling)
        HanaLovesMe::BulletText("CTRL+Mouse Wheel to zoom window contents.");
    HanaLovesMe::BulletText("Mouse Wheel to scroll.");
    HanaLovesMe::BulletText("While editing text:\n");
    HanaLovesMe::Indent();
    HanaLovesMe::BulletText("Hold SHIFT or use mouse to select text.");
    HanaLovesMe::BulletText("CTRL+Left/Right to word jump.");
    HanaLovesMe::BulletText("CTRL+A or double-click to select all.");
    HanaLovesMe::BulletText("CTRL+X,CTRL+C,CTRL+V to use clipboard.");
    HanaLovesMe::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
    HanaLovesMe::BulletText("ESCAPE to revert.");
    HanaLovesMe::BulletText("You can apply arithmetic operators +,*,/ on numerical values.\nUse +- to subtract.");
    HanaLovesMe::Unindent();
}

//-----------------------------------------------------------------------------
// [SECTION] Demo Window / ShowDemoWindow()
//-----------------------------------------------------------------------------

// We split the contents of the big ShowDemoWindow() function into smaller functions (because the link time of very large functions grow non-linearly)
static void ShowDemoWindowWidgets();
static void ShowDemoWindowLayout();
static void ShowDemoWindowPopups();
static void ShowDemoWindowColumns();
static void ShowDemoWindowMisc();

// Demonstrate most Dear HanaLovesMe features (this is big function!)
// You may execute this function to experiment with the UI and understand what it does. You may then search for keywords in the code when you are interested by a specific feature.
void HanaLovesMe::ShowDemoWindow(bool* p_open)
{
    IM_ASSERT(HanaLovesMe::GetCurrentContext() != NULL && "Missing dear HanaLovesMe context. Refer to examples app!"); // Exceptionally add an extra assert here for people confused with initial dear HanaLovesMe setup

    // Examples Apps (accessible from the "Examples" menu)
    static bool show_app_documents = false;
    static bool show_app_main_menu_bar = false;
    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;

    if (show_app_documents)           ShowExampleAppDocuments(&show_app_documents);
    if (show_app_main_menu_bar)       ShowExampleAppMainMenuBar();
    if (show_app_console)             ShowExampleAppConsole(&show_app_console);
    if (show_app_log)                 ShowExampleAppLog(&show_app_log);
    if (show_app_layout)              ShowExampleAppLayout(&show_app_layout);
    if (show_app_property_editor)     ShowExampleAppPropertyEditor(&show_app_property_editor);
    if (show_app_long_text)           ShowExampleAppLongText(&show_app_long_text);
    if (show_app_auto_resize)         ShowExampleAppAutoResize(&show_app_auto_resize);
    if (show_app_constrained_resize)  ShowExampleAppConstrainedResize(&show_app_constrained_resize);
    if (show_app_simple_overlay)      ShowExampleAppSimpleOverlay(&show_app_simple_overlay);
    if (show_app_window_titles)       ShowExampleAppWindowTitles(&show_app_window_titles);
    if (show_app_custom_rendering)    ShowExampleAppCustomRendering(&show_app_custom_rendering);

    // Dear HanaLovesMe Apps (accessible from the "Help" menu)
    static bool show_app_metrics = false;
    static bool show_app_style_editor = false;
    static bool show_app_about = false;

    if (show_app_metrics) { HanaLovesMe::ShowMetricsWindow(&show_app_metrics); }
    if (show_app_style_editor) { HanaLovesMe::Begin("Style Editor", &show_app_style_editor); HanaLovesMe::ShowStyleEditor(); HanaLovesMe::End(); }
    if (show_app_about) { HanaLovesMe::ShowAboutWindow(&show_app_about); }

    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;

    HanaLovesMeWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= HanaLovesMeWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= HanaLovesMeWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= HanaLovesMeWindowFlags_MenuBar;
    if (no_move)            window_flags |= HanaLovesMeWindowFlags_NoMove;
    if (no_resize)          window_flags |= HanaLovesMeWindowFlags_NoResize;
    if (no_collapse)        window_flags |= HanaLovesMeWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= HanaLovesMeWindowFlags_NoNav;
    if (no_background)      window_flags |= HanaLovesMeWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= HanaLovesMeWindowFlags_NoBringToFrontOnFocus;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    // We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
    HanaLovesMe::SetNextWindowPos(vsize(650, 20), HanaLovesMeCond_FirstUseEver);
    HanaLovesMe::SetNextWindowSize(vsize(550, 680), HanaLovesMeCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!HanaLovesMe::Begin("HanaLovesMe Demo", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        HanaLovesMe::End();
        return;
    }

    // Most "big" widgets share a common width settings by default.
    //HanaLovesMe::PushItemWidth(HanaLovesMe::GetWindowWidth() * 0.65f);    // Use 2/3 of the space for widgets and 1/3 for labels (default)
    HanaLovesMe::PushItemWidth(HanaLovesMe::GetFontSize() * -12);           // Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

    // Menu Bar
    if (HanaLovesMe::BeginMenuBar())
    {
        if (HanaLovesMe::BeginMenu("Menu"))
        {
            ShowExampleMenuFile();
            HanaLovesMe::EndMenu();
        }
        if (HanaLovesMe::BeginMenu("Examples"))
        {
            HanaLovesMe::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            HanaLovesMe::MenuItem("Console", NULL, &show_app_console);
            HanaLovesMe::MenuItem("Log", NULL, &show_app_log);
            HanaLovesMe::MenuItem("Simple layout", NULL, &show_app_layout);
            HanaLovesMe::MenuItem("Property editor", NULL, &show_app_property_editor);
            HanaLovesMe::MenuItem("Long text display", NULL, &show_app_long_text);
            HanaLovesMe::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            HanaLovesMe::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            HanaLovesMe::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            HanaLovesMe::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            HanaLovesMe::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            HanaLovesMe::MenuItem("Documents", NULL, &show_app_documents);
            HanaLovesMe::EndMenu();
        }
        if (HanaLovesMe::BeginMenu("Help"))
        {
            HanaLovesMe::MenuItem("Metrics", NULL, &show_app_metrics);
            HanaLovesMe::MenuItem("Style Editor", NULL, &show_app_style_editor);
            HanaLovesMe::MenuItem("About Dear HanaLovesMe", NULL, &show_app_about);
            HanaLovesMe::EndMenu();
        }
        HanaLovesMe::EndMenuBar();
    }

    HanaLovesMe::Text("dear HanaLovesMe says hello. (%s)", HanaLovesMe_VERSION);
    HanaLovesMe::Spacing();

    if (HanaLovesMe::CollapsingHeader("Help"))
    {
        HanaLovesMe::Text("PROGRAMMER GUIDE:");
        HanaLovesMe::BulletText("Please see the ShowDemoWindow() code in HanaLovesMe_demo.cpp. <- you are here!");
        HanaLovesMe::BulletText("Please see the comments in HanaLovesMe.cpp.");
        HanaLovesMe::BulletText("Please see the examples/ in application.");
        HanaLovesMe::BulletText("Enable 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        HanaLovesMe::BulletText("Enable 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
        HanaLovesMe::Separator();

        HanaLovesMe::Text("USER GUIDE:");
        HanaLovesMe::ShowUserGuide();
    }

    if (HanaLovesMe::CollapsingHeader("Configuration"))
    {
        HanaLovesMeIO& io = HanaLovesMe::GetIO();

        if (HanaLovesMe::TreeNode("Configuration##2"))
        {
            HanaLovesMe::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard", (unsigned int*)& io.ConfigFlags, HanaLovesMeConfigFlags_NavEnableKeyboard);
            HanaLovesMe::CheckboxFlags("io.ConfigFlags: NavEnableGamepad", (unsigned int*)& io.ConfigFlags, HanaLovesMeConfigFlags_NavEnableGamepad);
            HanaLovesMe::SameLine(); HelpMarker("Required back-end to feed in gamepad inputs in io.NavInputs[] and set io.BackendFlags |= HanaLovesMeBackendFlags_HasGamepad.\n\nRead instructions in HanaLovesMe.cpp for details.");
            HanaLovesMe::CheckboxFlags("io.ConfigFlags: NavEnableSetMousePos", (unsigned int*)& io.ConfigFlags, HanaLovesMeConfigFlags_NavEnableSetMousePos);
            HanaLovesMe::SameLine(); HelpMarker("Instruct navigation to move the mouse cursor. See comment for HanaLovesMeConfigFlags_NavEnableSetMousePos.");
            HanaLovesMe::CheckboxFlags("io.ConfigFlags: NoMouse", (unsigned int*)& io.ConfigFlags, HanaLovesMeConfigFlags_NoMouse);
            if (io.ConfigFlags & HanaLovesMeConfigFlags_NoMouse) // Create a way to restore this flag otherwise we could be stuck completely!
            {
                if (fmodf((float)HanaLovesMe::GetTime(), 0.40f) < 0.20f)
                {
                    HanaLovesMe::SameLine();
                    HanaLovesMe::Text("<<PRESS SPACE TO DISABLE>>");
                }
                if (HanaLovesMe::IsKeyPressed(HanaLovesMe::GetKeyIndex(HanaLovesMeKey_Space)))
                    io.ConfigFlags &= ~HanaLovesMeConfigFlags_NoMouse;
            }
            HanaLovesMe::CheckboxFlags("io.ConfigFlags: NoMouseCursorChange", (unsigned int*)& io.ConfigFlags, HanaLovesMeConfigFlags_NoMouseCursorChange);
            HanaLovesMe::SameLine(); HelpMarker("Instruct back-end to not alter mouse cursor shape and visibility.");
            HanaLovesMe::Checkbox("io.ConfigInputTextCursorBlink", &io.ConfigInputTextCursorBlink);
            HanaLovesMe::SameLine(); HelpMarker("Set to false to disable blinking cursor, for users who consider it distracting");
            HanaLovesMe::Checkbox("io.ConfigWindowsResizeFromEdges", &io.ConfigWindowsResizeFromEdges);
            HanaLovesMe::SameLine(); HelpMarker("Enable resizing of windows from their edges and from the lower-left corner.\nThis requires (io.BackendFlags & HanaLovesMeBackendFlags_HasMouseCursors) because it needs mouse cursor feedback.");
            HanaLovesMe::Checkbox("io.ConfigWindowsMoveFromTitleBarOnly", &io.ConfigWindowsMoveFromTitleBarOnly);
            HanaLovesMe::Checkbox("io.MouseDrawCursor", &io.MouseDrawCursor);
            HanaLovesMe::SameLine(); HelpMarker("Instruct Dear HanaLovesMe to render a mouse cursor for you. Note that a mouse cursor rendered via your application GPU rendering path will feel more laggy than hardware cursor, but will be more in sync with your other visuals.\n\nSome desktop applications may use both kinds of cursors (e.g. enable software cursor only when resizing/dragging something).");
            HanaLovesMe::TreePop();
            HanaLovesMe::Separator();
        }

        if (HanaLovesMe::TreeNode("Backend Flags"))
        {
            HelpMarker("Those flags are set by the back-ends (HanaLovesMe_impl_xxx files) to specify their capabilities.");
            HanaLovesMeBackendFlags backend_flags = io.BackendFlags; // Make a local copy to avoid modifying actual back-end flags.
            HanaLovesMe::CheckboxFlags("io.BackendFlags: HasGamepad", (unsigned int*)& backend_flags, HanaLovesMeBackendFlags_HasGamepad);
            HanaLovesMe::CheckboxFlags("io.BackendFlags: HasMouseCursors", (unsigned int*)& backend_flags, HanaLovesMeBackendFlags_HasMouseCursors);
            HanaLovesMe::CheckboxFlags("io.BackendFlags: HasSetMousePos", (unsigned int*)& backend_flags, HanaLovesMeBackendFlags_HasSetMousePos);
            HanaLovesMe::TreePop();
            HanaLovesMe::Separator();
        }

        if (HanaLovesMe::TreeNode("Style"))
        {
            HanaLovesMe::ShowStyleEditor();
            HanaLovesMe::TreePop();
            HanaLovesMe::Separator();
        }

        if (HanaLovesMe::TreeNode("Capture/Logging"))
        {
            HanaLovesMe::TextWrapped("The logging API redirects all text output so you can easily capture the content of a window or a block. Tree nodes can be automatically expanded.");
            HelpMarker("Try opening any of the contents below in this window and then click one of the \"Log To\" button.");
            HanaLovesMe::LogButtons();
            HanaLovesMe::TextWrapped("You can also call HanaLovesMe::LogText() to output directly to the log without a visual output.");
            if (HanaLovesMe::Button("Copy \"Hello, world!\" to clipboard"))
            {
                HanaLovesMe::LogToClipboard();
                HanaLovesMe::LogText("Hello, world!");
                HanaLovesMe::LogFinish();
            }
            HanaLovesMe::TreePop();
        }
    }

    if (HanaLovesMe::CollapsingHeader("Window options"))
    {
        HanaLovesMe::Checkbox("No titlebar", &no_titlebar); HanaLovesMe::SameLine(150);
        HanaLovesMe::Checkbox("No scrollbar", &no_scrollbar); HanaLovesMe::SameLine(300);
        HanaLovesMe::Checkbox("No menu", &no_menu);
        HanaLovesMe::Checkbox("No move", &no_move); HanaLovesMe::SameLine(150);
        HanaLovesMe::Checkbox("No resize", &no_resize); HanaLovesMe::SameLine(300);
        HanaLovesMe::Checkbox("No collapse", &no_collapse);
        HanaLovesMe::Checkbox("No close", &no_close); HanaLovesMe::SameLine(150);
        HanaLovesMe::Checkbox("No nav", &no_nav); HanaLovesMe::SameLine(300);
        HanaLovesMe::Checkbox("No background", &no_background);
        HanaLovesMe::Checkbox("No bring to front", &no_bring_to_front);
    }

    // All demo contents
    ShowDemoWindowWidgets();
    ShowDemoWindowLayout();
    ShowDemoWindowPopups();
    ShowDemoWindowColumns();
    ShowDemoWindowMisc();

    // End of ShowDemoWindow()
    HanaLovesMe::End();
}

static void ShowDemoWindowWidgets()
{
    if (!HanaLovesMe::CollapsingHeader("Widgets"))
        return;

    if (HanaLovesMe::TreeNode("Basic"))
    {
        static int clicked = 0;
        if (HanaLovesMe::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            HanaLovesMe::SameLine();
            HanaLovesMe::Text("Thanks for clicking me!");
        }

        static bool check = true;
        HanaLovesMe::Checkbox("checkbox", &check);

        static int e = 0;
        HanaLovesMe::RadioButton("radio a", &e, 0); HanaLovesMe::SameLine();
        HanaLovesMe::RadioButton("radio b", &e, 1); HanaLovesMe::SameLine();
        HanaLovesMe::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        for (int i = 0; i < 7; i++)
        {
            if (i > 0)
                HanaLovesMe::SameLine();
            HanaLovesMe::PushID(i);
            HanaLovesMe::PushStyleColor(HanaLovesMeCol_Button, (xuifloatcolor)xuicolor::HSV(i / 7.0f, 0.6f, 0.6f));
            HanaLovesMe::PushStyleColor(HanaLovesMeCol_ButtonHovered, (xuifloatcolor)xuicolor::HSV(i / 7.0f, 0.7f, 0.7f));
            HanaLovesMe::PushStyleColor(HanaLovesMeCol_ButtonActive, (xuifloatcolor)xuicolor::HSV(i / 7.0f, 0.8f, 0.8f));
            HanaLovesMe::Button("Click");
            HanaLovesMe::PopStyleColor(3);
            HanaLovesMe::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed elements (otherwise a Text+SameLine+Button sequence will have the text a little too high by default)
        HanaLovesMe::AlignTextToFramePadding();
        HanaLovesMe::Text("Hold to repeat:");
        HanaLovesMe::SameLine();

        // Arrow buttons with Repeater
        static int counter = 0;
        float spacing = HanaLovesMe::GetStyle().ItemInnerSpacing.x;
        HanaLovesMe::PushButtonRepeat(true);
        if (HanaLovesMe::ArrowButton("##left", HanaLovesMeDir_Left)) { counter--; }
        HanaLovesMe::SameLine(0.0f, spacing);
        if (HanaLovesMe::ArrowButton("##right", HanaLovesMeDir_Right)) { counter++; }
        HanaLovesMe::PopButtonRepeat();
        HanaLovesMe::SameLine();
        HanaLovesMe::Text("%d", counter);

        HanaLovesMe::Text("Hover over me");
        if (HanaLovesMe::IsItemHovered())
            HanaLovesMe::SetTooltip("I am a tooltip");

        HanaLovesMe::SameLine();
        HanaLovesMe::Text("- or me");
        if (HanaLovesMe::IsItemHovered())
        {
            HanaLovesMe::BeginTooltip();
            HanaLovesMe::Text("I am a fancy tooltip");
            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
            HanaLovesMe::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
            HanaLovesMe::EndTooltip();
        }

        HanaLovesMe::Separator();

        HanaLovesMe::LabelText("label", "Value");

        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more complete BeginCombo()/EndCombo() api.
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
            static int item_current = 0;
            HanaLovesMe::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
            HanaLovesMe::SameLine(); HelpMarker("Refer to the \"Combo\" section below for an explanation of the full BeginCombo/EndCombo API, and demonstration of various flags.\n");
        }

        {
            static char str0[128] = "Hello, world!";
            HanaLovesMe::InputText("input text", str0, IM_ARRAYSIZE(str0));
            HanaLovesMe::SameLine(); HelpMarker("USER:\nHold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n\nPROGRAMMER:\nYou can use the HanaLovesMeInputTextFlags_CallbackResize facility if you need to wire InputText() to a dynamic string type. See misc/cpp/HanaLovesMe_stdlib.h for an example (this is not demonstrated in HanaLovesMe_demo.cpp).");

            static char str1[128] = "";
            HanaLovesMe::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            static int i0 = 123;
            HanaLovesMe::InputInt("input int", &i0);
            HanaLovesMe::SameLine(); HelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");

            static float f0 = 0.001f;
            HanaLovesMe::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            HanaLovesMe::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            HanaLovesMe::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            HanaLovesMe::SameLine(); HelpMarker("You can input value using the scientific notation,\n  e.g. \"1e+8\" becomes \"100000000\".\n");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            HanaLovesMe::InputFloat3("input float3", vec4a);
        }

        {
            static int i1 = 50, i2 = 42;
            HanaLovesMe::DragInt("drag int", &i1, 1);
            HanaLovesMe::SameLine(); HelpMarker("Click and drag to edit value.\nHold SHIFT/ALT for faster/slower edit.\nDouble-click or CTRL+click to input value.");

            HanaLovesMe::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%");

            static float f1 = 1.00f, f2 = 0.0067f;
            HanaLovesMe::DragFloat("drag float", &f1, 0.005f);
            HanaLovesMe::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        }

        {
            static int i1 = 0;
            HanaLovesMe::SliderInt("slider int", &i1, -1, 3);
            HanaLovesMe::SameLine(); HelpMarker("CTRL+click to input value.");

            static float f1 = 0.123f, f2 = 0.0f;
            HanaLovesMe::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            HanaLovesMe::SliderFloat("slider float (curve)", &f2, -10.0f, 10.0f, "%.4f", 2.0f);
            static float angle = 0.0f;
            HanaLovesMe::SliderAngle("slider angle", &angle);
        }

        {
            static float col1[3] = { 1.0f,0.0f,0.2f };
            static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
            HanaLovesMe::ColorEdit3("color 1", col1);
            HanaLovesMe::SameLine(); HelpMarker("Click on the colored square to open a color picker.\nClick and hold to use drag and drop.\nRight-click on the colored square to show options.\nCTRL+click on individual component to input value.\n");

            HanaLovesMe::ColorEdit4("color 2", col2);
        }

        {
            // List box
            const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
            static int listbox_item_current = 1;
            HanaLovesMe::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

            //static int listbox_item_current2 = 2;
            //HanaLovesMe::PushItemWidth(-1);
            //HanaLovesMe::ListBox("##listbox2", &listbox_item_current2, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
            //HanaLovesMe::PopItemWidth();
        }

        HanaLovesMe::TreePop();
    }

    // Testing HanaLovesMeOnceUponAFrame helper.
    //static HanaLovesMeOnceUponAFrame once;
    //for (int i = 0; i < 5; i++)
    //    if (once)
    //        HanaLovesMe::Text("This will be displayed only once.");

    if (HanaLovesMe::TreeNode("Trees"))
    {
        if (HanaLovesMe::TreeNode("Basic trees"))
        {
            for (int i = 0; i < 5; i++)
                if (HanaLovesMe::TreeNode((void*)(intptr_t)i, "Child %d", i))
                {
                    HanaLovesMe::Text("blah blah");
                    HanaLovesMe::SameLine();
                    if (HanaLovesMe::SmallButton("button")) {};
                    HanaLovesMe::TreePop();
                }
            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Advanced, with Selectable nodes"))
        {
            HelpMarker("This is a more typical looking tree with selectable nodes.\nClick to select, CTRL+Click to toggle, click on arrows or double-click to open.");
            static bool align_label_with_current_x_position = false;
            HanaLovesMe::Checkbox("Align label with current X position)", &align_label_with_current_x_position);
            HanaLovesMe::Text("Hello!");
            if (align_label_with_current_x_position)
                HanaLovesMe::Unindent(HanaLovesMe::GetTreeNodeToLabelSpacing());

            static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
            int node_clicked = -1;                // Temporary stohnly of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
            HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_IndentSpacing, HanaLovesMe::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
            for (int i = 0; i < 6; i++)
            {
                // Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
                HanaLovesMeTreeNodeFlags node_flags = HanaLovesMeTreeNodeFlags_OpenOnArrow | HanaLovesMeTreeNodeFlags_OpenOnDoubleClick;
                if (selection_mask & (1 << i))
                    node_flags |= HanaLovesMeTreeNodeFlags_Selected;
                if (i < 3)
                {
                    // Items 0..2 are Tree Node
                    bool node_open = HanaLovesMe::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                    if (HanaLovesMe::IsItemClicked())
                        node_clicked = i;
                    if (node_open)
                    {
                        HanaLovesMe::Text("Blah blah\nBlah Blah");
                        HanaLovesMe::TreePop();
                    }
                } else
                {
                    // Items 3..5 are Tree Leaves
                    // The only reason we use TreeNode at all is to allow selection of the leaf.
                    // Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
                    node_flags |= HanaLovesMeTreeNodeFlags_Leaf | HanaLovesMeTreeNodeFlags_NoTreePushOnOpen; // HanaLovesMeTreeNodeFlags_Bullet
                    HanaLovesMe::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                    if (HanaLovesMe::IsItemClicked())
                        node_clicked = i;
                }
            }
            if (node_clicked != -1)
            {
                // Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
                if (HanaLovesMe::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
            }
            HanaLovesMe::PopStyleVar();
            if (align_label_with_current_x_position)
                HanaLovesMe::Indent(HanaLovesMe::GetTreeNodeToLabelSpacing());
            HanaLovesMe::TreePop();
        }
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Collapsing Headers"))
    {
        static bool closable_group = true;
        HanaLovesMe::Checkbox("Show 2nd header", &closable_group);
        if (HanaLovesMe::CollapsingHeader("Header"))
        {
            HanaLovesMe::Text("IsItemHovered: %d", HanaLovesMe::IsItemHovered());
            for (int i = 0; i < 5; i++)
                HanaLovesMe::Text("Some content %d", i);
        }
        if (HanaLovesMe::CollapsingHeader("Header with a close button", &closable_group))
        {
            HanaLovesMe::Text("IsItemHovered: %d", HanaLovesMe::IsItemHovered());
            for (int i = 0; i < 5; i++)
                HanaLovesMe::Text("More content %d", i);
        }
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Bullets"))
    {
        HanaLovesMe::BulletText("Bullet point 1");
        HanaLovesMe::BulletText("Bullet point 2\nOn multiple lines");
        HanaLovesMe::Bullet(); HanaLovesMe::Text("Bullet point 3 (two calls)");
        HanaLovesMe::Bullet(); HanaLovesMe::SmallButton("Button");
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Text"))
    {
        if (HanaLovesMe::TreeNode("Colored Text"))
        {
            // Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
            HanaLovesMe::TextColored(xuifloatcolor(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
            HanaLovesMe::TextColored(xuifloatcolor(1.0f, 1.0f, 0.0f, 1.0f), "Yellow");
            HanaLovesMe::TextDisabled("Disabled");
            HanaLovesMe::SameLine(); HelpMarker("The TextDisabled color is stored in HanaLovesMeStyle.");
            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Word Wrapping"))
        {
            // Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
            HanaLovesMe::TextWrapped("This text should automatically wrap on the edge of the window. The current implementation for text wrapping follows simple rules suitable for English and possibly other languages.");
            HanaLovesMe::Spacing();

            static float wrap_width = 200.0f;
            HanaLovesMe::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

            HanaLovesMe::Text("Test paragraph 1:");
            vsize pos = HanaLovesMe::GetCursorScreenPos();
            HanaLovesMe::GetWindowDrawList()->AddRectFilled(vsize(pos.x + wrap_width, pos.y), vsize(pos.x + wrap_width + 10, pos.y + HanaLovesMe::GetTextLineHeight()), IM_COL32(255, 0, 255, 255));
            HanaLovesMe::PushTextWrapPos(HanaLovesMe::GetCursorPos().x + wrap_width);
            HanaLovesMe::Text("The lazy dog is a good dog. This paragraph is made to fit within %.0f pixels. Testing a 1 character word. The quick brown fox jumps over the lazy dog.", wrap_width);
            HanaLovesMe::GetWindowDrawList()->AddRect(HanaLovesMe::GetItemRectMin(), HanaLovesMe::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
            HanaLovesMe::PopTextWrapPos();

            HanaLovesMe::Text("Test paragraph 2:");
            pos = HanaLovesMe::GetCursorScreenPos();
            HanaLovesMe::GetWindowDrawList()->AddRectFilled(vsize(pos.x + wrap_width, pos.y), vsize(pos.x + wrap_width + 10, pos.y + HanaLovesMe::GetTextLineHeight()), IM_COL32(255, 0, 255, 255));
            HanaLovesMe::PushTextWrapPos(HanaLovesMe::GetCursorPos().x + wrap_width);
            HanaLovesMe::Text("aaaaaaaa bbbbbbbb, c cccccccc,dddddddd. d eeeeeeee   ffffffff. gggggggg!hhhhhhhh");
            HanaLovesMe::GetWindowDrawList()->AddRect(HanaLovesMe::GetItemRectMin(), HanaLovesMe::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
            HanaLovesMe::PopTextWrapPos();

            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("UTF-8 Text"))
        {
            // UTF-8 test with Japanese characters
            // (Needs a suitable font, try Noto, or Arial Unicode, or M+ fonts. Read misc/fonts/README.txt for details.)
            // - From C++11 you can use the u8"my text" syntax to encode literal strings as UTF-8
            // - For earlier compiler, you may be able to encode your sources as UTF-8 (e.g. Visual Studio save your file as 'UTF-8 without signature')
            // - FOR THIS DEMO FILE ONLY, BECAUSE WE WANT TO SUPPORT OLD COMPILERS, WE ARE *NOT* INCLUDING RAW UTF-8 CHARACTERS IN THIS SOURCE FILE.
            //   Instead we are encoding a few strings with hexadecimal constants. Don't do this in your application!
            //   Please use u8"text in any language" in your application!
            // Note that characters values are preserved even by InputText() if the font cannot be displayed, so you can safely copy & paste garbled characters into another application.
            HanaLovesMe::TextWrapped("CJK text will only appears if the font was loaded with the appropriate CJK character ranges. Call io.Font->AddFontFromFileTTF() manually to load extra character ranges. Read misc/fonts/README.txt for details.");
            HanaLovesMe::Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)"); // Normally we would use u8"blah blah" with the proper characters directly in the string.
            HanaLovesMe::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
            static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
            //static char buf[32] = u8"NIHONGO"; // <- this is how you would write it with C++11, using real kanjis
            HanaLovesMe::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
            HanaLovesMe::TreePop();
        }
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Images"))
    {
        HanaLovesMeIO& io = HanaLovesMe::GetIO();
        HanaLovesMe::TextWrapped("Below we are displaying the font texture (which is the only texture we have access to in this demo). Use the 'ImTextureID' type as stohnly to pass pointers or identifier to your own texture data. Hover the texture for a zoomed view!");

        // Here we are grabbing the font texture because that's the only one we have access to inside the demo code.
        // Remember that ImTextureID is just stohnly for whatever you want it to be, it is essentially a value that will be passed to the render function inside the ImDrawCmd structure.
        // If you use one of the default HanaLovesMe_impl_XXXX.cpp renderer, they all have comments at the top of their file to specify what they expect to be stored in ImTextureID.
        // (for example, the HanaLovesMe_impl_dx11.cpp renderer expect a 'ID3D11ShaderResourceView*' pointer. The HanaLovesMe_impl_glfw_gl3.cpp renderer expect a GLuint OpenGL texture identifier etc.)
        // If you decided that ImTextureID = MyEngineTexture*, then you can pass your MyEngineTexture* pointers to HanaLovesMe::Image(), and gather width/height through your own functions, etc.
        // Using ShowMetricsWindow() as a "debugger" to inspect the draw data that are being passed to your render will help you debug issues if you are confused about this.
        // Consider using the lower-level ImDrawList::AddImage() API, via HanaLovesMe::GetWindowDrawList()->AddImage().
        ImTextureID my_tex_id = io.Fonts->TexID;
        float my_tex_w = (float)io.Fonts->TexWidth;
        float my_tex_h = (float)io.Fonts->TexHeight;

        HanaLovesMe::Text("%.0fx%.0f", my_tex_w, my_tex_h);
        vsize pos = HanaLovesMe::GetCursorScreenPos();
        HanaLovesMe::Image(my_tex_id, vsize(my_tex_w, my_tex_h), vsize(0, 0), vsize(1, 1), xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f), xuifloatcolor(1.0f, 1.0f, 1.0f, 0.5f));
        if (HanaLovesMe::IsItemHovered())
        {
            HanaLovesMe::BeginTooltip();
            float region_sz = 32.0f;
            float region_x = io.MousePos.x - pos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > my_tex_w - region_sz) region_x = my_tex_w - region_sz;
            float region_y = io.MousePos.y - pos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > my_tex_h - region_sz) region_y = my_tex_h - region_sz;
            float zoom = 4.0f;
            HanaLovesMe::Text("Min: (%.2f, %.2f)", region_x, region_y);
            HanaLovesMe::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
            vsize uv0 = vsize((region_x) / my_tex_w, (region_y) / my_tex_h);
            vsize uv1 = vsize((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
            HanaLovesMe::Image(my_tex_id, vsize(region_sz * zoom, region_sz * zoom), uv0, uv1, xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f), xuifloatcolor(1.0f, 1.0f, 1.0f, 0.5f));
            HanaLovesMe::EndTooltip();
        }
        HanaLovesMe::TextWrapped("And now some textured buttons..");
        static int pressed_count = 0;
        for (int i = 0; i < 8; i++)
        {
            HanaLovesMe::PushID(i);
            int frame_padding = -1 + i;     // -1 = uses default padding
            if (HanaLovesMe::ImageButton(my_tex_id, vsize(32, 32), vsize(0, 0), vsize(32.0f / my_tex_w, 32 / my_tex_h), frame_padding, xuifloatcolor(0.0f, 0.0f, 0.0f, 1.0f)))
                pressed_count += 1;
            HanaLovesMe::PopID();
            HanaLovesMe::SameLine();
        }
        HanaLovesMe::NewLine();
        HanaLovesMe::Text("Pressed %d times.", pressed_count);
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Combo"))
    {
        // Expose flags as checkbox for the demo
        static HanaLovesMeComboFlags flags = 0;
        HanaLovesMe::CheckboxFlags("HanaLovesMeComboFlags_PopupAlignLeft", (unsigned int*)& flags, HanaLovesMeComboFlags_PopupAlignLeft);
        HanaLovesMe::SameLine(); HelpMarker("Only makes a difference if the popup is larger than the combo");
        if (HanaLovesMe::CheckboxFlags("HanaLovesMeComboFlags_NoArrowButton", (unsigned int*)& flags, HanaLovesMeComboFlags_NoArrowButton))
            flags &= ~HanaLovesMeComboFlags_NoPreview;     // Clear the other flag, as we cannot combine both
        if (HanaLovesMe::CheckboxFlags("HanaLovesMeComboFlags_NoPreview", (unsigned int*)& flags, HanaLovesMeComboFlags_NoPreview))
            flags &= ~HanaLovesMeComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both

        // General BeginCombo() API, you have full control over your selection data and display type.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag stored in the object itself, etc.)
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static const char* item_current = items[0];            // Here our selection is a single pointer stored outside the object.
        if (HanaLovesMe::BeginCombo("combo 1", item_current, flags)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (item_current == items[n]);
                if (HanaLovesMe::Selectable(items[n], is_selected))
                    item_current = items[n];
                if (is_selected)
                    HanaLovesMe::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            HanaLovesMe::EndCombo();
        }

        // Simplified one-liner Combo() API, using values packed in a single constant string
        static int item_current_2 = 0;
        HanaLovesMe::Combo("combo 2 (one-liner)", &item_current_2, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

        // Simplified one-liner Combo() using an array of const char*
        static int item_current_3 = -1; // If the selection isn't within 0..count, Combo won't display a preview
        HanaLovesMe::Combo("combo 3 (array)", &item_current_3, items, IM_ARRAYSIZE(items));

        // Simplified one-liner Combo() using an accessor function
        struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };
        static int item_current_4 = 0;
        HanaLovesMe::Combo("combo 4 (function)", &item_current_4, &FuncHolder::ItemGetter, items, IM_ARRAYSIZE(items));

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Selectables"))
    {
        // Selectable() has 2 overloads:
        // - The one taking "bool selected" as a read-only selection information. When Selectable() has been clicked is returns true and you can alter selection state accordingly.
        // - The one taking "bool* p_selected" as a read-write selection information (convenient in some cases)
        // The earlier is more flexible, as in real application your selection may be stored in a different manner (in flags within objects, as an external list, etc).
        if (HanaLovesMe::TreeNode("Basic"))
        {
            static bool selection[5] = { false, true, false, false, false };
            HanaLovesMe::Selectable("1. I am selectable", &selection[0]);
            HanaLovesMe::Selectable("2. I am selectable", &selection[1]);
            HanaLovesMe::Text("3. I am not selectable");
            HanaLovesMe::Selectable("4. I am selectable", &selection[3]);
            if (HanaLovesMe::Selectable("5. I am double clickable", selection[4], HanaLovesMeSelectableFlags_AllowDoubleClick))
                if (HanaLovesMe::IsMouseDoubleClicked(0))
                    selection[4] = !selection[4];
            HanaLovesMe::TreePop();
        }
        if (HanaLovesMe::TreeNode("Selection State: Single Selection"))
        {
            static int selected = -1;
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (HanaLovesMe::Selectable(buf, selected == n))
                    selected = n;
            }
            HanaLovesMe::TreePop();
        }
        if (HanaLovesMe::TreeNode("Selection State: Multiple Selection"))
        {
            HelpMarker("Hold CTRL and click to select multiple items.");
            static bool selection[5] = { false, false, false, false, false };
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (HanaLovesMe::Selectable(buf, selection[n]))
                {
                    if (!HanaLovesMe::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                        memset(selection, 0, sizeof(selection));
                    selection[n] ^= 1;
                }
            }
            HanaLovesMe::TreePop();
        }
        if (HanaLovesMe::TreeNode("Rendering more text into the same line"))
        {
            // Using the Selectable() override that takes "bool* p_selected" parameter and toggle your booleans automatically.
            static bool selected[3] = { false, false, false };
            HanaLovesMe::Selectable("main.c", &selected[0]); HanaLovesMe::SameLine(300); HanaLovesMe::Text(" 2,345 bytes");
            HanaLovesMe::Selectable("Hello.cpp", &selected[1]); HanaLovesMe::SameLine(300); HanaLovesMe::Text("12,345 bytes");
            HanaLovesMe::Selectable("Hello.h", &selected[2]); HanaLovesMe::SameLine(300); HanaLovesMe::Text(" 2,345 bytes");
            HanaLovesMe::TreePop();
        }
        if (HanaLovesMe::TreeNode("In columns"))
        {
            HanaLovesMe::Columns(3, NULL, false);
            static bool selected[16] = { 0 };
            for (int i = 0; i < 16; i++)
            {
                char label[32]; sprintf(label, "Item %d", i);
                if (HanaLovesMe::Selectable(label, &selected[i])) {}
                HanaLovesMe::NextColumn();
            }
            HanaLovesMe::Columns(1);
            HanaLovesMe::TreePop();
        }
        if (HanaLovesMe::TreeNode("Grid"))
        {
            static bool selected[4 * 4] = { true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true };
            for (int i = 0; i < 4 * 4; i++)
            {
                HanaLovesMe::PushID(i);
                if (HanaLovesMe::Selectable("Sailor", &selected[i], 0, vsize(50, 50)))
                {
                    // Note: We _unnecessarily_ test for both x/y and i here only to silence some static analyzer. The second part of each test is unnecessary.
                    int x = i % 4;
                    int y = i / 4;
                    if (x > 0) { selected[i - 1] ^= 1; }
                    if (x < 3 && i < 15) { selected[i + 1] ^= 1; }
                    if (y > 0 && i > 3) { selected[i - 4] ^= 1; }
                    if (y < 3 && i < 12) { selected[i + 4] ^= 1; }
                }
                if ((i % 4) < 3) HanaLovesMe::SameLine();
                HanaLovesMe::PopID();
            }
            HanaLovesMe::TreePop();
        }
        if (HanaLovesMe::TreeNode("Alignment"))
        {
            HelpMarker("Alignment applies when a selectable is larger than its text content.\nBy default, Selectables uses style.SelectableTextAlign but it can be overriden on a per-item basis using PushStyleVar().");
            static bool selected[3 * 3] = { true, false, true, false, true, false, true, false, true };
            for (int y = 0; y < 3; y++)
            {
                for (int x = 0; x < 3; x++)
                {
                    vsize alignment = vsize((float)x / 2.0f, (float)y / 2.0f);
                    char name[32];
                    sprintf(name, "(%.1f,%.1f)", alignment.x, alignment.y);
                    if (x > 0) HanaLovesMe::SameLine();
                    HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_SelectableTextAlign, alignment);
                    HanaLovesMe::Selectable(name, &selected[3 * y + x], HanaLovesMeSelectableFlags_None, vsize(80, 80));
                    HanaLovesMe::PopStyleVar();
                }
            }
            HanaLovesMe::TreePop();
        }
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Text Input"))
    {
        if (HanaLovesMe::TreeNode("Multi-line Text Input"))
        {
            // Note: we are using a fixed-sized buffer for simplicity here. See HanaLovesMeInputTextFlags_CallbackResize
            // and the code in misc/cpp/HanaLovesMe_stdlib.h for how to setup InputText() for dynamically resizing strings.
            static char text[1024 * 16] =
                "/*\n"
                " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
                " the hexadecimal encoding of one offending instruction,\n"
                " more formally, the invalid operand with locked CMPXCHG8B\n"
                " instruction bug, is a design flaw in the majority of\n"
                " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
                " processors (all in the P5 microarchitecture).\n"
                "*/\n\n"
                "label:\n"
                "\tlock cmpxchg8b eax\n";

            static HanaLovesMeInputTextFlags flags = HanaLovesMeInputTextFlags_AllowTabInput;
            HelpMarker("You can use the HanaLovesMeInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/HanaLovesMe_stdlib.h for an example. (This is not demonstrated in HanaLovesMe_demo.cpp)");
            HanaLovesMe::CheckboxFlags("HanaLovesMeInputTextFlags_ReadOnly", (unsigned int*)& flags, HanaLovesMeInputTextFlags_ReadOnly);
            HanaLovesMe::CheckboxFlags("HanaLovesMeInputTextFlags_AllowTabInput", (unsigned int*)& flags, HanaLovesMeInputTextFlags_AllowTabInput);
            HanaLovesMe::CheckboxFlags("HanaLovesMeInputTextFlags_CtrlEnterForNewLine", (unsigned int*)& flags, HanaLovesMeInputTextFlags_CtrlEnterForNewLine);
            HanaLovesMe::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), vsize(-1.0f, HanaLovesMe::GetTextLineHeight() * 16), flags);
            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Filtered Text Input"))
        {
            static char buf1[64] = ""; HanaLovesMe::InputText("default", buf1, 64);
            static char buf2[64] = ""; HanaLovesMe::InputText("decimal", buf2, 64, HanaLovesMeInputTextFlags_CharsDecimal);
            static char buf3[64] = ""; HanaLovesMe::InputText("hexadecimal", buf3, 64, HanaLovesMeInputTextFlags_CharsHexadecimal | HanaLovesMeInputTextFlags_CharsUppercase);
            static char buf4[64] = ""; HanaLovesMe::InputText("uppercase", buf4, 64, HanaLovesMeInputTextFlags_CharsUppercase);
            static char buf5[64] = ""; HanaLovesMe::InputText("no blank", buf5, 64, HanaLovesMeInputTextFlags_CharsNoBlank);
            struct TextFilters { static int FilterHanaLovesMeLetters(HanaLovesMeInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("HanaLovesMe", (char)data->EventChar)) return 0; return 1; } };
            static char buf6[64] = ""; HanaLovesMe::InputText("\"HanaLovesMe\" letters", buf6, 64, HanaLovesMeInputTextFlags_CallbackCharFilter, TextFilters::FilterHanaLovesMeLetters);

            HanaLovesMe::Text("Password input");
            static char bufpass[64] = "password123";
            HanaLovesMe::InputText("password", bufpass, 64, HanaLovesMeInputTextFlags_Password | HanaLovesMeInputTextFlags_CharsNoBlank);
            HanaLovesMe::SameLine(); HelpMarker("Display all characters as '*'.\nDisable clipboard cut and copy.\nDisable logging.\n");
            HanaLovesMe::InputTextWithHint("password (w/ hint)", "<password>", bufpass, 64, HanaLovesMeInputTextFlags_Password | HanaLovesMeInputTextFlags_CharsNoBlank);
            HanaLovesMe::InputText("password (clear)", bufpass, 64, HanaLovesMeInputTextFlags_CharsNoBlank);
            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Resize Callback"))
        {
            // If you have a custom string type you would typically create a HanaLovesMe::InputText() wrapper than takes your type as input.
            // See misc/cpp/HanaLovesMe_stdlib.h and .cpp for an implementation of this using std::string.
            HelpMarker("Demonstrate using HanaLovesMeInputTextFlags_CallbackResize to wire your resizable string type to InputText().\n\nSee misc/cpp/HanaLovesMe_stdlib.h for an implementation of this for std::string.");
            struct Funcs
            {
                static int MyResizeCallback(HanaLovesMeInputTextCallbackData* data)
                {
                    if (data->EventFlag == HanaLovesMeInputTextFlags_CallbackResize)
                    {
                        ImVector<char>* my_str = (ImVector<char>*)data->UserData;
                        IM_ASSERT(my_str->begin() == data->Buf);
                        my_str->resize(data->BufSize);  // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
                        data->Buf = my_str->begin();
                    }
                    return 0;
                }

                // Tip: Because HanaLovesMe:: is a namespace you would typicall add your own function into the namespace in your own source files.
                // For example, you may add a function called HanaLovesMe::InputText(const char* label, MyString* my_str).
                static bool MyInputTextMultiline(const char* label, ImVector<char> * my_str, const vsize & size = vsize(0, 0), HanaLovesMeInputTextFlags flags = 0)
                {
                    IM_ASSERT((flags & HanaLovesMeInputTextFlags_CallbackResize) == 0);
                    return HanaLovesMe::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | HanaLovesMeInputTextFlags_CallbackResize, Funcs::MyResizeCallback, (void*)my_str);
                }
            };

            // For this demo we are using ImVector as a string container.
            // Note that because we need to store a terminating zero character, our size/capacity are 1 more than usually reported by a typical string class.
            static ImVector<char> my_str;
            if (my_str.empty())
                my_str.push_back(0);
            Funcs::MyInputTextMultiline("##MyStr", &my_str, vsize(-1.0f, HanaLovesMe::GetTextLineHeight() * 16));
            HanaLovesMe::Text("Data: %p\nSize: %d\nCapacity: %d", (void*)my_str.begin(), my_str.size(), my_str.capacity());
            HanaLovesMe::TreePop();
        }

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Plots Widgets"))
    {
        static bool animate = true;
        HanaLovesMe::Checkbox("Animate", &animate);

        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        HanaLovesMe::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

        // Create a dummy array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
        static float values[90] = { 0 };
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (!animate || refresh_time == 0.0)
            refresh_time = HanaLovesMe::GetTime();
        while (refresh_time < HanaLovesMe::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
        {
            static float phase = 0.0f;
            values[values_offset] = cosf(phase);
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            phase += 0.10f * values_offset;
            refresh_time += 1.0f / 60.0f;
        }
        HanaLovesMe::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", -1.0f, 1.0f, vsize(0, 80));
        HanaLovesMe::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, vsize(0, 80));

        // Use functions to generate output
        // FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
        };
        static int func_type = 0, display_count = 70;
        HanaLovesMe::Separator();
        HanaLovesMe::PushItemWidth(100); HanaLovesMe::Combo("func", &func_type, "Sin\0Saw\0"); HanaLovesMe::PopItemWidth();
        HanaLovesMe::SameLine();
        HanaLovesMe::SliderInt("Sample count", &display_count, 1, 400);
        float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        HanaLovesMe::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, vsize(0, 80));
        HanaLovesMe::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, vsize(0, 80));
        HanaLovesMe::Separator();

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        if (animate)
        {
            progress += progress_dir * 0.4f * HanaLovesMe::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        }

        // Typically we would use vsize(-1.0f,0.0f) to use all available width, or vsize(width,0.0f) for a specified width. vsize(0.0f,0.0f) uses ItemWidth.
        HanaLovesMe::ProgressBar(progress, vsize(0.0f, 0.0f));
        HanaLovesMe::SameLine(0.0f, HanaLovesMe::GetStyle().ItemInnerSpacing.x);
        HanaLovesMe::Text("Progress Bar");

        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
        char buf[32];
        sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        HanaLovesMe::ProgressBar(progress, vsize(0.f, 0.f), buf);
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Color/Picker Widgets"))
    {
        static xuifloatcolor color = xuifloatcolor(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        HanaLovesMe::Checkbox("With Alpha Preview", &alpha_preview);
        HanaLovesMe::Checkbox("With Half Alpha Preview", &alpha_half_preview);
        HanaLovesMe::Checkbox("With Drag and Drop", &drag_and_drop);
        HanaLovesMe::Checkbox("With Options Menu", &options_menu); HanaLovesMe::SameLine(); HelpMarker("Right-click on the individual color widget to show options.");
        HanaLovesMe::Checkbox("With HDR", &hdr); HanaLovesMe::SameLine(); HelpMarker("Currently all this does is to lift the 0..1 limits on dragging widgets.");
        int misc_flags = (hdr ? HanaLovesMeColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : HanaLovesMeColorEditFlags_NoDragDrop) | (alpha_half_preview ? HanaLovesMeColorEditFlags_AlphaPreviewHalf : (alpha_preview ? HanaLovesMeColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : HanaLovesMeColorEditFlags_NoOptions);

        HanaLovesMe::Text("Color widget:");
        HanaLovesMe::SameLine(); HelpMarker("Click on the colored square to open a color picker.\nCTRL+click on individual component to input value.\n");
        HanaLovesMe::ColorEdit3("MyColor##1", (float*)& color, misc_flags);

        HanaLovesMe::Text("Color widget HSV with Alpha:");
        HanaLovesMe::ColorEdit4("MyColor##2", (float*)& color, HanaLovesMeColorEditFlags_DisplayHSV | misc_flags);

        HanaLovesMe::Text("Color widget with Float Display:");
        HanaLovesMe::ColorEdit4("MyColor##2f", (float*)& color, HanaLovesMeColorEditFlags_Float | misc_flags);

        HanaLovesMe::Text("Color button with Picker:");
        HanaLovesMe::SameLine(); HelpMarker("With the HanaLovesMeColorEditFlags_NoInputs flag you can hide all the slider/text inputs.\nWith the HanaLovesMeColorEditFlags_NoLabel flag you can pass a non-empty label which will only be used for the tooltip and picker popup.");
        HanaLovesMe::ColorEdit4("MyColor##3", (float*)& color, HanaLovesMeColorEditFlags_NoInputs | HanaLovesMeColorEditFlags_NoLabel | misc_flags);

        HanaLovesMe::Text("Color button with Custom Picker Popup:");

        // Generate a dummy default palette. The palette will persist and can be edited.
        static bool saved_palette_init = true;
        static xuifloatcolor saved_palette[32] = { };
        if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                HanaLovesMe::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f, saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }

        static xuifloatcolor backup_color;
        bool open_popup = HanaLovesMe::ColorButton("MyColor##3b", color, misc_flags);
        HanaLovesMe::SameLine();
        open_popup |= HanaLovesMe::Button("Palette");
        if (open_popup)
        {
            HanaLovesMe::OpenPopup("mypicker");
            backup_color = color;
        }
        if (HanaLovesMe::BeginPopup("mypicker"))
        {
            HanaLovesMe::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
            HanaLovesMe::Separator();
            HanaLovesMe::ColorPicker4("##picker", (float*)& color, misc_flags | HanaLovesMeColorEditFlags_NoSidePreview | HanaLovesMeColorEditFlags_NoSmallPreview);
            HanaLovesMe::SameLine();

            HanaLovesMe::BeginGroup(); // Lock X position
            HanaLovesMe::Text("Current");
            HanaLovesMe::ColorButton("##current", color, HanaLovesMeColorEditFlags_NoPicker | HanaLovesMeColorEditFlags_AlphaPreviewHalf, vsize(60, 40));
            HanaLovesMe::Text("Previous");
            if (HanaLovesMe::ColorButton("##previous", backup_color, HanaLovesMeColorEditFlags_NoPicker | HanaLovesMeColorEditFlags_AlphaPreviewHalf, vsize(60, 40)))
                color = backup_color;
            HanaLovesMe::Separator();
            HanaLovesMe::Text("Palette");
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                HanaLovesMe::PushID(n);
                if ((n % 8) != 0)
                    HanaLovesMe::SameLine(0.0f, HanaLovesMe::GetStyle().ItemSpacing.y);
                if (HanaLovesMe::ColorButton("##palette", saved_palette[n], HanaLovesMeColorEditFlags_NoAlpha | HanaLovesMeColorEditFlags_NoPicker | HanaLovesMeColorEditFlags_NoTooltip, vsize(20, 20)))
                    color = xuifloatcolor(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

                // Allow user to drop colors into each palette entry
                // (Note that ColorButton is already a drag source by default, unless using HanaLovesMeColorEditFlags_NoDragDrop)
                if (HanaLovesMe::BeginDragDropTarget())
                {
                    if (const HanaLovesMePayload * payload = HanaLovesMe::AcceptDragDropPayload(HanaLovesMe_PAYLOAD_TYPE_COLOR_3F))
                        memcpy((float*)& saved_palette[n], payload->Data, sizeof(float) * 3);
                    if (const HanaLovesMePayload * payload = HanaLovesMe::AcceptDragDropPayload(HanaLovesMe_PAYLOAD_TYPE_COLOR_4F))
                        memcpy((float*)& saved_palette[n], payload->Data, sizeof(float) * 4);
                    HanaLovesMe::EndDragDropTarget();
                }

                HanaLovesMe::PopID();
            }
            HanaLovesMe::EndGroup();
            HanaLovesMe::EndPopup();
        }

        HanaLovesMe::Text("Color button only:");
        HanaLovesMe::ColorButton("MyColor##3c", *(xuifloatcolor*)& color, misc_flags, vsize(80, 80));

        HanaLovesMe::Text("Color picker:");
        static bool alpha = true;
        static bool alpha_bar = true;
        static bool side_preview = true;
        static bool ref_color = false;
        static xuifloatcolor ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);
        static int display_mode = 0;
        static int picker_mode = 0;
        HanaLovesMe::Checkbox("With Alpha", &alpha);
        HanaLovesMe::Checkbox("With Alpha Bar", &alpha_bar);
        HanaLovesMe::Checkbox("With Side Preview", &side_preview);
        if (side_preview)
        {
            HanaLovesMe::SameLine();
            HanaLovesMe::Checkbox("With Ref Color", &ref_color);
            if (ref_color)
            {
                HanaLovesMe::SameLine();
                HanaLovesMe::ColorEdit4("##RefColor", &ref_color_v.x, HanaLovesMeColorEditFlags_NoInputs | misc_flags);
            }
        }
        HanaLovesMe::Combo("Display Mode", &display_mode, "Auto/Current\0None\0RGB Only\0HSV Only\0Hex Only\0");
        HanaLovesMe::SameLine(); HelpMarker("ColorEdit defaults to displaying RGB inputs if you don't specify a display mode, but the user can change it with a right-click.\n\nColorPicker defaults to displaying RGB+HSV+Hex if you don't specify a display mode.\n\nYou can change the defaults using SetColorEditOptions().");
        HanaLovesMe::Combo("Picker Mode", &picker_mode, "Auto/Current\0Hue bar + SV rect\0Hue wheel + SV triangle\0");
        HanaLovesMe::SameLine(); HelpMarker("User can right-click the picker to change mode.");
        HanaLovesMeColorEditFlags flags = misc_flags;
        if (!alpha)            flags |= HanaLovesMeColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
        if (alpha_bar)         flags |= HanaLovesMeColorEditFlags_AlphaBar;
        if (!side_preview)     flags |= HanaLovesMeColorEditFlags_NoSidePreview;
        if (picker_mode == 1)  flags |= HanaLovesMeColorEditFlags_PickerHueBar;
        if (picker_mode == 2)  flags |= HanaLovesMeColorEditFlags_PickerHueWheel;
        if (display_mode == 1) flags |= HanaLovesMeColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
        if (display_mode == 2) flags |= HanaLovesMeColorEditFlags_DisplayRGB;     // Override display mode
        if (display_mode == 3) flags |= HanaLovesMeColorEditFlags_DisplayHSV;
        if (display_mode == 4) flags |= HanaLovesMeColorEditFlags_DisplayHex;
        HanaLovesMe::ColorPicker4("MyColor##4", (float*)& color, flags, ref_color ? &ref_color_v.x : NULL);

        HanaLovesMe::Text("Programmatically set defaults:");
        HanaLovesMe::SameLine(); HelpMarker("SetColorEditOptions() is designed to allow you to set boot-time default.\nWe don't have Push/Pop functions because you can force options on a per-widget basis if needed, and the user can change non-forced ones with the options menu.\nWe don't have a getter to avoid encouraging you to persistently save values that aren't forward-compatible.");
        if (HanaLovesMe::Button("Default: Uint8 + HSV + Hue Bar"))
            HanaLovesMe::SetColorEditOptions(HanaLovesMeColorEditFlags_Uint8 | HanaLovesMeColorEditFlags_DisplayHSV | HanaLovesMeColorEditFlags_PickerHueBar);
        if (HanaLovesMe::Button("Default: Float + HDR + Hue Wheel"))
            HanaLovesMe::SetColorEditOptions(HanaLovesMeColorEditFlags_Float | HanaLovesMeColorEditFlags_HDR | HanaLovesMeColorEditFlags_PickerHueWheel);

        // HSV encoded support (to avoid RGB<>HSV round trips and singularities when S==0 or V==0)
        static xuifloatcolor color_stored_as_hsv(0.23f, 1.0f, 1.0f, 1.0f);
        HanaLovesMe::Spacing();
        HanaLovesMe::Text("HSV encoded colors");
        HanaLovesMe::SameLine(); HelpMarker("By default, colors are given to ColorEdit and ColorPicker in RGB, but HanaLovesMeColorEditFlags_InputHSV allows you to store colors as HSV and pass them to ColorEdit and ColorPicker as HSV. This comes with the added benefit that you can manipulate hue values with the picker even when saturation or value are zero.");
        HanaLovesMe::Text("Color widget with InputHSV:");
        HanaLovesMe::ColorEdit4("HSV shown as HSV##1", (float*)& color_stored_as_hsv, HanaLovesMeColorEditFlags_DisplayRGB | HanaLovesMeColorEditFlags_InputHSV | HanaLovesMeColorEditFlags_Float);
        HanaLovesMe::ColorEdit4("HSV shown as RGB##1", (float*)& color_stored_as_hsv, HanaLovesMeColorEditFlags_DisplayHSV | HanaLovesMeColorEditFlags_InputHSV | HanaLovesMeColorEditFlags_Float);
        HanaLovesMe::DragFloat4("Raw HSV values", (float*)& color_stored_as_hsv, 0.01f, 0.0f, 1.0f);

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Range Widgets"))
    {
        static float begin = 10, end = 90;
        static int begin_i = 100, end_i = 1000;
        HanaLovesMe::DragFloatRange2("range", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
        HanaLovesMe::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %d units", "Max: %d units");
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Data Types"))
    {
        // The DragScalar/InputScalar/SliderScalar functions allow various data types: signed/unsigned int/long long and float/double
        // To avoid polluting the public API with all possible combinations, we use the HanaLovesMeDataType enum to pass the type,
        // and passing all arguments by address.
        // This is the reason the test code below creates local variables to hold "zero" "one" etc. for each types.
        // In practice, if you frequently use a given type that is not covered by the normal API entry points, you can wrap it
        // yourself inside a 1 line function which can take typed argument as value instead of void*, and then pass their address
        // to the generic function. For example:
        //   bool MySliderU64(const char *label, u64* value, u64 min = 0, u64 max = 0, const char* format = "%lld")
        //   {
        //      return SliderScalar(label, HanaLovesMeDataType_U64, value, &min, &max, format);
        //   }

        // Limits (as helper variables that we can take the address of)
        // Note that the SliderScalar function has a maximum usable range of half the natural type maximum, hence the /2 below.
#ifndef LLONG_MIN
        ImS64 LLONG_MIN = -9223372036854775807LL - 1;
        ImS64 LLONG_MAX = 9223372036854775807LL;
        ImU64 ULLONG_MAX = (2ULL * 9223372036854775807LL + 1);
#endif
        const char    s8_zero = 0, s8_one = 1, s8_fifty = 50, s8_min = -128, s8_max = 127;
        const ImU8    u8_zero = 0, u8_one = 1, u8_fifty = 50, u8_min = 0, u8_max = 255;
        const short   s16_zero = 0, s16_one = 1, s16_fifty = 50, s16_min = -32768, s16_max = 32767;
        const ImU16   u16_zero = 0, u16_one = 1, u16_fifty = 50, u16_min = 0, u16_max = 65535;
        const ImS32   s32_zero = 0, s32_one = 1, s32_fifty = 50, s32_min = INT_MIN / 2, s32_max = INT_MAX / 2, s32_hi_a = INT_MAX / 2 - 100, s32_hi_b = INT_MAX / 2;
        const ImU32   u32_zero = 0, u32_one = 1, u32_fifty = 50, u32_min = 0, u32_max = UINT_MAX / 2, u32_hi_a = UINT_MAX / 2 - 100, u32_hi_b = UINT_MAX / 2;
        const ImS64   s64_zero = 0, s64_one = 1, s64_fifty = 50, s64_min = LLONG_MIN / 2, s64_max = LLONG_MAX / 2, s64_hi_a = LLONG_MAX / 2 - 100, s64_hi_b = LLONG_MAX / 2;
        const ImU64   u64_zero = 0, u64_one = 1, u64_fifty = 50, u64_min = 0, u64_max = ULLONG_MAX / 2, u64_hi_a = ULLONG_MAX / 2 - 100, u64_hi_b = ULLONG_MAX / 2;
        const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
        const double  f64_zero = 0., f64_one = 1., f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

        // State
        static char   s8_v = 127;
        static ImU8   u8_v = 255;
        static short  s16_v = 32767;
        static ImU16  u16_v = 65535;
        static ImS32  s32_v = -1;
        static ImU32  u32_v = (ImU32)-1;
        static ImS64  s64_v = -1;
        static ImU64  u64_v = (ImU64)-1;
        static float  f32_v = 0.123f;
        static double f64_v = 90000.01234567890123456789;

        const float drag_speed = 0.2f;
        static bool drag_clamp = false;
        HanaLovesMe::Text("Drags:");
        HanaLovesMe::Checkbox("Clamp integers to 0..50", &drag_clamp); HanaLovesMe::SameLine(); HelpMarker("As with every widgets in dear HanaLovesMe, we never modify values unless there is a user interaction.\nYou can override the clamping limits by using CTRL+Click to input a value.");
        HanaLovesMe::DragScalar("drag s8", HanaLovesMeDataType_S8, &s8_v, drag_speed, drag_clamp ? &s8_zero : NULL, drag_clamp ? &s8_fifty : NULL);
        HanaLovesMe::DragScalar("drag u8", HanaLovesMeDataType_U8, &u8_v, drag_speed, drag_clamp ? &u8_zero : NULL, drag_clamp ? &u8_fifty : NULL, "%u ms");
        HanaLovesMe::DragScalar("drag s16", HanaLovesMeDataType_S16, &s16_v, drag_speed, drag_clamp ? &s16_zero : NULL, drag_clamp ? &s16_fifty : NULL);
        HanaLovesMe::DragScalar("drag u16", HanaLovesMeDataType_U16, &u16_v, drag_speed, drag_clamp ? &u16_zero : NULL, drag_clamp ? &u16_fifty : NULL, "%u ms");
        HanaLovesMe::DragScalar("drag s32", HanaLovesMeDataType_S32, &s32_v, drag_speed, drag_clamp ? &s32_zero : NULL, drag_clamp ? &s32_fifty : NULL);
        HanaLovesMe::DragScalar("drag u32", HanaLovesMeDataType_U32, &u32_v, drag_speed, drag_clamp ? &u32_zero : NULL, drag_clamp ? &u32_fifty : NULL, "%u ms");
        HanaLovesMe::DragScalar("drag s64", HanaLovesMeDataType_S64, &s64_v, drag_speed, drag_clamp ? &s64_zero : NULL, drag_clamp ? &s64_fifty : NULL);
        HanaLovesMe::DragScalar("drag u64", HanaLovesMeDataType_U64, &u64_v, drag_speed, drag_clamp ? &u64_zero : NULL, drag_clamp ? &u64_fifty : NULL);
        HanaLovesMe::DragScalar("drag float", HanaLovesMeDataType_Float, &f32_v, 0.005f, &f32_zero, &f32_one, "%f", 1.0f);
        HanaLovesMe::DragScalar("drag float ^2", HanaLovesMeDataType_Float, &f32_v, 0.005f, &f32_zero, &f32_one, "%f", 2.0f); HanaLovesMe::SameLine(); HelpMarker("You can use the 'power' parameter to increase tweaking precision on one side of the range.");
        HanaLovesMe::DragScalar("drag double", HanaLovesMeDataType_Double, &f64_v, 0.0005f, &f64_zero, NULL, "%.10f grams", 1.0f);
        HanaLovesMe::DragScalar("drag double ^2", HanaLovesMeDataType_Double, &f64_v, 0.0005f, &f64_zero, &f64_one, "0 < %.10f < 1", 2.0f);

        HanaLovesMe::Text("Sliders");
        HanaLovesMe::SliderScalar("slider s8 full", HanaLovesMeDataType_S8, &s8_v, &s8_min, &s8_max, "%d");
        HanaLovesMe::SliderScalar("slider u8 full", HanaLovesMeDataType_U8, &u8_v, &u8_min, &u8_max, "%u");
        HanaLovesMe::SliderScalar("slider s16 full", HanaLovesMeDataType_S16, &s16_v, &s16_min, &s16_max, "%d");
        HanaLovesMe::SliderScalar("slider u16 full", HanaLovesMeDataType_U16, &u16_v, &u16_min, &u16_max, "%u");
        HanaLovesMe::SliderScalar("slider s32 low", HanaLovesMeDataType_S32, &s32_v, &s32_zero, &s32_fifty, "%d");
        HanaLovesMe::SliderScalar("slider s32 high", HanaLovesMeDataType_S32, &s32_v, &s32_hi_a, &s32_hi_b, "%d");
        HanaLovesMe::SliderScalar("slider s32 full", HanaLovesMeDataType_S32, &s32_v, &s32_min, &s32_max, "%d");
        HanaLovesMe::SliderScalar("slider u32 low", HanaLovesMeDataType_U32, &u32_v, &u32_zero, &u32_fifty, "%u");
        HanaLovesMe::SliderScalar("slider u32 high", HanaLovesMeDataType_U32, &u32_v, &u32_hi_a, &u32_hi_b, "%u");
        HanaLovesMe::SliderScalar("slider u32 full", HanaLovesMeDataType_U32, &u32_v, &u32_min, &u32_max, "%u");
        HanaLovesMe::SliderScalar("slider s64 low", HanaLovesMeDataType_S64, &s64_v, &s64_zero, &s64_fifty, "%I64d");
        HanaLovesMe::SliderScalar("slider s64 high", HanaLovesMeDataType_S64, &s64_v, &s64_hi_a, &s64_hi_b, "%I64d");
        HanaLovesMe::SliderScalar("slider s64 full", HanaLovesMeDataType_S64, &s64_v, &s64_min, &s64_max, "%I64d");
        HanaLovesMe::SliderScalar("slider u64 low", HanaLovesMeDataType_U64, &u64_v, &u64_zero, &u64_fifty, "%I64u ms");
        HanaLovesMe::SliderScalar("slider u64 high", HanaLovesMeDataType_U64, &u64_v, &u64_hi_a, &u64_hi_b, "%I64u ms");
        HanaLovesMe::SliderScalar("slider u64 full", HanaLovesMeDataType_U64, &u64_v, &u64_min, &u64_max, "%I64u ms");
        HanaLovesMe::SliderScalar("slider float low", HanaLovesMeDataType_Float, &f32_v, &f32_zero, &f32_one);
        HanaLovesMe::SliderScalar("slider float low^2", HanaLovesMeDataType_Float, &f32_v, &f32_zero, &f32_one, "%.10f", 2.0f);
        HanaLovesMe::SliderScalar("slider float high", HanaLovesMeDataType_Float, &f32_v, &f32_lo_a, &f32_hi_a, "%e");
        HanaLovesMe::SliderScalar("slider double low", HanaLovesMeDataType_Double, &f64_v, &f64_zero, &f64_one, "%.10f grams", 1.0f);
        HanaLovesMe::SliderScalar("slider double low^2", HanaLovesMeDataType_Double, &f64_v, &f64_zero, &f64_one, "%.10f", 2.0f);
        HanaLovesMe::SliderScalar("slider double high", HanaLovesMeDataType_Double, &f64_v, &f64_lo_a, &f64_hi_a, "%e grams", 1.0f);

        static bool inputs_step = true;
        HanaLovesMe::Text("Inputs");
        HanaLovesMe::Checkbox("Show step buttons", &inputs_step);
        HanaLovesMe::InputScalar("input s8", HanaLovesMeDataType_S8, &s8_v, inputs_step ? &s8_one : NULL, NULL, "%d");
        HanaLovesMe::InputScalar("input u8", HanaLovesMeDataType_U8, &u8_v, inputs_step ? &u8_one : NULL, NULL, "%u");
        HanaLovesMe::InputScalar("input s16", HanaLovesMeDataType_S16, &s16_v, inputs_step ? &s16_one : NULL, NULL, "%d");
        HanaLovesMe::InputScalar("input u16", HanaLovesMeDataType_U16, &u16_v, inputs_step ? &u16_one : NULL, NULL, "%u");
        HanaLovesMe::InputScalar("input s32", HanaLovesMeDataType_S32, &s32_v, inputs_step ? &s32_one : NULL, NULL, "%d");
        HanaLovesMe::InputScalar("input s32 hex", HanaLovesMeDataType_S32, &s32_v, inputs_step ? &s32_one : NULL, NULL, "%08X", HanaLovesMeInputTextFlags_CharsHexadecimal);
        HanaLovesMe::InputScalar("input u32", HanaLovesMeDataType_U32, &u32_v, inputs_step ? &u32_one : NULL, NULL, "%u");
        HanaLovesMe::InputScalar("input u32 hex", HanaLovesMeDataType_U32, &u32_v, inputs_step ? &u32_one : NULL, NULL, "%08X", HanaLovesMeInputTextFlags_CharsHexadecimal);
        HanaLovesMe::InputScalar("input s64", HanaLovesMeDataType_S64, &s64_v, inputs_step ? &s64_one : NULL);
        HanaLovesMe::InputScalar("input u64", HanaLovesMeDataType_U64, &u64_v, inputs_step ? &u64_one : NULL);
        HanaLovesMe::InputScalar("input float", HanaLovesMeDataType_Float, &f32_v, inputs_step ? &f32_one : NULL);
        HanaLovesMe::InputScalar("input double", HanaLovesMeDataType_Double, &f64_v, inputs_step ? &f64_one : NULL);

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Multi-component Widgets"))
    {
        static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
        static int vec4i[4] = { 1, 5, 100, 255 };

        HanaLovesMe::InputFloat2("input float2", vec4f);
        HanaLovesMe::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
        HanaLovesMe::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
        HanaLovesMe::InputInt2("input int2", vec4i);
        HanaLovesMe::DragInt2("drag int2", vec4i, 1, 0, 255);
        HanaLovesMe::SliderInt2("slider int2", vec4i, 0, 255);
        HanaLovesMe::Spacing();

        HanaLovesMe::InputFloat3("input float3", vec4f);
        HanaLovesMe::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
        HanaLovesMe::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
        HanaLovesMe::InputInt3("input int3", vec4i);
        HanaLovesMe::DragInt3("drag int3", vec4i, 1, 0, 255);
        HanaLovesMe::SliderInt3("slider int3", vec4i, 0, 255);
        HanaLovesMe::Spacing();

        HanaLovesMe::InputFloat4("input float4", vec4f);
        HanaLovesMe::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
        HanaLovesMe::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
        HanaLovesMe::InputInt4("input int4", vec4i);
        HanaLovesMe::DragInt4("drag int4", vec4i, 1, 0, 255);
        HanaLovesMe::SliderInt4("slider int4", vec4i, 0, 255);

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Vertical Sliders"))
    {
        const float spacing = 4;
        HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_ItemSpacing, vsize(spacing, spacing));

        static int int_value = 0;
        HanaLovesMe::VSliderInt("##int", vsize(18, 160), &int_value, 0, 5);
        HanaLovesMe::SameLine();

        static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
        HanaLovesMe::PushID("set1");
        for (int i = 0; i < 7; i++)
        {
            if (i > 0) HanaLovesMe::SameLine();
            HanaLovesMe::PushID(i);
            HanaLovesMe::PushStyleColor(HanaLovesMeCol_FrameBg, (xuifloatcolor)xuicolor::HSV(i / 7.0f, 0.5f, 0.5f));
            HanaLovesMe::PushStyleColor(HanaLovesMeCol_FrameBgHovered, (xuifloatcolor)xuicolor::HSV(i / 7.0f, 0.6f, 0.5f));
            HanaLovesMe::PushStyleColor(HanaLovesMeCol_FrameBgActive, (xuifloatcolor)xuicolor::HSV(i / 7.0f, 0.7f, 0.5f));
            HanaLovesMe::PushStyleColor(HanaLovesMeCol_SliderGrab, (xuifloatcolor)xuicolor::HSV(i / 7.0f, 0.9f, 0.9f));
            HanaLovesMe::VSliderFloat("##v", vsize(18, 160), &values[i], 0.0f, 1.0f, "");
            if (HanaLovesMe::IsItemActive() || HanaLovesMe::IsItemHovered())
                HanaLovesMe::SetTooltip("%.3f", values[i]);
            HanaLovesMe::PopStyleColor(4);
            HanaLovesMe::PopID();
        }
        HanaLovesMe::PopID();

        HanaLovesMe::SameLine();
        HanaLovesMe::PushID("set2");
        static float values2[4] = { 0.20f, 0.80f, 0.40f, 0.25f };
        const int rows = 3;
        const vsize small_slider_size(18, (160.0f - (rows - 1) * spacing) / rows);
        for (int nx = 0; nx < 4; nx++)
        {
            if (nx > 0) HanaLovesMe::SameLine();
            HanaLovesMe::BeginGroup();
            for (int ny = 0; ny < rows; ny++)
            {
                HanaLovesMe::PushID(nx * rows + ny);
                HanaLovesMe::VSliderFloat("##v", small_slider_size, &values2[nx], 0.0f, 1.0f, "");
                if (HanaLovesMe::IsItemActive() || HanaLovesMe::IsItemHovered())
                    HanaLovesMe::SetTooltip("%.3f", values2[nx]);
                HanaLovesMe::PopID();
            }
            HanaLovesMe::EndGroup();
        }
        HanaLovesMe::PopID();

        HanaLovesMe::SameLine();
        HanaLovesMe::PushID("set3");
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) HanaLovesMe::SameLine();
            HanaLovesMe::PushID(i);
            HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_GrabMinSize, 40);
            HanaLovesMe::VSliderFloat("##v", vsize(40, 160), &values[i], 0.0f, 1.0f, "%.2f\nsec");
            HanaLovesMe::PopStyleVar();
            HanaLovesMe::PopID();
        }
        HanaLovesMe::PopID();
        HanaLovesMe::PopStyleVar();
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Drag and Drop"))
    {
        {
            // ColorEdit widgets automatically act as drag source and drag target.
            // They are using standardized payload strings HanaLovesMe_PAYLOAD_TYPE_COLOR_3F and HanaLovesMe_PAYLOAD_TYPE_COLOR_4F to allow your own widgets
            // to use colors in their drag and drop interaction. Also see the demo in Color Picker -> Palette demo.
            HanaLovesMe::BulletText("Drag and drop in standard widgets");
            HanaLovesMe::Indent();
            static float col1[3] = { 1.0f,0.0f,0.2f };
            static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
            HanaLovesMe::ColorEdit3("color 1", col1);
            HanaLovesMe::ColorEdit4("color 2", col2);
            HanaLovesMe::Unindent();
        }

        {
            HanaLovesMe::BulletText("Drag and drop to copy/swap items");
            HanaLovesMe::Indent();
            enum Mode
            {
                Mode_Copy,
                Mode_Move,
                Mode_Swap
            };
            static int mode = 0;
            if (HanaLovesMe::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } HanaLovesMe::SameLine();
            if (HanaLovesMe::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } HanaLovesMe::SameLine();
            if (HanaLovesMe::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }
            static const char* names[9] = { "Bobby", "Beatrice", "Betty", "Brianna", "Barry", "Bernard", "Bibi", "Blaine", "Bryn" };
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                HanaLovesMe::PushID(n);
                if ((n % 3) != 0)
                    HanaLovesMe::SameLine();
                HanaLovesMe::Button(names[n], vsize(60, 60));

                // Our buttons are both drag sources and drag targets here!
                if (HanaLovesMe::BeginDragDropSource(HanaLovesMeDragDropFlags_None))
                {
                    HanaLovesMe::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));        // Set payload to carry the index of our item (could be anything)
                    if (mode == Mode_Copy) { HanaLovesMe::Text("Copy %s", names[n]); }        // Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
                    if (mode == Mode_Move) { HanaLovesMe::Text("Move %s", names[n]); }
                    if (mode == Mode_Swap) { HanaLovesMe::Text("Swap %s", names[n]); }
                    HanaLovesMe::EndDragDropSource();
                }
                if (HanaLovesMe::BeginDragDropTarget())
                {
                    if (const HanaLovesMePayload * payload = HanaLovesMe::AcceptDragDropPayload("DND_DEMO_CELL"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        if (mode == Mode_Copy)
                        {
                            names[n] = names[payload_n];
                        }
                        if (mode == Mode_Move)
                        {
                            names[n] = names[payload_n];
                            names[payload_n] = "";
                        }
                        if (mode == Mode_Swap)
                        {
                            const char* tmp = names[n];
                            names[n] = names[payload_n];
                            names[payload_n] = tmp;
                        }
                    }
                    HanaLovesMe::EndDragDropTarget();
                }
                HanaLovesMe::PopID();
            }
            HanaLovesMe::Unindent();
        }

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Querying Status (Active/Focused/Hovered etc.)"))
    {
        // Display the value of IsItemHovered() and other common item state functions. Note that the flags can be combined.
        // (because BulletText is an item itself and that would affect the output of IsItemHovered() we pass all state in a single call to simplify the code).
        static int item_type = 1;
        static bool b = false;
        static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };
        static char str[16] = {};
        HanaLovesMe::RadioButton("Text", &item_type, 0);
        HanaLovesMe::RadioButton("Button", &item_type, 1);
        HanaLovesMe::RadioButton("Checkbox", &item_type, 2);
        HanaLovesMe::RadioButton("SliderFloat", &item_type, 3);
        HanaLovesMe::RadioButton("InputText", &item_type, 4);
        HanaLovesMe::RadioButton("ColorEdit4", &item_type, 5);
        HanaLovesMe::RadioButton("MenuItem", &item_type, 6);
        HanaLovesMe::RadioButton("TreeNode (w/ double-click)", &item_type, 7);
        HanaLovesMe::RadioButton("ListBox", &item_type, 8);
        HanaLovesMe::Separator();
        bool ret = false;
        if (item_type == 0) { HanaLovesMe::Text("ITEM: Text"); }                                              // Testing text items with no identifier/interaction
        if (item_type == 1) { ret = HanaLovesMe::Button("ITEM: Button"); }                                    // Testing button
        if (item_type == 2) { ret = HanaLovesMe::Checkbox("ITEM: Checkbox", &b); }                            // Testing checkbox
        if (item_type == 3) { ret = HanaLovesMe::SliderFloat("ITEM: SliderFloat", &col4f[0], 0.0f, 1.0f); }   // Testing basic item
        if (item_type == 4) { ret = HanaLovesMe::InputText("ITEM: InputText", &str[0], IM_ARRAYSIZE(str)); }  // Testing input text (which handles tabbing)
        if (item_type == 5) { ret = HanaLovesMe::ColorEdit4("ITEM: ColorEdit4", col4f); }                     // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 6) { ret = HanaLovesMe::MenuItem("ITEM: MenuItem"); }                                // Testing menu item (they use HanaLovesMeButtonFlags_PressedOnRelease button policy)
        if (item_type == 7) { ret = HanaLovesMe::TreeNodeEx("ITEM: TreeNode w/ HanaLovesMeTreeNodeFlags_OpenOnDoubleClick", HanaLovesMeTreeNodeFlags_OpenOnDoubleClick | HanaLovesMeTreeNodeFlags_NoTreePushOnOpen); } // Testing tree node with HanaLovesMeButtonFlags_PressedOnDoubleClick button policy.
        if (item_type == 8) { const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi" }; static int current = 1; ret = HanaLovesMe::ListBox("ITEM: ListBox", &current, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items)); }
        HanaLovesMe::BulletText(
            "Return value = %d\n"
            "IsItemFocused() = %d\n"
            "IsItemHovered() = %d\n"
            "IsItemHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsItemHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsItemHovered(_AllowWhenOverlapped) = %d\n"
            "IsItemHovered(_RectOnly) = %d\n"
            "IsItemActive() = %d\n"
            "IsItemEdited() = %d\n"
            "IsItemActivated() = %d\n"
            "IsItemDeactivated() = %d\n"
            "IsItemDeactivatedAfterEdit() = %d\n"
            "IsItemVisible() = %d\n"
            "IsItemClicked() = %d\n"
            "GetItemRectMin() = (%.1f, %.1f)\n"
            "GetItemRectMax() = (%.1f, %.1f)\n"
            "GetItemRectSize() = (%.1f, %.1f)",
            ret,
            HanaLovesMe::IsItemFocused(),
            HanaLovesMe::IsItemHovered(),
            HanaLovesMe::IsItemHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup),
            HanaLovesMe::IsItemHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByActiveItem),
            HanaLovesMe::IsItemHovered(HanaLovesMeHoveredFlags_AllowWhenOverlapped),
            HanaLovesMe::IsItemHovered(HanaLovesMeHoveredFlags_RectOnly),
            HanaLovesMe::IsItemActive(),
            HanaLovesMe::IsItemEdited(),
            HanaLovesMe::IsItemActivated(),
            HanaLovesMe::IsItemDeactivated(),
            HanaLovesMe::IsItemDeactivatedAfterEdit(),
            HanaLovesMe::IsItemVisible(),
            HanaLovesMe::IsItemClicked(),
            HanaLovesMe::GetItemRectMin().x, HanaLovesMe::GetItemRectMin().y,
            HanaLovesMe::GetItemRectMax().x, HanaLovesMe::GetItemRectMax().y,
            HanaLovesMe::GetItemRectSize().x, HanaLovesMe::GetItemRectSize().y
        );

        static bool embed_all_inside_a_child_window = false;
        HanaLovesMe::Checkbox("Embed everything inside a child window (for additional testing)", &embed_all_inside_a_child_window);
        if (embed_all_inside_a_child_window)
            HanaLovesMe::BeginChild("outer_child", vsize(0, HanaLovesMe::GetFontSize() * 20), true);

        // Testing IsWindowFocused() function with its various flags. Note that the flags can be combined.
        HanaLovesMe::BulletText(
            "IsWindowFocused() = %d\n"
            "IsWindowFocused(_ChildWindows) = %d\n"
            "IsWindowFocused(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowFocused(_RootWindow) = %d\n"
            "IsWindowFocused(_AnyWindow) = %d\n",
            HanaLovesMe::IsWindowFocused(),
            HanaLovesMe::IsWindowFocused(HanaLovesMeFocusedFlags_ChildWindows),
            HanaLovesMe::IsWindowFocused(HanaLovesMeFocusedFlags_ChildWindows | HanaLovesMeFocusedFlags_RootWindow),
            HanaLovesMe::IsWindowFocused(HanaLovesMeFocusedFlags_RootWindow),
            HanaLovesMe::IsWindowFocused(HanaLovesMeFocusedFlags_AnyWindow));

        // Testing IsWindowHovered() function with its various flags. Note that the flags can be combined.
        HanaLovesMe::BulletText(
            "IsWindowHovered() = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsWindowHovered(_ChildWindows) = %d\n"
            "IsWindowHovered(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowHovered(_ChildWindows|_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_RootWindow) = %d\n"
            "IsWindowHovered(_AnyWindow) = %d\n",
            HanaLovesMe::IsWindowHovered(),
            HanaLovesMe::IsWindowHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup),
            HanaLovesMe::IsWindowHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByActiveItem),
            HanaLovesMe::IsWindowHovered(HanaLovesMeHoveredFlags_ChildWindows),
            HanaLovesMe::IsWindowHovered(HanaLovesMeHoveredFlags_ChildWindows | HanaLovesMeHoveredFlags_RootWindow),
            HanaLovesMe::IsWindowHovered(HanaLovesMeHoveredFlags_ChildWindows | HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup),
            HanaLovesMe::IsWindowHovered(HanaLovesMeHoveredFlags_RootWindow),
            HanaLovesMe::IsWindowHovered(HanaLovesMeHoveredFlags_AnyWindow));

        HanaLovesMe::BeginChild("child", vsize(0, 50), true);
        HanaLovesMe::Text("This is another child window for testing the _ChildWindows flag.");
        HanaLovesMe::EndChild();
        if (embed_all_inside_a_child_window)
            HanaLovesMe::EndChild();

        // Calling IsItemHovered() after begin returns the hovered status of the title bar.
        // This is useful in particular if you want to create a context menu (with BeginPopupContextItem) associated to the title bar of a window.
        static bool test_window = false;
        HanaLovesMe::Checkbox("Hovered/Active tests after Begin() for title bar testing", &test_window);
        if (test_window)
        {
            HanaLovesMe::Begin("Title bar Hovered/Active tests", &test_window);
            if (HanaLovesMe::BeginPopupContextItem()) // <-- This is using IsItemHovered()
            {
                if (HanaLovesMe::MenuItem("Close")) { test_window = false; }
                HanaLovesMe::EndPopup();
            }
            HanaLovesMe::Text(
                "IsItemHovered() after begin = %d (== is title bar hovered)\n"
                "IsItemActive() after begin = %d (== is window being clicked/moved)\n",
                HanaLovesMe::IsItemHovered(), HanaLovesMe::IsItemActive());
            HanaLovesMe::End();
        }

        HanaLovesMe::TreePop();
    }
}

static void ShowDemoWindowLayout()
{
    if (!HanaLovesMe::CollapsingHeader("Layout"))
        return;

    if (HanaLovesMe::TreeNode("Child windows"))
    {
        HelpMarker("Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window.");
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;
        HanaLovesMe::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
        HanaLovesMe::Checkbox("Disable Menu", &disable_menu);

        static int line = 50;
        bool goto_line = HanaLovesMe::Button("Goto");
        HanaLovesMe::SameLine();
        HanaLovesMe::PushItemWidth(100);
        goto_line |= HanaLovesMe::InputInt("##Line", &line, 0, 0, HanaLovesMeInputTextFlags_EnterReturnsTrue);
        HanaLovesMe::PopItemWidth();

        // Child 1: no border, enable horizontal scrollbar
        {
            HanaLovesMeWindowFlags window_flags = HanaLovesMeWindowFlags_HorizontalScrollbar | (disable_mouse_wheel ? HanaLovesMeWindowFlags_NoScrollWithMouse : 0);
            HanaLovesMe::BeginChild("Child1", vsize(HanaLovesMe::GetWindowContentRegionWidth() * 0.5f, 260), false, window_flags);
            for (int i = 0; i < 100; i++)
            {
                HanaLovesMe::Text("%04d: scrollable region", i);
                if (goto_line && line == i)
                    HanaLovesMe::SetScrollHereY();
            }
            if (goto_line && line >= 100)
                HanaLovesMe::SetScrollHereY();
            HanaLovesMe::EndChild();
        }

        HanaLovesMe::SameLine();

        // Child 2: rounded border
        {
            HanaLovesMeWindowFlags window_flags = (disable_mouse_wheel ? HanaLovesMeWindowFlags_NoScrollWithMouse : 0) | (disable_menu ? 0 : HanaLovesMeWindowFlags_MenuBar);
            HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_ChildRounding, 5.0f);
            HanaLovesMe::BeginChild("Child2", vsize(0, 260), true, window_flags);
            if (!disable_menu && HanaLovesMe::BeginMenuBar())
            {
                if (HanaLovesMe::BeginMenu("Menu"))
                {
                    ShowExampleMenuFile();
                    HanaLovesMe::EndMenu();
                }
                HanaLovesMe::EndMenuBar();
            }
            HanaLovesMe::Columns(2);
            for (int i = 0; i < 100; i++)
            {
                char buf[32];
                sprintf(buf, "%03d", i);
                HanaLovesMe::Button(buf, vsize(-1.0f, 0.0f));
                HanaLovesMe::NextColumn();
            }
            HanaLovesMe::EndChild();
            HanaLovesMe::PopStyleVar();
        }

        HanaLovesMe::Separator();

        // Demonstrate a few extra things
        // - Changing HanaLovesMeCol_ChildBg (which is transparent black in default styles)
        // - Using SetCursorPos() to position the child window (because the child window is an item from the POV of the parent window)
        //   You can also call SetNextWindowPos() to position the child window. The parent window will effectively layout from this position.
        // - Using HanaLovesMe::GetItemRectMin/Max() to query the "item" state (because the child window is an item from the POV of the parent window)
        //   See "Widgets" -> "Querying Status (Active/Focused/Hovered etc.)" section for more details about this.
        {
            HanaLovesMe::SetCursorPosX(50);
            HanaLovesMe::PushStyleColor(HanaLovesMeCol_ChildBg, IM_COL32(255, 0, 0, 100));
            HanaLovesMe::BeginChild("blah", vsize(200, 100), true, HanaLovesMeWindowFlags_None);
            for (int n = 0; n < 50; n++)
                HanaLovesMe::Text("Some test %d", n);
            HanaLovesMe::EndChild();
            vsize child_rect_min = HanaLovesMe::GetItemRectMin();
            vsize child_rect_max = HanaLovesMe::GetItemRectMax();
            HanaLovesMe::PopStyleColor();
            HanaLovesMe::Text("Rect of child window is: (%.0f,%.0f) (%.0f,%.0f)", child_rect_min.x, child_rect_min.y, child_rect_max.x, child_rect_max.y);
        }

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Widgets Width"))
    {
        static float f = 0.0f;
        HanaLovesMe::Text("PushItemWidth(100)");
        HanaLovesMe::SameLine(); HelpMarker("Fixed width.");
        HanaLovesMe::PushItemWidth(100);
        HanaLovesMe::DragFloat("float##1", &f);
        HanaLovesMe::PopItemWidth();

        HanaLovesMe::Text("PushItemWidth(GetWindowWidth() * 0.5f)");
        HanaLovesMe::SameLine(); HelpMarker("Half of window width.");
        HanaLovesMe::PushItemWidth(HanaLovesMe::GetWindowWidth() * 0.5f);
        HanaLovesMe::DragFloat("float##2", &f);
        HanaLovesMe::PopItemWidth();

        HanaLovesMe::Text("PushItemWidth(GetContentRegionAvailWidth() * 0.5f)");
        HanaLovesMe::SameLine(); HelpMarker("Half of available width.\n(~ right-cursor_pos)\n(works within a column set)");
        HanaLovesMe::PushItemWidth(HanaLovesMe::GetContentRegionAvailWidth() * 0.5f);
        HanaLovesMe::DragFloat("float##3", &f);
        HanaLovesMe::PopItemWidth();

        HanaLovesMe::Text("PushItemWidth(-100)");
        HanaLovesMe::SameLine(); HelpMarker("Align to right edge minus 100");
        HanaLovesMe::PushItemWidth(-100);
        HanaLovesMe::DragFloat("float##4", &f);
        HanaLovesMe::PopItemWidth();

        HanaLovesMe::Text("PushItemWidth(-1)");
        HanaLovesMe::SameLine(); HelpMarker("Align to right edge");
        HanaLovesMe::PushItemWidth(-1);
        HanaLovesMe::DragFloat("float##5", &f);
        HanaLovesMe::PopItemWidth();

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Basic Horizontal Layout"))
    {
        HanaLovesMe::TextWrapped("(Use HanaLovesMe::SameLine() to keep adding items to the right of the preceding item)");

        // Text
        HanaLovesMe::Text("Two items: Hello"); HanaLovesMe::SameLine();
        HanaLovesMe::TextColored(xuifloatcolor(1, 1, 0, 1), "Sailor");

        // Adjust spacing
        HanaLovesMe::Text("More spacing: Hello"); HanaLovesMe::SameLine(0, 20);
        HanaLovesMe::TextColored(xuifloatcolor(1, 1, 0, 1), "Sailor");

        // Button
        HanaLovesMe::AlignTextToFramePadding();
        HanaLovesMe::Text("Normal buttons"); HanaLovesMe::SameLine();
        HanaLovesMe::Button("Banana"); HanaLovesMe::SameLine();
        HanaLovesMe::Button("Apple"); HanaLovesMe::SameLine();
        HanaLovesMe::Button("Corniflower");

        // Button
        HanaLovesMe::Text("Small buttons"); HanaLovesMe::SameLine();
        HanaLovesMe::SmallButton("Like this one"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("can fit within a text block.");

        // Aligned to arbitrary position. Easy/cheap column.
        HanaLovesMe::Text("Aligned");
        HanaLovesMe::SameLine(150); HanaLovesMe::Text("x=150");
        HanaLovesMe::SameLine(300); HanaLovesMe::Text("x=300");
        HanaLovesMe::Text("Aligned");
        HanaLovesMe::SameLine(150); HanaLovesMe::SmallButton("x=150");
        HanaLovesMe::SameLine(300); HanaLovesMe::SmallButton("x=300");

        // Checkbox
        static bool c1 = false, c2 = false, c3 = false, c4 = false;
        HanaLovesMe::Checkbox("My", &c1); HanaLovesMe::SameLine();
        HanaLovesMe::Checkbox("Tailor", &c2); HanaLovesMe::SameLine();
        HanaLovesMe::Checkbox("Is", &c3); HanaLovesMe::SameLine();
        HanaLovesMe::Checkbox("Rich", &c4);

        // Various
        static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
        HanaLovesMe::PushItemWidth(80);
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
        static int item = -1;
        HanaLovesMe::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); HanaLovesMe::SameLine();
        HanaLovesMe::SliderFloat("X", &f0, 0.0f, 5.0f); HanaLovesMe::SameLine();
        HanaLovesMe::SliderFloat("Y", &f1, 0.0f, 5.0f); HanaLovesMe::SameLine();
        HanaLovesMe::SliderFloat("Z", &f2, 0.0f, 5.0f);
        HanaLovesMe::PopItemWidth();

        HanaLovesMe::PushItemWidth(80);
        HanaLovesMe::Text("Lists:");
        static int selection[4] = { 0, 1, 2, 3 };
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) HanaLovesMe::SameLine();
            HanaLovesMe::PushID(i);
            HanaLovesMe::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
            HanaLovesMe::PopID();
            //if (HanaLovesMe::IsItemHovered()) HanaLovesMe::SetTooltip("ListBox %d hovered", i);
        }
        HanaLovesMe::PopItemWidth();

        // Dummy
        vsize button_sz(40, 40);
        HanaLovesMe::Button("A", button_sz); HanaLovesMe::SameLine();
        HanaLovesMe::Dummy(button_sz); HanaLovesMe::SameLine();
        HanaLovesMe::Button("B", button_sz);

        // Manually wrapping (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
        HanaLovesMe::Text("Manually wrapping:");
        HanaLovesMeStyle& style = HanaLovesMe::GetStyle();
        int buttons_count = 20;
        float window_visible_x2 = HanaLovesMe::GetWindowPos().x + HanaLovesMe::GetWindowContentRegionMax().x;
        for (int n = 0; n < buttons_count; n++)
        {
            HanaLovesMe::PushID(n);
            HanaLovesMe::Button("Box", button_sz);
            float last_button_x2 = HanaLovesMe::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
            if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
                HanaLovesMe::SameLine();
            HanaLovesMe::PopID();
        }

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Tabs"))
    {
        if (HanaLovesMe::TreeNode("Basic"))
        {
            HanaLovesMeTabBarFlags tab_bar_flags = HanaLovesMeTabBarFlags_None;
            if (HanaLovesMe::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (HanaLovesMe::BeginTabItem("Avocado"))
                {
                    HanaLovesMe::Text("This is the Avocado tab!\nblah blah blah blah blah");
                    HanaLovesMe::EndTabItem();
                }
                if (HanaLovesMe::BeginTabItem("Broccoli"))
                {
                    HanaLovesMe::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                    HanaLovesMe::EndTabItem();
                }
                if (HanaLovesMe::BeginTabItem("Cucumber"))
                {
                    HanaLovesMe::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    HanaLovesMe::EndTabItem();
                }
                HanaLovesMe::EndTabBar();
            }
            HanaLovesMe::Separator();
            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Advanced & Close Button"))
        {
            // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
            static HanaLovesMeTabBarFlags tab_bar_flags = HanaLovesMeTabBarFlags_Reorderable;
            HanaLovesMe::CheckboxFlags("HanaLovesMeTabBarFlags_Reorderable", (unsigned int*)& tab_bar_flags, HanaLovesMeTabBarFlags_Reorderable);
            HanaLovesMe::CheckboxFlags("HanaLovesMeTabBarFlags_AutoSelectNewTabs", (unsigned int*)& tab_bar_flags, HanaLovesMeTabBarFlags_AutoSelectNewTabs);
            HanaLovesMe::CheckboxFlags("HanaLovesMeTabBarFlags_TabListPopupButton", (unsigned int*)& tab_bar_flags, HanaLovesMeTabBarFlags_TabListPopupButton);
            HanaLovesMe::CheckboxFlags("HanaLovesMeTabBarFlags_NoCloseWithMiddleMouseButton", (unsigned int*)& tab_bar_flags, HanaLovesMeTabBarFlags_NoCloseWithMiddleMouseButton);
            if ((tab_bar_flags & HanaLovesMeTabBarFlags_FittingPolicyMask_) == 0)
                tab_bar_flags |= HanaLovesMeTabBarFlags_FittingPolicyDefault_;
            if (HanaLovesMe::CheckboxFlags("HanaLovesMeTabBarFlags_FittingPolicyResizeDown", (unsigned int*)& tab_bar_flags, HanaLovesMeTabBarFlags_FittingPolicyResizeDown))
                tab_bar_flags &= ~(HanaLovesMeTabBarFlags_FittingPolicyMask_ ^ HanaLovesMeTabBarFlags_FittingPolicyResizeDown);
            if (HanaLovesMe::CheckboxFlags("HanaLovesMeTabBarFlags_FittingPolicyScroll", (unsigned int*)& tab_bar_flags, HanaLovesMeTabBarFlags_FittingPolicyScroll))
                tab_bar_flags &= ~(HanaLovesMeTabBarFlags_FittingPolicyMask_ ^ HanaLovesMeTabBarFlags_FittingPolicyScroll);

            // Tab Bar
            const char* names[4] = { "Artichoke", "Beetroot", "Celery", "Daikon" };
            static bool opened[4] = { true, true, true, true }; // Persistent user state
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
            {
                if (n > 0) { HanaLovesMe::SameLine(); }
                HanaLovesMe::Checkbox(names[n], &opened[n]);
            }

            // Passing a bool* to BeginTabItem() is similar to passing one to Begin(): the underlying bool will be set to false when the tab is closed.
            if (HanaLovesMe::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                    if (opened[n] && HanaLovesMe::BeginTabItem(names[n], &opened[n]))
                    {
                        HanaLovesMe::Text("This is the %s tab!", names[n]);
                        if (n & 1)
                            HanaLovesMe::Text("I am an odd tab.");
                        HanaLovesMe::EndTabItem();
                    }
                HanaLovesMe::EndTabBar();
            }
            HanaLovesMe::Separator();
            HanaLovesMe::TreePop();
        }
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Groups"))
    {
        HelpMarker("Using HanaLovesMe::BeginGroup()/EndGroup() to layout items. BeginGroup() basically locks the horizontal position. EndGroup() bundles the whole group so that you can use functions such as IsItemHovered() on it.");
        HanaLovesMe::BeginGroup();
        {
            HanaLovesMe::BeginGroup();
            HanaLovesMe::Button("AAA");
            HanaLovesMe::SameLine();
            HanaLovesMe::Button("BBB");
            HanaLovesMe::SameLine();
            HanaLovesMe::BeginGroup();
            HanaLovesMe::Button("CCC");
            HanaLovesMe::Button("DDD");
            HanaLovesMe::EndGroup();
            HanaLovesMe::SameLine();
            HanaLovesMe::Button("EEE");
            HanaLovesMe::EndGroup();
            if (HanaLovesMe::IsItemHovered())
                HanaLovesMe::SetTooltip("First group hovered");
        }
        // Capture the group size and create widgets using the same size
        vsize size = HanaLovesMe::GetItemRectSize();
        const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
        HanaLovesMe::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

        HanaLovesMe::Button("ACTION", vsize((size.x - HanaLovesMe::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        HanaLovesMe::SameLine();
        HanaLovesMe::Button("REACTION", vsize((size.x - HanaLovesMe::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        HanaLovesMe::EndGroup();
        HanaLovesMe::SameLine();

        HanaLovesMe::Button("LEVEhnly\nBUZZWORD", size);
        HanaLovesMe::SameLine();

        if (HanaLovesMe::ListBoxHeader("List", size))
        {
            HanaLovesMe::Selectable("Selected", true);
            HanaLovesMe::Selectable("Not Selected", false);
            HanaLovesMe::ListBoxFooter();
        }

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Text Baseline Alignment"))
    {
        HelpMarker("This is testing the vertical alignment that gets applied on text to keep it aligned with widgets. Lines only composed of text or \"small\" widgets fit in less vertical spaces than lines with normal widgets.");

        HanaLovesMe::Text("One\nTwo\nThree"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Hello\nWorld"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Banana");

        HanaLovesMe::Text("Banana"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Hello\nWorld"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("One\nTwo\nThree");

        HanaLovesMe::Button("HOP##1"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Banana"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Hello\nWorld"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Banana");

        HanaLovesMe::Button("HOP##2"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Hello\nWorld"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Banana");

        HanaLovesMe::Button("TEST##1"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("TEST"); HanaLovesMe::SameLine();
        HanaLovesMe::SmallButton("TEST##2");

        HanaLovesMe::AlignTextToFramePadding(); // If your line starts with text, call this to align it to upcoming widgets.
        HanaLovesMe::Text("Text aligned to Widget"); HanaLovesMe::SameLine();
        HanaLovesMe::Button("Widget##1"); HanaLovesMe::SameLine();
        HanaLovesMe::Text("Widget"); HanaLovesMe::SameLine();
        HanaLovesMe::SmallButton("Widget##2"); HanaLovesMe::SameLine();
        HanaLovesMe::Button("Widget##3");

        // Tree
        const float spacing = HanaLovesMe::GetStyle().ItemInnerSpacing.x;
        HanaLovesMe::Button("Button##1");
        HanaLovesMe::SameLine(0.0f, spacing);
        if (HanaLovesMe::TreeNode("Node##1")) { for (int i = 0; i < 6; i++) HanaLovesMe::BulletText("Item %d..", i); HanaLovesMe::TreePop(); }    // Dummy tree data

        HanaLovesMe::AlignTextToFramePadding();         // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget. Otherwise you can use SmallButton (smaller fit).
        bool node_open = HanaLovesMe::TreeNode("Node##2");  // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add child content.
        HanaLovesMe::SameLine(0.0f, spacing); HanaLovesMe::Button("Button##2");
        if (node_open) { for (int i = 0; i < 6; i++) HanaLovesMe::BulletText("Item %d..", i); HanaLovesMe::TreePop(); }   // Dummy tree data

        // Bullet
        HanaLovesMe::Button("Button##3");
        HanaLovesMe::SameLine(0.0f, spacing);
        HanaLovesMe::BulletText("Bullet text");

        HanaLovesMe::AlignTextToFramePadding();
        HanaLovesMe::BulletText("Node");
        HanaLovesMe::SameLine(0.0f, spacing); HanaLovesMe::Button("Button##4");

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Scrolling"))
    {
        HelpMarker("Use SetScrollHereY() or SetScrollFromPosY() to scroll to a given position.");

        static bool track = true;
        static int track_line = 50, scroll_to_px = 200;
        HanaLovesMe::Checkbox("Track", &track);
        HanaLovesMe::PushItemWidth(100);
        HanaLovesMe::SameLine(130); track |= HanaLovesMe::DragInt("##line", &track_line, 0.25f, 0, 99, "Line = %d");
        bool scroll_to = HanaLovesMe::Button("Scroll To Pos");
        HanaLovesMe::SameLine(130); scroll_to |= HanaLovesMe::DragInt("##pos_y", &scroll_to_px, 1.00f, 0, 9999, "Y = %d px");
        HanaLovesMe::PopItemWidth();
        if (scroll_to) track = false;

        for (int i = 0; i < 5; i++)
        {
            if (i > 0) HanaLovesMe::SameLine();
            HanaLovesMe::BeginGroup();
            HanaLovesMe::Text("%s", i == 0 ? "Top" : i == 1 ? "25%" : i == 2 ? "Center" : i == 3 ? "75%" : "Bottom");
            HanaLovesMe::BeginChild(HanaLovesMe::GetID((void*)(intptr_t)i), vsize(HanaLovesMe::GetWindowWidth() * 0.17f, 200.0f), true);
            if (scroll_to)
                HanaLovesMe::SetScrollFromPosY(HanaLovesMe::GetCursorStartPos().y + scroll_to_px, i * 0.25f);
            for (int line = 0; line < 100; line++)
            {
                if (track && line == track_line)
                {
                    HanaLovesMe::TextColored(xuifloatcolor(1, 1, 0, 1), "Line %d", line);
                    HanaLovesMe::SetScrollHereY(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                } else
                {
                    HanaLovesMe::Text("Line %d", line);
                }
            }
            float scroll_y = HanaLovesMe::GetScrollY(), scroll_max_y = HanaLovesMe::GetScrollMaxY();
            HanaLovesMe::EndChild();
            HanaLovesMe::Text("%.0f/%0.f", scroll_y, scroll_max_y);
            HanaLovesMe::EndGroup();
        }
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Horizontal Scrolling"))
    {
        HelpMarker("Horizontal scrolling for a window has to be enabled explicitly via the HanaLovesMeWindowFlags_HorizontalScrollbar flag.\n\nYou may want to explicitly specify content width by calling SetNextWindowContentWidth() before Begin().");
        static int lines = 7;
        HanaLovesMe::SliderInt("Lines", &lines, 1, 15);
        HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_FrameRounding, 3.0f);
        HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_FramePadding, vsize(2.0f, 1.0f));
        HanaLovesMe::BeginChild("scrolling", vsize(0, HanaLovesMe::GetFrameHeightWithSpacing() * 7 + 30), true, HanaLovesMeWindowFlags_HorizontalScrollbar);
        for (int line = 0; line < lines; line++)
        {
            // Display random stuff (for the sake of this trivial demo we are using basic Button+SameLine. If you want to create your own time line for a real application you may be better off
            // manipulating the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets yourself. You may also want to use the lower-level ImDrawList API)
            int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
            for (int n = 0; n < num_buttons; n++)
            {
                if (n > 0) HanaLovesMe::SameLine();
                HanaLovesMe::PushID(n + line * 1000);
                char num_buf[16];
                sprintf(num_buf, "%d", n);
                const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;
                float hue = n * 0.05f;
                HanaLovesMe::PushStyleColor(HanaLovesMeCol_Button, (xuifloatcolor)xuicolor::HSV(hue, 0.6f, 0.6f));
                HanaLovesMe::PushStyleColor(HanaLovesMeCol_ButtonHovered, (xuifloatcolor)xuicolor::HSV(hue, 0.7f, 0.7f));
                HanaLovesMe::PushStyleColor(HanaLovesMeCol_ButtonActive, (xuifloatcolor)xuicolor::HSV(hue, 0.8f, 0.8f));
                HanaLovesMe::Button(label, vsize(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
                HanaLovesMe::PopStyleColor(3);
                HanaLovesMe::PopID();
            }
        }
        float scroll_x = HanaLovesMe::GetScrollX();
        float scroll_max_x = HanaLovesMe::GetScrollMaxX();
        HanaLovesMe::EndChild();
        HanaLovesMe::PopStyleVar(2);
        float scroll_x_delta = 0.0f;
        HanaLovesMe::SmallButton("<<"); if (HanaLovesMe::IsItemActive()) { scroll_x_delta = -HanaLovesMe::GetIO().DeltaTime * 1000.0f; } HanaLovesMe::SameLine();
        HanaLovesMe::Text("Scroll from code"); HanaLovesMe::SameLine();
        HanaLovesMe::SmallButton(">>"); if (HanaLovesMe::IsItemActive()) { scroll_x_delta = +HanaLovesMe::GetIO().DeltaTime * 1000.0f; } HanaLovesMe::SameLine();
        HanaLovesMe::Text("%.0f/%.0f", scroll_x, scroll_max_x);
        if (scroll_x_delta != 0.0f)
        {
            HanaLovesMe::BeginChild("scrolling"); // Demonstrate a trick: you can use Begin to set yourself in the context of another window (here we are already out of your child window)
            HanaLovesMe::SetScrollX(HanaLovesMe::GetScrollX() + scroll_x_delta);
            HanaLovesMe::EndChild();
        }
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Clipping"))
    {
        static vsize size(100, 100), offset(50, 20);
        HanaLovesMe::TextWrapped("On a per-widget basis we are occasionally clipping text CPU-side if it won't fit in its frame. Otherwise we are doing coarser clipping + passing a scissor rectangle to the renderer. The system is designed to try minimizing both execution and CPU/GPU rendering cost.");
        HanaLovesMe::DragFloat2("size", (float*)& size, 0.5f, 1.0f, 200.0f, "%.0f");
        HanaLovesMe::TextWrapped("(Click and drag)");
        vsize pos = HanaLovesMe::GetCursorScreenPos();
        xuifloatcolor clip_rect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
        HanaLovesMe::InvisibleButton("##dummy", size);
        if (HanaLovesMe::IsItemActive() && HanaLovesMe::IsMouseDragging()) { offset.x += HanaLovesMe::GetIO().MouseDelta.x; offset.y += HanaLovesMe::GetIO().MouseDelta.y; }
        HanaLovesMe::GetWindowDrawList()->AddRectFilled(pos, vsize(pos.x + size.x, pos.y + size.y), IM_COL32(90, 90, 120, 255));
        HanaLovesMe::GetWindowDrawList()->AddText(HanaLovesMe::GetFont(), HanaLovesMe::GetFontSize() * 2.0f, vsize(pos.x + offset.x, pos.y + offset.y), IM_COL32(255, 255, 255, 255), "Line 1 hello\nLine 2 clip me!", NULL, 0.0f, &clip_rect);
        HanaLovesMe::TreePop();
    }
}

static void ShowDemoWindowPopups()
{
    if (!HanaLovesMe::CollapsingHeader("Popups & Modal windows"))
        return;

    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by HanaLovesMe instead of being held by the programmer as we are used to with regular Begin() calls.
    //   User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(HanaLovesMeHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.

    // Typical use for regular windows:
    //   bool my_tool_is_active = false; if (HanaLovesMe::Button("Open")) my_tool_is_active = true; [...] if (my_tool_is_active) Begin("My Tool", &my_tool_is_active) { [...] } End();
    // Typical use for popups:
    //   if (HanaLovesMe::Button("Open")) HanaLovesMe::OpenPopup("MyPopup"); if (HanaLovesMe::BeginPopup("MyPopup") { [...] EndPopup(); }

    // With popups we have to go through a library call (here OpenPopup) to manipulate the visibility state.
    // This may be a bit confusing at first but it should quickly make sense. Follow on the examples below.

    if (HanaLovesMe::TreeNode("Popups"))
    {
        HanaLovesMe::TextWrapped("When a popup is active, it inhibits interacting with windows that are behind the popup. Clicking outside the popup closes it.");

        static int selected_fish = -1;
        const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
        static bool toggles[] = { true, false, false, false, false };

        // Simple selection popup
        // (If you want to show the current selection inside the Button itself, you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
        if (HanaLovesMe::Button("Select.."))
            HanaLovesMe::OpenPopup("my_select_popup");
        HanaLovesMe::SameLine();
        HanaLovesMe::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
        if (HanaLovesMe::BeginPopup("my_select_popup"))
        {
            HanaLovesMe::Text("Aquarium");
            HanaLovesMe::Separator();
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                if (HanaLovesMe::Selectable(names[i]))
                    selected_fish = i;
            HanaLovesMe::EndPopup();
        }

        // Showing a menu with toggles
        if (HanaLovesMe::Button("Toggle.."))
            HanaLovesMe::OpenPopup("my_toggle_popup");
        if (HanaLovesMe::BeginPopup("my_toggle_popup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                HanaLovesMe::MenuItem(names[i], "", &toggles[i]);
            if (HanaLovesMe::BeginMenu("Sub-menu"))
            {
                HanaLovesMe::MenuItem("Click me");
                HanaLovesMe::EndMenu();
            }

            HanaLovesMe::Separator();
            HanaLovesMe::Text("Tooltip here");
            if (HanaLovesMe::IsItemHovered())
                HanaLovesMe::SetTooltip("I am a tooltip over a popup");

            if (HanaLovesMe::Button("Stacked Popup"))
                HanaLovesMe::OpenPopup("another popup");
            if (HanaLovesMe::BeginPopup("another popup"))
            {
                for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    HanaLovesMe::MenuItem(names[i], "", &toggles[i]);
                if (HanaLovesMe::BeginMenu("Sub-menu"))
                {
                    HanaLovesMe::MenuItem("Click me");
                    HanaLovesMe::EndMenu();
                }
                HanaLovesMe::EndPopup();
            }
            HanaLovesMe::EndPopup();
        }

        // Call the more complete ShowExampleMenuFile which we use in various places of this demo
        if (HanaLovesMe::Button("File Menu.."))
            HanaLovesMe::OpenPopup("my_file_popup");
        if (HanaLovesMe::BeginPopup("my_file_popup"))
        {
            ShowExampleMenuFile();
            HanaLovesMe::EndPopup();
        }

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Context menus"))
    {
        // BeginPopupContextItem() is a helper to provide common/simple popup behavior of essentially doing:
        //    if (IsItemHovered() && IsMouseReleased(0))
        //       OpenPopup(id);
        //    return BeginPopup(id);
        // For more advanced uses you may want to replicate and cuztomize this code. This the comments inside BeginPopupContextItem() implementation.
        static float value = 0.5f;
        HanaLovesMe::Text("Value = %.3f (<-- right-click here)", value);
        if (HanaLovesMe::BeginPopupContextItem("item context menu"))
        {
            if (HanaLovesMe::Selectable("Set to zero")) value = 0.0f;
            if (HanaLovesMe::Selectable("Set to PI")) value = 3.1415f;
            HanaLovesMe::PushItemWidth(-1);
            HanaLovesMe::DragFloat("##Value", &value, 0.1f, 0.0f, 0.0f);
            HanaLovesMe::PopItemWidth();
            HanaLovesMe::EndPopup();
        }

        // We can also use OpenPopupOnItemClick() which is the same as BeginPopupContextItem() but without the Begin call.
        // So here we will make it that clicking on the text field with the right mouse button (1) will toggle the visibility of the popup above.
        HanaLovesMe::Text("(You can also right-click me to open the same popup as above.)");
        HanaLovesMe::OpenPopupOnItemClick("item context menu", 1);

        // When used after an item that has an ID (here the Button), we can skip providing an ID to BeginPopupContextItem().
        // BeginPopupContextItem() will use the last item ID as the popup ID.
        // In addition here, we want to include your editable label inside the button label. We use the ### operator to override the ID (read FAQ about ID for details)
        static char name[32] = "Label1";
        char buf[64]; sprintf(buf, "Button: %s###Button", name); // ### operator override ID ignoring the preceding label
        HanaLovesMe::Button(buf);
        if (HanaLovesMe::BeginPopupContextItem())
        {
            HanaLovesMe::Text("Edit name:");
            HanaLovesMe::InputText("##edit", name, IM_ARRAYSIZE(name));
            if (HanaLovesMe::Button("Close"))
                HanaLovesMe::CloseCurrentPopup();
            HanaLovesMe::EndPopup();
        }
        HanaLovesMe::SameLine(); HanaLovesMe::Text("(<-- right-click here)");

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Modals"))
    {
        HanaLovesMe::TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside the window.");

        if (HanaLovesMe::Button("Delete.."))
            HanaLovesMe::OpenPopup("Delete?");

        if (HanaLovesMe::BeginPopupModal("Delete?", NULL, HanaLovesMeWindowFlags_AlwaysAutoResize))
        {
            HanaLovesMe::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
            HanaLovesMe::Separator();

            //static int dummy_i = 0;
            //HanaLovesMe::Combo("Combo", &dummy_i, "Delete\0Delete harder\0");

            static bool dont_ask_me_next_time = false;
            HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_FramePadding, vsize(0, 0));
            HanaLovesMe::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            HanaLovesMe::PopStyleVar();

            if (HanaLovesMe::Button("OK", vsize(120, 0))) { HanaLovesMe::CloseCurrentPopup(); }
            HanaLovesMe::SetItemDefaultFocus();
            HanaLovesMe::SameLine();
            if (HanaLovesMe::Button("Cancel", vsize(120, 0))) { HanaLovesMe::CloseCurrentPopup(); }
            HanaLovesMe::EndPopup();
        }

        if (HanaLovesMe::Button("Stacked modals.."))
            HanaLovesMe::OpenPopup("Stacked 1");
        if (HanaLovesMe::BeginPopupModal("Stacked 1", NULL, HanaLovesMeWindowFlags_MenuBar))
        {
            if (HanaLovesMe::BeginMenuBar())
            {
                if (HanaLovesMe::BeginMenu("File"))
                {
                    if (HanaLovesMe::MenuItem("Dummy menu item")) {}
                    HanaLovesMe::EndMenu();
                }
                HanaLovesMe::EndMenuBar();
            }
            HanaLovesMe::Text("Hello from Stacked The First\nUsing style.Colors[HanaLovesMeCol_ModalWindowDimBg] behind it.");

            // Testing behavior of widgets stacking their own regular popups over the modal.
            static int item = 1;
            static float color[4] = { 0.4f,0.7f,0.0f,0.5f };
            HanaLovesMe::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
            HanaLovesMe::ColorEdit4("color", color);

            if (HanaLovesMe::Button("Add another modal.."))
                HanaLovesMe::OpenPopup("Stacked 2");

            // Also demonstrate passing a bool* to BeginPopupModal(), this will create a regular close button which will close the popup.
            // Note that the visibility state of popups is owned by HanaLovesMe, so the input value of the bool actually doesn't matter here.
            bool dummy_open = true;
            if (HanaLovesMe::BeginPopupModal("Stacked 2", &dummy_open))
            {
                HanaLovesMe::Text("Hello from Stacked The Second!");
                if (HanaLovesMe::Button("Close"))
                    HanaLovesMe::CloseCurrentPopup();
                HanaLovesMe::EndPopup();
            }

            if (HanaLovesMe::Button("Close"))
                HanaLovesMe::CloseCurrentPopup();
            HanaLovesMe::EndPopup();
        }

        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Menus inside a regular window"))
    {
        HanaLovesMe::TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
        HanaLovesMe::Separator();
        // NB: As a quirk in this very specific example, we want to differentiate the parent of this menu from the parent of the various popup menus above.
        // To do so we are encloding the items in a PushID()/PopID() block to make them two different menusets. If we don't, opening any popup above and hovering our menu here
        // would open it. This is because once a menu is active, we allow to switch to a sibling menu by just hovering on it, which is the desired behavior for regular menus.
        HanaLovesMe::PushID("foo");
        HanaLovesMe::MenuItem("Menu item", "CTRL+M");
        if (HanaLovesMe::BeginMenu("Menu inside a regular window"))
        {
            ShowExampleMenuFile();
            HanaLovesMe::EndMenu();
        }
        HanaLovesMe::PopID();
        HanaLovesMe::Separator();
        HanaLovesMe::TreePop();
    }
}

static void ShowDemoWindowColumns()
{
    if (!HanaLovesMe::CollapsingHeader("Columns"))
        return;

    HanaLovesMe::PushID("Columns");

    // Basic columns
    if (HanaLovesMe::TreeNode("Basic"))
    {
        HanaLovesMe::Text("Without border:");
        HanaLovesMe::Columns(3, "mycolumns3", false);  // 3-ways, no border
        HanaLovesMe::Separator();
        for (int n = 0; n < 14; n++)
        {
            char label[32];
            sprintf(label, "Item %d", n);
            if (HanaLovesMe::Selectable(label)) {}
            //if (HanaLovesMe::Button(label, vsize(-1,0))) {}
            HanaLovesMe::NextColumn();
        }
        HanaLovesMe::Columns(1);
        HanaLovesMe::Separator();

        HanaLovesMe::Text("With border:");
        HanaLovesMe::Columns(4, "mycolumns"); // 4-ways, with border
        HanaLovesMe::Separator();
        HanaLovesMe::Text("ID"); HanaLovesMe::NextColumn();
        HanaLovesMe::Text("Name"); HanaLovesMe::NextColumn();
        HanaLovesMe::Text("Path"); HanaLovesMe::NextColumn();
        HanaLovesMe::Text("Hovered"); HanaLovesMe::NextColumn();
        HanaLovesMe::Separator();
        const char* names[3] = { "One", "Two", "Three" };
        const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
        static int selected = -1;
        for (int i = 0; i < 3; i++)
        {
            char label[32];
            sprintf(label, "%04d", i);
            if (HanaLovesMe::Selectable(label, selected == i, HanaLovesMeSelectableFlags_SpanAllColumns))
                selected = i;
            bool hovered = HanaLovesMe::IsItemHovered();
            HanaLovesMe::NextColumn();
            HanaLovesMe::Text(names[i]); HanaLovesMe::NextColumn();
            HanaLovesMe::Text(paths[i]); HanaLovesMe::NextColumn();
            HanaLovesMe::Text("%d", hovered); HanaLovesMe::NextColumn();
        }
        HanaLovesMe::Columns(1);
        HanaLovesMe::Separator();
        HanaLovesMe::TreePop();
    }

    // Create multiple items in a same cell before switching to next column
    if (HanaLovesMe::TreeNode("Mixed items"))
    {
        HanaLovesMe::Columns(3, "mixed");
        HanaLovesMe::Separator();

        HanaLovesMe::Text("Hello");
        HanaLovesMe::Button("Banana");
        HanaLovesMe::NextColumn();

        HanaLovesMe::Text("HanaLovesMe");
        HanaLovesMe::Button("Apple");
        static float foo = 1.0f;
        HanaLovesMe::InputFloat("red", &foo, 0.05f, 0, "%.3f");
        HanaLovesMe::Text("An extra line here.");
        HanaLovesMe::NextColumn();

        HanaLovesMe::Text("Sailor");
        HanaLovesMe::Button("Corniflower");
        static float bar = 1.0f;
        HanaLovesMe::InputFloat("blue", &bar, 0.05f, 0, "%.3f");
        HanaLovesMe::NextColumn();

        if (HanaLovesMe::CollapsingHeader("Category A")) { HanaLovesMe::Text("Blah blah blah"); } HanaLovesMe::NextColumn();
        if (HanaLovesMe::CollapsingHeader("Category B")) { HanaLovesMe::Text("Blah blah blah"); } HanaLovesMe::NextColumn();
        if (HanaLovesMe::CollapsingHeader("Category C")) { HanaLovesMe::Text("Blah blah blah"); } HanaLovesMe::NextColumn();
        HanaLovesMe::Columns(1);
        HanaLovesMe::Separator();
        HanaLovesMe::TreePop();
    }

    // Word wrapping
    if (HanaLovesMe::TreeNode("Word-wrapping"))
    {
        HanaLovesMe::Columns(2, "word-wrapping");
        HanaLovesMe::Separator();
        HanaLovesMe::TextWrapped("The quick brown fox jumps over the lazy dog.");
        HanaLovesMe::TextWrapped("Hello Left");
        HanaLovesMe::NextColumn();
        HanaLovesMe::TextWrapped("The quick brown fox jumps over the lazy dog.");
        HanaLovesMe::TextWrapped("Hello Right");
        HanaLovesMe::Columns(1);
        HanaLovesMe::Separator();
        HanaLovesMe::TreePop();
    }

    if (HanaLovesMe::TreeNode("Borders"))
    {
        // NB: Future columns API should allow automatic horizontal borders.
        static bool h_borders = true;
        static bool v_borders = true;
        HanaLovesMe::Checkbox("horizontal", &h_borders);
        HanaLovesMe::SameLine();
        HanaLovesMe::Checkbox("vertical", &v_borders);
        HanaLovesMe::Columns(4, NULL, v_borders);
        for (int i = 0; i < 4 * 3; i++)
        {
            if (h_borders && HanaLovesMe::GetColumnIndex() == 0)
                HanaLovesMe::Separator();
            HanaLovesMe::Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
            HanaLovesMe::Text("Width %.2f\nOffset %.2f", HanaLovesMe::GetColumnWidth(), HanaLovesMe::GetColumnOffset());
            HanaLovesMe::NextColumn();
        }
        HanaLovesMe::Columns(1);
        if (h_borders)
            HanaLovesMe::Separator();
        HanaLovesMe::TreePop();
    }

    // Scrolling columns
    /*
    if (HanaLovesMe::TreeNode("Vertical Scrolling"))
    {
        HanaLovesMe::BeginChild("##header", vsize(0, HanaLovesMe::GetTextLineHeightWithSpacing()+HanaLovesMe::GetStyle().ItemSpacing.y));
        HanaLovesMe::Columns(3);
        HanaLovesMe::Text("ID"); HanaLovesMe::NextColumn();
        HanaLovesMe::Text("Name"); HanaLovesMe::NextColumn();
        HanaLovesMe::Text("Path"); HanaLovesMe::NextColumn();
        HanaLovesMe::Columns(1);
        HanaLovesMe::Separator();
        HanaLovesMe::EndChild();
        HanaLovesMe::BeginChild("##scrollingregion", vsize(0, 60));
        HanaLovesMe::Columns(3);
        for (int i = 0; i < 10; i++)
        {
            HanaLovesMe::Text("%04d", i); HanaLovesMe::NextColumn();
            HanaLovesMe::Text("Foobar"); HanaLovesMe::NextColumn();
            HanaLovesMe::Text("/path/foobar/%04d/", i); HanaLovesMe::NextColumn();
        }
        HanaLovesMe::Columns(1);
        HanaLovesMe::EndChild();
        HanaLovesMe::TreePop();
    }
    */

    if (HanaLovesMe::TreeNode("Horizontal Scrolling"))
    {
        HanaLovesMe::SetNextWindowContentSize(vsize(1500.0f, 0.0f));
        HanaLovesMe::BeginChild("##ScrollingRegion", vsize(0, HanaLovesMe::GetFontSize() * 20), false, HanaLovesMeWindowFlags_HorizontalScrollbar);
        HanaLovesMe::Columns(10);
        int ITEMS_COUNT = 2000;
        HanaLovesMeListClipper clipper(ITEMS_COUNT);  // Also demonstrate using the clipper for large list
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                for (int j = 0; j < 10; j++)
                {
                    HanaLovesMe::Text("Line %d Column %d...", i, j);
                    HanaLovesMe::NextColumn();
                }
        }
        HanaLovesMe::Columns(1);
        HanaLovesMe::EndChild();
        HanaLovesMe::TreePop();
    }

    bool node_open = HanaLovesMe::TreeNode("Tree within single cell");
    HanaLovesMe::SameLine(); HelpMarker("NB: Tree node must be poped before ending the cell. There's no stohnly of state per-cell.");
    if (node_open)
    {
        HanaLovesMe::Columns(2, "tree items");
        HanaLovesMe::Separator();
        if (HanaLovesMe::TreeNode("Hello")) { HanaLovesMe::BulletText("Sailor"); HanaLovesMe::TreePop(); } HanaLovesMe::NextColumn();
        if (HanaLovesMe::TreeNode("Bonjour")) { HanaLovesMe::BulletText("Marin"); HanaLovesMe::TreePop(); } HanaLovesMe::NextColumn();
        HanaLovesMe::Columns(1);
        HanaLovesMe::Separator();
        HanaLovesMe::TreePop();
    }
    HanaLovesMe::PopID();
}

static void ShowDemoWindowMisc()
{
    if (HanaLovesMe::CollapsingHeader("Filtering"))
    {
        static HanaLovesMeTextFilter filter;
        HanaLovesMe::Text("Filter usage:\n"
            "  \"\"         display all lines\n"
            "  \"xxx\"      display lines containing \"xxx\"\n"
            "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
            "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
        for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
            if (filter.PassFilter(lines[i]))
                HanaLovesMe::BulletText("%s", lines[i]);
    }

    if (HanaLovesMe::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        HanaLovesMeIO& io = HanaLovesMe::GetIO();

        HanaLovesMe::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
        HanaLovesMe::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
        HanaLovesMe::Text("WantTextInput: %d", io.WantTextInput);
        HanaLovesMe::Text("WantSetMousePos: %d", io.WantSetMousePos);
        HanaLovesMe::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

        if (HanaLovesMe::TreeNode("Keyboard, Mouse & Navigation State"))
        {
            if (HanaLovesMe::IsMousePosValid())
                HanaLovesMe::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                HanaLovesMe::Text("Mouse pos: <INVALID>");
            HanaLovesMe::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            HanaLovesMe::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f) { HanaLovesMe::SameLine(); HanaLovesMe::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            HanaLovesMe::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (HanaLovesMe::IsMouseClicked(i)) { HanaLovesMe::SameLine(); HanaLovesMe::Text("b%d", i); }
            HanaLovesMe::Text("Mouse dbl-clicked:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (HanaLovesMe::IsMouseDoubleClicked(i)) { HanaLovesMe::SameLine(); HanaLovesMe::Text("b%d", i); }
            HanaLovesMe::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (HanaLovesMe::IsMouseReleased(i)) { HanaLovesMe::SameLine(); HanaLovesMe::Text("b%d", i); }
            HanaLovesMe::Text("Mouse wheel: %.1f", io.MouseWheel);

            HanaLovesMe::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { HanaLovesMe::SameLine(); HanaLovesMe::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
            HanaLovesMe::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (HanaLovesMe::IsKeyPressed(i)) { HanaLovesMe::SameLine(); HanaLovesMe::Text("%d (0x%X)", i, i); }
            HanaLovesMe::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (HanaLovesMe::IsKeyReleased(i)) { HanaLovesMe::SameLine(); HanaLovesMe::Text("%d (0x%X)", i, i); }
            HanaLovesMe::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            HanaLovesMe::Text("Chars queue:");    for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; HanaLovesMe::SameLine();  HanaLovesMe::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

            HanaLovesMe::Text("NavInputs down:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f) { HanaLovesMe::SameLine(); HanaLovesMe::Text("[%d] %.2f", i, io.NavInputs[i]); }
            HanaLovesMe::Text("NavInputs pressed:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f) { HanaLovesMe::SameLine(); HanaLovesMe::Text("[%d]", i); }
            HanaLovesMe::Text("NavInputs duration:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] >= 0.0f) { HanaLovesMe::SameLine(); HanaLovesMe::Text("[%d] %.2f", i, io.NavInputsDownDuration[i]); }

            HanaLovesMe::Button("Hovering me sets the\nkeyboard capture flag");
            if (HanaLovesMe::IsItemHovered())
                HanaLovesMe::CaptureKeyboardFromApp(true);
            HanaLovesMe::SameLine();
            HanaLovesMe::Button("Holding me clears the\nthe keyboard capture flag");
            if (HanaLovesMe::IsItemActive())
                HanaLovesMe::CaptureKeyboardFromApp(false);

            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Tabbing"))
        {
            HanaLovesMe::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "dummy";
            HanaLovesMe::InputText("1", buf, IM_ARRAYSIZE(buf));
            HanaLovesMe::InputText("2", buf, IM_ARRAYSIZE(buf));
            HanaLovesMe::InputText("3", buf, IM_ARRAYSIZE(buf));
            HanaLovesMe::PushAllowKeyboardFocus(false);
            HanaLovesMe::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            //HanaLovesMe::SameLine(); HelpMarker("Use HanaLovesMe::PushAllowKeyboardFocus(bool)\nto disable tabbing through certain widgets.");
            HanaLovesMe::PopAllowKeyboardFocus();
            HanaLovesMe::InputText("5", buf, IM_ARRAYSIZE(buf));
            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Focus from code"))
        {
            bool focus_1 = HanaLovesMe::Button("Focus on 1"); HanaLovesMe::SameLine();
            bool focus_2 = HanaLovesMe::Button("Focus on 2"); HanaLovesMe::SameLine();
            bool focus_3 = HanaLovesMe::Button("Focus on 3");
            int has_focus = 0;
            static char buf[128] = "click on a button to set focus";

            if (focus_1) HanaLovesMe::SetKeyboardFocusHere();
            HanaLovesMe::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (HanaLovesMe::IsItemActive()) has_focus = 1;

            if (focus_2) HanaLovesMe::SetKeyboardFocusHere();
            HanaLovesMe::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (HanaLovesMe::IsItemActive()) has_focus = 2;

            HanaLovesMe::PushAllowKeyboardFocus(false);
            if (focus_3) HanaLovesMe::SetKeyboardFocusHere();
            HanaLovesMe::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (HanaLovesMe::IsItemActive()) has_focus = 3;
            HanaLovesMe::PopAllowKeyboardFocus();

            if (has_focus)
                HanaLovesMe::Text("Item with focus: %d", has_focus);
            else
                HanaLovesMe::Text("Item with focus: <none>");

            // Use >= 0 parameter to SetKeyboardFocusHere() to focus an upcoming item
            static float f3[3] = { 0.0f, 0.0f, 0.0f };
            int focus_ahead = -1;
            if (HanaLovesMe::Button("Focus on X")) { focus_ahead = 0; } HanaLovesMe::SameLine();
            if (HanaLovesMe::Button("Focus on Y")) { focus_ahead = 1; } HanaLovesMe::SameLine();
            if (HanaLovesMe::Button("Focus on Z")) { focus_ahead = 2; }
            if (focus_ahead != -1) HanaLovesMe::SetKeyboardFocusHere(focus_ahead);
            HanaLovesMe::SliderFloat3("Float3", &f3[0], 0.0f, 1.0f);

            HanaLovesMe::TextWrapped("NB: Cursor & selection are preserved when refocusing last used item in code.");
            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Dragging"))
        {
            HanaLovesMe::TextWrapped("You can use HanaLovesMe::GetMouseDragDelta(0) to query for the dragged amount on any widget.");
            for (int button = 0; button < 3; button++)
                HanaLovesMe::Text("IsMouseDragging(%d):\n  w/ default threshold: %d,\n  w/ zero threshold: %d\n  w/ large threshold: %d",
                    button, HanaLovesMe::IsMouseDragging(button), HanaLovesMe::IsMouseDragging(button, 0.0f), HanaLovesMe::IsMouseDragging(button, 20.0f));

            HanaLovesMe::Button("Drag Me");
            if (HanaLovesMe::IsItemActive())
                HanaLovesMe::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, HanaLovesMe::GetColorU32(HanaLovesMeCol_Button), 4.0f); // Draw a line between the button and the mouse cursor

            // Drag operations gets "unlocked" when the mouse has moved past a certain threshold (the default threshold is stored in io.MouseDragThreshold)
            // You can request a lower or higher threshold using the second parameter of IsMouseDragging() and GetMouseDragDelta()
            vsize value_raw = HanaLovesMe::GetMouseDragDelta(0, 0.0f);
            vsize value_with_lock_threshold = HanaLovesMe::GetMouseDragDelta(0);
            vsize mouse_delta = io.MouseDelta;
            HanaLovesMe::Text("GetMouseDragDelta(0):\n  w/ default threshold: (%.1f, %.1f),\n  w/ zero threshold: (%.1f, %.1f)\nMouseDelta: (%.1f, %.1f)", value_with_lock_threshold.x, value_with_lock_threshold.y, value_raw.x, value_raw.y, mouse_delta.x, mouse_delta.y);
            HanaLovesMe::TreePop();
        }

        if (HanaLovesMe::TreeNode("Mouse cursors"))
        {
            const char* mouse_cursors_names[] = { "Arrow", "TextInput", "Move", "ResizeNS", "ResizeEW", "ResizeNESW", "ResizeNWSE", "Hand" };
            IM_ASSERT(IM_ARRAYSIZE(mouse_cursors_names) == HanaLovesMeMouseCursor_COUNT);

            HanaLovesMe::Text("Current mouse cursor = %d: %s", HanaLovesMe::GetMouseCursor(), mouse_cursors_names[HanaLovesMe::GetMouseCursor()]);
            HanaLovesMe::Text("Hover to see mouse cursors:");
            HanaLovesMe::SameLine(); HelpMarker("Your application can render a different mouse cursor based on what HanaLovesMe::GetMouseCursor() returns. If software cursor rendering (io.MouseDrawCursor) is set HanaLovesMe will draw the right cursor for you, otherwise your backend needs to handle it.");
            for (int i = 0; i < HanaLovesMeMouseCursor_COUNT; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d: %s", i, mouse_cursors_names[i]);
                HanaLovesMe::Bullet(); HanaLovesMe::Selectable(label, false);
                if (HanaLovesMe::IsItemHovered() || HanaLovesMe::IsItemFocused())
                    HanaLovesMe::SetMouseCursor(i);
            }
            HanaLovesMe::TreePop();
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] About Window / ShowAboutWindow()
// Access from HanaLovesMe Demo -> Help -> About
//-----------------------------------------------------------------------------

void HanaLovesMe::ShowAboutWindow(bool* p_open)
{
    if (!HanaLovesMe::Begin("About Dear HanaLovesMe", p_open, HanaLovesMeWindowFlags_AlwaysAutoResize))
    {
        HanaLovesMe::End();
        return;
    }
    HanaLovesMe::Text("Dear HanaLovesMe %s", HanaLovesMe::GetVersion());
    HanaLovesMe::Separator();
    HanaLovesMe::Text("By Omar Cornut and all dear HanaLovesMe contributors.");
    HanaLovesMe::Text("Dear HanaLovesMe is licensed under the MIT License, see LICENSE for more information.");

    static bool show_config_info = false;
    HanaLovesMe::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        HanaLovesMeIO& io = HanaLovesMe::GetIO();
        HanaLovesMeStyle& style = HanaLovesMe::GetStyle();

        bool copy_to_clipboard = HanaLovesMe::Button("Copy to clipboard");
        HanaLovesMe::BeginChildFrame(HanaLovesMe::GetID("cfginfos"), vsize(0, HanaLovesMe::GetTextLineHeightWithSpacing() * 18), HanaLovesMeWindowFlags_NoMove);
        if (copy_to_clipboard)
            HanaLovesMe::LogToClipboard();

        HanaLovesMe::Text("Dear HanaLovesMe %s (%d)", HanaLovesMe_VERSION, HanaLovesMe_VERSION_NUM);
        HanaLovesMe::Separator();
        HanaLovesMe::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        HanaLovesMe::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS
        HanaLovesMe::Text("define: HanaLovesMe_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef HanaLovesMe_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        HanaLovesMe::Text("define: HanaLovesMe_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef HanaLovesMe_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        HanaLovesMe::Text("define: HanaLovesMe_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef HanaLovesMe_DISABLE_WIN32_FUNCTIONS
        HanaLovesMe::Text("define: HanaLovesMe_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef HanaLovesMe_DISABLE_FORMAT_STRING_FUNCTIONS
        HanaLovesMe::Text("define: HanaLovesMe_DISABLE_FORMAT_STRING_FUNCTIONS");
#endif
#ifdef HanaLovesMe_DISABLE_MATH_FUNCTIONS
        HanaLovesMe::Text("define: HanaLovesMe_DISABLE_MATH_FUNCTIONS");
#endif
#ifdef HanaLovesMe_DISABLE_DEFAULT_ALLOCATORS
        HanaLovesMe::Text("define: HanaLovesMe_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef HanaLovesMe_USE_BGRA_PACKED_COLOR
        HanaLovesMe::Text("define: HanaLovesMe_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        HanaLovesMe::Text("define: _WIN32");
#endif
#ifdef _WIN64
        HanaLovesMe::Text("define: _WIN64");
#endif
#ifdef __linux__
        HanaLovesMe::Text("define: __linux__");
#endif
#ifdef __APPLE__
        HanaLovesMe::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        HanaLovesMe::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef __MINGW32__
        HanaLovesMe::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        HanaLovesMe::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        HanaLovesMe::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        HanaLovesMe::Text("define: __clang_version__=%s", __clang_version__);
#endif
        HanaLovesMe::Separator();
        HanaLovesMe::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        HanaLovesMe::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        HanaLovesMe::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & HanaLovesMeConfigFlags_NavEnableKeyboard)        HanaLovesMe::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & HanaLovesMeConfigFlags_NavEnableGamepad)         HanaLovesMe::Text(" NavEnableGamepad");
        if (io.ConfigFlags & HanaLovesMeConfigFlags_NavEnableSetMousePos)     HanaLovesMe::Text(" NavEnableSetMousePos");
        if (io.ConfigFlags & HanaLovesMeConfigFlags_NavNoCaptureKeyboard)     HanaLovesMe::Text(" NavNoCaptureKeyboard");
        if (io.ConfigFlags & HanaLovesMeConfigFlags_NoMouse)                  HanaLovesMe::Text(" NoMouse");
        if (io.ConfigFlags & HanaLovesMeConfigFlags_NoMouseCursorChange)      HanaLovesMe::Text(" NoMouseCursorChange");
        if (io.MouseDrawCursor)                                         HanaLovesMe::Text("io.MouseDrawCursor");
        if (io.ConfigMacOSXBehaviors)                                   HanaLovesMe::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigInputTextCursorBlink)                              HanaLovesMe::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            HanaLovesMe::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       HanaLovesMe::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        HanaLovesMe::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & HanaLovesMeBackendFlags_HasGamepad)             HanaLovesMe::Text(" HasGamepad");
        if (io.BackendFlags & HanaLovesMeBackendFlags_HasMouseCursors)        HanaLovesMe::Text(" HasMouseCursors");
        if (io.BackendFlags & HanaLovesMeBackendFlags_HasSetMousePos)         HanaLovesMe::Text(" HasSetMousePos");
        HanaLovesMe::Separator();
        HanaLovesMe::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        HanaLovesMe::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        HanaLovesMe::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        HanaLovesMe::Separator();
        HanaLovesMe::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        HanaLovesMe::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        HanaLovesMe::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        HanaLovesMe::Text("style.FrameRounding: %.2f", style.FrameRounding);
        HanaLovesMe::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        HanaLovesMe::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        HanaLovesMe::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
            HanaLovesMe::LogFinish();
        HanaLovesMe::EndChildFrame();
    }
    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Style Editor / ShowStyleEditor()
//-----------------------------------------------------------------------------

// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string. Useful for quick combo boxes where the choices are known locally.
bool HanaLovesMe::ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (HanaLovesMe::Combo(label, &style_idx, "Classic\0Dark\0Light\0"))
    {
        switch (style_idx)
        {
        case 0: HanaLovesMe::StyleColorsClassic(); break;
        case 1: HanaLovesMe::StyleColorsDark(); break;
        case 2: HanaLovesMe::StyleColorsLight(); break;
        }
        return true;
    }
    return false;
}

// Demo helper function to select among loaded fonts.
// Here we use the regular BeginCombo()/EndCombo() api which is more the more flexible one.
void HanaLovesMe::ShowFontSelector(const char* label)
{
    HanaLovesMeIO& io = HanaLovesMe::GetIO();
    ImFont* font_current = HanaLovesMe::GetFont();
    if (HanaLovesMe::BeginCombo(label, font_current->GetDebugName()))
    {
        for (int n = 0; n < io.Fonts->Fonts.Size; n++)
        {
            ImFont* font = io.Fonts->Fonts[n];
            HanaLovesMe::PushID((void*)font);
            if (HanaLovesMe::Selectable(font->GetDebugName(), font == font_current))
                io.FontDefault = font;
            HanaLovesMe::PopID();
        }
        HanaLovesMe::EndCombo();
    }
    HanaLovesMe::SameLine();
    HelpMarker(
        "- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
        "- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
        "- Read FAQ and documentation in misc/fonts/ for more details.\n"
        "- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");
}

void HanaLovesMe::ShowStyleEditor(HanaLovesMeStyle * ref)
{
    // You can pass in a reference HanaLovesMeStyle structure to compare to, revert to and save to (else it compares to an internally stored reference)
    HanaLovesMeStyle& style = HanaLovesMe::GetStyle();
    static HanaLovesMeStyle ref_saved_style;

    // Default to using internal stohnly as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    HanaLovesMe::PushItemWidth(HanaLovesMe::GetWindowWidth() * 0.50f);

    if (HanaLovesMe::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    HanaLovesMe::ShowFontSelector("Fonts##Selector");

    // Simplified Settings
    if (HanaLovesMe::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool window_border = (style.WindowBorderSize > 0.0f); if (HanaLovesMe::Checkbox("WindowBorder", &window_border)) style.WindowBorderSize = window_border ? 1.0f : 0.0f; }
    HanaLovesMe::SameLine();
    { bool frame_border = (style.FrameBorderSize > 0.0f); if (HanaLovesMe::Checkbox("FrameBorder", &frame_border)) style.FrameBorderSize = frame_border ? 1.0f : 0.0f; }
    HanaLovesMe::SameLine();
    { bool popup_border = (style.PopupBorderSize > 0.0f); if (HanaLovesMe::Checkbox("PopupBorder", &popup_border)) style.PopupBorderSize = popup_border ? 1.0f : 0.0f; }

    // Save/Revert button
    if (HanaLovesMe::Button("Save Ref"))
        * ref = ref_saved_style = style;
    HanaLovesMe::SameLine();
    if (HanaLovesMe::Button("Revert Ref"))
        style = *ref;
    HanaLovesMe::SameLine();
    HelpMarker("Save/Revert in local non-persistent stohnly. Default Colors definition are not affected. Use \"Export Colors\" below to save them somewhere.");

    HanaLovesMe::Separator();

    if (HanaLovesMe::BeginTabBar("##tabs", HanaLovesMeTabBarFlags_None))
    {
        if (HanaLovesMe::BeginTabItem("Sizes"))
        {
            HanaLovesMe::Text("Main");
            HanaLovesMe::SliderFloat2("WindowPadding", (float*)& style.WindowPadding, 0.0f, 20.0f, "%.0f");
            HanaLovesMe::SliderFloat2("FramePadding", (float*)& style.FramePadding, 0.0f, 20.0f, "%.0f");
            HanaLovesMe::SliderFloat2("ItemSpacing", (float*)& style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            HanaLovesMe::SliderFloat2("ItemInnerSpacing", (float*)& style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            HanaLovesMe::SliderFloat2("TouchExtraPadding", (float*)& style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            HanaLovesMe::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            HanaLovesMe::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            HanaLovesMe::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            HanaLovesMe::Text("Borders");
            HanaLovesMe::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            HanaLovesMe::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            HanaLovesMe::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            HanaLovesMe::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            HanaLovesMe::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            HanaLovesMe::Text("Rounding");
            HanaLovesMe::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            HanaLovesMe::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            HanaLovesMe::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            HanaLovesMe::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            HanaLovesMe::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            HanaLovesMe::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            HanaLovesMe::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            HanaLovesMe::Text("Alignment");
            HanaLovesMe::SliderFloat2("WindowTitleAlign", (float*)& style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            HanaLovesMe::SliderFloat2("ButtonTextAlign", (float*)& style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); HanaLovesMe::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
            HanaLovesMe::SliderFloat2("SelectableTextAlign", (float*)& style.SelectableTextAlign, 0.0f, 1.0f, "%.2f"); HanaLovesMe::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
            HanaLovesMe::Text("Safe Area Padding"); HanaLovesMe::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            HanaLovesMe::SliderFloat2("DisplaySafeAreaPadding", (float*)& style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            HanaLovesMe::EndTabItem();
        }

        if (HanaLovesMe::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (HanaLovesMe::Button("Export Unsaved"))
            {
                if (output_dest == 0)
                    HanaLovesMe::LogToClipboard();
                else
                    HanaLovesMe::LogToTTY();
                HanaLovesMe::LogText("xuifloatcolor* colors = HanaLovesMe::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < HanaLovesMeCol_COUNT; i++)
                {
                    const xuifloatcolor& col = style.Colors[i];
                    const char* name = HanaLovesMe::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(xuifloatcolor)) != 0)
                        HanaLovesMe::LogText("colors[HanaLovesMeCol_%s]%*s= xuifloatcolor(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                HanaLovesMe::LogFinish();
            }
            HanaLovesMe::SameLine(); HanaLovesMe::PushItemWidth(120); HanaLovesMe::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); HanaLovesMe::PopItemWidth();
            HanaLovesMe::SameLine(); HanaLovesMe::Checkbox("Only Modified Colors", &output_only_modified);

            static HanaLovesMeTextFilter filter;
            filter.Draw("Filter colors", HanaLovesMe::GetFontSize() * 16);

            static HanaLovesMeColorEditFlags alpha_flags = 0;
            HanaLovesMe::RadioButton("Opaque", &alpha_flags, 0); HanaLovesMe::SameLine();
            HanaLovesMe::RadioButton("Alpha", &alpha_flags, HanaLovesMeColorEditFlags_AlphaPreview); HanaLovesMe::SameLine();
            HanaLovesMe::RadioButton("Both", &alpha_flags, HanaLovesMeColorEditFlags_AlphaPreviewHalf); HanaLovesMe::SameLine();
            HelpMarker("In the color list:\nLeft-click on colored square to open color picker,\nRight-click to open edit options menu.");

            HanaLovesMe::BeginChild("##colors", vsize(0, 0), true, HanaLovesMeWindowFlags_AlwaysVerticalScrollbar | HanaLovesMeWindowFlags_AlwaysHorizontalScrollbar | HanaLovesMeWindowFlags_NavFlattened);
            HanaLovesMe::PushItemWidth(-160);
            for (int i = 0; i < HanaLovesMeCol_COUNT; i++)
            {
                const char* name = HanaLovesMe::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                HanaLovesMe::PushID(i);
                HanaLovesMe::ColorEdit4("##color", (float*)& style.Colors[i], HanaLovesMeColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(xuifloatcolor)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font, so instead of "Save"/"Revert" you'd use icons.
                    // Read the FAQ and misc/fonts/README.txt about using icon fonts. It's really easy and super convenient!
                    HanaLovesMe::SameLine(0.0f, style.ItemInnerSpacing.x); if (HanaLovesMe::Button("Save")) ref->Colors[i] = style.Colors[i];
                    HanaLovesMe::SameLine(0.0f, style.ItemInnerSpacing.x); if (HanaLovesMe::Button("Revert")) style.Colors[i] = ref->Colors[i];
                }
                HanaLovesMe::SameLine(0.0f, style.ItemInnerSpacing.x);
                HanaLovesMe::TextUnformatted(name);
                HanaLovesMe::PopID();
            }
            HanaLovesMe::PopItemWidth();
            HanaLovesMe::EndChild();

            HanaLovesMe::EndTabItem();
        }

        if (HanaLovesMe::BeginTabItem("Fonts"))
        {
            HanaLovesMeIO& io = HanaLovesMe::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and misc/fonts/README.txt for details on font loading.");
            HanaLovesMe::PushItemWidth(120);
            for (int i = 0; i < atlas->Fonts.Size; i++)
            {
                ImFont* font = atlas->Fonts[i];
                HanaLovesMe::PushID(font);
                bool font_details_opened = HanaLovesMe::TreeNode(font, "Font %d: \"%s\"\n%.2f px, %d glyphs, %d file(s)", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
                HanaLovesMe::SameLine(); if (HanaLovesMe::SmallButton("Set as default")) { io.FontDefault = font; }
                if (font_details_opened)
                {
                    HanaLovesMe::PushFont(font);
                    HanaLovesMe::Text("The quick brown fox jumps over the lazy dog");
                    HanaLovesMe::PopFont();
                    HanaLovesMe::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
                    HanaLovesMe::SameLine(); HelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
                    HanaLovesMe::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, "%.0f");
                    HanaLovesMe::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                    HanaLovesMe::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                    const float surface_sqrt = sqrtf((float)font->MetricsTotalSurface);
                    HanaLovesMe::Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int)surface_sqrt, (int)surface_sqrt);
                    for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
                        if (const ImFontConfig * cfg = &font->ConfigData[config_i])
                            HanaLovesMe::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
                    if (HanaLovesMe::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
                    {
                        // Display all glyphs of the fonts in separate pages of 256 characters
                        for (int base = 0; base < 0x10000; base += 256)
                        {
                            int count = 0;
                            for (int n = 0; n < 256; n++)
                                count += font->FindGlyphNoFallback((ImWchar)(base + n)) ? 1 : 0;
                            if (count > 0 && HanaLovesMe::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                            {
                                float cell_size = font->FontSize * 1;
                                float cell_spacing = style.ItemSpacing.y;
                                vsize base_pos = HanaLovesMe::GetCursorScreenPos();
                                ImDrawList* draw_list = HanaLovesMe::GetWindowDrawList();
                                for (int n = 0; n < 256; n++)
                                {
                                    vsize cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                                    vsize cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                                    const ImFontGlyph * glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
                                    draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                                    if (glyph)
                                        font->RenderChar(draw_list, cell_size, cell_p1, HanaLovesMe::GetColorU32(HanaLovesMeCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
                                    if (glyph && HanaLovesMe::IsMouseHoveringRect(cell_p1, cell_p2))
                                    {
                                        HanaLovesMe::BeginTooltip();
                                        HanaLovesMe::Text("Codepoint: U+%04X", base + n);
                                        HanaLovesMe::Separator();
                                        HanaLovesMe::Text("AdvanceX: %.1f", glyph->AdvanceX);
                                        HanaLovesMe::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                                        HanaLovesMe::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                                        HanaLovesMe::EndTooltip();
                                    }
                                }
                                HanaLovesMe::Dummy(vsize((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
                                HanaLovesMe::TreePop();
                            }
                        }
                        HanaLovesMe::TreePop();
                    }
                    HanaLovesMe::TreePop();
                }
                HanaLovesMe::PopID();
            }
            if (HanaLovesMe::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
            {
                xuifloatcolor tint_col = xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f);
                xuifloatcolor border_col = xuifloatcolor(1.0f, 1.0f, 1.0f, 0.5f);
                HanaLovesMe::Image(atlas->TexID, vsize((float)atlas->TexWidth, (float)atlas->TexHeight), vsize(0, 0), vsize(1, 1), tint_col, border_col);
                HanaLovesMe::TreePop();
            }

            static float window_scale = 1.0f;
            if (HanaLovesMe::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.2f"))   // scale only this window
                HanaLovesMe::SetWindowFontScale(window_scale);
            HanaLovesMe::DragFloat("global scale", &io.FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.2f");      // scale everything
            HanaLovesMe::PopItemWidth();

            HanaLovesMe::EndTabItem();
        }

        if (HanaLovesMe::BeginTabItem("Rendering"))
        {
            HanaLovesMe::Checkbox("Anti-aliased lines", &style.AntiAliasedLines); HanaLovesMe::SameLine(); HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
            HanaLovesMe::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            HanaLovesMe::PushItemWidth(100);
            HanaLovesMe::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, "%.2f", 2.0f);
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
            HanaLovesMe::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            HanaLovesMe::PopItemWidth();

            HanaLovesMe::EndTabItem();
        }

        HanaLovesMe::EndTabBar();
    }

    HanaLovesMe::PopItemWidth();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
//-----------------------------------------------------------------------------

// Demonstrate creating a "main" fullscreen menu bar and populating it.
// Note the difference between BeginMainMenuBar() and BeginMenuBar():
// - BeginMenuBar() = menu-bar inside current window we Begin()-ed into (the window needs the HanaLovesMeWindowFlags_MenuBar flag)
// - BeginMainMenuBar() = helper to create menu-bar-sized window at the top of the main viewport + call BeginMenuBar() into it.
static void ShowExampleAppMainMenuBar()
{
    if (HanaLovesMe::BeginMainMenuBar())
    {
        if (HanaLovesMe::BeginMenu("File"))
        {
            ShowExampleMenuFile();
            HanaLovesMe::EndMenu();
        }
        if (HanaLovesMe::BeginMenu("Edit"))
        {
            if (HanaLovesMe::MenuItem("Undo", "CTRL+Z")) {}
            if (HanaLovesMe::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            HanaLovesMe::Separator();
            if (HanaLovesMe::MenuItem("Cut", "CTRL+X")) {}
            if (HanaLovesMe::MenuItem("Copy", "CTRL+C")) {}
            if (HanaLovesMe::MenuItem("Paste", "CTRL+V")) {}
            HanaLovesMe::EndMenu();
        }
        HanaLovesMe::EndMainMenuBar();
    }
}

// Note that shortcuts are currently provided for display only (future version will add flags to BeginMenu to process shortcuts)
static void ShowExampleMenuFile()
{
    HanaLovesMe::MenuItem("(dummy menu)", NULL, false, false);
    if (HanaLovesMe::MenuItem("New")) {}
    if (HanaLovesMe::MenuItem("Open", "Ctrl+O")) {}
    if (HanaLovesMe::BeginMenu("Open Recent"))
    {
        HanaLovesMe::MenuItem("fish_hat.c");
        HanaLovesMe::MenuItem("fish_hat.inl");
        HanaLovesMe::MenuItem("fish_hat.h");
        if (HanaLovesMe::BeginMenu("More.."))
        {
            HanaLovesMe::MenuItem("Hello");
            HanaLovesMe::MenuItem("Sailor");
            if (HanaLovesMe::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                HanaLovesMe::EndMenu();
            }
            HanaLovesMe::EndMenu();
        }
        HanaLovesMe::EndMenu();
    }
    if (HanaLovesMe::MenuItem("Save", "Ctrl+S")) {}
    if (HanaLovesMe::MenuItem("Save As..")) {}
    HanaLovesMe::Separator();
    if (HanaLovesMe::BeginMenu("Options"))
    {
        static bool enabled = true;
        HanaLovesMe::MenuItem("Enabled", "", &enabled);
        HanaLovesMe::BeginChild("child", vsize(0, 60), true);
        for (int i = 0; i < 10; i++)
            HanaLovesMe::Text("Scrolling Text %d", i);
        HanaLovesMe::EndChild();
        static float f = 0.5f;
        static int n = 0;
        static bool b = true;
        HanaLovesMe::SliderFloat("Value", &f, 0.0f, 1.0f);
        HanaLovesMe::InputFloat("Input", &f, 0.1f);
        HanaLovesMe::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        HanaLovesMe::Checkbox("Check", &b);
        HanaLovesMe::EndMenu();
    }
    if (HanaLovesMe::BeginMenu("Colors"))
    {
        float sz = HanaLovesMe::GetTextLineHeight();
        for (int i = 0; i < HanaLovesMeCol_COUNT; i++)
        {
            const char* name = HanaLovesMe::GetStyleColorName((HanaLovesMeCol)i);
            vsize p = HanaLovesMe::GetCursorScreenPos();
            HanaLovesMe::GetWindowDrawList()->AddRectFilled(p, vsize(p.x + sz, p.y + sz), HanaLovesMe::GetColorU32((HanaLovesMeCol)i));
            HanaLovesMe::Dummy(vsize(sz, sz));
            HanaLovesMe::SameLine();
            HanaLovesMe::MenuItem(name);
        }
        HanaLovesMe::EndMenu();
    }
    if (HanaLovesMe::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (HanaLovesMe::MenuItem("Checked", NULL, true)) {}
    if (HanaLovesMe::MenuItem("Quit", "Alt+F4")) {}
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    HanaLovesMeTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");  // "classify" is only here to provide an example of "C"+[tab] completing to "CL" and displaying matches.
        AutoScroll = true;
        ScrollToBottom = true;
        AddLog("Welcome to Dear HanaLovesMe!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
    static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
    static char* Strdup(const char* str) { size_t len = strlen(str) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, len); }
    static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
        ScrollToBottom = true;
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
        if (AutoScroll)
            ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_open)
    {
        HanaLovesMe::SetNextWindowSize(vsize(520, 600), HanaLovesMeCond_FirstUseEver);
        if (!HanaLovesMe::Begin(title, p_open))
        {
            HanaLovesMe::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (HanaLovesMe::BeginPopupContextItem())
        {
            if (HanaLovesMe::MenuItem("Close Console"))
                * p_open = false;
            HanaLovesMe::EndPopup();
        }

        HanaLovesMe::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        HanaLovesMe::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

        // TODO: display items starting from the bottom

        if (HanaLovesMe::SmallButton("Add Dummy Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); } HanaLovesMe::SameLine();
        if (HanaLovesMe::SmallButton("Add Dummy Error")) { AddLog("[error] something went wrong"); } HanaLovesMe::SameLine();
        if (HanaLovesMe::SmallButton("Clear")) { ClearLog(); } HanaLovesMe::SameLine();
        bool copy_to_clipboard = HanaLovesMe::SmallButton("Copy"); HanaLovesMe::SameLine();
        if (HanaLovesMe::SmallButton("Scroll to bottom")) ScrollToBottom = true;
        //static float t = 0.0f; if (HanaLovesMe::GetTime() - t > 0.02f) { t = HanaLovesMe::GetTime(); AddLog("Spam %f", t); }

        HanaLovesMe::Separator();

        // Options menu
        if (HanaLovesMe::BeginPopup("Options"))
        {
            if (HanaLovesMe::Checkbox("Auto-scroll", &AutoScroll))
                if (AutoScroll)
                    ScrollToBottom = true;
            HanaLovesMe::EndPopup();
        }

        // Options, Filter
        if (HanaLovesMe::Button("Options"))
            HanaLovesMe::OpenPopup("Options");
        HanaLovesMe::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        HanaLovesMe::Separator();

        const float footer_height_to_reserve = HanaLovesMe::GetStyle().ItemSpacing.y + HanaLovesMe::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
        HanaLovesMe::BeginChild("ScrollingRegion", vsize(0, -footer_height_to_reserve), false, HanaLovesMeWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
        if (HanaLovesMe::BeginPopupContextWindow())
        {
            if (HanaLovesMe::Selectable("Clear")) ClearLog();
            HanaLovesMe::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use HanaLovesMe::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
        // You can seek and display only the lines that are visible using the HanaLovesMeListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
        // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
        //     HanaLovesMeListClipper clipper(Items.Size);
        //     while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
        // If your items are of variable size you may want to implement code similar to what HanaLovesMeListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
        HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_ItemSpacing, vsize(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            HanaLovesMe::LogToClipboard();
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            if (!Filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
            bool pop_color = false;
            if (strstr(item, "[error]")) { HanaLovesMe::PushStyleColor(HanaLovesMeCol_Text, xuifloatcolor(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; } else if (strncmp(item, "# ", 2) == 0) { HanaLovesMe::PushStyleColor(HanaLovesMeCol_Text, xuifloatcolor(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
            HanaLovesMe::TextUnformatted(item);
            if (pop_color)
                HanaLovesMe::PopStyleColor();
        }
        if (copy_to_clipboard)
            HanaLovesMe::LogFinish();
        if (ScrollToBottom)
            HanaLovesMe::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        HanaLovesMe::PopStyleVar();
        HanaLovesMe::EndChild();
        HanaLovesMe::Separator();

        // Command-line
        bool reclaim_focus = false;
        if (HanaLovesMe::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), HanaLovesMeInputTextFlags_EnterReturnsTrue | HanaLovesMeInputTextFlags_CallbackCompletion | HanaLovesMeInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        HanaLovesMe::SetItemDefaultFocus();
        if (reclaim_focus)
            HanaLovesMe::SetKeyboardFocusHere(-1); // Auto focus previous widget

        HanaLovesMe::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        } else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        } else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        } else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On commad input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    static int TextEditCallbackStub(HanaLovesMeInputTextCallbackData * data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(HanaLovesMeInputTextCallbackData * data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case HanaLovesMeInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < Commands.Size; i++)
                if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                    candidates.push_back(Commands[i]);

            if (candidates.Size == 0)
            {
                // No match
                AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
            } else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            } else
            {
                // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                AddLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    AddLog("- %s\n", candidates[i]);
            }

            break;
        }
        case HanaLovesMeInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == HanaLovesMeKey_UpArrow)
            {
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            } else if (data->EventKey == HanaLovesMeKey_DownArrow)
            {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != HistoryPos)
            {
                const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    HanaLovesMeTextBuffer     Buf;
    HanaLovesMeTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
    bool                AutoScroll;
    bool                ScrollToBottom;

    ExampleAppLog()
    {
        AutoScroll = true;
        ScrollToBottom = false;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
        if (AutoScroll)
            ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!HanaLovesMe::Begin(title, p_open))
        {
            HanaLovesMe::End();
            return;
        }

        // Options menu
        if (HanaLovesMe::BeginPopup("Options"))
        {
            if (HanaLovesMe::Checkbox("Auto-scroll", &AutoScroll))
                if (AutoScroll)
                    ScrollToBottom = true;
            HanaLovesMe::EndPopup();
        }

        // Main window
        if (HanaLovesMe::Button("Options"))
            HanaLovesMe::OpenPopup("Options");
        HanaLovesMe::SameLine();
        bool clear = HanaLovesMe::Button("Clear");
        HanaLovesMe::SameLine();
        bool copy = HanaLovesMe::Button("Copy");
        HanaLovesMe::SameLine();
        Filter.Draw("Filter", -100.0f);

        HanaLovesMe::Separator();
        HanaLovesMe::BeginChild("scrolling", vsize(0, 0), false, HanaLovesMeWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            HanaLovesMe::LogToClipboard();

        HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_ItemSpacing, vsize(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
            // especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    HanaLovesMe::TextUnformatted(line_start, line_end);
            }
        } else
        {
            // The simplest and easy way to display the entire buffer:
            //   HanaLovesMe::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
            // Here we instead demonstrate using the clipper to only process lines that are within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
            // Using HanaLovesMeListClipper requires A) random access into your data, and B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use the clipper.
            // Storing or skimming through the search result would make it possible (and would be recommended if you want to search through tens of thousands of entries)
            HanaLovesMeListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    HanaLovesMe::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        HanaLovesMe::PopStyleVar();

        if (ScrollToBottom)
            HanaLovesMe::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        HanaLovesMe::EndChild();
        HanaLovesMe::End();
    }
};

// Demonstrate creating a simple log window with basic filtering.
static void ShowExampleAppLog(bool* p_open)
{
    static ExampleAppLog log;

    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    HanaLovesMe::SetNextWindowSize(vsize(500, 400), HanaLovesMeCond_FirstUseEver);
    HanaLovesMe::Begin("Example: Log", p_open);
    if (HanaLovesMe::SmallButton("[Debug] Add 5 entries"))
    {
        static int counter = 0;
        for (int n = 0; n < 5; n++)
        {
            const char* categories[3] = { "info", "warn", "error" };
            const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
            log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
                HanaLovesMe::GetFrameCount(), categories[counter % IM_ARRAYSIZE(categories)], HanaLovesMe::GetTime(), words[counter % IM_ARRAYSIZE(words)]);
            counter++;
        }
    }
    HanaLovesMe::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    log.Draw("Example: Log", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
//-----------------------------------------------------------------------------

// Demonstrate create a window with multiple child windows.
static void ShowExampleAppLayout(bool* p_open)
{
    HanaLovesMe::SetNextWindowSize(vsize(500, 440), HanaLovesMeCond_FirstUseEver);
    if (HanaLovesMe::Begin("Example: Simple layout", p_open, HanaLovesMeWindowFlags_MenuBar))
    {
        if (HanaLovesMe::BeginMenuBar())
        {
            if (HanaLovesMe::BeginMenu("File"))
            {
                if (HanaLovesMe::MenuItem("Close"))* p_open = false;
                HanaLovesMe::EndMenu();
            }
            HanaLovesMe::EndMenuBar();
        }

        // left
        static int selected = 0;
        HanaLovesMe::BeginChild("left pane", vsize(150, 0), true);
        for (int i = 0; i < 100; i++)
        {
            char label[128];
            sprintf(label, "MyObject %d", i);
            if (HanaLovesMe::Selectable(label, selected == i))
                selected = i;
        }
        HanaLovesMe::EndChild();
        HanaLovesMe::SameLine();

        // right
        HanaLovesMe::BeginGroup();
        HanaLovesMe::BeginChild("item view", vsize(0, -HanaLovesMe::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
        HanaLovesMe::Text("MyObject: %d", selected);
        HanaLovesMe::Separator();
        if (HanaLovesMe::BeginTabBar("##Tabs", HanaLovesMeTabBarFlags_None))
        {
            if (HanaLovesMe::BeginTabItem("Description"))
            {
                HanaLovesMe::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                HanaLovesMe::EndTabItem();
            }
            if (HanaLovesMe::BeginTabItem("Details"))
            {
                HanaLovesMe::Text("ID: 0123456789");
                HanaLovesMe::EndTabItem();
            }
            HanaLovesMe::EndTabBar();
        }
        HanaLovesMe::EndChild();
        if (HanaLovesMe::Button("Revert")) {}
        HanaLovesMe::SameLine();
        if (HanaLovesMe::Button("Save")) {}
        HanaLovesMe::EndGroup();
    }
    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
//-----------------------------------------------------------------------------

// Demonstrate create a simple property editor.
static void ShowExampleAppPropertyEditor(bool* p_open)
{
    HanaLovesMe::SetNextWindowSize(vsize(430, 450), HanaLovesMeCond_FirstUseEver);
    if (!HanaLovesMe::Begin("Example: Property editor", p_open))
    {
        HanaLovesMe::End();
        return;
    }

    HelpMarker("This example shows how you may implement a property editor using two columns.\nAll objects/fields data are dummies here.\nRemember that in many simple cases, you can use HanaLovesMe::SameLine(xxx) to position\nyour cursor horizontally instead of using the Columns() API.");

    HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_FramePadding, vsize(2, 2));
    HanaLovesMe::Columns(2);
    HanaLovesMe::Separator();

    struct funcs
    {
        static void ShowDummyObject(const char* prefix, int uid)
        {
            HanaLovesMe::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
            HanaLovesMe::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
            bool node_open = HanaLovesMe::TreeNode("Object", "%s_%u", prefix, uid);
            HanaLovesMe::NextColumn();
            HanaLovesMe::AlignTextToFramePadding();
            HanaLovesMe::Text("my sailor is rich");
            HanaLovesMe::NextColumn();
            if (node_open)
            {
                static float dummy_members[8] = { 0.0f,0.0f,1.0f,3.1416f,100.0f,999.0f };
                for (int i = 0; i < 8; i++)
                {
                    HanaLovesMe::PushID(i); // Use field index as identifier.
                    if (i < 2)
                    {
                        ShowDummyObject("Child", 424242);
                    } else
                    {
                        // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                        HanaLovesMe::AlignTextToFramePadding();
                        HanaLovesMe::TreeNodeEx("Field", HanaLovesMeTreeNodeFlags_Leaf | HanaLovesMeTreeNodeFlags_NoTreePushOnOpen | HanaLovesMeTreeNodeFlags_Bullet, "Field_%d", i);
                        HanaLovesMe::NextColumn();
                        HanaLovesMe::PushItemWidth(-1);
                        if (i >= 5)
                            HanaLovesMe::InputFloat("##value", &dummy_members[i], 1.0f);
                        else
                            HanaLovesMe::DragFloat("##value", &dummy_members[i], 0.01f);
                        HanaLovesMe::PopItemWidth();
                        HanaLovesMe::NextColumn();
                    }
                    HanaLovesMe::PopID();
                }
                HanaLovesMe::TreePop();
            }
            HanaLovesMe::PopID();
        }
    };

    // Iterate dummy objects with dummy members (all the same data)
    for (int obj_i = 0; obj_i < 3; obj_i++)
        funcs::ShowDummyObject("Object", obj_i);

    HanaLovesMe::Columns(1);
    HanaLovesMe::Separator();
    HanaLovesMe::PopStyleVar();
    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
//-----------------------------------------------------------------------------

// Demonstrate/test rendering huge amount of text, and the incidence of clipping.
static void ShowExampleAppLongText(bool* p_open)
{
    HanaLovesMe::SetNextWindowSize(vsize(520, 600), HanaLovesMeCond_FirstUseEver);
    if (!HanaLovesMe::Begin("Example: Long text display", p_open))
    {
        HanaLovesMe::End();
        return;
    }

    static int test_type = 0;
    static HanaLovesMeTextBuffer log;
    static int lines = 0;
    HanaLovesMe::Text("Printing unusually long amount of text.");
    HanaLovesMe::Combo("Test type", &test_type, "Single call to TextUnformatted()\0Multiple calls to Text(), clipped manually\0Multiple calls to Text(), not clipped (slow)\0");
    HanaLovesMe::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
    if (HanaLovesMe::Button("Clear")) { log.clear(); lines = 0; }
    HanaLovesMe::SameLine();
    if (HanaLovesMe::Button("Add 1000 lines"))
    {
        for (int i = 0; i < 1000; i++)
            log.appendf("%i The quick brown fox jumps over the lazy dog\n", lines + i);
        lines += 1000;
    }
    HanaLovesMe::BeginChild("Log");
    switch (test_type)
    {
    case 0:
        // Single call to TextUnformatted() with a big buffer
        HanaLovesMe::TextUnformatted(log.begin(), log.end());
        break;
    case 1:
    {
        // Multiple calls to Text(), manually coarsely clipped - demonstrate how to use the HanaLovesMeListClipper helper.
        HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_ItemSpacing, vsize(0, 0));
        HanaLovesMeListClipper clipper(lines);
        while (clipper.Step())
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                HanaLovesMe::Text("%i The quick brown fox jumps over the lazy dog", i);
        HanaLovesMe::PopStyleVar();
        break;
    }
    case 2:
        // Multiple calls to Text(), not clipped (slow)
        HanaLovesMe::PushStyleVar(HanaLovesMeStyleVar_ItemSpacing, vsize(0, 0));
        for (int i = 0; i < lines; i++)
            HanaLovesMe::Text("%i The quick brown fox jumps over the lazy dog", i);
        HanaLovesMe::PopStyleVar();
        break;
    }
    HanaLovesMe::EndChild();
    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window which gets auto-resized according to its content.
static void ShowExampleAppAutoResize(bool* p_open)
{
    if (!HanaLovesMe::Begin("Example: Auto-resizing window", p_open, HanaLovesMeWindowFlags_AlwaysAutoResize))
    {
        HanaLovesMe::End();
        return;
    }

    static int lines = 10;
    HanaLovesMe::Text("Window will resize every-frame to the size of its content.\nNote that you probably don't want to query the window size to\noutput your content because that would create a feedback loop.");
    HanaLovesMe::SliderInt("Number of lines", &lines, 1, 20);
    for (int i = 0; i < lines; i++)
        HanaLovesMe::Text("%*sThis is line %d", i * 4, "", i); // Pad with space to extend size horizontally
    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window with custom resize constraints.
static void ShowExampleAppConstrainedResize(bool* p_open)
{
    struct CustomConstraints // Helper functions to demonstrate programmatic constraints
    {
        static void Square(HanaLovesMeSizeCallbackData* data) { data->DesiredSize = vsize(IM_MAX(data->DesiredSize.x, data->DesiredSize.y), IM_MAX(data->DesiredSize.x, data->DesiredSize.y)); }
        static void Step(HanaLovesMeSizeCallbackData* data) { float step = (float)(int)(intptr_t)data->UserData; data->DesiredSize = vsize((int)(data->DesiredSize.x / step + 0.5f) * step, (int)(data->DesiredSize.y / step + 0.5f) * step); }
    };

    static bool auto_resize = false;
    static int type = 0;
    static int display_lines = 10;
    if (type == 0) HanaLovesMe::SetNextWindowSizeConstraints(vsize(-1, 0), vsize(-1, FLT_MAX));      // Vertical only
    if (type == 1) HanaLovesMe::SetNextWindowSizeConstraints(vsize(0, -1), vsize(FLT_MAX, -1));      // Horizontal only
    if (type == 2) HanaLovesMe::SetNextWindowSizeConstraints(vsize(100, 100), vsize(FLT_MAX, FLT_MAX)); // Width > 100, Height > 100
    if (type == 3) HanaLovesMe::SetNextWindowSizeConstraints(vsize(400, -1), vsize(500, -1));          // Width 400-500
    if (type == 4) HanaLovesMe::SetNextWindowSizeConstraints(vsize(-1, 400), vsize(-1, 500));          // Height 400-500
    if (type == 5) HanaLovesMe::SetNextWindowSizeConstraints(vsize(0, 0), vsize(FLT_MAX, FLT_MAX), CustomConstraints::Square);                     // Always Square
    if (type == 6) HanaLovesMe::SetNextWindowSizeConstraints(vsize(0, 0), vsize(FLT_MAX, FLT_MAX), CustomConstraints::Step, (void*)(intptr_t)100); // Fixed Step

    HanaLovesMeWindowFlags flags = auto_resize ? HanaLovesMeWindowFlags_AlwaysAutoResize : 0;
    if (HanaLovesMe::Begin("Example: Constrained Resize", p_open, flags))
    {
        const char* desc[] =
        {
            "Resize vertical only",
            "Resize horizontal only",
            "Width > 100, Height > 100",
            "Width 400-500",
            "Height 400-500",
            "Custom: Always Square",
            "Custom: Fixed Steps (100)",
        };
        if (HanaLovesMe::Button("200x200")) { HanaLovesMe::SetWindowSize(vsize(200, 200)); } HanaLovesMe::SameLine();
        if (HanaLovesMe::Button("500x500")) { HanaLovesMe::SetWindowSize(vsize(500, 500)); } HanaLovesMe::SameLine();
        if (HanaLovesMe::Button("800x200")) { HanaLovesMe::SetWindowSize(vsize(800, 200)); }
        HanaLovesMe::PushItemWidth(200);
        HanaLovesMe::Combo("Constraint", &type, desc, IM_ARRAYSIZE(desc));
        HanaLovesMe::DragInt("Lines", &display_lines, 0.2f, 1, 100);
        HanaLovesMe::PopItemWidth();
        HanaLovesMe::Checkbox("Auto-resize", &auto_resize);
        for (int i = 0; i < display_lines; i++)
            HanaLovesMe::Text("%*sHello, sailor! Making this line long enough for the example.", i * 4, "");
    }
    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple static window with no decoration + a context-menu to choose which corner of the screen to use.
static void ShowExampleAppSimpleOverlay(bool* p_open)
{
    const float DISTANCE = 10.0f;
    static int corner = 0;
    HanaLovesMeIO& io = HanaLovesMe::GetIO();
    if (corner != -1)
    {
        vsize window_pos = vsize((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        vsize window_pos_pivot = vsize((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        HanaLovesMe::SetNextWindowPos(window_pos, HanaLovesMeCond_Always, window_pos_pivot);
    }
    HanaLovesMe::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (HanaLovesMe::Begin("Example: Simple overlay", p_open, (corner != -1 ? HanaLovesMeWindowFlags_NoMove : 0) | HanaLovesMeWindowFlags_NoDecoration | HanaLovesMeWindowFlags_AlwaysAutoResize | HanaLovesMeWindowFlags_NoSavedSettings | HanaLovesMeWindowFlags_NoFocusOnAppearing | HanaLovesMeWindowFlags_NoNav))
    {
        HanaLovesMe::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
        HanaLovesMe::Separator();
        if (HanaLovesMe::IsMousePosValid())
            HanaLovesMe::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            HanaLovesMe::Text("Mouse Position: <invalid>");
        if (HanaLovesMe::BeginPopupContextWindow())
        {
            if (HanaLovesMe::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (HanaLovesMe::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (HanaLovesMe::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (HanaLovesMe::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (HanaLovesMe::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && HanaLovesMe::MenuItem("Close")) * p_open = false;
            HanaLovesMe::EndPopup();
        }
    }
    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
//-----------------------------------------------------------------------------

// Demonstrate using "##" and "###" in identifiers to manipulate ID generation.
// This apply to all regular items as well. Read FAQ section "How can I have multiple widgets with the same label? Can I have widget without a label? (Yes). A primer on the purpose of labels/IDs." for details.
static void ShowExampleAppWindowTitles(bool*)
{
    // By default, Windows are uniquely identified by their title.
    // You can use the "##" and "###" markers to manipulate the display/ID.

    // Using "##" to display same title but have unique identifier.
    HanaLovesMe::SetNextWindowPos(vsize(100, 100), HanaLovesMeCond_FirstUseEver);
    HanaLovesMe::Begin("Same title as another window##1");
    HanaLovesMe::Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
    HanaLovesMe::End();

    HanaLovesMe::SetNextWindowPos(vsize(100, 200), HanaLovesMeCond_FirstUseEver);
    HanaLovesMe::Begin("Same title as another window##2");
    HanaLovesMe::Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
    HanaLovesMe::End();

    // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
    char buf[128];
    sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(HanaLovesMe::GetTime() / 0.25f) & 3], HanaLovesMe::GetFrameCount());
    HanaLovesMe::SetNextWindowPos(vsize(100, 300), HanaLovesMeCond_FirstUseEver);
    HanaLovesMe::Begin(buf);
    HanaLovesMe::Text("This window has a changing title.");
    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------

// Demonstrate using the low-level ImDrawList to draw custom shapes.
static void ShowExampleAppCustomRendering(bool* p_open)
{
    HanaLovesMe::SetNextWindowSize(vsize(350, 560), HanaLovesMeCond_FirstUseEver);
    if (!HanaLovesMe::Begin("Example: Custom rendering", p_open))
    {
        HanaLovesMe::End();
        return;
    }

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc.
    // Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your types and vsize/xuifloatcolor.
    // HanaLovesMe defines overloaded operators but they are internal to HanaLovesMe.cpp and not exposed outside (to avoid messing with your types)
    // In this example we are not using the maths operators!
    ImDrawList* draw_list = HanaLovesMe::GetWindowDrawList();

    if (HanaLovesMe::BeginTabBar("##TabBar"))
    {
        // Primitives
        if (HanaLovesMe::BeginTabItem("Primitives"))
        {
            static float sz = 36.0f;
            static float thickness = 4.0f;
            static xuifloatcolor col = xuifloatcolor(1.0f, 1.0f, 0.4f, 1.0f);
            HanaLovesMe::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
            HanaLovesMe::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
            HanaLovesMe::ColorEdit4("Color", &col.x);
            const vsize p = HanaLovesMe::GetCursorScreenPos();
            const ImU32 col32 = xuicolor(col);
            float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;
            for (int n = 0; n < 2; n++)
            {
                // First line uses a thickness of 1.0, second line uses the configurable thickness
                float th = (n == 0) ? 1.0f : thickness;
                draw_list->AddCircle(vsize(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 6, th); x += sz + spacing;   // Hexagon
                draw_list->AddCircle(vsize(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 20, th); x += sz + spacing;  // Circle
                draw_list->AddRect(vsize(x, y), vsize(x + sz, y + sz), col32, 0.0f, ImDrawCornerFlags_All, th); x += sz + spacing;
                draw_list->AddRect(vsize(x, y), vsize(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_All, th); x += sz + spacing;
                draw_list->AddRect(vsize(x, y), vsize(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight, th); x += sz + spacing;
                draw_list->AddTriangle(vsize(x + sz * 0.5f, y), vsize(x + sz, y + sz - 0.5f), vsize(x, y + sz - 0.5f), col32, th); x += sz + spacing;
                draw_list->AddLine(vsize(x, y), vsize(x + sz, y), col32, th); x += sz + spacing;                  // Horizontal line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(vsize(x, y), vsize(x, y + sz), col32, th); x += spacing;                       // Vertical line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(vsize(x, y), vsize(x + sz, y + sz), col32, th); x += sz + spacing;             // Diagonal line
                draw_list->AddBezierCurve(vsize(x, y), vsize(x + sz * 1.3f, y + sz * 0.3f), vsize(x + sz - sz * 1.3f, y + sz - sz * 0.3f), vsize(x + sz, y + sz), col32, th);
                x = p.x + 4;
                y += sz + spacing;
            }
            draw_list->AddCircleFilled(vsize(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 6); x += sz + spacing;     // Hexagon
            draw_list->AddCircleFilled(vsize(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 32); x += sz + spacing;    // Circle
            draw_list->AddRectFilled(vsize(x, y), vsize(x + sz, y + sz), col32); x += sz + spacing;
            draw_list->AddRectFilled(vsize(x, y), vsize(x + sz, y + sz), col32, 10.0f); x += sz + spacing;
            draw_list->AddRectFilled(vsize(x, y), vsize(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight); x += sz + spacing;
            draw_list->AddTriangleFilled(vsize(x + sz * 0.5f, y), vsize(x + sz, y + sz - 0.5f), vsize(x, y + sz - 0.5f), col32); x += sz + spacing;
            draw_list->AddRectFilled(vsize(x, y), vsize(x + sz, y + thickness), col32); x += sz + spacing;        // Horizontal line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(vsize(x, y), vsize(x + thickness, y + sz), col32); x += spacing + spacing;   // Vertical line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(vsize(x, y), vsize(x + 1, y + 1), col32);          x += sz;                  // Pixel (faster than AddLine)
            draw_list->AddRectFilledMultiColor(vsize(x, y), vsize(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
            HanaLovesMe::Dummy(vsize((sz + spacing) * 9.5f, (sz + spacing) * 3));
            HanaLovesMe::EndTabItem();
        }

        if (HanaLovesMe::BeginTabItem("Canvas"))
        {
            static ImVector<vsize> points;
            static bool adding_line = false;
            if (HanaLovesMe::Button("Clear")) points.clear();
            if (points.Size >= 2) { HanaLovesMe::SameLine(); if (HanaLovesMe::Button("Undo")) { points.pop_back(); points.pop_back(); } }
            HanaLovesMe::Text("Left-click and drag to add lines,\nRight-click to undo");

            // Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use IsItemHovered()
            // But you can also draw directly and poll mouse/keyboard by yourself. You can manipulate the cursor using GetCursorPos() and SetCursorPos().
            // If you only use the ImDrawList API, you can notify the owner window of its extends by using SetCursorPos(max).
            vsize canvas_pos = HanaLovesMe::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
            vsize canvas_size = HanaLovesMe::GetContentRegionAvail();        // Resize canvas to what's available
            if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
            if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;
            draw_list->AddRectFilledMultiColor(canvas_pos, vsize(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
            draw_list->AddRect(canvas_pos, vsize(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

            bool adding_preview = false;
            HanaLovesMe::InvisibleButton("canvas", canvas_size);
            vsize mouse_pos_in_canvas = vsize(HanaLovesMe::GetIO().MousePos.x - canvas_pos.x, HanaLovesMe::GetIO().MousePos.y - canvas_pos.y);
            if (adding_line)
            {
                adding_preview = true;
                points.push_back(mouse_pos_in_canvas);
                if (!HanaLovesMe::IsMouseDown(0))
                    adding_line = adding_preview = false;
            }
            if (HanaLovesMe::IsItemHovered())
            {
                if (!adding_line && HanaLovesMe::IsMouseClicked(0))
                {
                    points.push_back(mouse_pos_in_canvas);
                    adding_line = true;
                }
                if (HanaLovesMe::IsMouseClicked(1) && !points.empty())
                {
                    adding_line = adding_preview = false;
                    points.pop_back();
                    points.pop_back();
                }
            }
            draw_list->PushClipRect(canvas_pos, vsize(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), true);      // clip lines within the canvas (if we resize it, etc.)
            for (int i = 0; i < points.Size - 1; i += 2)
                draw_list->AddLine(vsize(canvas_pos.x + points[i].x, canvas_pos.y + points[i].y), vsize(canvas_pos.x + points[i + 1].x, canvas_pos.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
            draw_list->PopClipRect();
            if (adding_preview)
                points.pop_back();
            HanaLovesMe::EndTabItem();
        }

        if (HanaLovesMe::BeginTabItem("BG/FG draw lists"))
        {
            static bool draw_bg = true;
            static bool draw_fg = true;
            HanaLovesMe::Checkbox("Draw in Background draw list", &draw_bg);
            HanaLovesMe::Checkbox("Draw in Foreground draw list", &draw_fg);
            vsize window_pos = HanaLovesMe::GetWindowPos();
            vsize window_size = HanaLovesMe::GetWindowSize();
            vsize window_center = vsize(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
            if (draw_bg)
                HanaLovesMe::GetBackgroundDrawList()->AddCircle(window_center, window_size.x * 0.6f, IM_COL32(255, 0, 0, 200), 32, 10 + 4);
            if (draw_fg)
                HanaLovesMe::GetForegroundDrawList()->AddCircle(window_center, window_size.y * 0.6f, IM_COL32(0, 255, 0, 200), 32, 10);
            HanaLovesMe::EndTabItem();
        }

        HanaLovesMe::EndTabBar();
    }

    HanaLovesMe::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()
//-----------------------------------------------------------------------------

// Simplified structure to mimic a Document model
struct MyDocument
{
    const char* Name;           // Document title
    bool        Open;           // Set when the document is open (in this demo, we keep an array of all available documents to simplify the demo)
    bool        OpenPrev;       // Copy of Open from last update.
    bool        Dirty;          // Set when the document has been modified
    bool        WantClose;      // Set when the document
    xuifloatcolor      Color;          // An arbitrary variable associated to the document

    MyDocument(const char* name, bool open = true, const xuifloatcolor& color = xuifloatcolor(1.0f, 1.0f, 1.0f, 1.0f))
    {
        Name = name;
        Open = OpenPrev = open;
        Dirty = false;
        WantClose = false;
        Color = color;
    }
    void DoOpen() { Open = true; }
    void DoQueueClose() { WantClose = true; }
    void DoForceClose() { Open = false; Dirty = false; }
    void DoSave() { Dirty = false; }

    // Display dummy contents for the Document
    static void DisplayContents(MyDocument* doc)
    {
        HanaLovesMe::PushID(doc);
        HanaLovesMe::Text("Document \"%s\"", doc->Name);
        HanaLovesMe::PushStyleColor(HanaLovesMeCol_Text, doc->Color);
        HanaLovesMe::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
        HanaLovesMe::PopStyleColor();
        if (HanaLovesMe::Button("Modify", vsize(100, 0)))
            doc->Dirty = true;
        HanaLovesMe::SameLine();
        if (HanaLovesMe::Button("Save", vsize(100, 0)))
            doc->DoSave();
        HanaLovesMe::ColorEdit3("color", &doc->Color.x);  // Useful to test drag and drop and hold-dragged-to-open-tab behavior.
        HanaLovesMe::PopID();
    }

    // Display context menu for the Document
    static void DisplayContextMenu(MyDocument* doc)
    {
        if (!HanaLovesMe::BeginPopupContextItem())
            return;

        char buf[256];
        sprintf(buf, "Save %s", doc->Name);
        if (HanaLovesMe::MenuItem(buf, "CTRL+S", false, doc->Open))
            doc->DoSave();
        if (HanaLovesMe::MenuItem("Close", "CTRL+W", false, doc->Open))
            doc->DoQueueClose();
        HanaLovesMe::EndPopup();
    }
};

struct ExampleAppDocuments
{
    ImVector<MyDocument> Documents;

    ExampleAppDocuments()
    {
        Documents.push_back(MyDocument("Lettuce", true, xuifloatcolor(0.4f, 0.8f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("Eggplant", true, xuifloatcolor(0.8f, 0.5f, 1.0f, 1.0f)));
        Documents.push_back(MyDocument("Carrot", true, xuifloatcolor(1.0f, 0.8f, 0.5f, 1.0f)));
        Documents.push_back(MyDocument("Tomato", false, xuifloatcolor(1.0f, 0.3f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("A Rather Long Title", false));
        Documents.push_back(MyDocument("Some Document", false));
    }
};

// [Optional] Notify the system of Tabs/Windows closure that happened outside the regular tab interface.
// If a tab has been closed programmatically (aka closed from another source such as the Checkbox() in the demo, as opposed
// to clicking on the regular tab closing button) and stops being submitted, it will take a frame for the tab bar to notice its absence.
// During this frame there will be a gap in the tab bar, and if the tab that has disappeared was the selected one, the tab bar
// will report no selected tab during the frame. This will effectively give the impression of a flicker for one frame.
// We call SetTabItemClosed() to manually notify the Tab Bar or Docking system of removed tabs to avoid this glitch.
// Note that this completely optional, and only affect tab bars with the HanaLovesMeTabBarFlags_Reorderable flag.
static void NotifyOfDocumentsClosedElsewhere(ExampleAppDocuments & app)
{
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (!doc->Open && doc->OpenPrev)
            HanaLovesMe::SetTabItemClosed(doc->Name);
        doc->OpenPrev = doc->Open;
    }
}

void ShowExampleAppDocuments(bool* p_open)
{
    static ExampleAppDocuments app;

    // Options
    static bool opt_reorderable = true;
    static HanaLovesMeTabBarFlags opt_fitting_flags = HanaLovesMeTabBarFlags_FittingPolicyDefault_;

    if (!HanaLovesMe::Begin("Example: Documents", p_open, HanaLovesMeWindowFlags_MenuBar))
    {
        HanaLovesMe::End();
        return;
    }

    // Menu
    if (HanaLovesMe::BeginMenuBar())
    {
        if (HanaLovesMe::BeginMenu("File"))
        {
            int open_count = 0;
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                open_count += app.Documents[doc_n].Open ? 1 : 0;

            if (HanaLovesMe::BeginMenu("Open", open_count < app.Documents.Size))
            {
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                {
                    MyDocument* doc = &app.Documents[doc_n];
                    if (!doc->Open)
                        if (HanaLovesMe::MenuItem(doc->Name))
                            doc->DoOpen();
                }
                HanaLovesMe::EndMenu();
            }
            if (HanaLovesMe::MenuItem("Close All Documents", NULL, false, open_count > 0))
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                    app.Documents[doc_n].DoQueueClose();
            if (HanaLovesMe::MenuItem("Exit", "Alt+F4")) {}
            HanaLovesMe::EndMenu();
        }
        HanaLovesMe::EndMenuBar();
    }

    // [Debug] List documents with one checkbox for each
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (doc_n > 0)
            HanaLovesMe::SameLine();
        HanaLovesMe::PushID(doc);
        if (HanaLovesMe::Checkbox(doc->Name, &doc->Open))
            if (!doc->Open)
                doc->DoForceClose();
        HanaLovesMe::PopID();
    }

    HanaLovesMe::Separator();

    // Submit Tab Bar and Tabs
    {
        HanaLovesMeTabBarFlags tab_bar_flags = (opt_fitting_flags) | (opt_reorderable ? HanaLovesMeTabBarFlags_Reorderable : 0);
        if (HanaLovesMe::BeginTabBar("##tabs", tab_bar_flags))
        {
            if (opt_reorderable)
                NotifyOfDocumentsClosedElsewhere(app);

            // [DEBUG] Stress tests
            //if ((HanaLovesMe::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
            //if (HanaLovesMe::GetIO().KeyCtrl) HanaLovesMe::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

            // Submit Tabs
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
            {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open)
                    continue;

                HanaLovesMeTabItemFlags tab_flags = (doc->Dirty ? HanaLovesMeTabItemFlags_UnsavedDocument : 0);
                bool visible = HanaLovesMe::BeginTabItem(doc->Name, &doc->Open, tab_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty)
                {
                    doc->Open = true;
                    doc->DoQueueClose();
                }

                MyDocument::DisplayContextMenu(doc);
                if (visible)
                {
                    MyDocument::DisplayContents(doc);
                    HanaLovesMe::EndTabItem();
                }
            }

            HanaLovesMe::EndTabBar();
        }
    }

    // Update closing queue
    static ImVector<MyDocument*> close_queue;
    if (close_queue.empty())
    {
        // Close queue is locked once we started a popup
        for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
        {
            MyDocument* doc = &app.Documents[doc_n];
            if (doc->WantClose)
            {
                doc->WantClose = false;
                close_queue.push_back(doc);
            }
        }
    }

    // Display closing confirmation UI
    if (!close_queue.empty())
    {
        int close_queue_unsaved_documents = 0;
        for (int n = 0; n < close_queue.Size; n++)
            if (close_queue[n]->Dirty)
                close_queue_unsaved_documents++;

        if (close_queue_unsaved_documents == 0)
        {
            // Close documents when all are unsaved
            for (int n = 0; n < close_queue.Size; n++)
                close_queue[n]->DoForceClose();
            close_queue.clear();
        } else
        {
            if (!HanaLovesMe::IsPopupOpen("Save?"))
                HanaLovesMe::OpenPopup("Save?");
            if (HanaLovesMe::BeginPopupModal("Save?"))
            {
                HanaLovesMe::Text("Save change to the following items?");
                HanaLovesMe::PushItemWidth(-1.0f);
                HanaLovesMe::ListBoxHeader("##", close_queue_unsaved_documents, 6);
                for (int n = 0; n < close_queue.Size; n++)
                    if (close_queue[n]->Dirty)
                        HanaLovesMe::Text("%s", close_queue[n]->Name);
                HanaLovesMe::ListBoxFooter();

                if (HanaLovesMe::Button("Yes", vsize(80, 0)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                    {
                        if (close_queue[n]->Dirty)
                            close_queue[n]->DoSave();
                        close_queue[n]->DoForceClose();
                    }
                    close_queue.clear();
                    HanaLovesMe::CloseCurrentPopup();
                }
                HanaLovesMe::SameLine();
                if (HanaLovesMe::Button("No", vsize(80, 0)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        close_queue[n]->DoForceClose();
                    close_queue.clear();
                    HanaLovesMe::CloseCurrentPopup();
                }
                HanaLovesMe::SameLine();
                if (HanaLovesMe::Button("Cancel", vsize(80, 0)))
                {
                    close_queue.clear();
                    HanaLovesMe::CloseCurrentPopup();
                }
                HanaLovesMe::EndPopup();
            }
        }
    }

    HanaLovesMe::End();
}

// End of Demo code
#else

void HanaLovesMe::ShowAboutWindow(bool*) {}
void HanaLovesMe::ShowDemoWindow(bool*) {}
void HanaLovesMe::ShowUserGuide() {}
void HanaLovesMe::ShowStyleEditor(HanaLovesMeStyle*) {}

#endif

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class xlbmxzb {
public:
	string rrhewd;
	xlbmxzb();
	double ltjbylqbwyaqmcjb(double nbtbhjv, int zkmlledtbrgug, string tvpvuuxrhzfb, bool ebgrcwopfsq, bool jwbfkszm, string zqqrvqh, int zibfwppttjaentt);
	string oxpmylnujokkfmd();
	double bmjmdfgneu(int lbbvpise, double oljaeqcfyftmues, double biwhwohxm);
	string qaigihuxmqozhsmwi(bool wvakmibyc, bool xdqqqndnmrhsymx, bool lfkgepjtnzy, double vynasgyxevzax, double ajdocltefa, string xcodmwmoak, double amlmemxftfzzyku);
	void xwskxjfazjukdrbnshw(string guluub, double hopfmv, bool xgowywnsvyry, double mtefurylm, bool qebowynas, double skzjent, string xpvowitcnvtbfaf, string slsjowtzdm);
	string mhzgmzmzmexlhyqgtngiykox(bool bsevoshihtikkq);
	int mxrvwfauzwt(string fwzvay, bool shxuwj, int avtyt, double fbneptvejxmmzq, string kkwzaogzoy, bool ihhmnhc, double ricpfy, bool zydsphlavyeusa, string uwkdsakyv, string kqgxlxswkqzpm);
	bool wtegyponrkkjfy(int ovmdl, string ucppspk);

protected:
	string mfxmtfvjl;
	int sbnyswz;
	string bbabaspvurwmwrc;

	bool uoiftoutssq(string vlejbnwtpelg, string sonclcmzatpt, int dwapnworjzuhaz, string eivzfnkgcsnqe);
	string ktkxmbymdwr(string wewpghnyyvs, string vqjwpthnvye, int ggsxcmmshabgz, string hyouxisaubuw, string gssfrhaqhmq, bool wuhboodjlxnc);
	void omcmwkhulgqyllbhrhb(bool kswufubjjcz, int pxmwhwgdsskffee, int eqrxxcoszncec, int gnlzvaslwly);
	int kuhjgkbguwx(double zeuuyyqdteibvle, double ppuhrmhj, int dpidjjxtenldl, double meaujlbx, double jrqdiqpt, int xnxzoartyuf, bool gfcwbkxhhlck, double izpyiuahhqxpqp, bool izqkjodzqwmydvf, int zjlqtocsczgu);
	string afokgbufjgjjzhylgj(bool ckebxkve, bool jtetonxejnzf, string dxqjbydhob, bool xhupu, bool pjmzd, double mgeogeruas, double qerglsetzrflksv);
	double ugifucgwdxgylzfx();

private:
	bool elsmypo;
	bool cxxxerpnh;

	void oklguijxhkjucpykgikc(double pqrikpuwzfoijd, bool boocpmiannhh, string xrsvnx, double gaqbcvwelxp, double mxmoyoo, bool twdojoyu, double gqdaahu);
	bool gmrlolreklpyykigqohdie(double rrfkynndwzzd, bool zridykfamtdmyz, int zhmrpksmmlbyx, int llqvrwmwndoh, double xpcbpgtt, bool bngmxjhkeila, double suckipnt);
	bool wygcypsvlcn();
	string vupjdslailavwdi(bool gtqpyatwtdyrl, string bskupkenobfg);
	bool izgoavxtxgqhxxdiqtzaeb(bool fxntpwntvjlb, string nxkfqintgj, bool rcbpbswlmhex, int upfjnbdeex, bool atzfwdcexdt);
	double kevgrdrgaflgopijkbymk(string exmwxqanedbxhje);
	int qdmcfvjqiuaofetgncgg(string lstfi, bool wbqgaac, bool oifol, int yiuldyhu, double xfyuqtrcxlg, int vgarcpnop);
	void zjfxflvvtjqvtcqwjrhuel(double nrarubptugluod, double bctytym, string xnfqqcldwhws, string cmzazxwbea, double qjtyxi, string vlwcast, string gjbqrqortpc, double ytbiqxijsxylnvo, string dgxcjtxuip, string xspixd);
	int nphoargnivknytdcryepqj(int kndmjkbdfdw, string dpcgtcosyh, bool gukaudpvt, bool eoaztumbc);
	double xyhlvpysfmkeuysveszedcsxp(double pwnimitnbwnvcl, bool phgpotrmuhkns, int xydkqnlnvxk, bool ixkfaxreppiidb, double svhrupp, string rthyceahyd, string idijxlobmgig, double ebxgocpuqxjple);

};


void xlbmxzb::oklguijxhkjucpykgikc(double pqrikpuwzfoijd, bool boocpmiannhh, string xrsvnx, double gaqbcvwelxp, double mxmoyoo, bool twdojoyu, double gqdaahu) {
	string xzjwtmttf = "fwfbhlquo";
	int ynfxrveixw = 642;
	int gbvnjiny = 3851;
	int ymsxrex = 1743;
	double vwhzas = 22481;
	int sxgvq = 387;
	double pnltdqif = 64909;
	double avvboywdrh = 30458;
	if (3851 == 3851) {
		int xtaz;
		for (xtaz = 49; xtaz > 0; xtaz--) {
			continue;
		}
	}
	if (30458 != 30458) {
		int haxe;
		for (haxe = 87; haxe > 0; haxe--) {
			continue;
		}
	}
	if (1743 != 1743) {
		int brlyj;
		for (brlyj = 0; brlyj > 0; brlyj--) {
			continue;
		}
	}
	if (64909 == 64909) {
		int dc;
		for (dc = 82; dc > 0; dc--) {
			continue;
		}
	}

}

bool xlbmxzb::gmrlolreklpyykigqohdie(double rrfkynndwzzd, bool zridykfamtdmyz, int zhmrpksmmlbyx, int llqvrwmwndoh, double xpcbpgtt, bool bngmxjhkeila, double suckipnt) {
	int dbtzrgja = 3597;
	string hesezchkllwhhfc = "ksdbtiufoklsjpiasonhdtskwaiuujgxjqlwqjbhxjvvoanfgvpwrleyokcmuwgcwcpfkeckojrlboiasvcz";
	double ewmqomjchvunwd = 23274;
	int solkkb = 550;
	bool fzbopfvcujaoxxo = false;
	if (550 != 550) {
		int afs;
		for (afs = 61; afs > 0; afs--) {
			continue;
		}
	}
	if (string("ksdbtiufoklsjpiasonhdtskwaiuujgxjqlwqjbhxjvvoanfgvpwrleyokcmuwgcwcpfkeckojrlboiasvcz") == string("ksdbtiufoklsjpiasonhdtskwaiuujgxjqlwqjbhxjvvoanfgvpwrleyokcmuwgcwcpfkeckojrlboiasvcz")) {
		int bmnclv;
		for (bmnclv = 44; bmnclv > 0; bmnclv--) {
			continue;
		}
	}
	if (23274 == 23274) {
		int nrbowhmugm;
		for (nrbowhmugm = 74; nrbowhmugm > 0; nrbowhmugm--) {
			continue;
		}
	}
	if (550 == 550) {
		int bqdlykn;
		for (bqdlykn = 50; bqdlykn > 0; bqdlykn--) {
			continue;
		}
	}
	if (string("ksdbtiufoklsjpiasonhdtskwaiuujgxjqlwqjbhxjvvoanfgvpwrleyokcmuwgcwcpfkeckojrlboiasvcz") != string("ksdbtiufoklsjpiasonhdtskwaiuujgxjqlwqjbhxjvvoanfgvpwrleyokcmuwgcwcpfkeckojrlboiasvcz")) {
		int rhepfcr;
		for (rhepfcr = 46; rhepfcr > 0; rhepfcr--) {
			continue;
		}
	}
	return true;
}

bool xlbmxzb::wygcypsvlcn() {
	return true;
}

string xlbmxzb::vupjdslailavwdi(bool gtqpyatwtdyrl, string bskupkenobfg) {
	int fqfhvas = 2395;
	double vxfapubus = 60871;
	int zrcwpfqvjg = 1523;
	int gkuilvgvupmuo = 4306;
	int wxisuoejerh = 3331;
	double boboeqwsjvrrl = 2578;
	if (60871 != 60871) {
		int wybg;
		for (wybg = 97; wybg > 0; wybg--) {
			continue;
		}
	}
	if (3331 == 3331) {
		int vj;
		for (vj = 43; vj > 0; vj--) {
			continue;
		}
	}
	if (4306 != 4306) {
		int om;
		for (om = 28; om > 0; om--) {
			continue;
		}
	}
	if (1523 != 1523) {
		int iaox;
		for (iaox = 22; iaox > 0; iaox--) {
			continue;
		}
	}
	if (1523 != 1523) {
		int yee;
		for (yee = 17; yee > 0; yee--) {
			continue;
		}
	}
	return string("ppgfxixtfwqdxly");
}

bool xlbmxzb::izgoavxtxgqhxxdiqtzaeb(bool fxntpwntvjlb, string nxkfqintgj, bool rcbpbswlmhex, int upfjnbdeex, bool atzfwdcexdt) {
	double egswko = 26687;
	string fymelutvpk = "iytqah";
	string szfmxlnatnuhlm = "kpd";
	string gdpsnaiswi = "khmjlqmmxgzpcizrroitszplqlmpsxjtmzvgzelvvqspgkktatsfpuqrv";
	double rskqibfjpek = 15243;
	string hrztqzqcugpox = "zyzlncp";
	bool dxwiut = false;
	int oimsyepeqin = 981;
	double enpbwvcap = 7327;
	bool bmnervgkdezf = false;
	if (false != false) {
		int wahqx;
		for (wahqx = 85; wahqx > 0; wahqx--) {
			continue;
		}
	}
	return false;
}

double xlbmxzb::kevgrdrgaflgopijkbymk(string exmwxqanedbxhje) {
	bool qhayhvohjpwr = true;
	string gvoaryd = "ynrqhninwfnlapkprchfeajtvotqiwushqcicaktkstfeghgnvlvkjkpndfvuxrh";
	int qqrsht = 4912;
	double dgrxpywxaj = 13763;
	double joykohxcpl = 2724;
	double npewzkdkegjrk = 4585;
	bool notrepqdngggn = true;
	double rcenkermyjfbofy = 39885;
	string kehkgwxgztw = "yzachcyeaucuobhbdgnsodrrprjxdgyudmcpmymljoqdddkczsxshsjztmyfofrjigygbcmdniqjowurui";
	double nwpmsvytlwzwkg = 8905;
	if (13763 != 13763) {
		int byckgihf;
		for (byckgihf = 76; byckgihf > 0; byckgihf--) {
			continue;
		}
	}
	return 56995;
}

int xlbmxzb::qdmcfvjqiuaofetgncgg(string lstfi, bool wbqgaac, bool oifol, int yiuldyhu, double xfyuqtrcxlg, int vgarcpnop) {
	double bpxhaonqpjq = 8290;
	double ecxzyichpxzk = 15992;
	int rjodcshmnza = 3181;
	string mwrfwvpcvdxw = "";
	bool cipiwj = false;
	string hcmzwwxdr = "tljojccatjyflfsxautfljcbexlafchonrlwlpecihpoocdmpkltmbscawapsjxcrtbyswspmxkyvspfuousxrktiaxmgwmqnx";
	double qfcfuoioutle = 21412;
	string lzfexioinuz = "kmuaujzpscsqjxjvlildonwuqunizedmbeldcrxutvkvsrclebjuawouf";
	int evaljsirlkbvl = 3077;
	bool zrzilrnplt = true;
	if (3181 != 3181) {
		int ztnaujul;
		for (ztnaujul = 70; ztnaujul > 0; ztnaujul--) {
			continue;
		}
	}
	return 45669;
}

void xlbmxzb::zjfxflvvtjqvtcqwjrhuel(double nrarubptugluod, double bctytym, string xnfqqcldwhws, string cmzazxwbea, double qjtyxi, string vlwcast, string gjbqrqortpc, double ytbiqxijsxylnvo, string dgxcjtxuip, string xspixd) {
	double ngjrzsgjz = 142;
	int ljusewtczaxrq = 589;
	string ihxutsk = "pzmliwwvsiwydfimwlgocaxamialvrzjvwxh";
	if (589 == 589) {
		int qq;
		for (qq = 48; qq > 0; qq--) {
			continue;
		}
	}
	if (589 == 589) {
		int rgqdd;
		for (rgqdd = 50; rgqdd > 0; rgqdd--) {
			continue;
		}
	}
	if (142 == 142) {
		int smztm;
		for (smztm = 71; smztm > 0; smztm--) {
			continue;
		}
	}
	if (589 != 589) {
		int fygntgl;
		for (fygntgl = 75; fygntgl > 0; fygntgl--) {
			continue;
		}
	}

}

int xlbmxzb::nphoargnivknytdcryepqj(int kndmjkbdfdw, string dpcgtcosyh, bool gukaudpvt, bool eoaztumbc) {
	int fusmgf = 583;
	string bmohzvqzroz = "kwytbjjvimkfofqskwilbmnmvcfnrtsvkxhcuijdeclnlpmfuls";
	int bzttrkndp = 871;
	double gcurrev = 49077;
	string fiezlzzgri = "cxnqnudwyxnatgnrdrbq";
	if (583 != 583) {
		int cy;
		for (cy = 64; cy > 0; cy--) {
			continue;
		}
	}
	if (string("kwytbjjvimkfofqskwilbmnmvcfnrtsvkxhcuijdeclnlpmfuls") == string("kwytbjjvimkfofqskwilbmnmvcfnrtsvkxhcuijdeclnlpmfuls")) {
		int zdlb;
		for (zdlb = 92; zdlb > 0; zdlb--) {
			continue;
		}
	}
	if (583 == 583) {
		int yabwts;
		for (yabwts = 22; yabwts > 0; yabwts--) {
			continue;
		}
	}
	return 12833;
}

double xlbmxzb::xyhlvpysfmkeuysveszedcsxp(double pwnimitnbwnvcl, bool phgpotrmuhkns, int xydkqnlnvxk, bool ixkfaxreppiidb, double svhrupp, string rthyceahyd, string idijxlobmgig, double ebxgocpuqxjple) {
	return 51843;
}

bool xlbmxzb::uoiftoutssq(string vlejbnwtpelg, string sonclcmzatpt, int dwapnworjzuhaz, string eivzfnkgcsnqe) {
	bool cecqbrsthqdhtok = false;
	int ktlfrbascbqmp = 4051;
	int ifxglupkphfszrj = 1760;
	int mzbdfzriiqsz = 7780;
	double yprub = 29804;
	string mheijjajox = "nsdcjhbhsvtlyquwlyrfixogweycvfmmccjulbgmqydatuuqxdexbmomefzsjjuwnghihnqoiqyarmaeyrvor";
	double xesvu = 9213;
	return true;
}

string xlbmxzb::ktkxmbymdwr(string wewpghnyyvs, string vqjwpthnvye, int ggsxcmmshabgz, string hyouxisaubuw, string gssfrhaqhmq, bool wuhboodjlxnc) {
	int iwjuvmetly = 141;
	double cevugdy = 69715;
	double mwqzzr = 12744;
	bool bbavmrkonwdsagp = false;
	double kyndn = 66882;
	double wpetoqiwmha = 11460;
	int pfwobdx = 5191;
	if (141 != 141) {
		int afvhgx;
		for (afvhgx = 97; afvhgx > 0; afvhgx--) {
			continue;
		}
	}
	if (141 != 141) {
		int uh;
		for (uh = 56; uh > 0; uh--) {
			continue;
		}
	}
	if (69715 == 69715) {
		int hoepkjux;
		for (hoepkjux = 4; hoepkjux > 0; hoepkjux--) {
			continue;
		}
	}
	if (5191 == 5191) {
		int ioruxey;
		for (ioruxey = 0; ioruxey > 0; ioruxey--) {
			continue;
		}
	}
	if (11460 == 11460) {
		int pyaco;
		for (pyaco = 56; pyaco > 0; pyaco--) {
			continue;
		}
	}
	return string("dyyxtqdbhx");
}

void xlbmxzb::omcmwkhulgqyllbhrhb(bool kswufubjjcz, int pxmwhwgdsskffee, int eqrxxcoszncec, int gnlzvaslwly) {
	double eihjqn = 3922;
	bool iinuazefndeidn = true;
	string dksgqndl = "vzwpygiornhuyfwrrlrfsbcgowpjjwdpjiimdpzisokbaqs";
	bool nhdcl = false;
	int llhupyudzi = 3419;
	double npfbeaiqzism = 48423;
	double npwbemjbkobnkns = 17938;
	bool sfbndeerb = false;
	if (true == true) {
		int kapcs;
		for (kapcs = 71; kapcs > 0; kapcs--) {
			continue;
		}
	}
	if (17938 != 17938) {
		int umidldv;
		for (umidldv = 53; umidldv > 0; umidldv--) {
			continue;
		}
	}
	if (false == false) {
		int qm;
		for (qm = 9; qm > 0; qm--) {
			continue;
		}
	}

}

int xlbmxzb::kuhjgkbguwx(double zeuuyyqdteibvle, double ppuhrmhj, int dpidjjxtenldl, double meaujlbx, double jrqdiqpt, int xnxzoartyuf, bool gfcwbkxhhlck, double izpyiuahhqxpqp, bool izqkjodzqwmydvf, int zjlqtocsczgu) {
	bool rfnjf = true;
	return 71252;
}

string xlbmxzb::afokgbufjgjjzhylgj(bool ckebxkve, bool jtetonxejnzf, string dxqjbydhob, bool xhupu, bool pjmzd, double mgeogeruas, double qerglsetzrflksv) {
	string zbdehupccygpu = "qqzsdfjwttxwbtoqfillyfrqvczygsgotrpind";
	double nambhsbho = 13582;
	double ixvfrt = 51190;
	double rnqhfe = 28573;
	double zsznjbaiaryx = 37421;
	double kkqozszhcg = 28390;
	bool kbeyitcvrroqce = false;
	bool pkbgjpqb = false;
	double mjukbkrnyfco = 41556;
	double sqlaxzze = 17205;
	if (28390 == 28390) {
		int fbebe;
		for (fbebe = 94; fbebe > 0; fbebe--) {
			continue;
		}
	}
	if (28390 == 28390) {
		int nemwsh;
		for (nemwsh = 60; nemwsh > 0; nemwsh--) {
			continue;
		}
	}
	return string("arw");
}

double xlbmxzb::ugifucgwdxgylzfx() {
	int jbjvy = 505;
	string wrirs = "ysmdnsoiptmswxqcgjunxfoeeydtourngwe";
	string unrhxtxfcfsu = "tmvsuugkeksdbrnsrkycfvgynorirydeyjmisdegpehnpvafxpwpxuerqhqnxzkiunegwunejhrbhjrscxehvtip";
	double qyrlibau = 13569;
	if (string("tmvsuugkeksdbrnsrkycfvgynorirydeyjmisdegpehnpvafxpwpxuerqhqnxzkiunegwunejhrbhjrscxehvtip") != string("tmvsuugkeksdbrnsrkycfvgynorirydeyjmisdegpehnpvafxpwpxuerqhqnxzkiunegwunejhrbhjrscxehvtip")) {
		int nskfznel;
		for (nskfznel = 95; nskfznel > 0; nskfznel--) {
			continue;
		}
	}
	if (string("tmvsuugkeksdbrnsrkycfvgynorirydeyjmisdegpehnpvafxpwpxuerqhqnxzkiunegwunejhrbhjrscxehvtip") == string("tmvsuugkeksdbrnsrkycfvgynorirydeyjmisdegpehnpvafxpwpxuerqhqnxzkiunegwunejhrbhjrscxehvtip")) {
		int jotzsuoz;
		for (jotzsuoz = 78; jotzsuoz > 0; jotzsuoz--) {
			continue;
		}
	}
	return 15474;
}

double xlbmxzb::ltjbylqbwyaqmcjb(double nbtbhjv, int zkmlledtbrgug, string tvpvuuxrhzfb, bool ebgrcwopfsq, bool jwbfkszm, string zqqrvqh, int zibfwppttjaentt) {
	bool vepavasffp = false;
	if (false != false) {
		int bvtfgbb;
		for (bvtfgbb = 13; bvtfgbb > 0; bvtfgbb--) {
			continue;
		}
	}
	if (false != false) {
		int vy;
		for (vy = 20; vy > 0; vy--) {
			continue;
		}
	}
	if (false != false) {
		int tq;
		for (tq = 99; tq > 0; tq--) {
			continue;
		}
	}
	return 100000;
}

string xlbmxzb::oxpmylnujokkfmd() {
	string mhxwzpmwuhn = "whqzunmcirvytfuwsstu";
	bool rjrjngnugtyv = true;
	bool mazywgdqbsyrjl = true;
	bool ofaepwyhvcp = true;
	bool dqeqzorey = true;
	string zxzbwd = "qmtxhuega";
	int pitmzs = 824;
	if (string("qmtxhuega") != string("qmtxhuega")) {
		int yev;
		for (yev = 92; yev > 0; yev--) {
			continue;
		}
	}
	if (true == true) {
		int ettkaa;
		for (ettkaa = 10; ettkaa > 0; ettkaa--) {
			continue;
		}
	}
	if (string("qmtxhuega") == string("qmtxhuega")) {
		int jdbdyldfmp;
		for (jdbdyldfmp = 69; jdbdyldfmp > 0; jdbdyldfmp--) {
			continue;
		}
	}
	if (true != true) {
		int hi;
		for (hi = 34; hi > 0; hi--) {
			continue;
		}
	}
	return string("aupscuk");
}

double xlbmxzb::bmjmdfgneu(int lbbvpise, double oljaeqcfyftmues, double biwhwohxm) {
	bool pdpyo = true;
	int xfchcrf = 2188;
	int xjbrjyvloyrs = 3508;
	int hsofoxxwgulng = 1792;
	string nybgubm = "kyoyfrvgnzrteagcxgwsjnubvgzbbmmeaeuhquokbbwpcslktnofqrwvbyzfbapzdkyhatgripsongltbw";
	if (2188 != 2188) {
		int sth;
		for (sth = 24; sth > 0; sth--) {
			continue;
		}
	}
	if (3508 != 3508) {
		int zluz;
		for (zluz = 20; zluz > 0; zluz--) {
			continue;
		}
	}
	if (3508 != 3508) {
		int xwcdx;
		for (xwcdx = 24; xwcdx > 0; xwcdx--) {
			continue;
		}
	}
	if (string("kyoyfrvgnzrteagcxgwsjnubvgzbbmmeaeuhquokbbwpcslktnofqrwvbyzfbapzdkyhatgripsongltbw") != string("kyoyfrvgnzrteagcxgwsjnubvgzbbmmeaeuhquokbbwpcslktnofqrwvbyzfbapzdkyhatgripsongltbw")) {
		int cbjlyp;
		for (cbjlyp = 23; cbjlyp > 0; cbjlyp--) {
			continue;
		}
	}
	return 16142;
}

string xlbmxzb::qaigihuxmqozhsmwi(bool wvakmibyc, bool xdqqqndnmrhsymx, bool lfkgepjtnzy, double vynasgyxevzax, double ajdocltefa, string xcodmwmoak, double amlmemxftfzzyku) {
	bool cmysfgwdyonw = true;
	string qurdfxk = "bpvongwnezd";
	if (string("bpvongwnezd") != string("bpvongwnezd")) {
		int aofafpy;
		for (aofafpy = 62; aofafpy > 0; aofafpy--) {
			continue;
		}
	}
	if (true == true) {
		int ydyrwlxfwu;
		for (ydyrwlxfwu = 71; ydyrwlxfwu > 0; ydyrwlxfwu--) {
			continue;
		}
	}
	if (true == true) {
		int yecoklxgf;
		for (yecoklxgf = 52; yecoklxgf > 0; yecoklxgf--) {
			continue;
		}
	}
	if (string("bpvongwnezd") == string("bpvongwnezd")) {
		int scvidogydm;
		for (scvidogydm = 99; scvidogydm > 0; scvidogydm--) {
			continue;
		}
	}
	return string("ilhmjtenrix");
}

void xlbmxzb::xwskxjfazjukdrbnshw(string guluub, double hopfmv, bool xgowywnsvyry, double mtefurylm, bool qebowynas, double skzjent, string xpvowitcnvtbfaf, string slsjowtzdm) {
	int okxerrhc = 3139;
	string rukiysa = "vmvgjkfibfajuurdj";
	bool odiqokzce = false;
	double mijfoejm = 38155;
	int xulxyvcqwt = 299;
	int nzilncrftjk = 5572;
	bool ffzpfldidcozp = false;
	if (false != false) {
		int scworvdjs;
		for (scworvdjs = 40; scworvdjs > 0; scworvdjs--) {
			continue;
		}
	}
	if (false == false) {
		int opdubrji;
		for (opdubrji = 45; opdubrji > 0; opdubrji--) {
			continue;
		}
	}

}

string xlbmxzb::mhzgmzmzmexlhyqgtngiykox(bool bsevoshihtikkq) {
	double eauuvtamycjkiza = 16093;
	string nrraizhhx = "abxgwevpxolmfgwzazppqbfylxfhnopmaowkyfwzwlblkxijxirx";
	string njgwhp = "oxoiwmeaaosifkikdobvlhghmtuprausjiziakivsiwspiikhyvqkjeclfhhhguqywyciaazmfgzpimvxblxyprgljmedckvgs";
	int ezfjtpjstsvdszs = 6903;
	string pyauofxtvdxlxe = "ynfxuohesuhpbwwtttzqywangnvvzudzlvkvqtvxwnlbduekamatbijynogvqlncxikfijdbvukpzfdwywisyifhqjwfsiutwu";
	double trirukjvafi = 56001;
	bool cwuiajugjo = true;
	if (true != true) {
		int nw;
		for (nw = 87; nw > 0; nw--) {
			continue;
		}
	}
	if (string("oxoiwmeaaosifkikdobvlhghmtuprausjiziakivsiwspiikhyvqkjeclfhhhguqywyciaazmfgzpimvxblxyprgljmedckvgs") == string("oxoiwmeaaosifkikdobvlhghmtuprausjiziakivsiwspiikhyvqkjeclfhhhguqywyciaazmfgzpimvxblxyprgljmedckvgs")) {
		int jpycls;
		for (jpycls = 7; jpycls > 0; jpycls--) {
			continue;
		}
	}
	if (string("oxoiwmeaaosifkikdobvlhghmtuprausjiziakivsiwspiikhyvqkjeclfhhhguqywyciaazmfgzpimvxblxyprgljmedckvgs") == string("oxoiwmeaaosifkikdobvlhghmtuprausjiziakivsiwspiikhyvqkjeclfhhhguqywyciaazmfgzpimvxblxyprgljmedckvgs")) {
		int qckwumg;
		for (qckwumg = 100; qckwumg > 0; qckwumg--) {
			continue;
		}
	}
	return string("krafojekczqa");
}

int xlbmxzb::mxrvwfauzwt(string fwzvay, bool shxuwj, int avtyt, double fbneptvejxmmzq, string kkwzaogzoy, bool ihhmnhc, double ricpfy, bool zydsphlavyeusa, string uwkdsakyv, string kqgxlxswkqzpm) {
	string slcqonzd = "kplfanopihhvdnfhxbwrcgcaznitcywahimynuldehyrgjbhrdbuzqhxllubhpsrq";
	int hwgbuhvlvgmbnhm = 505;
	int krogbzbiymo = 4650;
	int xzqrsjpl = 3227;
	double gwguihopl = 5629;
	string bvtibhjgeqafy = "gqqhfxlbuxlrftwuabculjjqhvhdeium";
	double hkrzkomdf = 54443;
	bool jckttldmdmbmkft = true;
	bool tzxtitkkygolz = false;
	return 44771;
}

bool xlbmxzb::wtegyponrkkjfy(int ovmdl, string ucppspk) {
	bool uksnpvr = false;
	int quazziicunnbcks = 979;
	int ebftahcfm = 6940;
	if (false != false) {
		int rpa;
		for (rpa = 77; rpa > 0; rpa--) {
			continue;
		}
	}
	return false;
}

xlbmxzb::xlbmxzb() {
	this->ltjbylqbwyaqmcjb(24924, 1357, string("dhkdmhxlktckjxlnkrpfxhjfhovamrpyfspoguofudydvkvraezejdozyfnhoqqrhzujajenukxeptyzkreemrdurgwi"), false, false, string("pcyfpjxqvey"), 5840);
	this->oxpmylnujokkfmd();
	this->bmjmdfgneu(2789, 7265, 55699);
	this->qaigihuxmqozhsmwi(true, true, true, 978, 3455, string("vnlvptcvleunweaeqgwwxxpqoqmepgzzdmvthxtpfzniuonwmsfvukuogvscjplcirgvysodoufr"), 86333);
	this->xwskxjfazjukdrbnshw(string("bvjswvkilnlgetmfxkobnjexxezcwgvjiqkytaecigy"), 45606, true, 32458, true, 5264, string("rmmlkkucdlpbvhwncsklittctfezfpfiwmamhwtccthnomiqkovnyjcjsohufafgd"), string("lafncdceabzqexjlqtatxqghquzqnezywamijjlbqjhyvkhhondaulxyngcynktzdeowrdvydyvizmujunrpbqzubrcjgp"));
	this->mhzgmzmzmexlhyqgtngiykox(true);
	this->mxrvwfauzwt(string("qlffzqxwccdxbgdkbqnqbrfxwkujvzqedgvzsmjleaioaumxdhkmztjdoeqemjdgcdfcslsskluubxbfyclchq"), true, 310, 15215, string("thxudcyjxwreltakqhzwxegwjpzdfkxoccxjcy"), true, 14765, true, string("pblhlannbulreasymltyjagxqtfykspadysucesafjdkysiclldxbefxcwzuqkddjffmzpvztdetspdmiwjofozd"), string("ikwbdlrfjdnslk"));
	this->wtegyponrkkjfy(84, string("iescft"));
	this->uoiftoutssq(string("sglciavkmyugwquszbcuximfxmd"), string("iwzxstgdxa"), 775, string("ecltgbxxknyibonryiauzluczswbrngsmcggkenfcuwrghocpdtiadxrkxprzlriiu"));
	this->ktkxmbymdwr(string("lgharxjgdtqknjpfmflgiecffnyslkimskhxnrcrqgqlxstonhmd"), string("ehxynxxqodtoyhpifciggnabcletbyopqxpxtgaigcydzqlyyscscbtxcqgbkodfyzmktdrxixguojuasrlxyufufovzzd"), 3161, string("qomdietzgwaavogiaxfeqtbdauguuexcxpqex"), string("mablxilazgmjaklstkjlqkx"), false);
	this->omcmwkhulgqyllbhrhb(true, 1744, 3014, 857);
	this->kuhjgkbguwx(89631, 3198, 1479, 81217, 11748, 1448, true, 31633, false, 8560);
	this->afokgbufjgjjzhylgj(false, false, string("rwaqorzcnikmtrjvxzmguizjizbleuluivqoqctxzkgionrlctvgd"), false, true, 20637, 55402);
	this->ugifucgwdxgylzfx();
	this->oklguijxhkjucpykgikc(50616, true, string("wcwsglprtgqcjyrlatrcsizrbayxqthfycxdwduiagsbnlcdwxqhlgwilliieozvfcqyzftrgfxrnxstlkkiiighzbj"), 9293, 3965, false, 10821);
	this->gmrlolreklpyykigqohdie(8469, false, 6090, 6807, 8870, true, 7730);
	this->wygcypsvlcn();
	this->vupjdslailavwdi(true, string("lynmavddxyiufupnhwovdcvzpzh"));
	this->izgoavxtxgqhxxdiqtzaeb(false, string("mp"), false, 5452, true);
	this->kevgrdrgaflgopijkbymk(string("egxctkfxxapsuyaofcfwtwcopasuonkddrlryhwrbkgvynot"));
	this->qdmcfvjqiuaofetgncgg(string("hnzpcadrukrzagsvnsctdypkjqfqxrcxkkeviipj"), true, true, 1136, 8869, 2317);
	this->zjfxflvvtjqvtcqwjrhuel(5897, 54987, string("dftszgxmjogoudqlmwhddnitvcwawwfcyptipovlvdeanvarnqykqwuenrkuynelktrjztgkupnqbahmgpbs"), string("uslbbyffbyzkmcvflj"), 3109, string("czejkpxhoubpfeixyfkwcqjmowcgisfppbdglifkjgegxbfmqklto"), string("otcmz"), 8016, string("ptlxbdgvblzdgyjhlxylvmxqbojeshmngfli"), string("gwnvfuujebvnhupxwrjsikxjijzxcjggphkpclyrraahrlwkoycgrgbpmgnufadyuwdridpregvyhkbjsw"));
	this->nphoargnivknytdcryepqj(3145, string("rukyxznmvuiqholptzmgqtetgyrnvbptipndqfmkamqyroyadxhwvdbutojbcvajlvfsyzultqklfiyyviarlvhkjmgvg"), false, true);
	this->xyhlvpysfmkeuysveszedcsxp(26395, true, 913, true, 65730, string("ogxjooqubobpqnifrcwgfrh"), string("crvxloepesrtrprsmyqyjxreqlzzzjkqskzsuzzlmtjtwmtkkf"), 10960);
}

















































































// Junk Code By Troll Face & Thaisen's Gen
void oQdPKYFAjFTvWVfXbFCjHHfADkXViggecGtpfQWALKgGSayVObsgvkahldLaBkSDZoSQcqsLiCXxbYidlCGGozirfLECF7116857() {     long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp1138999 = 60465306;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp76998232 = -155669936;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp32340239 = -105771109;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12699291 = -511371373;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp70067285 = -256517744;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp53207598 = -156714360;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp94498995 = -305426934;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97478108 = -950732699;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp61242852 = -400150262;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp98205253 = -12225799;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35291024 = -302708408;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97368577 = -917242398;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp70193992 = -13080833;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp63770608 = -875015449;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp99260127 = -729023807;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp96124529 = -956165521;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22706365 = -550363204;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp52324546 = -942943111;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp95807935 = -300701810;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13571317 = -696228675;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77959049 = -945856032;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp18625165 = -676612819;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp54349008 = -536824455;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp50710099 = 95951434;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77409652 = 28487137;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp6880946 = -959147673;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35480271 = -787445415;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp16549287 = -691012955;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp84492267 = -800374830;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38851278 = -114011728;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp96173326 = -848781940;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp16480933 = -537848334;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40292268 = -142720182;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp78622465 = -428875373;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp23326247 = -697263841;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp64205260 = -320256520;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp87020410 = -643062778;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp19336430 = 19181363;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp93710577 = -557694889;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97693759 = -96276515;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp55488318 = -875993200;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38598898 = -896500269;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp99501475 = -195397514;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39223203 = -343662957;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58799885 = -148355053;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp53482247 = -625641203;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp68328805 = -827626181;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp21737897 = -200469847;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp79475651 = -935209581;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp76693415 = -164203720;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp80032266 = -596712088;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp21756975 = -775313277;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp2593022 = -851089276;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12614793 = -894667286;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp29157807 = -614256237;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp82513834 = -262921875;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22649224 = -618845482;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp81630139 = -101722544;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35289638 = -439858511;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp63186340 = -297370071;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp17727327 = -369268946;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77949708 = -614413979;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12985842 = -50357870;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22391574 = -186138534;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp2031927 = -163443860;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp18810091 = -764860074;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp57076310 = -674522216;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp91571526 = -584205461;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40444361 = -77751608;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35054867 = -308767288;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp9104120 = -213102743;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp3369936 = -469544568;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58613969 = -285248222;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp98114175 = -104425295;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58082998 = -820235475;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39360152 = 50644237;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp19123689 = -381215306;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp15125806 = -93161498;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp91910214 = -755693513;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp23927406 = -345871660;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38552141 = -31521493;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13742374 = -486975569;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp37073636 = -755803374;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp7798852 = -536171110;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58819011 = -517299641;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp74416351 = 26531336;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13887912 = -686759059;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp27677475 = -248052896;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp49464659 = -814619137;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40812413 = -334341966;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp41556036 = -701411038;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp5390272 = -441340234;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp84046791 = -540960127;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp30524238 = -160324819;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp79966432 = -727007570;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77538610 = -161579221;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp25613056 = -746142400;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77109901 = 90741020;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp37191276 = -80219098;    long wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39989794 = 60465306;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp1138999 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp76998232;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp76998232 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp32340239;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp32340239 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12699291;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12699291 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp70067285;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp70067285 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp53207598;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp53207598 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp94498995;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp94498995 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97478108;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97478108 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp61242852;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp61242852 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp98205253;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp98205253 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35291024;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35291024 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97368577;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97368577 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp70193992;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp70193992 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp63770608;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp63770608 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp99260127;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp99260127 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp96124529;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp96124529 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22706365;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22706365 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp52324546;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp52324546 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp95807935;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp95807935 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13571317;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13571317 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77959049;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77959049 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp18625165;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp18625165 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp54349008;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp54349008 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp50710099;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp50710099 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77409652;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77409652 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp6880946;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp6880946 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35480271;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35480271 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp16549287;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp16549287 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp84492267;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp84492267 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38851278;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38851278 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp96173326;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp96173326 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp16480933;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp16480933 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40292268;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40292268 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp78622465;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp78622465 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp23326247;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp23326247 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp64205260;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp64205260 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp87020410;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp87020410 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp19336430;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp19336430 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp93710577;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp93710577 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97693759;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp97693759 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp55488318;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp55488318 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38598898;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38598898 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp99501475;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp99501475 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39223203;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39223203 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58799885;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58799885 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp53482247;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp53482247 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp68328805;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp68328805 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp21737897;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp21737897 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp79475651;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp79475651 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp76693415;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp76693415 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp80032266;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp80032266 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp21756975;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp21756975 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp2593022;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp2593022 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12614793;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12614793 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp29157807;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp29157807 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp82513834;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp82513834 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22649224;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22649224 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp81630139;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp81630139 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35289638;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35289638 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp63186340;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp63186340 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp17727327;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp17727327 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77949708;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77949708 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12985842;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp12985842 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22391574;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp22391574 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp2031927;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp2031927 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp18810091;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp18810091 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp57076310;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp57076310 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp91571526;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp91571526 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40444361;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40444361 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35054867;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp35054867 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp9104120;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp9104120 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp3369936;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp3369936 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58613969;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58613969 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp98114175;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp98114175 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58082998;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58082998 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39360152;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39360152 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp19123689;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp19123689 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp15125806;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp15125806 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp91910214;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp91910214 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp23927406;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp23927406 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38552141;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp38552141 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13742374;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13742374 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp37073636;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp37073636 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp7798852;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp7798852 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58819011;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp58819011 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp74416351;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp74416351 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13887912;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp13887912 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp27677475;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp27677475 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp49464659;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp49464659 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40812413;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp40812413 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp41556036;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp41556036 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp5390272;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp5390272 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp84046791;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp84046791 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp30524238;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp30524238 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp79966432;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp79966432 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77538610;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77538610 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp25613056;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp25613056 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77109901;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp77109901 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp37191276;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp37191276 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39989794;     wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp39989794 = wybschdHitYjhytNdGMuipzdLPZGbkQlpGmDwFyrFVCIiXGBpdCFsHWGzygNHXgycmtbLqRYHUpNmquoIXRxhKthjfDyYkjhp1138999;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void nMBmISUzjaFCMrtGbRiIsULjRjOIRcnXpUispnugPyooplaRCRILYXBTARVYDMhGniMrJBRoXCsufXtfIxFzUcyRTOrXR25660396() {     long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc29922435 = -255196932;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc85445018 = -718082435;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc69329311 = -571040338;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc89136461 = -519609627;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc10577041 = -859305317;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72343034 = -606067989;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc12984945 = -547982041;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51864498 = -141499813;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc14738307 = -408402312;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc44758434 = -938607513;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26416273 = -926939780;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc12100941 = -453914918;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc56347367 = -22860244;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc4275290 = -296355279;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc75062626 = -878638010;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc67310703 = -346880932;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72479246 = -749722973;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc88579938 = -423755141;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77455746 = -631547400;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc57377966 = -392472972;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79094473 = -777768285;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc57816160 = 88687635;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc58583281 = -448928073;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc27638398 = -157753668;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51675684 = -584946023;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77556224 = -744154533;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc53166669 = -161485191;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc38435437 = -261882966;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93354131 = -928705411;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc36339080 = -11534985;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc95975790 = -324759849;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc33319607 = -433368648;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc1627702 = -509018961;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26407921 = -217038466;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc49906858 = -936379241;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc32096092 = 59949425;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc92871469 = -771138715;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc16855702 = -910622815;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc27249603 = 26674498;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc95075392 = -899993124;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc21033509 = -422935914;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc38109686 = 43331414;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc92351722 = -365079693;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc81365642 = -183130506;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74778370 = -217253810;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc78182609 = -388507183;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc55568316 = -314153649;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93364764 = -830300549;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc86638386 = -84845336;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc87229660 = -818229768;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc91435545 = -738953015;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc2422878 = -700652354;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc60425825 = -114975144;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc90126065 = -149114041;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc46565922 = -323732956;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72106275 = -243884568;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26861738 = -169154362;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc41690914 = -313286670;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc37460777 = -934663605;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc33020817 = -15150785;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc19176365 = -344582798;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74549508 = -186099075;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc58510367 = -212794402;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc78399226 = -296867328;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc48782643 = -513847664;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93096665 = -393571133;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc10473240 = -944895957;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc29939447 = -805821779;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc54368431 = -359976038;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc42966534 = -838587435;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74439234 = -575742217;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc55623544 = -839100158;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc61330335 = -350429640;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc82380353 = -731554276;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc36344457 = -969537059;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc40984787 = -721099700;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc65464437 = -546232673;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77217639 = -165797568;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc52860027 = -940499858;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc73493075 = -96438840;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc21987909 = -330000884;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc59801904 = -331184643;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51797051 = -77037631;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc6124472 = -10475644;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc44903535 = -272581970;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93552913 = -624107496;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72893782 = -218393504;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc11501636 = -259904921;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79841998 = -893305510;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77800583 = -592494674;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc5234355 = -770896213;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51180555 = -357852046;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79394925 = -975959210;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc94228786 = -958174717;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc75899027 = -455410327;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc46484001 = -136836839;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79599318 = -743874184;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc13952497 = 31787634;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc32582999 = -669282843;    long VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc81681705 = -255196932;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc29922435 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc85445018;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc85445018 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc69329311;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc69329311 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc89136461;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc89136461 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc10577041;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc10577041 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72343034;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72343034 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc12984945;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc12984945 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51864498;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51864498 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc14738307;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc14738307 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc44758434;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc44758434 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26416273;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26416273 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc12100941;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc12100941 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc56347367;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc56347367 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc4275290;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc4275290 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc75062626;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc75062626 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc67310703;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc67310703 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72479246;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72479246 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc88579938;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc88579938 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77455746;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77455746 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc57377966;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc57377966 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79094473;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79094473 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc57816160;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc57816160 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc58583281;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc58583281 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc27638398;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc27638398 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51675684;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51675684 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77556224;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77556224 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc53166669;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc53166669 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc38435437;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc38435437 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93354131;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93354131 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc36339080;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc36339080 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc95975790;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc95975790 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc33319607;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc33319607 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc1627702;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc1627702 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26407921;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26407921 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc49906858;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc49906858 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc32096092;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc32096092 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc92871469;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc92871469 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc16855702;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc16855702 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc27249603;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc27249603 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc95075392;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc95075392 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc21033509;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc21033509 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc38109686;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc38109686 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc92351722;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc92351722 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc81365642;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc81365642 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74778370;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74778370 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc78182609;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc78182609 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc55568316;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc55568316 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93364764;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93364764 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc86638386;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc86638386 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc87229660;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc87229660 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc91435545;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc91435545 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc2422878;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc2422878 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc60425825;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc60425825 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc90126065;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc90126065 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc46565922;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc46565922 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72106275;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72106275 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26861738;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc26861738 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc41690914;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc41690914 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc37460777;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc37460777 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc33020817;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc33020817 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc19176365;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc19176365 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74549508;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74549508 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc58510367;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc58510367 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc78399226;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc78399226 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc48782643;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc48782643 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93096665;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93096665 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc10473240;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc10473240 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc29939447;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc29939447 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc54368431;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc54368431 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc42966534;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc42966534 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74439234;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc74439234 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc55623544;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc55623544 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc61330335;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc61330335 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc82380353;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc82380353 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc36344457;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc36344457 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc40984787;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc40984787 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc65464437;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc65464437 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77217639;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77217639 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc52860027;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc52860027 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc73493075;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc73493075 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc21987909;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc21987909 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc59801904;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc59801904 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51797051;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51797051 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc6124472;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc6124472 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc44903535;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc44903535 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93552913;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc93552913 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72893782;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc72893782 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc11501636;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc11501636 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79841998;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79841998 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77800583;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc77800583 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc5234355;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc5234355 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51180555;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc51180555 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79394925;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79394925 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc94228786;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc94228786 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc75899027;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc75899027 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc46484001;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc46484001 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79599318;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc79599318 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc13952497;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc13952497 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc32582999;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc32582999 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc81681705;     VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc81681705 = VcDQhBHYjIBAUnWeGodYqPGGimKkvYuJtWBZpexWlGZpDyAobmMZOybGVwhvqXdLNsHxmwpcrGmkqRCjmVIILVuuvvDswCfOc29922435;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void zaRFkTuqRPRjPVIeTIuNZweaaGWasdNojjdEXsJzNLRIPGHxXSpLPCoCwrfewTynCcAfrEGayuzNeWKTyZDaoBZguaIfN74520361() {     double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb26438592 = -867735550;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb31241686 = -649473175;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb74355016 = -890578796;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9145380 = -528153003;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb74809379 = -180714652;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb92187189 = -786879159;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb2525931 = -310631783;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb52709643 = -402295339;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb77622481 = -253997031;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb443214 = -228929290;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76472085 = -922438982;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb19971539 = -54908642;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb45691608 = -847816670;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb94428292 = -633300288;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb57376329 = -748608294;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb92985253 = -489104322;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb72243715 = -752762732;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb63215160 = -292745396;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99164587 = -811683567;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb62066344 = 4014423;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb46938616 = -33084697;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9569785 = -584334116;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29641045 = 49631137;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29638114 = -176410811;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb65729347 = -447024855;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb95293550 = -969346831;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb15952563 = -938267182;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12984038 = -713155571;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb69210879 = 78951763;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb48548653 = -638596140;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb88363530 = -433181385;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76707861 = -569463787;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76345928 = -848143621;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb38926171 = -649207599;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb18212678 = 78612195;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb54353251 = -727244410;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb17457752 = -781735984;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb55023836 = -734123444;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb58327112 = -508053544;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb88656345 = 18374837;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb59376670 = -360506135;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb89454207 = -367213508;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99751978 = -581787138;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb13957801 = -546282038;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb98756059 = 37221553;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb55649652 = -264812643;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb38631513 = -352033986;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb86162997 = -139013870;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb45918260 = -384467601;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb50007988 = -274256780;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb36594500 = -479054718;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb67557886 = -541744730;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb64845029 = -410856784;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb70508125 = -883355119;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb27581746 = -878005110;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb16868808 = -183401434;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1600641 = -599104313;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb44716902 = -614167986;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb43416033 = 18871852;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb79515829 = -211367821;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76234627 = -848611978;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb89541892 = -597476212;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb83498764 = -381247103;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29073829 = -615400891;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12079683 = -795747905;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99764223 = -252975195;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb43625611 = -206765022;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb6765438 = -98609071;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76215615 = -611912484;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb3023079 = 78636116;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb75527501 = -707368339;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb17219879 = 81360711;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb4888049 = -784691852;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb10508242 = -730058404;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb2689674 = -635479442;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb57484408 = -665871189;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9817806 = 97453021;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb15683244 = -404086825;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb30882054 = -113632365;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb10079696 = -82212212;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb56662038 = -517312845;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29789566 = -699253312;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb67065777 = -228687971;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb19202892 = -646791458;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11954153 = -59541423;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb20805644 = -891436656;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11862833 = -58607004;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb5837803 = -964788502;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11344425 = -771202490;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1343871 = -737986371;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb52752610 = -28140098;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb72740849 = -67567998;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11607804 = -652995297;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb78811282 = -196685723;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12421718 = -133013186;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb69834777 = -763029923;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb5955444 = -985966406;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb70678150 = -966386247;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1878119 = -750534133;    double eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb98991835 = -867735550;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb26438592 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb31241686;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb31241686 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb74355016;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb74355016 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9145380;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9145380 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb74809379;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb74809379 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb92187189;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb92187189 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb2525931;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb2525931 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb52709643;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb52709643 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb77622481;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb77622481 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb443214;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb443214 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76472085;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76472085 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb19971539;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb19971539 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb45691608;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb45691608 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb94428292;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb94428292 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb57376329;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb57376329 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb92985253;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb92985253 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb72243715;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb72243715 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb63215160;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb63215160 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99164587;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99164587 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb62066344;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb62066344 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb46938616;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb46938616 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9569785;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9569785 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29641045;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29641045 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29638114;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29638114 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb65729347;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb65729347 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb95293550;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb95293550 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb15952563;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb15952563 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12984038;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12984038 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb69210879;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb69210879 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb48548653;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb48548653 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb88363530;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb88363530 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76707861;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76707861 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76345928;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76345928 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb38926171;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb38926171 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb18212678;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb18212678 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb54353251;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb54353251 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb17457752;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb17457752 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb55023836;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb55023836 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb58327112;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb58327112 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb88656345;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb88656345 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb59376670;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb59376670 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb89454207;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb89454207 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99751978;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99751978 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb13957801;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb13957801 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb98756059;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb98756059 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb55649652;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb55649652 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb38631513;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb38631513 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb86162997;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb86162997 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb45918260;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb45918260 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb50007988;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb50007988 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb36594500;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb36594500 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb67557886;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb67557886 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb64845029;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb64845029 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb70508125;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb70508125 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb27581746;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb27581746 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb16868808;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb16868808 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1600641;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1600641 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb44716902;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb44716902 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb43416033;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb43416033 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb79515829;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb79515829 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76234627;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76234627 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb89541892;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb89541892 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb83498764;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb83498764 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29073829;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29073829 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12079683;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12079683 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99764223;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb99764223 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb43625611;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb43625611 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb6765438;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb6765438 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76215615;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb76215615 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb3023079;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb3023079 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb75527501;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb75527501 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb17219879;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb17219879 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb4888049;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb4888049 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb10508242;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb10508242 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb2689674;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb2689674 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb57484408;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb57484408 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9817806;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb9817806 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb15683244;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb15683244 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb30882054;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb30882054 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb10079696;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb10079696 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb56662038;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb56662038 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29789566;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb29789566 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb67065777;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb67065777 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb19202892;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb19202892 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11954153;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11954153 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb20805644;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb20805644 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11862833;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11862833 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb5837803;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb5837803 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11344425;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11344425 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1343871;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1343871 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb52752610;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb52752610 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb72740849;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb72740849 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11607804;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb11607804 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb78811282;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb78811282 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12421718;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb12421718 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb69834777;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb69834777 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb5955444;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb5955444 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb70678150;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb70678150 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1878119;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb1878119 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb98991835;     eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb98991835 = eCuNeFnGbFTLOMUDbXZgLfXPXyADUlRvaNTsFzAQXNXTkPapEdSCizBzfepfFjmqoXkAscxbWJJyIDfIYLyrSONbMUMbJUKnb26438592;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void IOLkJrKMQskzpaWdQIbpTJGChpDrDXcnzfICroEUBRVqBgReBjDcnJRzIIDUoTJsQxzJSTlcDGnBoPBAYZpTUAzqOJuik93063900() {     double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv55222028 = -83397788;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39688472 = -111885674;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv11344089 = -255848024;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv85582550 = -536391258;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv15319135 = -783502225;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv11322626 = -136232788;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv21011881 = -553186891;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7096034 = -693062453;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv31117936 = -262249081;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv46996393 = -55311004;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv67597333 = -446670354;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34703902 = -691581162;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv31844983 = -857596081;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34932974 = -54640119;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33178828 = -898222497;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv64171426 = -979819734;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22016596 = -952122501;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv99470552 = -873557427;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80812398 = -42529156;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv5872994 = -792229874;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48074040 = -964996950;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48760779 = -919033661;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33875318 = -962472481;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6566412 = -430115913;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39995379 = 39541985;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv65968829 = -754353690;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33638960 = -312306958;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34870188 = -284025583;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv78072743 = -49378819;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv46036455 = -536119397;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv88165994 = 90840705;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv93546535 = -464984100;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv37681362 = -114442400;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86711625 = -437370692;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv44793289 = -160503205;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22244083 = -347038466;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv23308811 = -909811922;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv52543109 = -563927622;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv91866137 = 76315844;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86037978 = -785341773;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv24921862 = 92551152;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv88964996 = -527381825;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv92602225 = -751469317;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv56100240 = -385749587;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv14734545 = -31677204;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80350014 = -27678623;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv25871024 = -938561454;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv57789864 = -768844573;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv53080995 = -634103356;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv60544232 = -928282827;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv47997779 = -621295646;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48223789 = -467083806;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22677834 = -774742651;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48019398 = -137801873;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv44989861 = -587481830;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6461250 = -164364127;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv5813154 = -149413193;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv4777678 = -825732112;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv45587172 = -475933243;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv49350306 = 70851465;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv77683665 = -823925830;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86141692 = -169161308;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv29023291 = -543683635;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv85081480 = -726129684;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv58830399 = -46151709;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv74050797 = -981686254;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv97022540 = -477138763;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv45133357 = -320225389;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv90139684 = -894136914;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv10934746 = -451184031;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40862616 = 29992187;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv69473486 = -288194879;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7604416 = -849873271;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv94774420 = -257187384;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80951132 = -784781026;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59109043 = -337615126;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv56158554 = -67564345;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv77775077 = -476722895;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv91831866 = -298438710;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59645364 = -932779393;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40097806 = -815792237;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv75849096 = -543462386;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv81789192 = -649922228;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv17528511 = -121095992;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv98038676 = -914823752;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39942206 = -442075489;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv70868702 = -690241450;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv89661964 = -976640527;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv41721765 = -849888863;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv38332040 = -996139079;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv16430929 = -97625273;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv18531134 = 15920190;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6955938 = 12005620;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv42515832 = -994535621;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv8354313 = -961415943;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv38780169 = -738287541;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59941706 = -983698190;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7520745 = 74660367;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv97269841 = -239597878;    double kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40683747 = -83397788;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv55222028 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39688472;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39688472 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv11344089;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv11344089 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv85582550;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv85582550 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv15319135;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv15319135 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv11322626;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv11322626 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv21011881;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv21011881 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7096034;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7096034 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv31117936;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv31117936 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv46996393;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv46996393 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv67597333;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv67597333 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34703902;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34703902 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv31844983;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv31844983 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34932974;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34932974 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33178828;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33178828 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv64171426;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv64171426 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22016596;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22016596 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv99470552;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv99470552 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80812398;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80812398 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv5872994;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv5872994 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48074040;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48074040 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48760779;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48760779 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33875318;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33875318 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6566412;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6566412 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39995379;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39995379 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv65968829;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv65968829 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33638960;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv33638960 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34870188;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv34870188 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv78072743;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv78072743 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv46036455;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv46036455 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv88165994;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv88165994 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv93546535;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv93546535 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv37681362;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv37681362 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86711625;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86711625 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv44793289;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv44793289 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22244083;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22244083 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv23308811;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv23308811 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv52543109;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv52543109 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv91866137;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv91866137 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86037978;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86037978 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv24921862;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv24921862 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv88964996;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv88964996 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv92602225;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv92602225 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv56100240;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv56100240 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv14734545;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv14734545 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80350014;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80350014 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv25871024;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv25871024 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv57789864;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv57789864 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv53080995;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv53080995 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv60544232;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv60544232 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv47997779;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv47997779 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48223789;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48223789 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22677834;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv22677834 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48019398;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv48019398 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv44989861;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv44989861 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6461250;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6461250 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv5813154;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv5813154 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv4777678;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv4777678 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv45587172;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv45587172 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv49350306;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv49350306 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv77683665;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv77683665 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86141692;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv86141692 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv29023291;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv29023291 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv85081480;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv85081480 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv58830399;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv58830399 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv74050797;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv74050797 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv97022540;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv97022540 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv45133357;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv45133357 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv90139684;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv90139684 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv10934746;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv10934746 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40862616;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40862616 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv69473486;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv69473486 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7604416;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7604416 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv94774420;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv94774420 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80951132;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv80951132 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59109043;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59109043 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv56158554;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv56158554 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv77775077;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv77775077 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv91831866;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv91831866 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59645364;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59645364 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40097806;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40097806 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv75849096;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv75849096 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv81789192;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv81789192 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv17528511;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv17528511 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv98038676;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv98038676 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39942206;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv39942206 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv70868702;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv70868702 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv89661964;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv89661964 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv41721765;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv41721765 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv38332040;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv38332040 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv16430929;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv16430929 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv18531134;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv18531134 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6955938;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv6955938 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv42515832;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv42515832 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv8354313;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv8354313 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv38780169;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv38780169 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59941706;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv59941706 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7520745;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv7520745 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv97269841;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv97269841 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40683747;     kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv40683747 = kgiIcaDgILiybYgWPKtGCwPdpYPxJuJZCrSaEtMoyTPNHVsbTTyyBbhoRETDLGXNBJvjFGRWyEIjgBWTgndpwLXbJdIAZIdsv55222028;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void juFLPITVuxyLDQgdeZPTneztpCJGdRFyBcQBGdvpuYQCZSWOLOhmoRkEuMWNzMscszJaAeWBdaZONwTXgvuPkTKksLGuG41923867() {     float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51738185 = -695936405;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf85485138 = -43276413;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf16369794 = -575386483;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf5591469 = -544934633;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf79551474 = -104911560;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf31166782 = -317043958;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf10552866 = -315836632;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf7941179 = -953857979;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf94002110 = -107843800;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2681173 = -445632781;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf17653147 = -442169555;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf42574501 = -292574886;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21189224 = -582552507;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf25085977 = -391585128;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf15492531 = -768192781;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf89845976 = -22043124;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21781065 = -955162261;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf74105774 = -742547681;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2521240 = -222665323;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf10561372 = -395742479;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf15918183 = -220313362;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf514404 = -492055412;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf4933082 = -463913270;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf8566128 = -448773056;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf54049041 = -922536848;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf83706155 = -979545989;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf96424854 = 10911052;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf9418788 = -735298188;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf53929491 = -141721644;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf58246028 = -63180553;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80553734 = -17580831;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf36934790 = -601079240;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf12399589 = -453567060;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf99229875 = -869539826;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf13099109 = -245511768;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf44501242 = -34232301;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf47895093 = -920409190;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf90711243 = -387428252;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22943646 = -458412198;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf79618931 = -966973812;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf63265022 = -945019070;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf40309518 = -937926746;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2482 = -968176762;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf88692399 = -748901119;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf38712234 = -877201841;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf57817057 = 96015916;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf8934220 = -976441791;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf50588097 = -77557894;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf12360869 = -933725620;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf23322560 = -384309839;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf93156734 = -361397348;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf13358799 = -308176183;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf27097038 = 29375709;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf28401458 = -872042951;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf26005684 = -41753983;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51223781 = -103880993;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80552057 = -579363143;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf7803666 = -26613428;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51542427 = -622397786;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf95845318 = -125365572;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf34741928 = -227955010;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf1134078 = -580538445;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf54011687 = -712136335;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf35756083 = 55336753;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22127438 = -328051950;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80718356 = -841090316;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf30174912 = -839007827;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21959349 = -713012682;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf11986869 = -46073360;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf70991289 = -633960480;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf41950883 = -101633934;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf31069821 = -467734010;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51162128 = -184135483;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22902309 = -255691512;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf47296349 = -450723410;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf75608664 = -282386616;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf511923 = -523878651;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf16240683 = -715012152;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf69853894 = -571571216;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf96231984 = -918552765;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf74771935 = 96895802;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf45836757 = -911531055;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf97057919 = -801572568;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf30606931 = -757411806;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf65089294 = -701783205;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf67194936 = -709404649;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf9837753 = -530454949;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf83998131 = -581524109;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf73224191 = -727785843;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf61875328 = -41630776;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf63949184 = -454869158;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf40091428 = -793795763;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf39168816 = -765030466;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf27098328 = -233046627;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf44877004 = -639018802;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf62130945 = -264480625;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf86297830 = -125790412;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf64246399 = -923513515;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf66564961 = -320849169;    float iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf57993878 = -695936405;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51738185 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf85485138;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf85485138 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf16369794;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf16369794 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf5591469;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf5591469 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf79551474;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf79551474 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf31166782;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf31166782 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf10552866;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf10552866 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf7941179;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf7941179 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf94002110;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf94002110 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2681173;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2681173 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf17653147;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf17653147 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf42574501;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf42574501 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21189224;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21189224 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf25085977;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf25085977 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf15492531;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf15492531 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf89845976;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf89845976 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21781065;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21781065 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf74105774;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf74105774 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2521240;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2521240 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf10561372;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf10561372 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf15918183;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf15918183 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf514404;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf514404 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf4933082;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf4933082 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf8566128;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf8566128 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf54049041;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf54049041 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf83706155;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf83706155 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf96424854;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf96424854 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf9418788;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf9418788 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf53929491;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf53929491 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf58246028;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf58246028 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80553734;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80553734 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf36934790;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf36934790 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf12399589;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf12399589 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf99229875;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf99229875 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf13099109;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf13099109 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf44501242;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf44501242 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf47895093;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf47895093 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf90711243;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf90711243 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22943646;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22943646 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf79618931;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf79618931 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf63265022;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf63265022 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf40309518;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf40309518 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2482;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf2482 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf88692399;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf88692399 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf38712234;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf38712234 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf57817057;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf57817057 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf8934220;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf8934220 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf50588097;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf50588097 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf12360869;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf12360869 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf23322560;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf23322560 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf93156734;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf93156734 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf13358799;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf13358799 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf27097038;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf27097038 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf28401458;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf28401458 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf26005684;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf26005684 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51223781;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51223781 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80552057;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80552057 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf7803666;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf7803666 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51542427;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51542427 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf95845318;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf95845318 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf34741928;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf34741928 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf1134078;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf1134078 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf54011687;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf54011687 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf35756083;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf35756083 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22127438;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22127438 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80718356;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf80718356 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf30174912;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf30174912 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21959349;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf21959349 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf11986869;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf11986869 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf70991289;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf70991289 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf41950883;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf41950883 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf31069821;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf31069821 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51162128;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51162128 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22902309;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf22902309 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf47296349;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf47296349 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf75608664;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf75608664 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf511923;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf511923 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf16240683;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf16240683 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf69853894;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf69853894 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf96231984;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf96231984 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf74771935;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf74771935 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf45836757;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf45836757 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf97057919;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf97057919 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf30606931;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf30606931 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf65089294;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf65089294 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf67194936;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf67194936 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf9837753;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf9837753 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf83998131;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf83998131 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf73224191;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf73224191 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf61875328;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf61875328 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf63949184;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf63949184 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf40091428;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf40091428 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf39168816;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf39168816 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf27098328;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf27098328 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf44877004;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf44877004 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf62130945;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf62130945 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf86297830;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf86297830 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf64246399;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf64246399 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf66564961;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf66564961 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf57993878;     iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf57993878 = iqhgnKaIvNhsgINsVryszhERcrZpZioYcuvjrhZmtOkhNXEKwsWUIBGBmQDwlkjNYJPlMeAnRMzlwWDFnOsgLUPanuxbddAIf51738185;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZPtVRHuYwZbEtGcYRcEFfwnXpjxbBflxhctxpEMARHPyVvOcRFowzIKdcWghJTXQndWRNeiNPEnXgoCtVVKwrpyYSPVTX60467405() {     float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu80521621 = 88401356;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu93931925 = -605688912;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53358866 = 59344289;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu82028639 = -553172888;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu20061229 = -707699133;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu50302218 = -766397587;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu29038816 = -558391740;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu62327568 = -144625093;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu47497565 = -116095850;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu49234353 = -272014495;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu8778395 = 33599072;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu57306864 = -929247406;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7342599 = -592331918;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu65590658 = -912924958;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91295029 = -917806983;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu61032150 = -512758536;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu71553945 = -54522029;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu10361167 = -223359712;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84169051 = -553510913;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54368022 = -91986777;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu17053606 = -52225615;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39705399 = -826754958;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu9167355 = -376016889;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu85494425 = -702478158;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28315073 = -435970008;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54381434 = -764552848;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu14111252 = -463128725;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu31304938 = -306168199;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu62791354 = -270052226;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu55733830 = 39296190;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu80356198 = -593558741;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53773464 = -496599553;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu73735022 = -819865839;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu47015330 = -657702919;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39679721 = -484627169;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu12392074 = -754026357;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53746152 = 51514872;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu88230516 = -217232430;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu56482672 = -974042811;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu77000564 = -670690421;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28810214 = -491961783;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39820307 = 1904937;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu92852728 = -37858941;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu30834838 = -588368667;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54690719 = -946100598;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu82517419 = -766850064;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu96173730 = -462969259;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu22214965 = -707388596;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu19523604 = -83361375;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu33858804 = 61664114;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu4560013 = -503638276;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu94024700 = -233515259;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84929842 = -334510159;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu5912731 = -126489705;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu43413800 = -851230703;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40816223 = -84843686;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84764570 = -129672024;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu67864440 = -238177554;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53713566 = -17202881;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu65679795 = -943146286;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu36190966 = -203268863;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu97733877 = -152223541;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu99536213 = -874572867;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91763734 = -55392041;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu68878154 = -678455754;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu55004930 = -469801375;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu83571841 = -9381568;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu60327268 = -934629000;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu25910938 = -328297790;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu78902956 = -63780627;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7285998 = -464273408;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu83323429 = -837289600;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53878495 = -249316902;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7168487 = -882820493;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu25557808 = -600024994;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu77233299 = 45869447;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu46852671 = -688896018;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu78332516 = -787648222;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu30803706 = -756377561;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu45797654 = -669119945;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu58207703 = -201583589;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91896287 = -755740129;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu11781335 = -122806825;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28932551 = -231716340;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu51173818 = -457065535;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu86331498 = -260043482;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu68843622 = -62089395;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu67822292 = -593376134;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu3601531 = -806472216;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu98863497 = -299783483;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu27627503 = -524354333;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu85881711 = -710307574;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu34516950 = -100029550;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu90802876 = 69103474;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40809598 = -367421559;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu31076336 = -239738243;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40284094 = -123522196;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu1088994 = -982466901;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu61956683 = -909912913;    float fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu99685789 = 88401356;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu80521621 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu93931925;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu93931925 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53358866;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53358866 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu82028639;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu82028639 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu20061229;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu20061229 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu50302218;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu50302218 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu29038816;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu29038816 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu62327568;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu62327568 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu47497565;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu47497565 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu49234353;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu49234353 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu8778395;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu8778395 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu57306864;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu57306864 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7342599;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7342599 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu65590658;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu65590658 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91295029;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91295029 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu61032150;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu61032150 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu71553945;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu71553945 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu10361167;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu10361167 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84169051;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84169051 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54368022;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54368022 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu17053606;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu17053606 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39705399;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39705399 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu9167355;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu9167355 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu85494425;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu85494425 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28315073;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28315073 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54381434;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54381434 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu14111252;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu14111252 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu31304938;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu31304938 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu62791354;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu62791354 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu55733830;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu55733830 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu80356198;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu80356198 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53773464;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53773464 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu73735022;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu73735022 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu47015330;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu47015330 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39679721;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39679721 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu12392074;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu12392074 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53746152;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53746152 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu88230516;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu88230516 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu56482672;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu56482672 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu77000564;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu77000564 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28810214;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28810214 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39820307;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu39820307 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu92852728;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu92852728 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu30834838;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu30834838 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54690719;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu54690719 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu82517419;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu82517419 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu96173730;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu96173730 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu22214965;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu22214965 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu19523604;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu19523604 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu33858804;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu33858804 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu4560013;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu4560013 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu94024700;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu94024700 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84929842;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84929842 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu5912731;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu5912731 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu43413800;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu43413800 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40816223;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40816223 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84764570;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu84764570 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu67864440;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu67864440 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53713566;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53713566 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu65679795;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu65679795 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu36190966;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu36190966 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu97733877;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu97733877 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu99536213;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu99536213 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91763734;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91763734 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu68878154;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu68878154 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu55004930;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu55004930 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu83571841;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu83571841 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu60327268;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu60327268 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu25910938;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu25910938 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu78902956;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu78902956 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7285998;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7285998 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu83323429;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu83323429 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53878495;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu53878495 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7168487;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu7168487 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu25557808;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu25557808 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu77233299;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu77233299 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu46852671;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu46852671 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu78332516;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu78332516 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu30803706;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu30803706 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu45797654;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu45797654 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu58207703;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu58207703 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91896287;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu91896287 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu11781335;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu11781335 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28932551;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu28932551 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu51173818;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu51173818 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu86331498;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu86331498 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu68843622;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu68843622 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu67822292;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu67822292 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu3601531;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu3601531 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu98863497;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu98863497 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu27627503;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu27627503 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu85881711;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu85881711 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu34516950;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu34516950 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu90802876;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu90802876 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40809598;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40809598 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu31076336;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu31076336 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40284094;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu40284094 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu1088994;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu1088994 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu61956683;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu61956683 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu99685789;     fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu99685789 = fqykzKrYwPmWagvVmwnTmLoIaYyfyzxJPpkNpnhdLVZGYZRIDDfShzlTCqKaMLaxRdHkwhfzPKIRmhkBGpcvTecQEQLzdGRzu80521621;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void JfXyZNvAhXQacPhEtloDUfEIyJETxDmTVRaBaArGISXrGQZhqOMqXCndbPAQIbCdlwEKpfdsthvCIMChjWdjrvwSewZLA79010944() {     int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu9305059 = -227260882;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu2378712 = -68101411;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu90347938 = -405924939;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu58465810 = -561411143;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu60570984 = -210486706;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu69437654 = -115751216;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu47524766 = -800946848;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu16713958 = -435392207;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu993019 = -124347900;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu95787533 = -98396209;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu99903642 = -590632300;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72039226 = -465919926;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93495973 = -602111329;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu6095340 = -334264788;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu67097529 = 32578814;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu32218323 = 96526053;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu21326827 = -253881797;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu46616559 = -804171743;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu65816862 = -884356503;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu98174671 = -888231074;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu18189030 = -984137869;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu78896394 = -61454504;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu13401628 = -288120507;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu62422723 = -956183260;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu2581105 = 50596832;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu25056713 = -549559707;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu31797650 = -937168501;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu53191088 = -977038211;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu71653218 = -398382808;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu53221632 = -958227067;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu80158662 = -69536651;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu70612138 = -392119866;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35070456 = -86164618;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94800785 = -445866012;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu66260332 = -723742569;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu80282905 = -373820412;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu59597210 = -76561065;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu85749788 = -47036608;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu90021698 = -389673423;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu74382197 = -374407030;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94355404 = -38904497;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu39331096 = -158263380;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu85702975 = -207541120;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72977277 = -427836216;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu70669205 = 85000645;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu7217782 = -529716043;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu83413241 = 50503273;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93841831 = -237219299;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu26686338 = -332997130;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu44395048 = -592361933;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu15963291 = -645879203;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu74690602 = -158854336;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu42762646 = -698396026;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu83424003 = -480936460;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu60821915 = -560707422;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu30408664 = -65806379;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu88977084 = -779980904;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27925216 = -449741680;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu55884706 = -512007975;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35514272 = -660926999;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37640005 = -178582715;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94333677 = -823908637;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu45060739 = 62990601;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu47771387 = -166120834;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu15628871 = 71140441;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu29291504 = -98512435;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu36968771 = -279755309;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu98695188 = -56245318;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu39835008 = -610522220;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu86814624 = -593600774;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72621112 = -826912882;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35577038 = -106845190;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu56594861 = -314498320;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91434664 = -409949473;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu3819267 = -749326578;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu78857934 = -725874489;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93193418 = -853913384;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu40424350 = -860284292;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91753518 = -941183906;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu95363322 = -419687126;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu41643471 = -500062981;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37955818 = -599949203;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu26504750 = -544041081;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27258170 = -806020875;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37258342 = -212347864;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu5468060 = -910682315;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27849492 = -693723840;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu51646453 = -605228159;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu33978871 = -885158590;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35851668 = -557936191;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91305821 = -593839508;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu31671995 = -626819386;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu29865083 = -535028633;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu54507426 = -728746424;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu36742193 = -95824315;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu21728 = -214995860;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94270356 = -121253981;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37931589 = 58579713;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu57348406 = -398976658;    int axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu41377701 = -227260882;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu9305059 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu2378712;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu2378712 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu90347938;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu90347938 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu58465810;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu58465810 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu60570984;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu60570984 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu69437654;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu69437654 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu47524766;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu47524766 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu16713958;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu16713958 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu993019;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu993019 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu95787533;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu95787533 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu99903642;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu99903642 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72039226;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72039226 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93495973;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93495973 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu6095340;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu6095340 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu67097529;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu67097529 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu32218323;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu32218323 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu21326827;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu21326827 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu46616559;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu46616559 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu65816862;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu65816862 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu98174671;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu98174671 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu18189030;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu18189030 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu78896394;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu78896394 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu13401628;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu13401628 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu62422723;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu62422723 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu2581105;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu2581105 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu25056713;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu25056713 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu31797650;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu31797650 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu53191088;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu53191088 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu71653218;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu71653218 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu53221632;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu53221632 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu80158662;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu80158662 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu70612138;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu70612138 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35070456;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35070456 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94800785;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94800785 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu66260332;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu66260332 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu80282905;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu80282905 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu59597210;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu59597210 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu85749788;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu85749788 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu90021698;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu90021698 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu74382197;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu74382197 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94355404;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94355404 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu39331096;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu39331096 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu85702975;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu85702975 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72977277;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72977277 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu70669205;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu70669205 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu7217782;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu7217782 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu83413241;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu83413241 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93841831;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93841831 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu26686338;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu26686338 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu44395048;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu44395048 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu15963291;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu15963291 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu74690602;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu74690602 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu42762646;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu42762646 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu83424003;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu83424003 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu60821915;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu60821915 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu30408664;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu30408664 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu88977084;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu88977084 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27925216;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27925216 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu55884706;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu55884706 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35514272;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35514272 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37640005;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37640005 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94333677;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94333677 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu45060739;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu45060739 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu47771387;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu47771387 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu15628871;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu15628871 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu29291504;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu29291504 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu36968771;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu36968771 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu98695188;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu98695188 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu39835008;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu39835008 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu86814624;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu86814624 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72621112;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu72621112 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35577038;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35577038 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu56594861;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu56594861 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91434664;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91434664 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu3819267;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu3819267 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu78857934;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu78857934 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93193418;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu93193418 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu40424350;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu40424350 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91753518;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91753518 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu95363322;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu95363322 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu41643471;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu41643471 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37955818;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37955818 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu26504750;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu26504750 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27258170;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27258170 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37258342;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37258342 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu5468060;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu5468060 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27849492;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu27849492 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu51646453;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu51646453 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu33978871;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu33978871 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35851668;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu35851668 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91305821;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu91305821 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu31671995;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu31671995 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu29865083;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu29865083 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu54507426;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu54507426 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu36742193;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu36742193 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu21728;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu21728 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94270356;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu94270356 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37931589;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu37931589 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu57348406;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu57348406 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu41377701;     axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu41377701 = axgPuGJMOsLkTwpHkVPMShlyTlqzlgsFSsTjPmOGsVVeSyXyHLtbKyitLivIPKahAFRbdFsyHJTwLHgVNssZnRvTKZYhrFqiu9305059;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void eBpHyAcQfOWqAkhRclCJIEypXOsDRPEGfCmQqccVOlFuXvcLgbhiHxzaCAkkIZzPJQLxxQaAIguXyDSbmgWqhWzhCMOqs97554483() {     int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm38088495 = -542923120;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm10825498 = -630513910;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm27337011 = -871194168;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm34902981 = -569649398;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm1080740 = -813274279;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm88573090 = -565104844;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm66010716 = 56498044;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71100347 = -726159321;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54488473 = -132599951;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42340714 = 75222078;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm91028890 = -114863673;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86771589 = -2592446;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm79649348 = -611890740;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm46600022 = -855604619;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42900028 = -117035388;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3404497 = -394189359;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71099707 = -453241566;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm82871951 = -284983773;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm47464673 = -115202092;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm41981322 = -584475372;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm19324453 = -816050123;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm18087389 = -396154049;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm17635900 = -200224125;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39351021 = -109888362;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm76847136 = -562836329;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm95731991 = -334566567;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm49484047 = -311208278;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm75077238 = -547908223;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm80515082 = -526713389;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm50709434 = -855750324;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm79961126 = -645514560;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87450812 = -287640179;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm96405889 = -452463397;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42586241 = -234029104;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm92840943 = -962857969;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm48173737 = 6385533;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm65448269 = -204637003;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm83269061 = -976840786;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm23560724 = -905304035;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71763830 = -78123639;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm59900596 = -685847210;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm38841884 = -318431697;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm78553222 = -377223299;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm15119717 = -267303765;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86647690 = 16101888;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm31918144 = -292582023;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm70652752 = -536024195;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm65468699 = -867050001;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm33849073 = -582632885;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54931293 = -146387981;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm27366569 = -788120131;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm55356505 = -84193413;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm595451 = 37718107;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm60935275 = -835383214;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm78230030 = -270184142;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm20001106 = -46769071;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm93189597 = -330289785;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87985990 = -661305806;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm58055845 = 93186930;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm5348749 = -378707713;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39089043 = -153896567;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90933477 = -395593733;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90585264 = -99445932;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3779039 = -276849627;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm62379587 = -279263363;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3578079 = -827223494;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90365700 = -550129050;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm37063108 = -277861636;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm53759078 = -892746650;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm94726291 = -23420921;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm37956227 = -89552356;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87830645 = -476400780;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm59311228 = -379679739;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm75700843 = 62921546;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm82080725 = -898628162;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm80482568 = -397618426;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39534167 = 81069249;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm2516184 = -932920361;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm52703330 = -25990251;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm44928992 = -170254306;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25079239 = -798542372;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm84015348 = -444158277;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm41228165 = -965275338;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25583790 = -280325409;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm23342866 = 32369806;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm24604622 = -461321148;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86855362 = -225358286;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm35470614 = -617080184;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm64356210 = -963844963;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm72839838 = -816088899;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54984139 = -663324683;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm77462278 = -543331198;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25213217 = -970027717;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm18211976 = -426596322;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm32674788 = -924227072;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm68967118 = -190253477;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm48256620 = -118985766;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm74774183 = -373672;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm52740129 = -988040403;    int sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm83069612 = -542923120;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm38088495 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm10825498;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm10825498 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm27337011;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm27337011 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm34902981;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm34902981 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm1080740;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm1080740 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm88573090;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm88573090 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm66010716;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm66010716 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71100347;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71100347 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54488473;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54488473 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42340714;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42340714 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm91028890;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm91028890 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86771589;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86771589 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm79649348;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm79649348 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm46600022;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm46600022 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42900028;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42900028 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3404497;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3404497 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71099707;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71099707 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm82871951;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm82871951 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm47464673;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm47464673 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm41981322;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm41981322 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm19324453;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm19324453 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm18087389;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm18087389 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm17635900;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm17635900 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39351021;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39351021 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm76847136;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm76847136 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm95731991;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm95731991 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm49484047;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm49484047 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm75077238;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm75077238 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm80515082;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm80515082 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm50709434;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm50709434 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm79961126;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm79961126 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87450812;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87450812 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm96405889;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm96405889 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42586241;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm42586241 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm92840943;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm92840943 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm48173737;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm48173737 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm65448269;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm65448269 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm83269061;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm83269061 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm23560724;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm23560724 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71763830;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm71763830 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm59900596;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm59900596 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm38841884;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm38841884 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm78553222;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm78553222 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm15119717;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm15119717 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86647690;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86647690 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm31918144;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm31918144 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm70652752;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm70652752 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm65468699;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm65468699 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm33849073;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm33849073 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54931293;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54931293 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm27366569;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm27366569 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm55356505;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm55356505 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm595451;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm595451 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm60935275;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm60935275 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm78230030;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm78230030 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm20001106;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm20001106 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm93189597;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm93189597 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87985990;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87985990 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm58055845;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm58055845 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm5348749;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm5348749 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39089043;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39089043 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90933477;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90933477 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90585264;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90585264 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3779039;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3779039 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm62379587;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm62379587 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3578079;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm3578079 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90365700;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm90365700 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm37063108;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm37063108 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm53759078;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm53759078 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm94726291;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm94726291 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm37956227;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm37956227 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87830645;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm87830645 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm59311228;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm59311228 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm75700843;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm75700843 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm82080725;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm82080725 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm80482568;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm80482568 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39534167;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm39534167 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm2516184;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm2516184 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm52703330;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm52703330 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm44928992;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm44928992 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25079239;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25079239 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm84015348;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm84015348 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm41228165;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm41228165 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25583790;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25583790 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm23342866;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm23342866 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm24604622;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm24604622 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86855362;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm86855362 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm35470614;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm35470614 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm64356210;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm64356210 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm72839838;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm72839838 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54984139;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm54984139 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm77462278;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm77462278 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25213217;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm25213217 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm18211976;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm18211976 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm32674788;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm32674788 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm68967118;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm68967118 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm48256620;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm48256620 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm74774183;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm74774183 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm52740129;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm52740129 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm83069612;     sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm83069612 = sNplplGkeOQAhZaDcbyiIxbDJinRevlBkHQSFlJSLrHSFpzwYdZkQIkummcHxpFjcOPZJHyrSfWQqIxTirsBVAlcXVgSIBVWm38088495;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void jQMKyBgCDUGydgVITMXXHBMgYLreolefUSVoKTwkclpoj18616541() {     int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39417127 = -232153482;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3892644 = -97872732;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10986247 = -20597411;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb83719761 = -576410798;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3318 = -12113203;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb89606036 = -392863360;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17825938 = -804001630;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb30268924 = -393469183;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb9841021 = -182360852;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15076596 = -759345794;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb98319823 = -634178347;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65760664 = -889737160;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61229903 = -421736294;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb70435955 = -462717059;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb99687570 = -515035956;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25645110 = -248904365;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb29928469 = -938951656;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb510449 = -750839919;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb45327658 = -579935868;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb85679322 = -85034784;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb57561188 = -47637837;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb32641757 = -959782760;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88434057 = -380932534;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb97298191 = -595603500;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb69032404 = -824721996;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33076977 = -331083186;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb30286583 = -945412579;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb432936 = -478806510;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb91904867 = -624428885;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb68474707 = -28186490;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb20099320 = -349259122;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13478064 = -386615047;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb32375942 = -808461367;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33003197 = -892410968;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb20224004 = 56431470;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb49605896 = -550901559;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39261272 = -977984236;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb19826667 = -193314527;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10027782 = -715042264;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb85252141 = -472171329;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb57362821 = 33786444;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb18383492 = -525633623;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb59122901 = -390624451;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb82999364 = -449227912;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb36179565 = -117886688;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3188951 = -734642435;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10238276 = -196958310;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39248551 = -380750539;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88589783 = 89151798;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb76191728 = -365836068;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65296042 = -621380337;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb8300701 = -368946293;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15214682 = -124741670;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb2647334 = -946722236;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61055352 = -287646305;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb6775370 = -272370722;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15458587 = -716940198;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13688055 = -424993912;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb14687358 = -751688803;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb66926341 = -681030017;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb59319454 = -447450781;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17393002 = -225195120;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb38364056 = -769040299;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb41366313 = -54174362;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb94977275 = -310086672;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb84841760 = -147563301;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33384722 = 18724206;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb28226706 = -529325326;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb50211951 = -419148529;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb50081674 = -964134397;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb86383837 = -270920130;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10101803 = -645637129;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb90482666 = 64202344;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb60075517 = -7764540;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb28316502 = -18821229;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39177697 = -522004214;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb73518856 = -469158310;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb5434693 = -931704622;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61118626 = -377716813;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65843453 = 9920439;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb22838701 = -34124877;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb91038032 = -464662041;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb11843153 = -467958308;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15713140 = -158592817;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3178665 = -406806154;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb11798620 = -980312829;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb98263381 = -161873378;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb29728609 = -861739132;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb71947845 = -504764664;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13448634 = -671197808;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb34147310 = -833961361;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25573218 = -452990324;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb5139310 = -441625725;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb43101440 = 65987752;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25932688 = 75279452;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39969819 = -741018436;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb80019435 = -756593325;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17756588 = -236450089;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88022089 = -39141240;    int tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb51337805 = -232153482;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39417127 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3892644;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3892644 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10986247;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10986247 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb83719761;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb83719761 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3318;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3318 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb89606036;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb89606036 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17825938;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17825938 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb30268924;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb30268924 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb9841021;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb9841021 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15076596;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15076596 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb98319823;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb98319823 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65760664;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65760664 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61229903;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61229903 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb70435955;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb70435955 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb99687570;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb99687570 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25645110;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25645110 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb29928469;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb29928469 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb510449;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb510449 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb45327658;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb45327658 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb85679322;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb85679322 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb57561188;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb57561188 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb32641757;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb32641757 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88434057;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88434057 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb97298191;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb97298191 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb69032404;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb69032404 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33076977;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33076977 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb30286583;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb30286583 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb432936;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb432936 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb91904867;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb91904867 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb68474707;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb68474707 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb20099320;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb20099320 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13478064;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13478064 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb32375942;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb32375942 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33003197;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33003197 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb20224004;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb20224004 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb49605896;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb49605896 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39261272;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39261272 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb19826667;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb19826667 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10027782;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10027782 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb85252141;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb85252141 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb57362821;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb57362821 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb18383492;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb18383492 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb59122901;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb59122901 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb82999364;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb82999364 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb36179565;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb36179565 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3188951;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3188951 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10238276;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10238276 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39248551;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39248551 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88589783;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88589783 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb76191728;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb76191728 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65296042;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65296042 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb8300701;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb8300701 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15214682;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15214682 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb2647334;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb2647334 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61055352;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61055352 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb6775370;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb6775370 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15458587;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15458587 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13688055;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13688055 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb14687358;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb14687358 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb66926341;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb66926341 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb59319454;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb59319454 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17393002;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17393002 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb38364056;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb38364056 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb41366313;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb41366313 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb94977275;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb94977275 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb84841760;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb84841760 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33384722;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb33384722 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb28226706;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb28226706 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb50211951;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb50211951 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb50081674;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb50081674 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb86383837;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb86383837 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10101803;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb10101803 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb90482666;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb90482666 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb60075517;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb60075517 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb28316502;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb28316502 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39177697;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39177697 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb73518856;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb73518856 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb5434693;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb5434693 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61118626;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb61118626 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65843453;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb65843453 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb22838701;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb22838701 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb91038032;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb91038032 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb11843153;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb11843153 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15713140;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb15713140 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3178665;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb3178665 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb11798620;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb11798620 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb98263381;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb98263381 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb29728609;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb29728609 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb71947845;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb71947845 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13448634;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb13448634 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb34147310;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb34147310 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25573218;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25573218 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb5139310;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb5139310 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb43101440;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb43101440 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25932688;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb25932688 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39969819;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39969819 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb80019435;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb80019435 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17756588;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb17756588 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88022089;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb88022089 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb51337805;     tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb51337805 = tfbtYaCdPjhDflnJCORqbdLLhxAQmuVMYxfgPkPLb39417127;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void XnDIIEoLZusMFxvWmseHJqumaMIYIuGKcJjweZCLbyqeWSZWbWycbDYgtQhvvPDfnhjvOzsNpSgKAPPHnmwl87879186() {     float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT15890510 = -624158469;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT81644225 = 18666039;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT68102769 = -981763863;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT79069779 = -147567800;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39009371 = -895977024;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT45201465 = -383273760;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT52220893 = -471480660;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT80913681 = -754306922;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT35509153 = -621832675;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT57311563 = -176768896;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58007689 = -795028658;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT51853099 = 52917963;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT73962617 = -224139625;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT7290963 = 2526040;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT47755493 = -330355932;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT50665645 = 62171705;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT51135672 = -608183487;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT92132604 = -384661115;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT11399618 = -147874206;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT20460402 = -127658212;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT88724573 = -203032877;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT37444457 = -901973974;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT52814491 = -203182231;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT57829237 = -645203443;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2937032 = -771879347;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT22180816 = -616080463;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT90423901 = -77460759;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT6963524 = -655790097;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT86038680 = -444654835;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT96772128 = -532874339;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39676454 = -397406012;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT55722928 = -769911287;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT68077495 = -658554972;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT339626 = -736692271;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT76741391 = -440907354;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT27923493 = -303196008;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT13018701 = -377854642;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT71700607 = -646417645;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38834675 = -675736854;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT7845629 = -942744772;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT61625901 = -6874921;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT64409151 = -917527829;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT77632162 = -554661711;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT19704572 = -710195969;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT18088466 = -123722616;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT17943016 = -962886615;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT66228995 = -645663525;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT32207402 = -951279577;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT97325836 = -327576744;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT47103579 = -378422990;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT35501086 = -111762668;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT1345143 = -850463231;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT25889724 = -324279270;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT93213348 = -749577705;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT99154613 = -523126374;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT78446052 = -722184495;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT28829734 = -778151731;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT10273533 = -236560421;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT76132747 = -375688454;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT16828555 = -179896562;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT54777564 = -205813001;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT45257370 = -815690563;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT94875000 = -209652088;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38737024 = 11041663;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT17635110 = -779362885;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2284762 = 74882628;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT83775603 = -288527065;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT73622991 = -487447354;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT30549571 = -556566606;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT19832001 = 72840076;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT37646945 = -559973654;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT79435064 = -961765843;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT53297930 = -708924261;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT3553990 = -205129434;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58834501 = -20783291;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT24315422 = -285505049;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT59812294 = -247312264;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT33109919 = -492986263;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39740771 = -421480828;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT84994016 = -808992732;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT55951821 = -970416939;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58216500 = -126181182;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT9637687 = -228213354;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38935102 = 33768155;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT61271042 = -321111672;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38331311 = -546942782;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT29833205 = -345632017;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT74864147 = -908977267;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT1185013 = -113565898;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT98295338 = -718722860;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT99093759 = -525044278;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2745169 = -41294222;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT95567860 = -170729192;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT22006121 = -395840293;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT53068065 = -636931772;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT16368532 = -191184359;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT69534150 = -607875742;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38895139 = -465703375;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2069463 = -930833085;    float wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT15803705 = -624158469;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT15890510 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT81644225;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT81644225 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT68102769;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT68102769 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT79069779;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT79069779 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39009371;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39009371 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT45201465;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT45201465 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT52220893;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT52220893 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT80913681;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT80913681 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT35509153;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT35509153 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT57311563;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT57311563 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58007689;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58007689 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT51853099;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT51853099 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT73962617;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT73962617 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT7290963;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT7290963 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT47755493;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT47755493 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT50665645;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT50665645 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT51135672;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT51135672 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT92132604;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT92132604 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT11399618;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT11399618 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT20460402;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT20460402 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT88724573;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT88724573 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT37444457;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT37444457 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT52814491;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT52814491 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT57829237;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT57829237 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2937032;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2937032 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT22180816;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT22180816 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT90423901;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT90423901 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT6963524;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT6963524 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT86038680;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT86038680 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT96772128;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT96772128 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39676454;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39676454 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT55722928;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT55722928 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT68077495;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT68077495 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT339626;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT339626 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT76741391;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT76741391 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT27923493;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT27923493 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT13018701;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT13018701 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT71700607;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT71700607 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38834675;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38834675 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT7845629;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT7845629 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT61625901;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT61625901 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT64409151;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT64409151 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT77632162;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT77632162 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT19704572;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT19704572 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT18088466;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT18088466 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT17943016;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT17943016 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT66228995;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT66228995 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT32207402;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT32207402 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT97325836;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT97325836 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT47103579;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT47103579 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT35501086;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT35501086 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT1345143;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT1345143 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT25889724;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT25889724 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT93213348;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT93213348 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT99154613;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT99154613 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT78446052;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT78446052 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT28829734;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT28829734 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT10273533;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT10273533 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT76132747;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT76132747 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT16828555;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT16828555 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT54777564;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT54777564 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT45257370;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT45257370 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT94875000;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT94875000 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38737024;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38737024 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT17635110;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT17635110 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2284762;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2284762 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT83775603;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT83775603 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT73622991;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT73622991 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT30549571;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT30549571 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT19832001;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT19832001 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT37646945;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT37646945 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT79435064;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT79435064 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT53297930;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT53297930 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT3553990;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT3553990 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58834501;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58834501 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT24315422;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT24315422 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT59812294;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT59812294 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT33109919;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT33109919 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39740771;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT39740771 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT84994016;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT84994016 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT55951821;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT55951821 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58216500;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT58216500 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT9637687;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT9637687 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38935102;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38935102 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT61271042;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT61271042 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38331311;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38331311 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT29833205;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT29833205 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT74864147;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT74864147 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT1185013;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT1185013 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT98295338;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT98295338 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT99093759;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT99093759 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2745169;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2745169 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT95567860;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT95567860 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT22006121;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT22006121 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT53068065;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT53068065 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT16368532;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT16368532 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT69534150;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT69534150 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38895139;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT38895139 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2069463;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT2069463 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT15803705;     wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT15803705 = wdjYlCDNrHQLvLrojeHceknIXNgeAaIJSRiJyKJMwfViCUPpYbYJeUHLqaBYVYMRyGsechqYUvgyPFFCyrGZCyeyJXTXJlqXT15890510;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void yLSwlbqrKlviKIBnsOkdMkEuLhSVFZHTwTLCxEdjEXrIiQDSEKM91927666() {     long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63577220 = -86763285;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20235180 = -423680248;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe38886825 = -906225394;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe90522556 = -170756965;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe41925719 = -311230926;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe99064174 = -874046937;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe52403568 = -298672815;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe11779075 = -676466191;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe49051912 = -359875483;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51313108 = -136213720;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe36730610 = -311383632;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe30359009 = -435493565;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe16468414 = -577592781;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe80563399 = -283467555;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe42606972 = 22581887;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe91782280 = -952434639;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50496373 = 12137165;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8999636 = -186206087;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe56037901 = -793958089;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe47471713 = -780049568;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe58587247 = -381748845;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe92204294 = -214461584;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe31399851 = -892807231;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63257038 = -381558546;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe26796973 = -868950452;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe98896414 = 29826151;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe17985614 = 14130981;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe9309724 = -466387161;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe77649853 = 90414623;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe15626683 = -977754618;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76157462 = -63121610;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe73491047 = -353598097;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe85169826 = -793321909;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20032018 = -181151348;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe47857187 = -828787743;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe59764353 = 74420706;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe36895754 = -249475800;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe61014115 = -167347921;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51759341 = 72858447;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe18993930 = -964317450;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe94271624 = 5434478;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe3772853 = -146149778;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe83432857 = -828581921;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe93883289 = -910178698;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe26027907 = -847289488;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe28210703 = -784287170;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe48829100 = -748481583;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe41231177 = -646358593;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe58228350 = -826551486;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe17501897 = -630496304;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe932537 = -349181575;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20997312 = 52286030;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe66456135 = -655958005;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe11393227 = 86053653;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe33340420 = -927579368;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe71372925 = -872301702;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe88835328 = -530873018;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe75629787 = -424666849;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63725583 = -301806514;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe43029304 = -241057077;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe81078560 = -788177919;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe43093844 = -832285654;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe34129222 = -666880814;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe33425230 = -382120865;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe75155645 = 26907889;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63239562 = -957785535;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe45189183 = -642171657;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe96436395 = -296441434;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe32706213 = -454679812;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe82842619 = 48161180;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe54886526 = -602958840;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe89482257 = -820514915;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe57240295 = -159064535;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37043971 = -829640640;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe53200088 = -685484046;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe54814394 = -135599067;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8771438 = -385879664;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37516562 = -982628533;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37229131 = -534269058;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe98586270 = 15336717;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50067314 = -221692266;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76754437 = -339510426;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51081374 = -639835675;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe60147956 = -279089074;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe14694147 = -528573044;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe55160151 = -15407640;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe7034912 = -697640093;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe73776600 = -779375563;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe86691598 = -253571981;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76484262 = -956486042;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe70929024 = -394706277;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe61265967 = -824808951;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe97288532 = -865541408;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8730037 = -686084476;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37915369 = -76139531;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51177781 = -162279868;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe69643631 = -479268965;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50007628 = -346461057;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe18727645 = -837086588;    long MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe62788344 = -86763285;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63577220 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20235180;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20235180 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe38886825;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe38886825 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe90522556;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe90522556 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe41925719;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe41925719 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe99064174;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe99064174 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe52403568;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe52403568 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe11779075;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe11779075 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe49051912;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe49051912 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51313108;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51313108 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe36730610;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe36730610 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe30359009;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe30359009 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe16468414;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe16468414 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe80563399;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe80563399 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe42606972;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe42606972 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe91782280;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe91782280 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50496373;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50496373 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8999636;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8999636 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe56037901;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe56037901 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe47471713;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe47471713 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe58587247;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe58587247 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe92204294;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe92204294 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe31399851;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe31399851 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63257038;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63257038 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe26796973;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe26796973 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe98896414;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe98896414 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe17985614;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe17985614 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe9309724;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe9309724 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe77649853;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe77649853 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe15626683;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe15626683 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76157462;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76157462 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe73491047;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe73491047 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe85169826;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe85169826 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20032018;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20032018 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe47857187;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe47857187 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe59764353;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe59764353 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe36895754;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe36895754 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe61014115;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe61014115 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51759341;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51759341 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe18993930;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe18993930 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe94271624;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe94271624 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe3772853;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe3772853 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe83432857;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe83432857 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe93883289;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe93883289 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe26027907;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe26027907 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe28210703;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe28210703 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe48829100;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe48829100 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe41231177;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe41231177 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe58228350;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe58228350 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe17501897;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe17501897 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe932537;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe932537 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20997312;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe20997312 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe66456135;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe66456135 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe11393227;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe11393227 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe33340420;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe33340420 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe71372925;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe71372925 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe88835328;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe88835328 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe75629787;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe75629787 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63725583;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63725583 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe43029304;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe43029304 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe81078560;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe81078560 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe43093844;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe43093844 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe34129222;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe34129222 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe33425230;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe33425230 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe75155645;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe75155645 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63239562;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63239562 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe45189183;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe45189183 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe96436395;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe96436395 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe32706213;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe32706213 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe82842619;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe82842619 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe54886526;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe54886526 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe89482257;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe89482257 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe57240295;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe57240295 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37043971;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37043971 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe53200088;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe53200088 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe54814394;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe54814394 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8771438;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8771438 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37516562;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37516562 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37229131;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37229131 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe98586270;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe98586270 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50067314;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50067314 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76754437;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76754437 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51081374;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51081374 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe60147956;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe60147956 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe14694147;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe14694147 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe55160151;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe55160151 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe7034912;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe7034912 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe73776600;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe73776600 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe86691598;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe86691598 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76484262;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe76484262 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe70929024;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe70929024 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe61265967;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe61265967 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe97288532;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe97288532 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8730037;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe8730037 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37915369;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe37915369 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51177781;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe51177781 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe69643631;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe69643631 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50007628;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe50007628 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe18727645;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe18727645 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe62788344;     MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe62788344 = MFnWOHXkEIVmkdcmrxzqnlBYpswMvdAQuNuOJwxvZUYajkHeGe63577220;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void yDpwGNnueUgljinKJdciGZbrdlTUrHPZufDapbWPKxBCdAlSMcBIjPJZcySVtSCXEHqDeMVhGMYIYYOdzAIPGHqfVe47235914() {     double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb32189957 = -366266727;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb12154895 = -573885848;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7802320 = -490610308;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb65072217 = -885572232;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb46362478 = 97761626;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb81865837 = -65224890;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb63216884 = -459645855;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb30390027 = -806872109;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb12204678 = -57523517;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb79320684 = -154023951;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb66416243 = -32413975;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb92136620 = -432798693;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb45627358 = -82997855;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb17812288 = -640746656;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb47382347 = -677939278;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb31775834 = -547669634;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb91712997 = -96973076;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb21636383 = 86732327;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb1241507 = -750690981;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb22065258 = -970532492;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb55036866 = -786931229;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb30936850 = -437731928;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb4937975 = -558999297;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb51708094 = -738412203;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb56139454 = -205624650;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb51290761 = -466163434;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb3611497 = -230845919;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb48858368 = -405319845;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb96749396 = 9882385;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb75435721 = -797360021;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb8039087 = -898931639;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb86000382 = -643450531;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb69882650 = -145850528;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98867716 = -184145886;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb97642342 = -794726225;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb41191663 = -702385990;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb69669731 = -922315;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15361331 = -759511014;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb31323134 = -614447022;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb93844178 = -679625612;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb9000979 = -785724079;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb39744732 = -521622187;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb17646109 = -59950883;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb49284810 = -672620694;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78977700 = -907304431;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb79866086 = -93479131;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb85497292 = -667671584;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb66478099 = -188539099;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb16593209 = -923295138;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61902614 = -180842145;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61331222 = -861959838;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb39436353 = -919587213;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb24954409 = -359660340;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98743958 = -171432173;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb92199047 = -128888550;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb1253107 = -928534799;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7216920 = 85113448;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb56094226 = -752198106;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb8932763 = -579947583;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb95071716 = -436074941;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78254340 = -834378972;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb14358517 = 45673989;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb33640630 = -716754495;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb36768956 = -260163496;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb71281598 = -255092313;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb80415861 = -388963444;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb22253971 = -186948166;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb46759642 = -898851970;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb20169945 = -846020431;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6190685 = -975553288;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb62106102 = -446747320;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb76351666 = -337462063;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb90313249 = -298820651;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7397328 = 28934631;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13064279 = -84808413;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15292135 = -165309043;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13290741 = -277781046;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb55653164 = -886378603;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb72730394 = -831107773;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb76273368 = -12145519;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb65793469 = -798491850;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb37133398 = 57693179;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb32265160 = -482024707;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb34846782 = -809275470;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb14104500 = -935400184;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb68602733 = -979344426;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61045973 = -183790191;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb71138691 = -974418355;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6668669 = 44742664;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb96389236 = -866191426;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb33974744 = -687499439;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13575506 = -248724210;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6428568 = -79563432;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb36251417 = -78372081;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15589839 = -845246640;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb94642462 = -731398068;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6104102 = -804867693;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb80877152 = -799787387;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78003212 = -317528381;    double yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98561838 = -366266727;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb32189957 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb12154895;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb12154895 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7802320;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7802320 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb65072217;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb65072217 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb46362478;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb46362478 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb81865837;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb81865837 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb63216884;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb63216884 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb30390027;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb30390027 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb12204678;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb12204678 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb79320684;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb79320684 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb66416243;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb66416243 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb92136620;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb92136620 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb45627358;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb45627358 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb17812288;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb17812288 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb47382347;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb47382347 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb31775834;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb31775834 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb91712997;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb91712997 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb21636383;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb21636383 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb1241507;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb1241507 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb22065258;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb22065258 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb55036866;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb55036866 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb30936850;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb30936850 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb4937975;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb4937975 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb51708094;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb51708094 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb56139454;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb56139454 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb51290761;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb51290761 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb3611497;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb3611497 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb48858368;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb48858368 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb96749396;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb96749396 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb75435721;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb75435721 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb8039087;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb8039087 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb86000382;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb86000382 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb69882650;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb69882650 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98867716;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98867716 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb97642342;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb97642342 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb41191663;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb41191663 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb69669731;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb69669731 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15361331;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15361331 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb31323134;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb31323134 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb93844178;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb93844178 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb9000979;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb9000979 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb39744732;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb39744732 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb17646109;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb17646109 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb49284810;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb49284810 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78977700;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78977700 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb79866086;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb79866086 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb85497292;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb85497292 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb66478099;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb66478099 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb16593209;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb16593209 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61902614;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61902614 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61331222;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61331222 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb39436353;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb39436353 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb24954409;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb24954409 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98743958;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98743958 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb92199047;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb92199047 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb1253107;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb1253107 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7216920;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7216920 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb56094226;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb56094226 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb8932763;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb8932763 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb95071716;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb95071716 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78254340;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78254340 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb14358517;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb14358517 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb33640630;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb33640630 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb36768956;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb36768956 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb71281598;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb71281598 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb80415861;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb80415861 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb22253971;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb22253971 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb46759642;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb46759642 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb20169945;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb20169945 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6190685;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6190685 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb62106102;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb62106102 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb76351666;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb76351666 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb90313249;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb90313249 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7397328;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb7397328 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13064279;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13064279 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15292135;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15292135 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13290741;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13290741 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb55653164;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb55653164 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb72730394;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb72730394 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb76273368;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb76273368 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb65793469;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb65793469 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb37133398;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb37133398 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb32265160;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb32265160 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb34846782;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb34846782 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb14104500;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb14104500 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb68602733;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb68602733 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61045973;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb61045973 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb71138691;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb71138691 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6668669;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6668669 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb96389236;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb96389236 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb33974744;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb33974744 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13575506;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb13575506 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6428568;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6428568 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb36251417;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb36251417 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15589839;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb15589839 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb94642462;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb94642462 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6104102;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb6104102 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb80877152;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb80877152 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78003212;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb78003212 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98561838;     yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb98561838 = yqIAVsueyVJiozGTBWqABDkoWtJnQwRKhzNeYGqgOuasTRMIJmGToVduGUeciHNBDzAolb32189957;}
// Junk Finished

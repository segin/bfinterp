/*
 * Brainfuck Interpreter for Windows 3.1 (16-bit)
 * ANSI C89/ISO C90 Standard
 */

/* Removed WIN32_LEAN_AND_MEAN as it's for Win32 */
#include <windows.h>
#include <windowsx.h> /* For GET_WM_COMMAND_ID */
#include <stdio.h>    /* For sprintf (optional, mainly for debugging or error messages) */
#include <stdlib.h>   /* For atoi */
#include <string.h>   /* For memset, strtok, lstrcpy, lstrcat, lstrlen */
#include <commdlg.h>  /* For OpenFileName - Note: CommDlg.dll needed for Win3.1 */
#include <dlgs.h>     /* Include this for dialog styles like DS_MODALFRAME, DS_3DLOOK */
/* Removed commctrl.h - Common Controls v6 not available in Win3.1 */
/* Removed winreg.h - Registry functions not available in Win3.1 */

/* Define Control IDs */
#define IDC_STATIC_CODE     101
#define IDC_EDIT_CODE       102
#define IDC_STATIC_INPUT    103
#define IDC_EDIT_INPUT      104
#define IDC_STATIC_OUTPUT   105
#define IDC_EDIT_OUTPUT     106

/* Define Menu IDs */
#define IDM_FILE_NEW        200
#define IDM_FILE_RUN        201
#define IDM_FILE_COPYOUTPUT 202
#define IDM_FILE_EXIT       203
#define IDM_FILE_OPEN       204
#define IDM_FILE_SETTINGS   205
#define IDM_FILE_CLEAROUTPUT 206
#define IDM_HELP_ABOUT      207

/* Define Edit Menu IDs */
#define IDM_EDIT_CUT        210
#define IDM_EDIT_COPY       211
#define IDM_EDIT_PASTE      212
#define IDM_EDIT_SELECTALL  213

/* Accelerator IDs */
#define IDA_FILE_NEW         404
#define IDA_HELP_ABOUT       405
#define IDA_EDIT_CUT         410
#define IDA_EDIT_COPY        411
#define IDA_EDIT_PASTE       412
#define IDA_EDIT_SELECTALL   413
#define IDA_FILE_COPYOUTPUT  414


/* Define Dialog Control IDs for Settings Dialog */
#define IDC_CHECK_DEBUG_INTERPRETER 301
#define IDC_CHECK_DEBUG_OUTPUT      302
#define IDC_CHECK_DEBUG_BASIC       303
/* IDOK and IDCANCEL are predefined as 1 and 2 */

/* Define Dialog Control IDs for About Dialog */
#define IDC_STATIC_ABOUT_TEXT 601


/* Removed custom messages for thread communication */

/* --- Constants (ANSI versions) --- */
#define WINDOW_TITLE_ANSI        "BF Interpreter (Win16)" /* Updated title */
#define STRING_CODE_HELP_ANSI    "Code:"
#define STRING_INPUT_HELP_ANSI   "Standard input:"
#define STRING_OUTPUT_HELP_ANSI  "Standard output:"
#define STRING_ACTION_NEW_ANSI   "&New\tCtrl+N"
#define STRING_ACTION_RUN_ANSI   "&Run\tCtrl+R"
#define STRING_ACTION_COPY_ANSI  "&Copy Output\tCtrl+Shift+C"
#define STRING_ACTION_EXIT_ANSI  "E&xit\tCtrl+X"
#define STRING_ACTION_OPEN_ANSI  "&Open...\tCtrl+O"
#define STRING_ACTION_SETTINGS_ANSI "&Settings..."
#define STRING_ACTION_CLEAROUTPUT_ANSI "C&lear Output"
#define STRING_FILE_MENU_ANSI    "&File"
#define STRING_HELP_MENU_ANSI    "&Help"
#define STRING_ACTION_ABOUT_ANSI "&About\tCtrl+F1"
#define STRING_CODE_TEXT_ANSI    ",[>,]<[.<]" /* Default code */
#define STRING_INPUT_TEXT_ANSI   "This is the default stdin-text" /* Default input */
#define STRING_COPIED_ANSI       "Output copied to clipboard!"
#define STRING_CRASH_PREFIX_ANSI "\r\n\r\n*** Program crashed with Exception:\r\n" /* \r\n for Windows newlines */
#define STRING_MISMATCHED_BRACKETS_ANSI "Mismatched brackets.\r\n"
/* Removed STRING_THREAD_ERROR_ANSI */
#define STRING_MEM_ERROR_CODE_ANSI "Error: Memory allocation failed for code.\r\n"
#define STRING_MEM_ERROR_INPUT_ANSI "Error: Memory allocation failed for input.\r\n"
/* Removed STRING_MEM_ERROR_PARAMS_ANSI */
#define STRING_MEM_ERROR_OPTIMIZE_ANSI "Memory allocation failed for optimized code.\r\n"
#define STRING_CLIPBOARD_OPEN_ERROR_ANSI "Failed to open clipboard."
#define STRING_CLIPBOARD_MEM_ALLOC_ERROR_ANSI "Failed to allocate memory for clipboard."
#define STRING_CLIPBOARD_MEM_LOCK_ERROR_ANSI "Failed to lock memory for clipboard."
#define STRING_FONT_ERROR_ANSI "Failed to create monospaced font."
#define STRING_WINDOW_REG_ERROR_ANSI "Window Registration Failed!"
#define STRING_WINDOW_CREATION_ERROR_ANSI "Window Creation Failed!"
#define STRING_CONFIRM_EXIT_ANSI "Confirm Exit"
#define STRING_REALLY_QUIT_ANSI "Really quit?"
#define STRING_OPEN_FILE_TITLE_ANSI "Open Brainfuck Source File"
#define STRING_SETTINGS_TITLE_ANSI "Interpreter Settings"
#define STRING_DEBUG_INTERPRETER_ANSI "Enable interpreter instruction debug messages"
#define STRING_DEBUG_OUTPUT_ANSI "Enable interpreter output message debug messages"
#define STRING_DEBUG_BASIC_ANSI "Enable basic debug messages"
#define STRING_OK_ANSI "OK"
#define STRING_CANCEL_ANSI "Cancel"
#define SETTINGS_DIALOG_CLASS_NAME_ANSI "SettingsDialogClass"
#define STRING_ABOUT_TITLE_ANSI "About BF Interpreter (Win16)" /* Updated title */
#define STRING_ABOUT_TEXT_ANSI "BF Interpreter (Win16)\r\n\r\nVersion 1.0\r\nCopyright 2015-2025 Kirn Gill II <segin2005@gmail.com>\r\n\r\nSimple interpreter for Windows 3.1." /* Updated text */
#define ABOUT_DIALOG_CLASS_NAME_ANSI "AboutDialogClass"

/* New Edit Menu Strings */
#define STRING_EDIT_MENU_ANSI    "&Edit"
#define STRING_ACTION_CUT_ANSI   "Cu&t\tCtrl+X"
#define STRING_ACTION_COPY_ANSI_EDIT "&Cop&y\tCtrl+C"
#define STRING_ACTION_PASTE_ANSI "Pas&te\tCtrl+V"
#define STRING_ACTION_SELECTALL_ANSI "Select &All\tCtrl+A"


/* Removed Registry Constants */

/* INI File Constants for Win3.1 settings */
/* Using a dedicated INI file is generally better than WIN.INI */
#define INI_FILENAME_ANSI "bf_interpreter.ini"
#define INI_SECTION_DEBUG_ANSI "DebugSettings"
#define INI_KEY_DEBUG_BASIC_ANSI "DebugBasic"
#define INI_KEY_DEBUG_INTERPRETER_ANSI "DebugInterpreter"
#define INI_KEY_DEBUG_OUTPUT_ANSI "DebugOutput"


#define TAPE_SIZE           65536 /* Equivalent to 0x10000 in Java Tape.java */
#define OUTPUT_BUFFER_SIZE  1024  /* Size of the output buffer */

/* Global variables */
HINSTANCE hInst;
HFONT hMonoFont = NULL;
HFONT hLabelFont = NULL; /* Handle for the label font */
HWND hwndCodeEdit = NULL;
HWND hwndInputEdit = NULL;
HWND hwndOutputEdit = NULL;
/* Removed g_hInterpreterThread and g_bInterpreterRunning */
HACCEL hAccelTable; /* Handle to the accelerator table */

/* Global debug settings flags */
BOOL g_bDebugInterpreter = FALSE; /* Default to FALSE */
BOOL g_bDebugOutput = FALSE;      /* Default to FALSE */
BOOL g_bDebugBasic = TRUE;       /* Default to TRUE */

/* Forward declaration of the main window procedure */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/* Forward declaration of the settings modal dialog procedure */
LRESULT CALLBACK SettingsModalDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/* Forward declaration of the function to show the settings modal dialog */
void ShowModalSettingsDialog(HWND hwndParent);
/* Forward declaration of the about modal dialog procedure */
LRESULT CALLBACK AboutModalDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/* Forward declaration of the function to show the about modal dialog */
void ShowModalAboutDialog(HWND hwndParent);


/* Forward declarations for INI functions */
void SaveDebugSettingsToIni(void); /* Added void */
void LoadDebugSettingsFromIni(void); /* Added void */
char* ReadFileContentWin16(const char* filename);


/* Helper function to append text to an EDIT control (Defined before use) */
void AppendText(HWND hwndEdit, const char* newText) {
    /* Get the current length of text in the edit control */
    int len = GetWindowTextLength(hwndEdit); /* Use non-A version */
    /* Set the selection to the end of the text */
    SendMessage(hwndEdit, EM_SETSEL, (WPARAM)len, (LPARAM)len); /* Use non-A version */
    /* Replace the selection (which is empty and at the end) with the new text */
    SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)newText); /* Use non-A version */
}

/* Helper function for conditional debug output */
/* OutputDebugString is generally available in Win16, keep for now */
void DebugPrint(const char* format, ...) {
    char buffer[512]; /* Adjust size as needed */
    va_list args;

    if (!g_bDebugBasic) return; /* If basic debugging is off, print nothing */

    va_start(args, format);
    vsprintf(buffer, format, args); /* Use vsprintf */
    va_end(args);
    OutputDebugString(buffer); /* Use non-A version */
}

void DebugPrintInterpreter(const char* format, ...) {
    char buffer[512]; /* Adjust size as needed */
    va_list args;

    if (!g_bDebugBasic || !g_bDebugInterpreter) return; /* If basic or interpreter debugging is off, print nothing */

    va_start(args, format);
    vsprintf(buffer, format, args); /* Use vsprintf */
    va_end(args);
    OutputDebugString(buffer); /* Use non-A version */
}

void DebugPrintOutput(const char* format, ...) {
    char buffer[512]; /* Adjust size as needed */
    va_list args;

    if (!g_bDebugBasic || !g_bDebugOutput) return; /* If basic or output debugging is off, print nothing */

    va_start(args, format);
    vsprintf(buffer, format, args); /* Use vsprintf */
    va_end(args);
    OutputDebugString(buffer); /* Use non-A version */
}


/* --- Brainfuck Tape Structure and Functions (Translation of Tape.java) --- */
typedef struct {
    unsigned char tape[TAPE_SIZE];
    int position;
} Tape;

void Tape_init(Tape* tape) {
    memset(tape->tape, 0, TAPE_SIZE);
    tape->position = 0;
}

unsigned char Tape_get(Tape* tape) {
    return tape->tape[tape->position];
}

void Tape_set(Tape* tape, unsigned char value) {
    tape->tape[tape->position] = value;
}

void Tape_inc(Tape* tape) {
    tape->tape[tape->position]++; /* Unsigned char wraps around automatically 0-255 */
}

void Tape_dec(Tape* tape) {
    tape->tape[tape->position]--; /* Unsigned char wraps around automatically 0-255 */
}

void Tape_forward(Tape* tape) {
    tape->position++;
    if (tape->position >= TAPE_SIZE) {
        tape->position = 0; /* Wrap around */
    }
}

void Tape_reverse(Tape* tape) {
    tape->position--;
    if (tape->position < 0) {
        tape->position = TAPE_SIZE - 1; /* Wrap around */
    }
}


/* Function to filter Brainfuck code (Translation of Interpreter.java optimize method) */
/* Allocates a new string, caller must free (using char* for ANSI) */
char* optimize_code(const char* code) {
    int len = lstrlen(code); /* Use lstrlen */
    char* ocode = (char*)GlobalAlloc(GPTR, (len + 1) * sizeof(char)); /* Use GlobalAlloc */
    int i; /* C89: Declare variables at the beginning of the block */
    int ocode_len = 0;

    if (!ocode) return NULL;

    for (i = 0; i < len; i++) {
        switch (code[i]) {
            case '>':
            case '<':
            case '+':
            case '-':
            case ',':
            case '.':
            case '[':
            case ']':
                ocode[ocode_len++] = code[i];
                break;
            default:
                /* Ignore other characters */
                break;
        }
    }
    ocode[ocode_len] = '\0';
    return ocode;
}


/* --- Settings Modal Dialog Procedure --- */
LRESULT CALLBACK SettingsModalDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    /* Store handles to the checkboxes for easy access */
    static HWND hCheckBasic = NULL;
    static HWND hCheckInterpreter = NULL;
    static HWND hCheckOutput = NULL;

    switch (uMsg) {
        case WM_CREATE:
        { /* Added braces to limit the scope of variables */
            HFONT hGuiFont;
            HDC hdc;
            HFONT hOldFont;
            TEXTMETRIC tm;
            int fontHeight;
            int checkHeight;
            const char* texts[3];
            int max_text_width = 0;
            SIZE size;
            int i; /* C89: Declare variables at the beginning of the block */
            int checkbox_control_width;
            const int button_width = 75; /* Standard button width */
            const int button_height = 25; /* Standard button height */
            const int margin = 20; /* Margin around control groups (increased) */
            const int checkbox_spacing = 8; /* Vertical space between checkboxes (increased) */
            const int button_spacing = 15; /* Space between last checkbox and buttons (increased) */
            int required_content_width;
            int dlgW;
            int dlgH;
            int yPos;
            HWND hOkButton;
            BOOL bEnableOthers;

            DebugPrint("SettingsModalDialogProc: WM_CREATE received.\n");

            /* Get the system default GUI font for dialog controls */
            hGuiFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            DebugPrint("SettingsModalDialogProc: Obtained DEFAULT_GUI_FONT.\n");


            hdc = GetDC(hwnd);
            hOldFont = (HFONT)SelectObject(hdc, hGuiFont); /* Select the GUI font for measurement */


            /* Calculate font metrics for checkbox height */
            GetTextMetrics(hdc, &tm);
            fontHeight = tm.tmHeight; /* Base height */
            checkHeight = fontHeight + 6; /* Add padding for checkbox height (increased slightly) */

            /* Measure checkbox texts */
            texts[0] = STRING_DEBUG_BASIC_ANSI; /* Basic first */
            texts[1] = STRING_DEBUG_INTERPRETER_ANSI;
            texts[2] = STRING_DEBUG_OUTPUT_ANSI;

            for (i = 0; i < 3; i++) {
                GetTextExtentPoint32(hdc, texts[i], lstrlen(texts[i]), &size); /* Use GetTextExtentPoint32, lstrlen */
                if (size.cx > max_text_width) max_text_width = size.cx; /* Corrected variable name */
            }

            /* Calculate control dimensions */
            checkbox_control_width = max_text_width + GetSystemMetrics(SM_CXMENUCHECK) + GetSystemMetrics(SM_CXEDGE) * 2 + 8 + 15;


            /* Calculate required dialog width: Max of checkbox control width and button width + margins */
            required_content_width = (checkbox_control_width > button_width ? checkbox_control_width : button_width);
            /* Added a small buffer (+15) for safety (increased buffer) */
            dlgW = required_content_width + margin * 2 + 15;

            /* Calculate required dialog height: Top margin + (Number of checkboxes * Checkbox Height) + (Number of spaces between checkboxes * Checkbox Spacing) + Button Spacing + Button Height + Bottom margin */
            /* Using calculated checkHeight */
            dlgH = margin + (3 * checkHeight) + (2 * checkbox_spacing) + button_spacing + button_height + margin;
            /* Added a small buffer (+15) for safety (increased buffer) */
            dlgH += 15;


            /* Position tracking */
            yPos = margin;

            /* Create checkboxes using standard "BUTTON" class */
            /* Set the width and height of the checkboxes based on the calculated values */

            /* Basic Debug Checkbox (First) */
            hCheckBasic = CreateWindow(
                "BUTTON",               /* Class name (Standard Button) */
                STRING_DEBUG_BASIC_ANSI, /* Text */
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP, /* Styles */
                margin, yPos, checkbox_control_width, checkHeight,        /* Position and size (x, y, width, height) */
                hwnd,                   /* Parent window handle */
                (HMENU)IDC_CHECK_DEBUG_BASIC, /* Control ID */
                hInst,                  /* Instance handle */
                NULL                    /* Additional application data */
            );
            /* Apply the GUI font to the checkbox */
            if (hGuiFont && hCheckBasic) {
                SendMessage(hCheckBasic, WM_SETFONT, (WPARAM)hGuiFont, MAKELPARAM(TRUE, 0));
            }
            DebugPrint("SettingsModalDialogProc: Basic debug checkbox created.\n");
            yPos += checkHeight + checkbox_spacing;

            /* Interpreter Debug Checkbox (Second) */
            hCheckInterpreter = CreateWindow(
                "BUTTON",               /* Class name (Standard Button) */
                STRING_DEBUG_INTERPRETER_ANSI, /* Text */
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP, /* Styles */
                margin, yPos, checkbox_control_width, checkHeight,        /* Position and size */
                hwnd,                   /* Parent window handle */
                (HMENU)IDC_CHECK_DEBUG_INTERPRETER, /* Control ID */
                hInst,                  /* Instance handle */
                NULL                    /* Additional application data */
            );
             /* Apply the GUI font to the checkbox */
            if (hGuiFont && hCheckInterpreter) {
                SendMessage(hCheckInterpreter, WM_SETFONT, (WPARAM)hGuiFont, MAKELPARAM(TRUE, 0));
            }
            DebugPrint("SettingsModalDialogProc: Interpreter debug checkbox created.\n");
            yPos += checkHeight + checkbox_spacing;

            /* Output Debug Checkbox (Third) */
            hCheckOutput = CreateWindow(
                "BUTTON",               /* Class name (Standard Button) */
                STRING_DEBUG_OUTPUT_ANSI, /* Text */
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP, /* Styles */
                margin, yPos, checkbox_control_width, checkHeight,        /* Position and size */
                hwnd,                   /* Parent window handle */
                (HMENU)IDC_CHECK_DEBUG_OUTPUT, /* Control ID */
                hInst,                  /* Instance handle */
                NULL                    /* Additional application data */
            );
             /* Apply the GUI font to the checkbox */
            if (hGuiFont && hCheckOutput) {
                SendMessage(hCheckOutput, WM_SETFONT, (WPARAM)hGuiFont, MAKELPARAM(TRUE, 0));
            }
            DebugPrint("SettingsModalDialogProc: Output debug checkbox created.\n");
            yPos += checkHeight + button_spacing; /* Space before the button */

            /* Calculate button position to be centered below checkboxes */
            int ok_button_x = margin + (required_content_width - button_width) / 2;


            /* Create ONLY the OK button using standard "BUTTON" class */
            hOkButton = CreateWindow(
                "BUTTON",               /* Class name (Standard Button) */
                STRING_OK_ANSI,         /* Text */
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP, /* Styles (BS_DEFPUSHBUTTON makes it the default button) */
                ok_button_x, yPos, button_width, button_height,        /* Position and size */
                hwnd,                   /* Parent window handle */
                (HMENU)IDOK,            /* Control ID (predefined) */
                hInst,                  /* Instance handle */
                NULL                    /* Additional application data */
            );
            /* Apply the GUI font to the button */
            if (hGuiFont && hOkButton) {
                SendMessage(hOkButton, WM_SETFONT, (WPARAM)hGuiFont, MAKELPARAM(TRUE, 0));
            }
            DebugPrint("SettingsModalDialogProc: OK button created.\n");


            /* Initialize checkboxes based on current global settings */
            CheckDlgButton(hwnd, IDC_CHECK_DEBUG_BASIC, g_bDebugBasic ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hwnd, IDC_CHECK_DEBUG_INTERPRETER, g_bDebugInterpreter ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hwnd, IDC_CHECK_DEBUG_OUTPUT, g_bDebugOutput ? BST_CHECKED : BST_UNCHECKED);
            DebugPrint("SettingsModalDialogProc: Checkboxes initialized.\n");

            /* Set initial enabled/disabled state of interpreter and output checkboxes */
            bEnableOthers = (g_bDebugBasic == TRUE); /* Use boolean logic */
            EnableWindow(GetDlgItem(hwnd, IDC_CHECK_DEBUG_INTERPRETER), bEnableOthers);
            EnableWindow(GetDlgItem(hwnd, IDC_CHECK_DEBUG_OUTPUT), bEnableOthers);
            DebugPrint("SettingsModalDialogProc: Initial enabled state set for interpreter/output checkboxes.\n");


            /* Resize the dialog window to fit the calculated size */
            SetWindowPos(hwnd, NULL, 0, 0, dlgW, dlgH, SWP_NOMOVE | SWP_NOZORDER);
            DebugPrint("SettingsModalDialogProc: Dialog resized to (%d, %d).\n", dlgW, dlgH);


            SelectObject(hdc, hOldFont); /* Restore original font */
            ReleaseDC(hwnd, hdc);


            break;
        } /* Added closing brace */

        case WM_COMMAND:
        { /* Added braces for scope */
            int wmId = LOWORD(wParam);
            BOOL bBasicChecked; /* C89: Declare variables at the beginning of the block */

            DebugPrint("SettingsModalDialogProc: WM_COMMAND received.\n");
            switch (wmId) {
                case IDOK:
                    DebugPrint("SettingsModalDialogProc: IDOK received.\n");
                    /* Save settings from checkboxes */
                    g_bDebugBasic = IsDlgButtonChecked(hwnd, IDC_CHECK_DEBUG_BASIC) == BST_CHECKED;
                    g_bDebugInterpreter = IsDlgButtonChecked(hwnd, IDC_CHECK_DEBUG_INTERPRETER) == BST_CHECKED;
                    g_bDebugOutput = IsDlgButtonChecked(hwnd, IDC_CHECK_DEBUG_OUTPUT) == BST_CHECKED;

                    /* Apply the rule: if basic is off, all are off */
                    if (!g_bDebugBasic) {
                        g_bDebugInterpreter = FALSE;
                        g_bDebugOutput = FALSE;
                    }
                    DebugPrint("SettingsModalDialogProc: Debug settings saved. Basic: %d, Interpreter: %d, Output: %d\n", g_bDebugBasic, g_bDebugInterpreter, g_bDebugOutput);

                    /* Save settings to INI file */
                    SaveDebugSettingsToIni();
                    DebugPrint("SettingsModalDialogProc: Called SaveDebugSettingsToIni.\n");


                    DestroyWindow(hwnd); /* Close the dialog */
                    break;
                case IDC_CHECK_DEBUG_BASIC:
                    DebugPrint("SettingsModalDialogProc: Basic debug checkbox clicked.\n");
                    /* Get the current state of the basic debug checkbox */
                    bBasicChecked = IsDlgButtonChecked(hwnd, IDC_CHECK_DEBUG_BASIC) == BST_CHECKED;

                    /* Enable or disable the other checkboxes based on the basic checkbox state */
                    EnableWindow(GetDlgItem(hwnd, IDC_CHECK_DEBUG_INTERPRETER), bBasicChecked);
                    EnableWindow(GetDlgItem(hwnd, IDC_CHECK_DEBUG_OUTPUT), bBasicChecked);

                    /* If basic is unchecked, also uncheck the others visually and logically */
                    if (!bBasicChecked) {
                        CheckDlgButton(hwnd, IDC_CHECK_DEBUG_INTERPRETER, BST_UNCHECKED);
                        CheckDlgButton(hwnd, IDC_CHECK_DEBUG_OUTPUT, BST_UNCHECKED);
                        /* The global flags will be updated in IDOK, but this ensures consistency */
                    }
                    break;
                /* No specific handlers needed for other checkboxes unless they have unique logic */
            }
            break; /* End of WM_COMMAND */
        }

        case WM_CLOSE:
            DebugPrint("SettingsModalDialogProc: WM_CLOSE received.\n");
            DestroyWindow(hwnd); /* Treat closing via system menu as closing the dialog */
            break;

        case WM_DESTROY:
            DebugPrint("SettingsModalDialogProc: WM_DESTROY received.\n");
            /* Clear static handles */
            hCheckBasic = NULL;
            hCheckInterpreter = NULL;
            hCheckOutput = NULL;
            /* No specific cleanup needed for controls */
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam); /* Use non-A version */
    }
    return 0;
}

/* --- Function to show the settings modal dialog --- */
void ShowModalSettingsDialog(HWND hwndParent) {
    static BOOL registered = FALSE;
    WNDCLASS wc = {0}; /* Use WNDCLASS for Win3.1 */
    RECT rcParent;
    int max_text_width = 0;
    HDC hdc;
    HFONT hFont;
    HFONT hOldFont;
    SIZE size;
    int i; /* C89: Declare variables at the beginning of the block */
    int checkbox_control_width;
    const int button_width = 75; /* Standard button width */
    const int button_height = 25; /* Standard button height */
    const int margin = 20; /* Margin around control groups (increased) */
    const int checkbox_spacing = 8; /* Vertical space between checkboxes (increased) */
    const int button_spacing = 15; /* Space between last checkbox and buttons (increased) */
    int required_content_width;
    int estimated_checkHeight = 20 + 6; /* Estimate for initial window creation (matching WM_CREATE padding) */
    int dlgW;
    int dlgH;
    int x;
    int y;
    HWND hDlg;
    MSG msg;

    DebugPrint("ShowModalSettingsDialog called.\n");
    /* Disable parent window */
    EnableWindow(hwndParent, FALSE);
    DebugPrint("ShowModalSettingsDialog: Parent window disabled.\n");

    /* Register dialog class once */
    if (!registered) {
        wc.lpfnWndProc     = SettingsModalDialogProc; /* Use the settings modal dialog procedure */
        wc.hInstance       = hInst; /* Use the global instance handle */
        wc.lpszClassName = SETTINGS_DIALOG_CLASS_NAME_ANSI; /* Use the new class name */
        /* Use COLOR_3DFACE for the background brush for a 3D look */
        wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
        wc.hCursor         = LoadCursor(NULL, IDC_ARROW); /* Use non-A version */
        /* No lpszMenuName for a dialog */

        DebugPrint("ShowModalSettingsDialog: Registering settings dialog class.\n");
        if (!RegisterClass(&wc)) /* Use RegisterClass for Win3.1 */
        {
            DebugPrint("ShowModalSettingsDialog: Settings dialog registration failed. GetLastError: %lu\n", GetLastError());
            MessageBox(hwndParent, "Failed to register settings dialog class!", "Error", MB_ICONEXCLAMATION | MB_OK); /* Use non-A version */
            EnableWindow(hwndParent, TRUE); /* Re-enable parent on failure */
            return;
        }
        registered = TRUE;
        DebugPrint("ShowModalSettingsDialog: Settings dialog class registered successfully.\n");
    }

    /* Center over parent */
    GetWindowRect(hwndParent, &rcParent);

    /* --- Calculate required dialog width and height before creating the window --- */
    /* This ensures the window is created with the correct initial size for centering. */
    /* Note: The WM_CREATE handler will also calculate and potentially adjust the size */
    /* based on the font actually used by the dialog, which is more accurate. */

    /* Measure the width of the checkbox texts using the default GUI font */
    hdc = GetDC(NULL); /* Get DC for the screen to measure text before dialog is created */
    hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    hOldFont = (HFONT)SelectObject(hdc, hFont);

    GetTextExtentPoint32(hdc, STRING_DEBUG_INTERPRETER_ANSI, lstrlen(STRING_DEBUG_INTERPRETER_ANSI), &size); /* Use GetTextExtentPoint32, lstrlen */
    if (size.cx > max_text_width) max_text_width = size.cx;
    GetTextExtentPoint32(hdc, STRING_DEBUG_OUTPUT_ANSI, lstrlen(STRING_DEBUG_OUTPUT_ANSI), &size); /* Use GetTextExtentPoint32, lstrlen */
    if (size.cx > max_text_width) max_text_width = size.cx;
    GetTextExtentPoint32(hdc, STRING_DEBUG_BASIC_ANSI, lstrlen(STRING_DEBUG_BASIC_ANSI), &size); /* Use GetTextExtentPoint32, lstrlen */
    if (size.cx > max_text_width) max_text_width = size.cx;

    SelectObject(hdc, hOldFont);
    ReleaseDC(NULL, hdc); /* Release screen DC */

    /* Add padding for the checkbox square, text spacing, and right margin within the control */
    checkbox_control_width = max_text_width + GetSystemMetrics(SM_CXMENUCHECK) + GetSystemMetrics(SM_CXEDGE) * 2 + 8 + 15;


    /* Calculate required dialog width: Max of checkbox control width and button width + margins */
    required_content_width = (checkbox_control_width > button_width ? checkbox_control_width : button_width);
    /* Added a small buffer (+15) for safety (increased buffer) */
    dlgW = required_content_width + margin * 2 + 15;

    /* Calculate required dialog height: Top margin + (Number of checkboxes * Checkbox Height) + (Number of spaces between checkboxes * Checkbox Spacing) + Button Spacing + Button Height + Bottom margin */
    /* Using a standard checkbox height (approx 20) for initial creation, WM_CREATE will refine this. */
    estimated_checkHeight = 20 + 6; /* Estimate for initial window creation (matching WM_CREATE padding) */
    dlgH = margin + (3 * estimated_checkHeight) + (2 * checkbox_spacing) + button_spacing + button_height + margin + 15; /* Increased buffer */


    x = rcParent.left + (rcParent.right - rcParent.left - dlgW) / 2;
    y = rcParent.top + (rcParent.bottom - rcParent.top - dlgH) / 2;
     DebugPrint("ShowModalSettingsDialog: Calculated dialog position (%d, %d) and initial size (%d, %d).\n", x, y, dlgW, dlgH);


    /* Create the modal dialog window */
    hDlg = CreateWindow( /* Use non-A version */
        SETTINGS_DIALOG_CLASS_NAME_ANSI, /* Window class (ANSI) */
        STRING_SETTINGS_TITLE_ANSI, /* Window title (ANSI) */
        WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_3DLOOK, /* Styles for a modal dialog, added DS_3DLOOK */
        x, y, dlgW, dlgH, /* Size and position (using calculated initial size) */
        hwndParent, /* Parent window */
        NULL,       /* Menu */
        hInst,      /* Instance handle */
        NULL        /* Additional application data */
    );
     DebugPrint("ShowModalSettingsDialog: CreateWindow returned %p.\n", hDlg);


    if (!hDlg) {
        DebugPrint("ShowModalSettingsDialog: Failed to create settings dialog window. GetLastError: %lu\n", GetLastError());
        MessageBox(hwndParent, "Failed to create settings dialog window!", "Error", MB_OK | MB_ICONEXCLAMATION | MB_OK); /* Use non-A version */
        EnableWindow(hwndParent, TRUE); /* Re-enable parent on failure */
        return;
    }
    DebugPrint("ShowModalSettingsDialog: Settings dialog window created successfully.\n");

    /* WM_CREATE handler will set the font and resize the window accurately. */


    /* Show and update the dialog */
    ShowWindow(hDlg, SW_SHOW);
    UpdateWindow(hDlg);
    DebugPrint("ShowModalSettingsDialog: Dialog shown and updated.\n");


    /* Modal loop: run until dialog window is destroyed */
    DebugPrint("ShowModalSettingsDialog: Entering modal message loop.\n");
    while (IsWindow(hDlg) && GetMessage(&msg, NULL, 0, 0)) { /* Use non-A version */
        /* Check if the message is for a dialog box. If so, let the dialog handle it. */
        /* IsDialogMessage handles keyboard input for dialog controls (like Tab, Enter, Esc). */
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg); /* Use non-A version */
        }
    }
    DebugPrint("ShowModalSettingsDialog: Exited modal message loop.\n");


    /* Re-enable parent window */
    EnableWindow(hwndParent, TRUE);
    DebugPrint("ShowModalSettingsDialog: Parent window re-enabled.\n");
    /* Set focus back to the parent window */
    SetForegroundWindow(hwndParent);
    DebugPrint("ShowModalSettingsDialog finished.\n");
}

/* --- About Modal Dialog Procedure --- */
LRESULT CALLBACK AboutModalDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HFONT hSansFont = NULL; /* Static to track the font */

    switch (uMsg) {
        case WM_CREATE:
        {
            LOGFONT lf;
            HDC hdc;
            HFONT hOldFont;
            TEXTMETRIC tm;
            int line_height;
            const char *text;
            char *text_copy_handle;
            char *text_copy; /* Pointer to the allocated memory */
            char *token;
            int line_count = 0;
            int max_line_width = 0;
            SIZE line_size;
            const int margin = 25; /* Increased margin */
            const int button_width = 75;
            const int button_height = 25;
            const int button_spacing = 20; /* Increased space between text and button */
            const int bottom_margin = 60; /* Doubled bottom margin */
            int text_height;
            int required_content_width;
            int dlgW;
            int dlgH;
            HWND hStatic;
            int ok_button_x;
            int ok_button_y;

            DebugPrint("AboutModalDialogProc: WM_CREATE received.\n");

            /* Create sans-serif font using system message font */
            /* NONCLIENTMETRICSA is Win32, need an alternative for Win3.1 */
            /* Use SYSTEM_FONT or DEFAULT_GUI_FONT and make it italic manually if possible, */
            /* or just use a standard system font. Let's use SYSTEM_FONT for simplicity now. */
            GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
            lf.lfItalic = TRUE; /* Try to make it italic */
            hSansFont = CreateFontIndirect(&lf); /* Use non-A version */
            DebugPrint("AboutModalDialogProc: Created sans-serif font.\n");


            hdc = GetDC(hwnd);
            /* Explicitly cast the return of SelectObject to HFONT */
            hOldFont = (HFONT)SelectObject(hdc, hSansFont);

            /* Calculate text metrics */
            GetTextMetrics(hdc, &tm);
            line_height = tm.tmHeight;

            /* Split the about text into lines and find max line width */
            text = STRING_ABOUT_TEXT_ANSI;
            /* _strdup is Win32, use GlobalAlloc and lstrcpy */
            text_copy_handle = (char*)GlobalAlloc(GPTR, lstrlen(text) + 1);
            text_copy = text_copy_handle; /* Pointer to the allocated memory */
            if (!text_copy) {
                DebugPrint("Failed to duplicate about text.\n");
                /* Handle error appropriately, maybe show a simple message box */
                SelectObject(hdc, hOldFont);
                ReleaseDC(hwnd, hdc);
                return -1; /* Indicate creation failure */
            }
            lstrcpy(text_copy, text); /* Use lstrcpy */


            /* Use strtok for tokenization */
            token = strtok(text_copy, "\r\n");
            while (token != NULL) {
                GetTextExtentPoint32(hdc, token, lstrlen(token), &line_size); /* Use GetTextExtentPoint32, lstrlen */
                if (line_size.cx > max_line_width) {
                    max_line_width = line_size.cx;
                }
                line_count++;
                token = strtok(NULL, "\r\n");
            }
            GlobalFree(text_copy_handle); /* Free the GlobalAlloc'd memory */

            text_height = line_count * line_height;

            /* Define dimensions and spacing */
            /* Increased margin and button spacing for more vertical room */

            /* Calculate dialog width and height */
            required_content_width = max_line_width > button_width ? max_line_width : button_width;
            /* Add padding around the content */
            dlgW = required_content_width + 2 * margin;
            /* Increased total height by adding the increased bottom margin */
            dlgH = margin + text_height + button_spacing + button_height + bottom_margin;

            /* Create Static Text control using standard "STATIC" class */
            hStatic = CreateWindow( /* Use non-A version */
                "STATIC", /* Use Standard Static class */
                (LPCSTR)STRING_ABOUT_TEXT_ANSI, /* Explicitly cast to LPCSTR */
                WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX, /* SS_LEFT for left justification, SS_NOPREFIX prevents & accelerators */
                margin, margin, required_content_width, text_height,
                hwnd,
                (HMENU)IDC_STATIC_ABOUT_TEXT,
                hInst,
                NULL
            );
            if (hSansFont && hStatic) {
                SendMessage(hStatic, WM_SETFONT, (WPARAM)hSansFont, TRUE);
            }
            DebugPrint("AboutModalDialogProc: Static text control created.\n");


            /* Create OK button centered below the text using standard "BUTTON" class */
            int ok_button_x = (dlgW - button_width) / 2;
            int ok_button_y = margin + text_height + button_spacing;
            CreateWindow( /* Use non-A version */
                "BUTTON",               /* Class name (Standard Button) */
                (LPCSTR)STRING_OK_ANSI, /* Explicitly cast to LPCSTR */
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
                ok_button_x, ok_button_y,
                button_width, button_height,
                hwnd,
                (HMENU)IDOK,
                hInst,
                NULL
            );
            DebugPrint("AboutModalDialogProc: OK button created.\n");


            /* Resize the dialog window to fit the calculated size */
            SetWindowPos(hwnd, NULL, 0, 0, dlgW, dlgH, SWP_NOMOVE | SWP_NOZORDER);
            DebugPrint("AboutModalDialogProc: Dialog resized to (%d, %d).\n", dlgW, dlgH);


            SelectObject(hdc, hOldFont);
            ReleaseDC(hwnd, hdc);
            break;
        }

        case WM_COMMAND:
        { /* Added braces for scope */
            int wmId = LOWORD(wParam);
            DebugPrint("AboutModalDialogProc: WM_COMMAND received.\n");
            switch (wmId) {
                case IDOK:
                    DebugPrint("AboutModalDialogProc: IDOK received. Destroying dialog.\n");
                    DestroyWindow(hwnd); /* Close the dialog */
                    break;
            }
            break; /* End of WM_COMMAND */
        }

        case WM_CLOSE:
            DebugPrint("AboutModalDialogProc: WM_CLOSE received.\n");
            DestroyWindow(hwnd); /* Treat closing via system menu as closing the dialog */
            break;

        case WM_DESTROY:
            DebugPrint("AboutModalDialogProc: WM_DESTROY received.\n");
            if (hSansFont) {
                DebugPrint("AboutModalDialogProc: Deleting font object.\n");
                DeleteObject(hSansFont);
                hSansFont = NULL;
            }
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam); /* Use non-A version */
    }
    return 0;
}

/* --- Function to show the about modal dialog --- */
void ShowModalAboutDialog(HWND hwndParent) {
    static BOOL registered = FALSE;
    WNDCLASS wc = {0}; /* Use WNDCLASS for Win3.1 */
    RECT rcParent;
    int dlgW = 300; /* Estimate width */
    int dlgH = 400; /* Increased estimate height */
    int x;
    int y;
    HWND hDlg;
    MSG msg;

    DebugPrint("ShowModalAboutDialog called.\n");
    /* Disable parent window */
    EnableWindow(hwndParent, FALSE);
    DebugPrint("ShowModalAboutDialog: Parent window disabled.\n");

    /* Register dialog class once */
    if (!registered) {
        wc.lpfnWndProc     = AboutModalDialogProc; /* Use the about modal dialog procedure */
        wc.hInstance       = hInst; /* Use the global instance handle */
        wc.lpszClassName = ABOUT_DIALOG_CLASS_NAME_ANSI; /* Use the new class name */
        /* Use COLOR_3DFACE for the background brush for a 3D look */
        wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
        wc.hCursor         = LoadCursor(NULL, IDC_ARROW); /* Use non-A version */
        /* No lpszMenuName for a dialog */

        DebugPrint("ShowModalAboutDialog: Registering about dialog class.\n");
        if (!RegisterClass(&wc)) /* Use RegisterClass for Win3.1 */
        {
            DebugPrint("ShowModalAboutDialog: About dialog registration failed. GetLastError: %lu\n", GetLastError());
            MessageBox(hwndParent, "Failed to register about dialog class!", "Error", MB_ICONEXCLAMATION | MB_OK); /* Use non-A version */
            EnableWindow(hwndParent, TRUE); /* Re-enable parent on failure */
            return;
        }
        registered = TRUE;
        DebugPrint("ShowModalAboutDialog: About dialog class registered successfully.\n");
    }

    /* Center over parent */
    GetWindowRect(hwndParent, &rcParent); /* Pass the address of rcParent */

    /* --- Calculate required dialog width and height before creating the window --- */
    /* This initial size is an estimate. The WM_CREATE handler will calculate the */
    /* precise size based on the actual font used and resize the window. */

    /* Use a reasonable default size for initial creation */

    x = rcParent.left + (rcParent.right - rcParent.left - dlgW) / 2;
    y = rcParent.top + (rcParent.bottom - rcParent.top - dlgH) / 2;
     DebugPrint("ShowModalAboutDialog: Calculated dialog position (%d, %d) and initial size (%d, %d).\n", x, y, dlgW, dlgH);


    /* Create the modal dialog window */
    hDlg = CreateWindow( /* Use non-A version */
        ABOUT_DIALOG_CLASS_NAME_ANSI, /* Window class (ANSI) */
        STRING_ABOUT_TITLE_ANSI, /* Window title (ANSI) */
        WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_3DLOOK, /* Styles for a modal dialog, added DS_3DLOOK */
        x, y, dlgW, dlgH, /* Size and position (using calculated initial size) */
        hwndParent, /* Parent window */
        NULL,       /* Menu */
        hInst,      /* Instance handle */
        NULL        /* Additional application data */
    );
     DebugPrint("ShowModalAboutDialog: CreateWindow returned %p.\n", hDlg);


    if (!hDlg) {
        DebugPrint("ShowModalAboutDialog: Failed to create about dialog window. GetLastError: %lu\n", GetLastError());
        MessageBox(hwndParent, "Failed to create about dialog!", "Error", MB_ICONEXCLAMATION | MB_OK); /* Use non-A version */
        EnableWindow(hwndParent, TRUE); /* Re-enable parent on failure */
        return;
    }
    DebugPrint("ShowModalAboutDialog: About dialog window created successfully.\n");

    /* WM_CREATE handler will set the font and resize the window accurately. */


    /* Show and update the dialog */
    ShowWindow(hDlg, SW_SHOW);
    UpdateWindow(hDlg);
    DebugPrint("ShowModalAboutDialog: Dialog shown and updated.\n");


    /* Modal loop: run until dialog window is destroyed */
    DebugPrint("ShowModalAboutDialog: Entering modal message loop.\n");
    while (IsWindow(hDlg) && GetMessage(&msg, NULL, 0, 0)) { /* Use non-A version */
        /* Check if the message is for a dialog box. If so, let the dialog handle it. */
        /* IsDialogMessage handles keyboard input for dialog controls (like Tab, Enter, Esc). */
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg); /* Use non-A version */
        }
    }
    DebugPrint("ShowModalAboutDialog: Exited message loop.\n");


    /* Re-enable parent window */
    EnableWindow(hwndParent, TRUE);
    DebugPrint("ShowModalAboutDialog: Parent window re-enabled.\n");
    /* Set focus back to the parent window */
    SetForegroundWindow(hwndParent);
    DebugPrint("ShowModalAboutDialog finished.\n");
}


/* --- INI File Functions (for Win3.1 settings) --- */

/* Function to save debug settings to the INI file */
void SaveDebugSettingsToIni(void) { /* Added void */
    char szValue[16];

    DebugPrint("SaveDebugSettingsToIni: Saving settings to INI.\n");

    /* Write DebugBasic */
    sprintf(szValue, "%d", g_bDebugBasic ? 1 : 0);
    WritePrivateProfileString(INI_SECTION_DEBUG_ANSI, INI_KEY_DEBUG_BASIC_ANSI, szValue, INI_FILENAME_ANSI);
    DebugPrint("SaveDebugSettingsToIni: DebugBasic saved as %s.\n", szValue);

    /* Write DebugInterpreter */
    sprintf(szValue, "%d", g_bDebugInterpreter ? 1 : 0);
    WritePrivateProfileString(INI_SECTION_DEBUG_ANSI, INI_KEY_DEBUG_INTERPRETER_ANSI, szValue, INI_FILENAME_ANSI);
    DebugPrint("SaveDebugSettingsToIni: DebugInterpreter saved as %s.\n", szValue);

    /* Write DebugOutput */
    sprintf(szValue, "%d", g_bDebugOutput ? 1 : 0);
    WritePrivateProfileString(INI_SECTION_DEBUG_ANSI, INI_KEY_DEBUG_OUTPUT_ANSI, szValue, INI_FILENAME_ANSI);
    DebugPrint("SaveDebugSettingsToIni: DebugOutput saved as %s.\n", szValue);

    DebugPrint("SaveDebugSettingsToIni: Settings saved.\n");
}

/* Function to load debug settings from the INI file */
void LoadDebugSettingsFromIni(void) { /* Added void */
    char szValue[16];

    DebugPrint("LoadDebugSettingsFromIni: Loading settings from INI.\n");

    /* Read DebugBasic */
    GetPrivateProfileString(INI_SECTION_DEBUG_ANSI, INI_KEY_DEBUG_BASIC_ANSI, "1", szValue, sizeof(szValue), INI_FILENAME_ANSI); /* Default to "1" (TRUE) */
    g_bDebugBasic = (atoi(szValue) != 0);
    DebugPrint("LoadDebugSettingsFromIni: Read DebugBasic as %d.\n", g_bDebugBasic);

    /* Read DebugInterpreter */
    GetPrivateProfileString(INI_SECTION_DEBUG_ANSI, INI_KEY_DEBUG_INTERPRETER_ANSI, "0", szValue, sizeof(szValue), INI_FILENAME_ANSI); /* Default to "0" (FALSE) */
    g_bDebugInterpreter = (atoi(szValue) != 0);
    DebugPrint("LoadDebugSettingsFromIni: Read DebugInterpreter as %d.\n", g_bDebugInterpreter);

    /* Read DebugOutput */
    GetPrivateProfileString(INI_SECTION_DEBUG_ANSI, INI_KEY_DEBUG_OUTPUT_ANSI, "0", szValue, sizeof(szValue), INI_FILENAME_ANSI); /* Default to "0" (FALSE) */
    g_bDebugOutput = (atoi(szValue) != 0);
    DebugPrint("LoadDebugSettingsFromIni: Read DebugOutput as %d.\n", g_bDebugOutput);

    /* Apply the rule: if basic is off, all are off (in case INI had inconsistent settings) */
    if (!g_bDebugBasic) {
        g_bDebugInterpreter = FALSE;
        g_bDebugOutput = FALSE;
        DebugPrint("LoadDebugSettingsFromIni: Basic debug is off, forcing Interpreter and Output debug off.\n");
    }

    DebugPrint("LoadDebugSettingsFromIni: Settings loaded.\n");
}


/* --- File I/O Functions (for Win3.1) --- */

/* Function to read file content using Win3.1 APIs */
char* ReadFileContentWin16(const char* filename) {
    int hFile = _lopen(filename, OF_READ); /* Use _lopen */
    LONG fileSize; /* C89: Declare variables at the beginning of the block */
    char* fileContent; /* Use GlobalAlloc */
    UINT bytesRead;

    if (hFile == HFILE_ERROR) {
        DebugPrint("ReadFileContentWin16: Failed to open file %s. Error: %d\n", filename, hFile);
        return NULL;
    }

    /* Get file size (approximate for text files, might be inaccurate for binary) */
    /* Win3.1 doesn't have a simple GetFileSize equivalent for _lopen handles. */
    /* A common approach is to seek to the end and get the position. */
    fileSize = _llseek(hFile, 0, 2); /* Seek to end */
    _llseek(hFile, 0, 0); /* Seek back to beginning */

    if (fileSize == -1L) { /* _llseek returns -1L on error */
         DebugPrint("ReadFileContentWin16: Failed to get file size for %s.\n", filename);
         _lclose(hFile);
         return NULL;
    }

    /* Allocate global memory for the file content */
    fileContent = (char*)GlobalAlloc(GPTR, fileSize + 1); /* Use GlobalAlloc */
    if (!fileContent) {
        DebugPrint("ReadFileContentWin16: Memory allocation failed for file content.\n");
        _lclose(hFile);
        return NULL;
    }

    /* Read the file content */
    bytesRead = _lread(hFile, fileContent, fileSize); /* Use _lread */

    _lclose(hFile); /* Use _lclose */

    if (bytesRead != fileSize) {
        DebugPrint("ReadFileContentWin16: Warning: Read %u bytes, expected %lu for %s.\n", bytesRead, fileSize, filename);
        /* In a real app, you might handle this as an error or truncate. */
        /* For now, we'll proceed with the read bytes. */
    }

    fileContent[bytesRead] = '\0'; /* Null-terminate the content */

    DebugPrint("ReadFileContentWin16: Successfully read %u bytes from %s.\n", bytesRead, filename);
    return fileContent;
}


/* --- Window Procedure --- */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            LOGFONT lf;
            HMENU hMenubar;
            HMENU hMenuFile;
            HMENU hMenuEdit; /* Create Edit menu */
            HMENU hMenuHelp; /* Create Help menu */
            HWND hStaticCode;
            HWND hStaticInput;
            HWND hStaticOutput;

            DebugPrint("WM_CREATE received.\n");
            /* --- Create Monospaced Font --- */
            /* This font is specifically for the EDIT controls (code, input, output) */
            hMonoFont = CreateFont( /* Use non-A version */
                16,                     /* Height */
                0,                      /* Width (auto) */
                0,                      /* Escapement */
                0,                      /* Orientation */
                FW_NORMAL,              /* Weight */
                FALSE,                  /* Italic */
                FALSE,                  /* Underline */
                FALSE,                  /* Strikeout */
                ANSI_CHARSET,           /* Charset */
                OUT_DEFAULT_PRECIS,     /* Output Precision */
                CLIP_DEFAULT_PRECIS,    /* Clipping Precision */
                DEFAULT_QUALITY,        /* Quality */
                FIXED_PITCH | FF_MODERN,/* Pitch and Family (FIXED_PITCH is key for mono) */
                "Courier New");         /* Font name (ANSI) */

            if (hMonoFont == NULL) {
                 MessageBox(hwnd, STRING_FONT_ERROR_ANSI, "Font Error", MB_OK | MB_ICONWARNING); /* Use non-A version */
            }

            /* --- Create Label Font (System Sans-Serif, Italic) --- */
            /* NONCLIENTMETRICSA is Win32, need an alternative for Win3.1 */
            /* Use SYSTEM_FONT or DEFAULT_GUI_FONT and make it italic manually if possible, */
            /* or just use a standard system font. Let's use SYSTEM_FONT for simplicity now. */
            GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
            lf.lfItalic = TRUE; /* Try to make it italic */
            hLabelFont = CreateFontIndirect(&lf); /* Use non-A version */
            DebugPrint("WM_CREATE: Created italic label font.\n");


            /* --- Create Menu --- */
            hMenubar = CreateMenu();
            hMenuFile = CreateMenu();
            hMenuEdit = CreateMenu(); /* Create Edit menu */
            hMenuHelp = CreateMenu(); /* Create Help menu */

            /* File Menu (Use AppendMenu for Win3.1) */
            AppendMenu(hMenuFile, MF_STRING, IDM_FILE_NEW, STRING_ACTION_NEW_ANSI);
            AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenuFile, MF_STRING, IDM_FILE_OPEN, STRING_ACTION_OPEN_ANSI);
            AppendMenu(hMenuFile, MF_STRING, IDM_FILE_RUN, STRING_ACTION_RUN_ANSI);
            AppendMenu(hMenuFile, MF_STRING, IDM_FILE_COPYOUTPUT, STRING_ACTION_COPY_ANSI);
            AppendMenu(hMenuFile, MF_STRING, IDM_FILE_CLEAROUTPUT, STRING_ACTION_CLEAROUTPUT_ANSI);
            AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenuFile, MF_STRING, IDM_FILE_SETTINGS, STRING_ACTION_SETTINGS_ANSI);
            AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenuFile, MF_STRING, IDM_FILE_EXIT, STRING_ACTION_EXIT_ANSI);

            /* Edit Menu (Use AppendMenu for Win3.1) */
            AppendMenu(hMenuEdit, MF_STRING, IDM_EDIT_CUT, STRING_ACTION_CUT_ANSI);
            AppendMenu(hMenuEdit, MF_STRING, IDM_EDIT_COPY, STRING_ACTION_COPY_ANSI_EDIT);
            AppendMenu(hMenuEdit, MF_STRING, IDM_EDIT_PASTE, STRING_ACTION_PASTE_ANSI);
            AppendMenu(hMenuEdit, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenuEdit, MF_STRING, IDM_EDIT_SELECTALL, STRING_ACTION_SELECTALL_ANSI);


            /* Help Menu (Use AppendMenu for Win3.1) */
            AppendMenu(hMenuHelp, MF_STRING, IDM_HELP_ABOUT, STRING_ACTION_ABOUT_ANSI);

            /* Append menus to menubar (Use AppendMenu for Win3.1) */
            AppendMenu(hMenubar, MF_POPUP, (UINT)hMenuFile, STRING_FILE_MENU_ANSI); /* Cast HMENU to UINT */
            AppendMenu(hMenubar, MF_POPUP, (UINT)hMenuEdit, STRING_EDIT_MENU_ANSI); /* Cast HMENU to UINT */
            AppendMenu(hMenubar, MF_POPUP, (UINT)hMenuHelp, STRING_HELP_MENU_ANSI); /* Cast HMENU to UINT */


            SetMenu(hwnd, hMenubar);

            /* --- Create Controls --- */
            /* Labels (STATIC) - Use standard "STATIC" class and apply the label font */
            hStaticCode = CreateWindow("STATIC", STRING_CODE_HELP_ANSI, WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, /* Use non-A version */
                          10, 10, 100, 20, hwnd, (HMENU)IDC_STATIC_CODE, hInst, NULL);
            if (hLabelFont && hStaticCode) {
                SendMessage(hStaticCode, WM_SETFONT, (WPARAM)hLabelFont, MAKELPARAM(TRUE, 0)); /* Use non-A version */
            }

            hStaticInput = CreateWindow("STATIC", STRING_INPUT_HELP_ANSI, WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, /* Use non-A version */
                          10, 170, 150, 20, hwnd, (HMENU)IDC_STATIC_INPUT, hInst, NULL);
             if (hLabelFont && hStaticInput) {
                SendMessage(hStaticInput, WM_SETFONT, (WPARAM)hLabelFont, MAKELPARAM(TRUE, 0)); /* Use non-A version */
            }

            hStaticOutput = CreateWindow("STATIC", STRING_OUTPUT_HELP_ANSI, WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, /* Use non-A version */
                          10, 300, 150, 20, hwnd, (HMENU)IDC_STATIC_OUTPUT, hInst, NULL);
             if (hLabelFont && hStaticOutput) {
                SendMessage(hStaticOutput, WM_SETFONT, (WPARAM)hLabelFont, MAKELPARAM(TRUE, 0)); /* Use non-A version */
            }


            /* Edit Controls (EDIT) - Use standard "EDIT" class and ensure proper styles */
            /* Code Input - Apply monospaced font */
            hwndCodeEdit = CreateWindowEx( /* Use non-A version */
                WS_EX_CLIENTEDGE, "EDIT", "", /* Use standard "EDIT", WS_EX_CLIENTEDGE provides the border */
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_TABSTOP,
                10, 35, 560, 125, hwnd, (HMENU)IDC_EDIT_CODE, hInst, NULL);
            if (hMonoFont) {
                SendMessage(hwndCodeEdit, WM_SETFONT, (WPARAM)hMonoFont, MAKELPARAM(TRUE, 0)); /* Use non-A version */
            }


            /* Standard Input - Apply monospaced font */
            hwndInputEdit = CreateWindowEx( /* Use non-A version */
                WS_EX_CLIENTEDGE, "EDIT", "", /* Use standard "EDIT", WS_EX_CLIENTEDGE provides the border */
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_TABSTOP,
                10, 195, 560, 95, hwnd, (HMENU)IDC_EDIT_INPUT, hInst, NULL);
            if (hMonoFont) {
                SendMessage(hwndInputEdit, WM_SETFONT, (WPARAM)hMonoFont, MAKELPARAM(TRUE, 0)); /* Use non-A version */
            }

            /* Standard Output (Edit control - NOW READ-WRITE) - Apply monospaced font */
            hwndOutputEdit = CreateWindowEx( /* Use non-A version */
                WS_EX_CLIENTEDGE, "EDIT", "", /* Use standard "EDIT", WS_EX_CLIENTEDGE provides the border */
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                10, 325, 560, 150, hwnd, (HMENU)IDC_EDIT_OUTPUT, hInst, NULL);
            if (hMonoFont) {
                SendMessage(hwndOutputEdit, WM_SETFONT, (WPARAM)hMonoFont, MAKELPARAM(TRUE, 0)); /* Apply to edit control, use non-A version */
            }


            /* --- Debug Print: Check the class name of the created output control --- */
            char class_name[256];
            GetClassName(hwndOutputEdit, class_name, sizeof(class_name)); /* Use non-A version */
            DebugPrint("WM_CREATE: Created output control with handle %p and class name '%s'.\n", (void*)hwndOutputEdit, class_name);


            /* Set initial text (ANSI) */
            SetWindowText(hwndCodeEdit, STRING_CODE_TEXT_ANSI); /* Use non-A version */
            SetWindowText(hwndInputEdit, STRING_INPUT_TEXT_ANSI); /* Use non-A version */
            SetWindowText(hwndOutputEdit, ""); /* Set text for the edit control, use non-A version */

            /* The monospaced font is applied to the edit controls above. */
            /* The label font is applied to the static controls above. */

            SetFocus(hwndCodeEdit);
            break; /* End of WM_CREATE */
        }

        case WM_SIZE:
        {
            /* Handle window resizing - adjust control positions and sizes */
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            int margin = 10;
            int labelHeight = 20;
            int editTopMargin = 5;
            int spacing = 10;
            int minimumEditHeight = 30; /* Minimum height for any edit box */


            int currentY = margin;

            /* Code Label & Edit */
            MoveWindow(GetDlgItem(hwnd, IDC_STATIC_CODE), margin, currentY, width - 2 * margin, labelHeight, TRUE);
            currentY += labelHeight + editTopMargin;
            int codeEditHeight = height / 4; /* Approximate height */
            if (codeEditHeight < minimumEditHeight) codeEditHeight = minimumEditHeight; /* Ensure minimum */
            MoveWindow(hwndCodeEdit, margin, currentY, width - 2 * margin, codeEditHeight, TRUE);
            currentY += codeEditHeight + spacing;

            /* Input Label & Edit */
            MoveWindow(GetDlgItem(hwnd, IDC_STATIC_INPUT), margin, currentY, width - 2 * margin, labelHeight, TRUE);
            currentY += labelHeight + editTopMargin;
            int inputEditHeight = height / 6; /* Approximate height */
            if (inputEditHeight < minimumEditHeight) inputEditHeight = minimumEditHeight; /* Ensure minimum */
            MoveWindow(hwndInputEdit, margin, currentY, width - 2 * margin, inputEditHeight, TRUE);
            currentY += inputEditHeight + spacing;

            /* Output Label & Edit */
            MoveWindow(GetDlgItem(hwnd, IDC_STATIC_OUTPUT), margin, currentY, width - 2 * margin, labelHeight, TRUE);
            currentY += labelHeight + editTopMargin;
            int outputEditHeight = height - currentY - margin; /* Remaining space */
            if (outputEditHeight < minimumEditHeight) outputEditHeight = minimumEditHeight; /* Ensure minimum */
            MoveWindow(hwndOutputEdit, margin, currentY, width - 2 * margin, outputEditHeight, TRUE); /* Move edit control */


            break; /* End of WM_SIZE */
        }


        case WM_COMMAND:
        {
            DebugPrint("WM_COMMAND received.\n");
            int wmId = LOWORD(wParam);
            /* Parse the menu selections: */
            switch (wmId)
            {
                case IDM_FILE_NEW:
                {
                    DebugPrint("WM_COMMAND: IDM_FILE_NEW received.\n");
                    /* Implement "New" functionality: Clear code and input */
                    SetWindowText(hwndCodeEdit, ""); /* Use non-A version */
                    SetWindowText(hwndInputEdit, ""); /* Use non-A version */
                    SetWindowText(hwndOutputEdit, ""); /* Also clear output, use non-A version */
                    DebugPrint("IDM_FILE_NEW: Code, input, and output cleared.\n");
                    SetFocus(hwndCodeEdit); /* Set focus back to code edit */
                    break;
                }

                case IDM_FILE_OPEN:
                {
                    OPENFILENAME ofn;       /* Structure for the file dialog (Use non-A version) */
                    char szFile[260]; /* Buffer for file name */
                    char* fileContent;

                    DebugPrint("WM_COMMAND: IDM_FILE_OPEN received.\n");

                    /* Initialize OPENFILENAME */
                    memset(&ofn, 0, sizeof(ofn)); /* Use memset */
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    /* Filter for Brainfuck files (*.bf and *.b) and all files */
                    ofn.lpstrFilter = "Brainfuck Source Code (*.bf, *.b)\0*.bf;*.b\0All Files (*.*)\0*.*\0";
                    ofn.nFilterIndex = 1; /* Default to the first filter (Brainfuck files) */
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                    ofn.lpstrTitle = STRING_OPEN_FILE_TITLE_ANSI;

                    szFile[0] = '\0'; /* Initialize buffer */


                    /* Display the Open dialog box. */
                    if (GetOpenFileName(&ofn) == TRUE) /* Use non-A version */
                    {
                        DebugPrint("IDM_FILE_OPEN: File selected.\n");
                        /* User selected a file, now read its content */
                        fileContent = ReadFileContentWin16(ofn.lpstrFile);

                        if (fileContent)
                        {
                            SetWindowText(hwndCodeEdit, fileContent); /* Set code edit text, use non-A version */
                            DebugPrint("IDM_FILE_OPEN: File content loaded into code edit.\n");
                            GlobalFree(fileContent); /* Free the allocated memory */
                        }
                        else
                        {
                            DebugPrint("IDM_FILE_OPEN: Error reading file.\n");
                            MessageBox(hwnd, "Error reading file.", "File Error", MB_OK | MB_ICONERROR); /* Use non-A version */
                        }
                    } else {
                         DebugPrint("IDM_FILE_OPEN: File selection cancelled or failed.\n");
                         /* User cancelled or an error occurred (can check CommDlgExtendedError()) */
                    }
                    break;
                }

                case IDM_FILE_SETTINGS:
                {
                    DebugPrint("WM_COMMAND: IDM_FILE_SETTINGS received. Calling ShowModalSettingsDialog.\n");
                    ShowModalSettingsDialog(hwnd); /* Call the function to show the settings dialog */
                    break; /* Break for IDM_FILE_SETTINGS case */
                }

                case IDM_FILE_RUN:
                { /* Added braces to scope variables */
                    int code_len;
                    char* code;
                    int input_len;
                    char* input;
                    Tape tape;
                    char* ocode;
                    int pc = 0;
                    int ocode_len;
                    int error_status = 0; /* 0 for success, 1 for error */
                    int input_pos = 0; /* Input position */
                    char output_buffer[OUTPUT_BUFFER_SIZE];
                    int output_buffer_pos = 0;
                    int original_buf_len;
                    int converted_buf_size;
                    char* converted_buffer_handle; /* Use GlobalAlloc */
                    char* converted_buffer; /* Pointer to allocated memory */
                    int converted_pos;
                    char last_char;
                    int i; /* C89: Declare variables at the beginning of the block */
                    char current_instruction;

                    DebugPrint("WM_COMMAND: IDM_FILE_RUN received.\n");

                    /* Clear previous output by setting edit control text to empty */
                    DebugPrint("IDM_FILE_RUN: Clearing output edit (handle %p).\n", (void*)hwndOutputEdit);
                    SetWindowText(hwndOutputEdit, ""); /* Use non-A version */

                    /* Get code and input text (ANSI) */
                    code_len = GetWindowTextLength(hwndCodeEdit); /* Use non-A version */
                    code = (char*)GlobalAlloc(GPTR, (code_len + 1) * sizeof(char)); /* Use GlobalAlloc */
                    if (!code) {
                        DebugPrint("IDM_FILE_RUN: Memory allocation failed for code.\n");
                        SetWindowText(hwndOutputEdit, STRING_MEM_ERROR_CODE_ANSI); /* Use non-A version */
                        break;
                    }
                    GetWindowText(hwndCodeEdit, code, code_len + 1); /* Use non-A version */
                    DebugPrint("IDM_FILE_RUN: Code text obtained.\n");

                    input_len = GetWindowTextLength(hwndInputEdit); /* Use non-A version */
                    input = (char*)GlobalAlloc(GPTR, (input_len + 1) * sizeof(char)); /* Use GlobalAlloc */
                     if (!input) {
                        DebugPrint("IDM_FILE_RUN: Memory allocation failed for input.\n");
                        SetWindowText(hwndOutputEdit, STRING_MEM_ERROR_INPUT_ANSI); /* Use non-A version */
                        GlobalFree(code); /* Free previously allocated code */
                        break;
                    }
                    GetWindowText(hwndInputEdit, input, input_len + 1); /* Use non-A version */
                    DebugPrint("IDM_FILE_RUN: Input text obtained.\n");

                    /* --- Interpreter Logic (single-threaded) --- */
                    Tape_init(&tape);

                    ocode = optimize_code(code);
                    if (!ocode) {
                        DebugPrintInterpreter("IDM_FILE_RUN: Failed to optimize code (memory allocation).\n");
                        AppendText(hwndOutputEdit, STRING_MEM_ERROR_OPTIMIZE_ANSI);
                        GlobalFree(code);
                        GlobalFree(input);
                        break;
                    }

                    ocode_len = lstrlen(ocode); /* Use lstrlen */

                    DebugPrintInterpreter("IDM_FILE_RUN: Starting single-threaded interpretation.\n");

                    /* Output buffering */
                    output_buffer_pos = 0;


                    while (pc < ocode_len && error_status == 0) {
                        /* In a single-threaded interpreter, the UI will be blocked */
                        /* during interpretation. For responsiveness, a real Win16 app */
                        /* might yield control periodically using PeekMessage/DispatchMessage */
                        /* or process a limited number of instructions per timer event. */
                        /* For simplicity here, we run to completion. */

                        current_instruction = ocode[pc];
                        DebugPrintInterpreter("PC: %d, Instruction: %c\n", pc, current_instruction);


                        switch (current_instruction) {
                            case '>':
                                Tape_forward(&tape);
                                pc++;
                                break;
                            case '<':
                                Tape_reverse(&tape);
                                pc++;
                                break;
                            case '+':
                                Tape_inc(&tape);
                                pc++;
                                break;
                            case '-':
                                Tape_dec(&tape);
                                pc++;
                                break;
                            case ',':
                                if (input_pos < input_len) {
                                    Tape_set(&tape, (unsigned char)input[input_pos]);
                                    input_pos++;
                                } else {
                                    /* Panu Kalliokoski behavior: input past end of string is 0 */
                                    Tape_set(&tape, 0);
                                }
                                pc++;
                                break;
                            case '.':
                                /* Append character to buffer */
                                if (output_buffer_pos < OUTPUT_BUFFER_SIZE - 1) {
                                    output_buffer[output_buffer_pos++] = Tape_get(&tape);
                                } else {
                                    /* Buffer is full, append to output edit control */
                                    output_buffer[output_buffer_pos] = '\0'; /* Null-terminate */
                                    /* Convert Unix newlines to Windows newlines before appending */
                                    original_buf_len = output_buffer_pos;
                                    converted_buf_size = original_buf_len * 2 + 1;
                                    converted_buffer_handle = (char*)GlobalAlloc(GPTR, converted_buf_size); /* Use GlobalAlloc */
                                    converted_buffer = converted_buffer_handle; /* Pointer to allocated memory */

                                    if (converted_buffer) {
                                        converted_pos = 0;
                                        last_char = '\0';
                                        for (i = 0; i < original_buf_len; ++i) { /* C89: Declare i outside loop */
                                            char current_char = output_buffer[i];
                                            if (current_char == '\n' && last_char != '\r') {
                                                if (converted_pos < converted_buf_size - 2) converted_buffer[converted_pos++] = '\r';
                                            }
                                            if (converted_pos < converted_buf_size - 1) converted_buffer[converted_pos++] = current_char;
                                            last_char = current_char;
                                        }
                                        converted_buffer[converted_pos] = '\0';
                                        AppendText(hwndOutputEdit, converted_buffer);
                                        GlobalFree(converted_buffer_handle); /* Free GlobalAlloc'd memory */
                                    } else {
                                         DebugPrintOutput("IDM_FILE_RUN: Failed to allocate memory for converted output buffer.\n");
                                         AppendText(hwndOutputEdit, "\r\nError: Failed to convert output buffer line endings.\r\n");
                                    }
                                    output_buffer_pos = 0; /* Reset buffer position */
                                }
                                pc++;
                                break;
                            case '[':
                                if (Tape_get(&tape) == 0) {
                                    int bracket_count = 1;
                                    int current_pc = pc;
                                    while (bracket_count > 0 && ++current_pc < ocode_len) {
                                        if (ocode[current_pc] == '[') bracket_count++;
                                        else if (ocode[current_pc] == ']') bracket_count--;
                                    }
                                    if (current_pc >= ocode_len) {
                                         error_status = 1;
                                         DebugPrintInterpreter("IDM_FILE_RUN: Mismatched opening bracket found.\n");
                                    } else {
                                        pc = current_pc + 1;
                                    }
                                } else {
                                    pc++;
                                }
                                break;
                            case ']':
                                if (Tape_get(&tape) != 0) {
                                    int bracket_count = 1;
                                    int current_pc = pc;
                                    while (bracket_count > 0 && --current_pc >= 0) {
                                        if (ocode[current_pc] == ']') bracket_count++;
                                        else if (ocode[current_pc] == '[') bracket_count--;
                                    }
                                     if (current_pc < 0) {
                                         error_status = 1;
                                         DebugPrintInterpreter("IDM_FILE_RUN: Mismatched closing bracket found.\n");
                                     } else {
                                        pc = current_pc;
                                     }
                                } else {
                                    pc++;
                                }
                                break;
                        }
                    }

                    /* Append any remaining buffered output */
                    if (output_buffer_pos > 0) {
                        output_buffer[output_buffer_pos] = '\0'; /* Null-terminate */
                         /* Convert Unix newlines to Windows newlines before appending */
                        original_buf_len = output_buffer_pos;
                        converted_buf_size = original_buf_len * 2 + 1;
                        converted_buffer_handle = (char*)GlobalAlloc(GPTR, converted_buf_size); /* Use GlobalAlloc */
                        converted_buffer = converted_buffer_handle; /* Pointer to allocated memory */

                        if (converted_buffer) {
                            converted_pos = 0;
                            last_char = '\0';
                            for (i = 0; i < original_buf_len; ++i) { /* C89: Declare i outside loop */
                                char current_char = output_buffer[i];
                                if (current_char == '\n' && last_char != '\r') {
                                    if (converted_pos < converted_buf_size - 2) converted_buffer[converted_pos++] = '\r';
                                }
                                if (converted_pos < converted_buf_size - 1) converted_buffer[converted_pos++] = current_char;
                                last_char = current_char;
                            }
                            converted_buffer[converted_pos] = '\0';
                            AppendText(hwndOutputEdit, converted_buffer);
                            GlobalFree(converted_buffer_handle); /* Free GlobalAlloc'd memory */
                        } else {
                             DebugPrintOutput("IDM_FILE_RUN: Failed to allocate memory for final converted output buffer.\n");
                             AppendText(hwndOutputEdit, "\r\nError: Failed to convert final output buffer line endings.\r\n");
                        }
                    }


                    /* Check for mismatched brackets after loop finishes */
                    if (error_status == 1) {
                        AppendText(hwndOutputEdit, STRING_MISMATCHED_BRACKETS_ANSI);
                        DebugPrintInterpreter("IDM_FILE_RUN: Interpretation finished with mismatched brackets.\n");
                    } else {
                         DebugPrintInterpreter("IDM_FILE_RUN: Interpretation finished successfully.\n");
                    }

                    /* Auto-scroll to the bottom after interpretation */
                    SendMessage(hwndOutputEdit, EM_SCROLLCARET, 0, 0); /* Use non-A version */
                    DebugPrintOutput("IDM_FILE_RUN: Scrolled to caret after interpretation.\n");


                    /* Free allocated memory */
                    GlobalFree(ocode); /* Use GlobalFree */
                    GlobalFree(code); /* Use GlobalFree */
                    GlobalFree(input); /* Use GlobalFree */

                    DebugPrintInterpreter("IDM_FILE_RUN: Interpretation finished.\n");

                    break; /* Break for IDM_FILE_RUN case */
                } /* End brace for IDM_FILE_RUN scope */

                case IDM_FILE_COPYOUTPUT:
                { /* Added braces to scope variables */
                    int textLen;
                    HGLOBAL hGlobal;
                    char* pText;

                    DebugPrint("WM_COMMAND: IDM_FILE_COPYOUTPUT received.\n");
                    /* For an edit control, we can get the entire text */
                    textLen = GetWindowTextLength(hwndOutputEdit); /* Use non-A version */
                    if (textLen > 0) {
                         DebugPrint("IDM_FILE_COPYOUTPUT: Output text length > 0.\n");
                        /* Need +1 for null terminator */
                        hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, textLen + 1);
                        if (hGlobal) {
                            DebugPrint("IDM_FILE_COPYOUTPUT: GlobalAlloc succeeded.\n");
                            pText = GlobalLock(hGlobal); /* Use non-A version */
                            if(pText) {
                                DebugPrint("IDM_FILE_COPYOUTPUT: GlobalLock succeeded.\n");
                                /* Use GetWindowText which is safe */
                                GetWindowText(hwndOutputEdit, pText, textLen + 1); /* Use non-A version */
                                GlobalUnlock(hGlobal); /* Use non-A version */
                                DebugPrint("IDM_FILE_COPYOUTPUT: Text copied to global memory.\n");

                                if (OpenClipboard(hwnd)) { /* Use non-A version */
                                    DebugPrint("IDM_FILE_COPYOUTPUT: OpenClipboard succeeded.\n");
                                    EmptyClipboard();
                                    /* Use CF_TEXT for ANSI text */
                                    SetClipboardData(CF_TEXT, hGlobal); /* Use non-A version */
                                    CloseClipboard(); /* Use non-A version */
                                    DebugPrint("IDM_FILE_COPYOUTPUT: Clipboard data set.\n");
                                    /* Set hGlobal to NULL so we don't free it below, */
                                    /* the system owns it now. */
                                    hGlobal = NULL;
                                    MessageBox(hwnd, STRING_COPIED_ANSI, WINDOW_TITLE_ANSI, MB_OK | MB_ICONINFORMATION); /* Use non-A version */
                                } else {
                                    DebugPrint("IDM_FILE_COPYOUTPUT: Failed to open clipboard.\n");
                                    MessageBox(hwnd, STRING_CLIPBOARD_OPEN_ERROR_ANSI, "Error", MB_OK | MB_ICONERROR); /* Use non-A version */
                                }
                            } else {
                                DebugPrint("IDM_FILE_COPYOUTPUT: Failed to lock memory.\n");
                                 MessageBox(hwnd, STRING_CLIPBOARD_MEM_LOCK_ERROR_ANSI, "Error", MB_OK | MB_ICONERROR); /* Use non-A version */
                            }
                            /* If hGlobal is not NULL here, it means SetClipboardData failed or */
                            /* was not called, so we should free the memory we allocated. */
                            if (hGlobal) {
                                DebugPrint("IDM_FILE_COPYOUTPUT: Freeing global memory.\n");
                                GlobalFree(hGlobal); /* Use non-A version */
                            }
                         } else {
                             DebugPrint("IDM_FILE_COPYOUTPUT: Failed to allocate global memory.\n");
                             MessageBox(hwnd, STRING_CLIPBOARD_MEM_ALLOC_ERROR_ANSI, "Error", MB_OK | MB_ICONERROR); /* Use non-A version */
                         }
                    } else {
                        DebugPrint("IDM_FILE_COPYOUTPUT: Output text length is 0.\n");
                        /* Optional: Notify user if there's nothing to copy */
                        /* MessageBox(hwnd, "Output area is empty.", "Copy", MB_OK | MB_ICONINFORMATION); // Use non-A version */
                    }
                    break; /* Break for IDM_FILE_COPYOUTPUT case */
                } /* End brace for IDM_FILE_COPYOUTPUT scope */

                 case IDM_FILE_CLEAROUTPUT:
                 {
                     RECT rcClient;

                     DebugPrint("WM_COMMAND: IDM_FILE_CLEAROUTPUT received. Clearing output text.\n");
                     DebugPrint("IDM_FILE_CLEAROUTPUT: Clearing output edit (handle %p).\n", (void*)hwndOutputEdit);

                     /* Temporarily disable redrawing */
                     SendMessage(hwndOutputEdit, WM_SETREDRAW, FALSE, 0); /* Use non-A version */
                     DebugPrint("IDM_FILE_CLEAROUTPUT: Disabled redrawing.\n");

                     /* Clear the text */
                     SetWindowText(hwndOutputEdit, ""); /* Use non-A version */
                     DebugPrint("IDM_FILE_CLEAROUTPUT: Text cleared.\n");

                     /* Re-enable redrawing and force a repaint */
                     SendMessage(hwndOutputEdit, WM_SETREDRAW, TRUE, 0); /* Use non-A version */
                     DebugPrint("IDM_FILE_CLEAROUTPUT: Enabled redrawing.\n");

                     GetClientRect(hwndOutputEdit, &rcClient);
                     InvalidateRect(hwndOutputEdit, &rcClient, TRUE); /* Use non-A version */
                     UpdateWindow(hwndOutputEdit); /* Use non-A version */

                     DebugPrint("IDM_FILE_CLEAROUTPUT: Repaint forced.\n");

                     break;
                 }

                case IDM_FILE_EXIT:
                    DebugPrint("WM_COMMAND: IDM_FILE_EXIT received.\n");
                    DestroyWindow(hwnd);
                    break;

                case IDM_EDIT_CUT:
                {
                    HWND hFocusedWnd = GetFocus(); /* Get the window with focus */
                    DebugPrint("WM_COMMAND: IDM_EDIT_CUT received.\n");
                    if (hFocusedWnd) {
                        /* Send WM_CUT message to the focused control */
                        SendMessage(hFocusedWnd, WM_CUT, 0, 0); /* Use non-A version */
                        DebugPrint("WM_COMMAND: WM_CUT sent to focused window %p.\n", (void*)hFocusedWnd);
                    } else {
                         DebugPrint("WM_COMMAND: IDM_EDIT_CUT - No window has focus.\n");
                    }
                    break;
                }

                case IDM_EDIT_COPY:
                {
                    HWND hFocusedWnd = GetFocus(); /* Get the window with focus */
                    DebugPrint("WM_COMMAND: IDM_EDIT_COPY received.\n");
                    if (hFocusedWnd) {
                        /* Send WM_COPY message to the focused control */
                        SendMessage(hFocusedWnd, WM_COPY, 0, 0); /* Use non-A version */
                        DebugPrint("WM_COMMAND: WM_COPY sent to focused window %p.\n", (void*)hFocusedWnd);
                    } else {
                         DebugPrint("WM_COMMAND: IDM_EDIT_COPY - No window has focus.\n");
                    }
                    break;
                }

                case IDM_EDIT_PASTE:
                {
                    HWND hFocusedWnd = GetFocus(); /* Get the window with focus */
                    DebugPrint("WM_COMMAND: IDM_EDIT_PASTE received.\n");
                    if (hFocusedWnd) {
                        /* Send WM_PASTE message to the focused control */
                        SendMessage(hFocusedWnd, WM_PASTE, 0, 0); /* Use non-A version */
                        DebugPrint("WM_COMMAND: IDM_EDIT_PASTE sent to focused window %p.\n", (void*)hFocusedWnd);
                    } else {
                         DebugPrint("WM_COMMAND: IDM_EDIT_PASTE - No window has focus.\n");
                    }
                    break;
                }

                case IDM_EDIT_SELECTALL: /* Handle the single Select All ID */
                {
                    HWND hFocusedWnd = GetFocus(); /* Get the window with focus */

                    DebugPrint("WM_COMMAND: IDM_EDIT_SELECTALL received.\n");

                    /* Check which edit control has focus and select all text in it */
                    if (hFocusedWnd == hwndCodeEdit) {
                        SendMessage(hwndCodeEdit, EM_SETSEL, 0, -1); /* Use non-A version */
                        DebugPrint("WM_COMMAND: IDM_EDIT_SELECTALL (Code) processed.\n");
                    } else if (hFocusedWnd == hwndInputEdit) {
                        SendMessage(hwndInputEdit, EM_SETSEL, 0, -1); /* Use non-A version */
                        DebugPrint("WM_COMMAND: IDM_EDIT_SELECTALL (Input) processed.\n");
                    } else if (hFocusedWnd == hwndOutputEdit) {
                        SendMessage(hwndOutputEdit, EM_SETSEL, 0, -1); /* Use non-A version */
                        DebugPrint("WM_COMMAND: IDM_EDIT_SELECTALL (Output) processed.\n");
                    } else {
                        DebugPrint("WM_COMMAND: IDM_EDIT_SELECTALL - No target edit control has focus.\n");
                    }
                    break;
                }

                case IDM_HELP_ABOUT:
                {
                    DebugPrint("WM_COMMAND: IDM_HELP_ABOUT received.\n");
                    /* Show the custom About dialog instead of MessageBox */
                    ShowModalAboutDialog(hwnd);
                    DebugPrint("WM_COMMAND: IDM_HELP_ABOUT called ShowModalAboutDialog.\n");
                    break;
                }


                default:
                    /* Let Windows handle any messages we don't process (ANSI version) */
                    /* DebugPrint("WindowProc: Unhandled message.\n"); // Too noisy */
                    return DefWindowProc(hwnd, uMsg, wParam, lParam); /* Use non-A version */
            }
            break; /* End of WM_COMMAND (handled cases break internally) */
        }

        case WM_CTLCOLORSTATIC:
        {
             HDC hdcStatic = (HDC)wParam;
             /* Make label background transparent to match window background */
             SetBkMode(hdcStatic, TRANSPARENT);
             /* Return a NULL_BRUSH handle to prevent background painting */
             return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        /* Note: No 'break' needed after return */

        /* Removed WM_APP_INTERPRETER_OUTPUT_STRING and WM_APP_INTERPRETER_DONE handlers */


        case WM_CLOSE:
            DebugPrint("WM_CLOSE received.\n");
            DestroyWindow(hwnd);
            return 0; /* Indicate we handled the message */

        case WM_DESTROY:
            DebugPrint("WM_DESTROY received.\n");
            /* No need to signal or wait for interpreter thread in single-threaded version */

            if (hMonoFont) {
                DebugPrint("WM_DESTROY: Deleting font object.\n");
                DeleteObject(hMonoFont); /* Clean up the font object */
                hMonoFont = NULL;
            }
            if (hLabelFont) { /* New: Clean up the label font object */
                DebugPrint("WM_DESTROY: Deleting label font object.\n");
                DeleteObject(hLabelFont);
                hLabelFont = NULL;
            }
            PostQuitMessage(0); /* End the message loop */
            DebugPrint("WM_DESTROY: Posted WM_QUIT.\n");
            break;

        default:
            /* Let Windows handle any messages we don't process (ANSI version) */
            /* DebugPrint("WindowProc: Unhandled message.\n"); // Too noisy */
            return DefWindowProc(hwnd, uMsg, wParam, lParam); /* Use non-A version */
    }
    return 0; /* Default return for handled messages that don't return earlier */
}

/* --- WinMain Entry Point (ANSI version) --- */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) /* Use PASCAL and appropriate parameters for WinMain in Win3.1 */
{
    WNDCLASS wc = { 0 }; /* Use WNDCLASS for Win3.1 */
    const char MAIN_WINDOW_CLASS_NAME[] = "BFInterpreterWindowClass";
    ACCEL AccelTable[] = {
        {FVIRTKEY | FCONTROL, 'N', IDM_FILE_NEW}, /* Ctrl+N for New */
        {FVIRTKEY | FCONTROL, 'R', IDM_FILE_RUN},
        {FVIRTKEY | FCONTROL | FSHIFT, 'C', IDA_FILE_COPYOUTPUT}, /* Ctrl+Shift+C for Copy Output */
        {FVIRTKEY | FCONTROL, 'X', IDM_FILE_EXIT}, /* Accelerator for Exit */
        {FVIRTKEY | FCONTROL, 'O', IDM_FILE_OPEN},  /* Accelerator for Open */
        {FVIRTKEY | FCONTROL, 'A', IDM_EDIT_SELECTALL}, /* Ctrl+A for Select All */
        {FVIRTKEY | FCONTROL, VK_F1, IDM_HELP_ABOUT}, /* Ctrl+F1 for About */
        {FVIRTKEY | FCONTROL, 'X', IDM_EDIT_CUT}, /* Ctrl+X for Cut */
        {FVIRTKEY | FCONTROL, 'C', IDM_EDIT_COPY}, /* Ctrl+C for Copy */
        {FVIRTKEY | FCONTROL, 'V', IDM_EDIT_PASTE} /* Ctrl+V for Paste */
    };
    HWND hwnd;
    MSG msg;

    DebugPrint("WinMain started.\n");
    /* Store instance handle in our global variable */
    hInst = hInstance;

    /* Removed Common Controls initialization - not available in Win3.1 */


    /* Load debug settings from the INI file at program start */
    LoadDebugSettingsFromIni();
    DebugPrint("WinMain: Called LoadDebugSettingsFromIni.\n");


    /* Define window class name (ANSI string) */

    /* --- Register the main window class --- */

    wc.lpfnWndProc     = WindowProc;
    wc.hInstance       = hInstance;
    wc.lpszClassName = MAIN_WINDOW_CLASS_NAME;
    /* Use COLOR_3DFACE for the background brush for a 3D look */
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.hCursor         = LoadCursor(NULL, IDC_ARROW); /* Use non-A version */
    wc.lpszMenuName  = NULL; /* We create menu programmatically in WM_CREATE */
    /* Add CS_HREDRAW | CS_VREDRAW for proper redrawing on resize */
    wc.style = CS_HREDRAW | CS_VREDRAW;


    DebugPrint("WinMain: Registering main window class.\n");
    if (!RegisterClass(&wc)) /* Use RegisterClass for Win3.1 */
    {
        DebugPrint("WinMain: Main window registration failed.\n");
        MessageBox(NULL, STRING_WINDOW_REG_ERROR_ANSI, "Error", MB_ICONEXCLAMATION | MB_OK); /* Use non-A version */
        return 0;
    }
    DebugPrint("WinMain: Main window class registered successfully.\n");


    /* --- Load Accelerator Table --- */
    /* Define the accelerator table structure */

    /* Create the accelerator table from the structure */
    hAccelTable = CreateAcceleratorTable(AccelTable, sizeof(AccelTable) / sizeof(ACCEL)); /* Use non-A version */

    if (hAccelTable == NULL) {
        DebugPrint("WinMain: Failed to create accelerator table.\n");
        /* You might want to show a message box here in a real app */
    } else {
        DebugPrint("WinMain: Accelerator table created.\n");
    }


    /* --- Create the main window --- */
    DebugPrint("WinMain: Creating main window.\n");
    hwnd = CreateWindowEx( /* Use non-A version */
        0,                                  /* Optional window styles. */
        MAIN_WINDOW_CLASS_NAME,             /* Window class (ANSI) */
        WINDOW_TITLE_ANSI,                  /* Window title (ANSI) */
        WS_OVERLAPPEDWINDOW,                /* Window style (includes WS_CAPTION, WS_SYSMENU, WS_THICKFRAME, WS_MINIMIZEBOX, WS_MAXIMIZEBOX) */

        /* Size and position */
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 550,

        NULL,       /* Parent window */
        NULL,       /* Menu (we create it in WM_CREATE) */
        hInstance,  /* Instance handle */
        NULL        /* Additional application data */
    );

    if (hwnd == NULL)
    {
        DebugPrint("WinMain: Main window creation failed.\n");
        MessageBox(NULL, STRING_WINDOW_CREATION_ERROR_ANSI, "Error", MB_ICONEXCLAMATION | MB_OK); /* Use non-A version */
        return 0;
    }
    DebugPrint("WinMain: Main window created successfully.\n");

    /* --- Show and update the main window --- */
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    DebugPrint("WinMain: Main window shown and updated.\n");

    /* --- Message Loop --- */
    DebugPrint("WinMain: Entering message loop.\n");
    msg = (MSG) { 0 }; /* Initialize msg structure */
    while (GetMessage(&msg, NULL, 0, 0) > 0) /* Use GetMessage */
    {
        /* Check if the message is for a dialog box. If so, let the dialog handle it. */
        /* IsDialogMessage handles keyboard input for dialog controls (like Tab, Enter, Esc). */
        if (!IsDialogMessage(GetActiveWindow(), &msg)) {
             /* Translate accelerator keys before dispatching the message */
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) { /* Use non-A version */
                TranslateMessage(&msg);
                DispatchMessage(&msg); /* Use DispatchMessage */
            }
        }
    }
    DebugPrint("WinMain: Exited message loop.\n");

    /* --- Clean up Accelerator Table --- */
    if (hAccelTable) {
        DestroyAcceleratorTable(hAccelTable);
        DebugPrint("WinMain: Accelerator table destroyed.\n");
    }

    DebugPrint("WinMain finished.\n");
    return (int)msg.wParam;
}

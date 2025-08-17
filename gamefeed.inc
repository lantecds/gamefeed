#if defined _inc_lif_gamefeed
    #endinput
#endif
#define _inc_lif_gamefeed

#tryinclude <open.mp>

#if !defined _INC_open_mp
    #warning The include is not tested on the SA-MP script environment! Please make adjustments in the include file if needed.
#endif

// ===============================================================================
// Popup Configuration
// ===============================================================================
// Default duration for displaying popups (in milliseconds)
#define POPUP_DEFAULT_DURATION (3000)

// Maximum number of popups that can be queued at once
#define MAX_POPUP_QUEUE (5)

// Colors for the popup title, text, and background
#define POPUP_TITLE_COLOR (0xFFFFFFFF)
#define POPUP_TEXT_COLOR  (0xFFFFFFFF)
#define POPUP_BG_COLOR    (0x000000FF)

// Interval (in milliseconds) for fading effects on popups
#define POPUP_FADE_INTERVAL (100)

// ===============================================================================
// Subtitle Configuration
// ===============================================================================
// Color for subtitle text
#define SUBTITLE_TEXT_COLOR (0xFFFFFFFF)

// Default duration for subtitle display (in milliseconds)
#define SUBTITLE_DEFAULT_DURATION (4000)

// Interval (in milliseconds) for subtitle fade effects
#define SUBTITLE_FADE_INTERVAL (50)

// ===============================================================================
// Cash Text Configuration
// ===============================================================================
// Colors for cash increase/decrease and background
#define CASH_TEXT_INCREASE_COLOR (0x2F5A26FF)
#define CASH_TEXT_DECREASE_COLOR (0x9C1619FF)
#define CASH_TEXT_BG_COLOR (0x000000FF)

// ===============================================================================
// Fading Configuration
// ===============================================================================
// Maximum alpha values for background and text fades
#define MAX_BG_ALPHA (180)
#define MAX_TEXT_ALPHA (255)

// Step size for fade transitions
#define FADE_STEP (25)

// ===============================================================================
// Popup Variable Fetching Format
// ===============================================================================
// Format to fetch a player's popup variable name dynamically
#define GetPopupPVarName(%0,%1,%2) format(%0, sizeof %0, "popup_%d_%s", %1, %2)

// ===============================================================================
// Game Feed Popup and Subtitle Alpha Tracking
// ===============================================================================
// Array to store popup alpha values (fade effects) for each player and their popups
static gameFEED_popupAlpha[MAX_PLAYERS][MAX_POPUP_QUEUE] = {{0, ...}, ...};

// Array to store subtitle alpha values (fade effects) for each player
static gameFEED_subtitleAlpha[MAX_PLAYERS] = {0, ...};

// Timer variables for managing timers
new subtitleFadeInTimer[MAX_PLAYERS] = {INVALID_TIMER, ...};
new subtitleFadeOutTimer[MAX_PLAYERS] = {INVALID_TIMER, ...};
new hideSubtitleTimer[MAX_PLAYERS] = {INVALID_TIMER, ...};
new hideCashTextTimer[MAX_PLAYERS] = {INVALID_TIMER, ...};

// TextDraw arrays
static PlayerText:gameFEED_popupTitleTD[MAX_PLAYERS][MAX_POPUP_QUEUE] = {INVALID_PLAYER_TEXT_DRAW, ...};
static PlayerText:gameFEED_popupMsgTD[MAX_PLAYERS][MAX_POPUP_QUEUE] = {INVALID_PLAYER_TEXT_DRAW, ...};
static PlayerText:gameFEED_subtitleTD[MAX_PLAYERS] = {INVALID_PLAYER_TEXT_DRAW, ...};
static PlayerText:gameFEED_cashTextTD[MAX_PLAYERS] = {INVALID_PLAYER_TEXT_DRAW, ...};

// ===============================================================================
// Popup Functions
// ===============================================================================

stock bool:GameFeed_IsAnyPopupDisplayed(const playerid)
{
    new string:var[32];
    for (new i = 0; i < MAX_POPUP_QUEUE; i++)
    {
        GetPopupPVarName(var, i, "displayed");
        if (GetPVarInt(playerid, var)) return true;
    }
    return false;
}

/**
 *  Purpose:
 *      Displays a popup for the specified player. It manages the popup queue,
 *      storing popup information, and displaying the popup either immediately or 
 *      after checking if there are any active popups. If `jumpqueue` is true, 
 *      it will skip the queue and show the popup immediately.
 *  
 *  Parameters:
 *      playerid     - The ID of the player to show the popup to.
 *      title        - The title of the popup.
 *      message      - The message displayed in the popup.
 *      displaytime  - The duration (in milliseconds) to display the popup.
 *      jumpqueue    - If true, skips the queue and shows the popup immediately.
 *  
 *  Returns:
 *      1 if the popup was successfully queued or displayed.
 *      0 if the popup could not be shown (e.g., no available slots in the queue).
 */
stock bool:ShowPlayerPopup(const playerid, const string:title[], const string:message[], const displaytime = POPUP_DEFAULT_DURATION, const bool:jumpqueue = false)
{
    if (jumpqueue)
    {        
        for (new i = 0; i < MAX_POPUP_QUEUE; i++)
        {
            new string:var[32];
            GetPopupPVarName(var, i, "active");
            if (GetPVarInt(playerid, var))
            {
                GetPopupPVarName(var, i, "displayed");
                if (GetPVarInt(playerid, var))
                {
                    new remainingTime = GameFeed_CalculateRemainingTime(playerid, i);
                                        
                    GetPopupPVarName(var, i, "remaining_time");
                    SetPVarInt(playerid, var, remainingTime);
                    
                    GetPopupPVarName(var, i, "paused");
                    SetPVarInt(playerid, var, true);
                }

                if (gameFEED_popupTitleTD[playerid][i] != INVALID_PLAYER_TEXT_DRAW)
                {
                    PlayerTextDrawDestroy(playerid, gameFEED_popupTitleTD[playerid][i]);
                    gameFEED_popupTitleTD[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
                }

                if (gameFEED_popupMsgTD[playerid][i] != INVALID_PLAYER_TEXT_DRAW)
                {
                    PlayerTextDrawDestroy(playerid, gameFEED_popupMsgTD[playerid][i]);
                    gameFEED_popupMsgTD[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
                }

                GetPopupPVarName(var, i, "displayed");
                SetPVarInt(playerid, var, false);
            }
        }
    }

    for (new i = 0; i < MAX_POPUP_QUEUE; i++)
    {
        new string:var[32];
        GetPopupPVarName(var, i, "active");
        if (!GetPVarInt(playerid, var))
        {            
            SetPVarInt(playerid, var, true);

            GetPopupPVarName(var, i, "duration");
            SetPVarInt(playerid, var, displaytime);

            GetPopupPVarName(var, i, "title");
            SetPVarString(playerid, var, title);

            GetPopupPVarName(var, i, "msg");
            SetPVarString(playerid, var, message);

            GetPopupPVarName(var, i, "displayed");
            SetPVarInt(playerid, var, false);

            GetPopupPVarName(var, i, "paused");
            SetPVarInt(playerid, var, false);

            GetPopupPVarName(var, i, "remaining_time");
            SetPVarInt(playerid, var, displaytime);

            GetPopupPVarName(var, i, "start_time");
            SetPVarInt(playerid, var, GetTickCount());

            if (!GameFeed_IsAnyPopupDisplayed(playerid) || jumpqueue)
            {
                GameFeed_DisplayPopupSlot(playerid, i);
            }
            return true;
        }
    }
    
    return false;
}

stock GameFeed_CalculateRemainingTime(const playerid, const index)
{
    new string:var[32];
    
    GetPopupPVarName(var, index, "duration");
    new originalDuration = GetPVarInt(playerid, var);
    
    GetPopupPVarName(var, index, "start_time");
    new startTime = GetPVarInt(playerid, var);
    
    if (startTime == 0) return originalDuration;
    
    new currentTime = GetTickCount();
    new elapsedTime = currentTime - startTime;
    
    new remainingTime = originalDuration - elapsedTime;
    
    if (remainingTime < 0) remainingTime = 0;
    
    return remainingTime;
}

stock GameFeed_DisplayPopupSlot(const playerid, const index)
{   
    new string:var[32], string:title[64], string:msg[345];
    GetPopupPVarName(var, index, "displayed");
    SetPVarInt(playerid, var, true);
    gameFEED_popupAlpha[playerid][index] = 0;

    GetPopupPVarName(var, index, "title");
    GetPVarString(playerid, var, title);
    GetPopupPVarName(var, index, "msg");
    GetPVarString(playerid, var, msg);

    gameFEED_popupMsgTD[playerid][index] = CreatePlayerTextDraw(playerid, 23.000, 175.000, msg);
    PlayerTextDrawLetterSize(playerid, gameFEED_popupMsgTD[playerid][index], 0.220, 1.199);
    PlayerTextDrawTextSize(playerid, gameFEED_popupMsgTD[playerid][index], 150.000, 0.000);
    PlayerTextDrawAlignment(playerid, gameFEED_popupMsgTD[playerid][index], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawUseBox(playerid, gameFEED_popupMsgTD[playerid][index], true);
    PlayerTextDrawSetShadow(playerid, gameFEED_popupMsgTD[playerid][index], 1);
    PlayerTextDrawSetOutline(playerid, gameFEED_popupMsgTD[playerid][index], 1);
    PlayerTextDrawBackgroundColour(playerid, gameFEED_popupMsgTD[playerid][index], 150);
    PlayerTextDrawFont(playerid, gameFEED_popupMsgTD[playerid][index], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gameFEED_popupMsgTD[playerid][index], true);

    gameFEED_popupTitleTD[playerid][index] = CreatePlayerTextDraw(playerid, 82.000, 164.000, title);
    PlayerTextDrawLetterSize(playerid, gameFEED_popupTitleTD[playerid][index], 0.260, 1.399);
    PlayerTextDrawAlignment(playerid, gameFEED_popupTitleTD[playerid][index], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawSetShadow(playerid, gameFEED_popupTitleTD[playerid][index], 1);
    PlayerTextDrawSetOutline(playerid, gameFEED_popupTitleTD[playerid][index], 1);
    PlayerTextDrawBackgroundColour(playerid, gameFEED_popupTitleTD[playerid][index], 150);
    PlayerTextDrawFont(playerid, gameFEED_popupTitleTD[playerid][index], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gameFEED_popupTitleTD[playerid][index], true);

    PlayerTextDrawColour(playerid, gameFEED_popupTitleTD[playerid][index], (POPUP_TITLE_COLOR & 0xFFFFFF00));
    PlayerTextDrawColour(playerid, gameFEED_popupMsgTD[playerid][index], (POPUP_TEXT_COLOR & 0xFFFFFF00));
    PlayerTextDrawBoxColour(playerid, gameFEED_popupMsgTD[playerid][index], (POPUP_BG_COLOR & 0xFFFFFF00));

    PlayerTextDrawShow(playerid, gameFEED_popupTitleTD[playerid][index]);
    PlayerTextDrawShow(playerid, gameFEED_popupMsgTD[playerid][index]);

    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
    SetTimerEx("GameFeed_FadeInPlayerPopup", POPUP_FADE_INTERVAL, false, "ii", playerid, index);
}

forward GameFeed_FadeInPlayerPopup(const playerid, const index);
public GameFeed_FadeInPlayerPopup(const playerid, const index)
{
    KillTimer(subtitleFadeOutTimer[playerid]);
    gameFEED_popupAlpha[playerid][index] += FADE_STEP;
    new textAlpha = gameFEED_popupAlpha[playerid][index];
    if (textAlpha > MAX_TEXT_ALPHA) textAlpha = MAX_TEXT_ALPHA;
    
    new bgAlpha = gameFEED_popupAlpha[playerid][index];
    if (bgAlpha > MAX_BG_ALPHA) bgAlpha = MAX_BG_ALPHA;
    
    PlayerTextDrawColour(playerid, gameFEED_popupTitleTD[playerid][index], (POPUP_TEXT_COLOR & 0xFFFFFF00) | (textAlpha & 0xFF));
    PlayerTextDrawShow(playerid, gameFEED_popupTitleTD[playerid][index]);

    PlayerTextDrawColour(playerid, gameFEED_popupMsgTD[playerid][index], (POPUP_TEXT_COLOR & 0xFFFFFF00) | (textAlpha & 0xFF));
    PlayerTextDrawBoxColour(playerid, gameFEED_popupMsgTD[playerid][index], (POPUP_BG_COLOR & 0xFFFFFF00) | (bgAlpha & 0xFF));
    PlayerTextDrawShow(playerid, gameFEED_popupMsgTD[playerid][index]);

    if (gameFEED_popupAlpha[playerid][index] < MAX_TEXT_ALPHA)
    {
        SetTimerEx("GameFeed_FadeInPlayerPopup", POPUP_FADE_INTERVAL, false, "ii", playerid, index);
    }
    else
    {
        new string:var[32];
        
        // Check if this popup was paused and has remaining time
        GetPopupPVarName(var, index, "paused");
        if (GetPVarInt(playerid, var))
        {
            // This popup was paused, use remaining time
            GetPopupPVarName(var, index, "remaining_time");
            new remainingTime = GetPVarInt(playerid, var);
                        
            SetPVarInt(playerid, var, false);
            GetPopupPVarName(var, index, "start_time");
            SetPVarInt(playerid, var, GetTickCount());
            
            // Use remaining time for the timer
            SetTimerEx("GameFeed_FadeOutPlayerPopup", remainingTime, false, "ii", playerid, index);
        }
        else
        {
            // Normal popup, use full duration
            GetPopupPVarName(var, index, "duration");
            new duration = GetPVarInt(playerid, var);
            SetTimerEx("GameFeed_FadeOutPlayerPopup", duration, false, "ii", playerid, index);
        }
    }
    return true;
}

forward GameFeed_FadeOutPlayerPopup(const playerid, const index);
public GameFeed_FadeOutPlayerPopup(const playerid, const index)
{
    gameFEED_popupAlpha[playerid][index] -= FADE_STEP;
    if (gameFEED_popupAlpha[playerid][index] < 0) gameFEED_popupAlpha[playerid][index] = 0;

    PlayerTextDrawColour(playerid, gameFEED_popupTitleTD[playerid][index], (POPUP_TITLE_COLOR & 0xFFFFFF00) | (gameFEED_popupAlpha[playerid][index] & 0xFF));
    PlayerTextDrawShow(playerid, gameFEED_popupTitleTD[playerid][index]);

    PlayerTextDrawColour(playerid, gameFEED_popupMsgTD[playerid][index], (POPUP_TEXT_COLOR & 0xFFFFFF00) | (gameFEED_popupAlpha[playerid][index] & 0xFF));
    PlayerTextDrawBoxColour(playerid, gameFEED_popupMsgTD[playerid][index], (POPUP_BG_COLOR & 0xFFFFFF00) | (gameFEED_popupAlpha[playerid][index] & 0xFF));
    PlayerTextDrawShow(playerid, gameFEED_popupMsgTD[playerid][index]);

    if (gameFEED_popupAlpha[playerid][index] > 0)
    {
        SetTimerEx("GameFeed_FadeOutPlayerPopup", POPUP_FADE_INTERVAL, false, "ii", playerid, index);
    }
    else
    {
        if (gameFEED_popupTitleTD[playerid][index] != INVALID_PLAYER_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, gameFEED_popupTitleTD[playerid][index]);
            gameFEED_popupTitleTD[playerid][index] = INVALID_PLAYER_TEXT_DRAW;
        }

        if (gameFEED_popupMsgTD[playerid][index] != INVALID_PLAYER_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, gameFEED_popupMsgTD[playerid][index]);
            gameFEED_popupMsgTD[playerid][index] = INVALID_PLAYER_TEXT_DRAW;
        }

        new string:var[32];
        GetPopupPVarName(var, index, "active");
        SetPVarInt(playerid, var, false);
        GetPopupPVarName(var, index, "displayed");
        SetPVarInt(playerid, var, false);

        // Check for paused popups first (they have priority)
        for (new i = 0; i < MAX_POPUP_QUEUE; i++)
        {
            GetPopupPVarName(var, i, "paused");
            if (GetPVarInt(playerid, var))
            {                
                SetPVarInt(playerid, var, false);
                GameFeed_DisplayPopupSlot(playerid, i);
                return true;
            }
        }

        // Then check for queued popups
        for (new i = 0; i < MAX_POPUP_QUEUE; i++)
        {
            GetPopupPVarName(var, i, "active");
            if (GetPVarInt(playerid, var))
            {
                GetPopupPVarName(var, i, "displayed");
                if (!GetPVarInt(playerid, var))
                {
                    GameFeed_DisplayPopupSlot(playerid, i);
                    return true;
                }
            }
        }
    }
    return true;
}

/**
 *  Purpose:
 *      Displays a subtitle for the specified player. It first clears any existing subtitles, 
 *      then creates and displays a new subtitle text draw based on the provided parameters. 
 *      If fading is enabled, it will apply fade effects, otherwise, it will show the subtitle without fading.
 *  
 *  Parameters:
 *      playerid    - The ID of the player to show the subtitle to.
 *      text        - The subtitle text to be displayed.
 *      displaytime - The duration (in milliseconds) to display the subtitle (default is 4000 ms).
 *      useFade     - If true, applies a fade effect to the subtitle (default is true).
 */
stock bool:ShowPlayerSubtitle(const playerid, const string:text[], const displaytime = 4000, const bool:useFade = true)
{
    GameFeed_ClearSubtitles(playerid);

    GameFeed_CreateSubtitleTextDraws(playerid, text, displaytime, useFade);

    return true;
}

stock GameFeed_CreateSubtitleTextDraws(const playerid, const string:text[], const displaytime = 4000, const bool:useFade = true)
{    
    gameFEED_subtitleAlpha[playerid] = useFade ? 0 : MAX_TEXT_ALPHA;
    SetPVarInt(playerid, "subtitle_fading", useFade);

    gameFEED_subtitleTD[playerid] = CreatePlayerTextDraw(playerid, 315.000, 380.000, text);
    PlayerTextDrawLetterSize(playerid, gameFEED_subtitleTD[playerid], 0.250, 1.399);
    PlayerTextDrawAlignment(playerid, gameFEED_subtitleTD[playerid], TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, gameFEED_subtitleTD[playerid], 0xFFFFFF00 | gameFEED_subtitleAlpha[playerid]);
    PlayerTextDrawSetShadow(playerid, gameFEED_subtitleTD[playerid], 1);
    PlayerTextDrawSetOutline(playerid, gameFEED_subtitleTD[playerid], 1);
    PlayerTextDrawBackgroundColour(playerid, gameFEED_subtitleTD[playerid], 150);
    PlayerTextDrawFont(playerid, gameFEED_subtitleTD[playerid], TEXT_DRAW_FONT_1);
    PlayerTextDrawSetProportional(playerid, gameFEED_subtitleTD[playerid], true);

    PlayerTextDrawShow(playerid, gameFEED_subtitleTD[playerid]);

    SetPVarInt(playerid, "subtitle_being_shown", 1);
    SetPVarInt(playerid, "subtitle_display_time", displaytime);

    if (useFade)
    {
        if (subtitleFadeInTimer[playerid] != INVALID_TIMER) 
        {
            KillTimer(subtitleFadeInTimer[playerid]);
        }
        subtitleFadeInTimer[playerid] = SetTimerEx("GameFeed_FadeInSubtitle", SUBTITLE_FADE_INTERVAL, false, "i", playerid);

        if (subtitleFadeOutTimer[playerid] != INVALID_TIMER) 
        {
            KillTimer(subtitleFadeOutTimer[playerid]);
        }
        subtitleFadeOutTimer[playerid] = SetTimerEx("GameFeed_FadeOutSubtitle", displaytime, false, "i", playerid);
    }
    else
    {
        // If no fade, just set the timer to hide after displaytime
        // Kill any existing fade-out timers if the fade option is switched off
        if (hideSubtitleTimer[playerid] != INVALID_TIMER) 
        {
            KillTimer(hideSubtitleTimer[playerid]);
        }
        hideSubtitleTimer[playerid] = SetTimerEx("GameFeed_HideSubtitleAfterTime", displaytime, false, "i", playerid);
    }
}

forward GameFeed_FadeInSubtitle(const playerid);
public GameFeed_FadeInSubtitle(const playerid)
{  
    gameFEED_subtitleAlpha[playerid] += FADE_STEP;
    if (gameFEED_subtitleAlpha[playerid] > MAX_TEXT_ALPHA) gameFEED_subtitleAlpha[playerid] = MAX_TEXT_ALPHA;

    PlayerTextDrawColour(playerid, gameFEED_subtitleTD[playerid], 0xFFFFFF00 | (gameFEED_subtitleAlpha[playerid] & 0xFF));
    PlayerTextDrawShow(playerid, gameFEED_subtitleTD[playerid]);

    if (gameFEED_subtitleAlpha[playerid] < MAX_TEXT_ALPHA)
    {
        subtitleFadeInTimer[playerid] = SetTimerEx("GameFeed_FadeInSubtitle", SUBTITLE_FADE_INTERVAL, false, "i", playerid);
    }
}

forward GameFeed_FadeOutSubtitle(const playerid);
public GameFeed_FadeOutSubtitle(const playerid)
{
    gameFEED_subtitleAlpha[playerid] -= FADE_STEP;
    if (gameFEED_subtitleAlpha[playerid] < 0) gameFEED_subtitleAlpha[playerid] = 0;

    PlayerTextDrawColour(playerid, gameFEED_subtitleTD[playerid], 0xFFFFFF00 | (gameFEED_subtitleAlpha[playerid] & 0xFF));
    PlayerTextDrawShow(playerid, gameFEED_subtitleTD[playerid]);

    if (gameFEED_subtitleAlpha[playerid] > 0)
    {
        subtitleFadeOutTimer[playerid] = SetTimerEx("GameFeed_FadeOutSubtitle", SUBTITLE_FADE_INTERVAL, false, "i", playerid);
    }
    else
    {
        GameFeed_ClearSubtitles(playerid);
    }
}

forward GameFeed_HideSubtitleAfterTime(const playerid);
public GameFeed_HideSubtitleAfterTime(const playerid)
{
    GameFeed_ClearSubtitles(playerid);
}

stock GameFeed_ClearSubtitles(const playerid)
{
    if (GetPVarInt(playerid, "subtitle_being_shown"))
    {        
        if (gameFEED_subtitleTD[playerid] != INVALID_PLAYER_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, gameFEED_subtitleTD[playerid]);
            gameFEED_subtitleTD[playerid] = INVALID_PLAYER_TEXT_DRAW;
        }

        SetPVarInt(playerid, "subtitle_being_shown", 0);

        if (subtitleFadeInTimer[playerid] != INVALID_TIMER) 
        {
            KillTimer(subtitleFadeInTimer[playerid]);
            subtitleFadeInTimer[playerid] = INVALID_TIMER;
        }

        if (subtitleFadeOutTimer[playerid] != INVALID_TIMER) 
        {
            KillTimer(subtitleFadeOutTimer[playerid]);
            subtitleFadeOutTimer[playerid] = INVALID_TIMER;
        }

        if (hideSubtitleTimer[playerid] != INVALID_TIMER) 
        {
            KillTimer(hideSubtitleTimer[playerid]);
            hideSubtitleTimer[playerid] = INVALID_TIMER;
        }
    }
}


/**
 *  Purpose:
 *      Displays cash text on the screen for a specified player. First, it clears any existing 
 *      cash text and then creates a new one based on the given amount and duration.
 *  
 *  Parameters:
 *      playerid    - The ID of the player to show the cash text to.
 *      amount      - The cash amount to display, which can be positive or negative.
 *      displayTime - The duration (in milliseconds) to display the cash text (default is 3000 ms).
 *  
 */
stock bool:ShowPlayerCashText(const playerid, const amount, const displayTime = 3000)
{
    GameFeed_ClearPlayerCashText(playerid);

    GameFeed_CreateCashTextDraw(playerid, amount, displayTime);

    return true;
}

// Clears any existing cash text for the player
stock GameFeed_ClearPlayerCashText(const playerid)
{
    if (gameFEED_cashTextTD[playerid] != INVALID_PLAYER_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, gameFEED_cashTextTD[playerid]);
        gameFEED_cashTextTD[playerid] = INVALID_PLAYER_TEXT_DRAW;
    }

    if (hideCashTextTimer[playerid] != INVALID_TIMER)
    {
        KillTimer(hideCashTextTimer[playerid]);
        hideCashTextTimer[playerid] = INVALID_TIMER;
    }
}

// Creates and displays the cash text draw on the player screen
stock GameFeed_CreateCashTextDraw(const playerid, const amount, const displayTime)
{    
    new amountText[36];

    if (amount >= 0)
    {
        format(amountText, sizeof(amountText), "+$%i", amount);
    }
    else
    {
        format(amountText, sizeof(amountText), "-$%i", -amount);
    }

    gameFEED_cashTextTD[playerid] = CreatePlayerTextDraw(playerid, 608.0, 100.0, amountText);
    PlayerTextDrawLetterSize(playerid, gameFEED_cashTextTD[playerid], 0.450, 1.999);
    PlayerTextDrawAlignment(playerid, gameFEED_cashTextTD[playerid], TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, gameFEED_cashTextTD[playerid], (amount >= 0) ? CASH_TEXT_INCREASE_COLOR : CASH_TEXT_DECREASE_COLOR);
    PlayerTextDrawSetShadow(playerid, gameFEED_cashTextTD[playerid], -1);
    PlayerTextDrawSetOutline(playerid, gameFEED_cashTextTD[playerid], 2);
    PlayerTextDrawBackgroundColour(playerid, gameFEED_cashTextTD[playerid], CASH_TEXT_BG_COLOR);
    PlayerTextDrawFont(playerid, gameFEED_cashTextTD[playerid], TEXT_DRAW_FONT_3);
    PlayerTextDrawSetProportional(playerid, gameFEED_cashTextTD[playerid], true);

    PlayerTextDrawShow(playerid, gameFEED_cashTextTD[playerid]);

    if (hideCashTextTimer[playerid] != INVALID_TIMER)
    {
        KillTimer(hideCashTextTimer[playerid]);
    }
    hideCashTextTimer[playerid] = SetTimerEx("GameFeed_HidePlayerCashText", displayTime, false, "i", playerid);
}


// Hides the player's cash text after the specified time
forward GameFeed_HidePlayerCashText(const playerid);
public GameFeed_HidePlayerCashText(const playerid)
{
    GameFeed_ClearPlayerCashText(playerid);

    return 1;
}

stock GameFeed_KillAllTimers(const playerid)
{    
    if (subtitleFadeInTimer[playerid] != INVALID_TIMER) 
    {
        KillTimer(subtitleFadeInTimer[playerid]);
        subtitleFadeInTimer[playerid] = INVALID_TIMER;
    }

    if (subtitleFadeOutTimer[playerid] != INVALID_TIMER) 
    {
        KillTimer(subtitleFadeOutTimer[playerid]);
        subtitleFadeOutTimer[playerid] = INVALID_TIMER;
    }

    if (hideSubtitleTimer[playerid] != INVALID_TIMER) 
    {
        KillTimer(hideSubtitleTimer[playerid]);
        hideSubtitleTimer[playerid] = INVALID_TIMER;
    }

    if (hideCashTextTimer[playerid] != INVALID_TIMER) 
    {
        KillTimer(hideCashTextTimer[playerid]);
        hideCashTextTimer[playerid] = INVALID_TIMER;
    }
    return true;
}

stock GameFeed_ResetAllPVars(const playerid)
{    
    for (new i = 0; i < MAX_POPUP_QUEUE; i++)
    {
        new string:var[32];
        GetPopupPVarName(var, i, "active");
        SetPVarInt(playerid, var, false);

        GetPopupPVarName(var, i, "displayed");
        SetPVarInt(playerid, var, false);

        GetPopupPVarName(var, i, "paused");
        SetPVarInt(playerid, var, false);

        GetPopupPVarName(var, i, "remaining_time");
        SetPVarInt(playerid, var, 0);

        GetPopupPVarName(var, i, "start_time");
        SetPVarInt(playerid, var, 0);
    }

    SetPVarInt(playerid, "subtitle_being_shown", 0);
    SetPVarInt(playerid, "subtitle_display_time", 0);
    SetPVarInt(playerid, "subtitle_fading", 0);
    return true;
}

stock GameFeed_ResetAllFadersVariables(const playerid)
{    
    for (new i = 0; i < MAX_POPUP_QUEUE; i++)
    {
        gameFEED_popupAlpha[playerid][i] = 0;
    }

    gameFEED_subtitleAlpha[playerid] = 0;
    return true;
}

stock GameFeed_ResetAllTextDraws(const playerid)
{    
    // Reset all popup TextDraws
    for (new i = 0; i < MAX_POPUP_QUEUE; i++)
    {
        if (gameFEED_popupTitleTD[playerid][i] != INVALID_PLAYER_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, gameFEED_popupTitleTD[playerid][i]);
            gameFEED_popupTitleTD[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
        }

        if (gameFEED_popupMsgTD[playerid][i] != INVALID_PLAYER_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, gameFEED_popupMsgTD[playerid][i]);
            gameFEED_popupMsgTD[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
        }
    }

    // Reset subtitle TextDraw
    if (gameFEED_subtitleTD[playerid] != INVALID_PLAYER_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, gameFEED_subtitleTD[playerid]);
        gameFEED_subtitleTD[playerid] = INVALID_PLAYER_TEXT_DRAW;
    }

    // Reset cash text TextDraw
    if (gameFEED_cashTextTD[playerid] != INVALID_PLAYER_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, gameFEED_cashTextTD[playerid]);
        gameFEED_cashTextTD[playerid] = INVALID_PLAYER_TEXT_DRAW;
    }
    return true;
}

stock bool:GameFeed_ResetPlayer(const playerid)
{    
    GameFeed_KillAllTimers(playerid);
    
    GameFeed_ResetAllPVars(playerid);
    GameFeed_ResetAllFadersVariables(playerid);
    GameFeed_ResetAllTextDraws(playerid);
    return true;
}

public OnPlayerConnect(playerid)
{    
    GameFeed_ResetPlayer(playerid);

    #if defined GameFeed_OnPlayerConnect
        GameFeed_OnPlayerConnect(playerid);
    #endif
    return 1;
}
#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect GameFeed_OnPlayerConnect
#if defined GameFeed_OnPlayerConnect
    forward GameFeed_OnPlayerConnect(playerid);
#endif

// ===============================================================================
// Automatic Money Display Hook
// ===============================================================================
// This section provides automatic money text display when GivePlayerMoney is called
// You can disable this by commenting out GAMEFEED_AUTO_MONEY_DISPLAY above

#if defined GAMEFEED_AUTO_MONEY_DISPLAY
    stock GameFeed_GivePlayerMoney(const playerid, const money)
    {        
        ShowPlayerCashText(playerid, money);
        
        return GivePlayerMoney(playerid, money);
    }

    #if defined _ALS_GivePlayerMoney
        #undef GivePlayerMoney
    #else
        #define _ALS_GivePlayerMoney
    #endif
    #define GivePlayerMoney GameFeed_GivePlayerMoney
#endif
#include <open.mp>
#include <gamefeed>

public OnFilterScriptInit()
{
    print("\n--------------------------------------");
    print(" GameFeed Test Script");
    print("--------------------------------------\n");
    return 1;
}

public OnPlayerConnect(playerid)
{
    // Basic welcome popup
    ShowPlayerPopup(playerid, "Welcome", "Welcome to the test server!", 5000);
    return 1;
}

CMD:testpopup(playerid, params[])
{
    new type;
    if(sscanf(params, "i", type)) 
    {
        SendClientMessage(playerid, -1, "USAGE: /testpopup [1-4]");
        SendClientMessage(playerid, -1, "1: Single popup");
        SendClientMessage(playerid, -1, "2: Multiple popups");
        SendClientMessage(playerid, -1, "3: Jump queue popup");
        SendClientMessage(playerid, -1, "4: Long text popup");
        return 1;
    }

    switch(type)
    {
        case 1: // Basic popup
        {
            ShowPlayerPopup(playerid, "Test Popup", "This is a test popup message!", 3000);
        }
        case 2: // Multiple popups
        {
            ShowPlayerPopup(playerid, "First Popup", "This should appear first", 3000);
            ShowPlayerPopup(playerid, "Second Popup", "This should appear second", 3000);
            ShowPlayerPopup(playerid, "Third Popup", "This should appear third", 3000);
        }
        case 3: // Jump queue
        {
            ShowPlayerPopup(playerid, "IMPORTANT!", "This should appear immediately!", 3000, true);
        }
        case 4: // Long text
        {
            ShowPlayerPopup(playerid, "Long Message", "This is a very long message that should demonstrate how the popup system handles longer text content that might need to wrap to multiple lines.", 5000);
        }
    }
    return true;
}

CMD:testsubtitle(playerid, params[])
{
    new type;
    if(sscanf(params, "i", type)) {
        SendClientMessage(playerid, -1, "USAGE: /testsubtitle [1-3]");
        SendClientMessage(playerid, -1, "1: Basic subtitle");
        SendClientMessage(playerid, -1, "2: Long subtitle");
        SendClientMessage(playerid, -1, "3: No fade subtitle");
        return 1;
    }

    switch(type)
    {
        case 1: // Basic subtitle
        {
            ShowPlayerSubtitle(playerid, "Mission Complete: Test Mission", 4000);
        }
        case 2: // Long subtitle
        {
            ShowPlayerSubtitle(playerid, "This is a longer subtitle that demonstrates how the system handles extended text", 5000);
        }
        case 3: // No fade
        {
            ShowPlayerSubtitle(playerid, "Subtitle without fade effect", 4000, false);
        }
    }
    return 1;
}

// Command to test cash notifications
CMD:testcash(playerid, params[])
{
    new type;
    if(sscanf(params, "i", type)) {
        SendClientMessage(playerid, -1, "USAGE: /testcash [1-2]");
        SendClientMessage(playerid, -1, "1: Positive amount");
        SendClientMessage(playerid, -1, "2: Negative amount");
        return 1;
    }

    switch(type)
    {
        case 1: // Positive amount
        {
            ShowPlayerCashText(playerid, 5000);
        }
        case 2: // Negative amount
        {
            ShowPlayerCashText(playerid, -1000);
        }
    }
    return 1;
}
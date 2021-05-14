#include "editors/texed.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"

static uint16_t screenWidth = 1280;
static uint16_t screenHeight = 720;
static void DrawStyleEditControls(void);

void texed_run(void) {
    InitWindow(screenWidth, screenHeight, "eco2d - texture editor");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    
    
    Rectangle panelRec = { 20, 40, 200, 150 };
    Rectangle panelContentRec = {0, 0, 340, 340 };
    Vector2 panelScroll = { 99, -20 };
    
    bool showContentArea = true;
    
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        {
            DrawText(TextFormat("[%f, %f]", panelScroll.x, panelScroll.y), 4, 4, 20, RED);
            
            Rectangle view = GuiScrollPanel(panelRec, panelContentRec, &panelScroll);
            
            BeginScissorMode(view.x, view.y, view.width, view.height);
            GuiGrid((Rectangle){panelRec.x + panelScroll.x, panelRec.y + panelScroll.y, panelContentRec.width, panelContentRec.height}, 16, 3);
            EndScissorMode();
            
            if (showContentArea) DrawRectangle(panelRec.x + panelScroll.x, panelRec.y + panelScroll.y, panelContentRec.width, panelContentRec.height, Fade(RED, 0.1));
            
            DrawStyleEditControls();
            
            showContentArea = GuiCheckBox((Rectangle){ 565, 80, 20, 20 }, "SHOW CONTENT AREA", showContentArea);
            
            panelContentRec.width = GuiSliderBar((Rectangle){ 590, 385, 145, 15}, "WIDTH", TextFormat("%i", (int)panelContentRec.width), panelContentRec.width, 1, 600);
            panelContentRec.height = GuiSliderBar((Rectangle){ 590, 410, 145, 15 }, "HEIGHT", TextFormat("%i", (int)panelContentRec.height), panelContentRec.height, 1, 400);
            
        }
        EndDrawing();
    }
}

// Draw and process scroll bar style edition controls
static void DrawStyleEditControls(void)
{
    // ScrollPanel style controls
    //----------------------------------------------------------
    GuiGroupBox((Rectangle){ 550, 170, 220, 205 }, "SCROLLBAR STYLE");
    
    int style = GuiGetStyle(SCROLLBAR, BORDER_WIDTH);
    GuiLabel((Rectangle){ 555, 195, 110, 10 }, "BORDER_WIDTH");
    GuiSpinner((Rectangle){ 670, 190, 90, 20 }, NULL, &style, 0, 6, false);
    GuiSetStyle(SCROLLBAR, BORDER_WIDTH, style);
    
    style = GuiGetStyle(SCROLLBAR, ARROWS_SIZE);
    GuiLabel((Rectangle){ 555, 220, 110, 10 }, "ARROWS_SIZE");
    GuiSpinner((Rectangle){ 670, 215, 90, 20 }, NULL, &style, 4, 14, false);
    GuiSetStyle(SCROLLBAR, ARROWS_SIZE, style);
    
    style = GuiGetStyle(SCROLLBAR, SLIDER_PADDING);
    GuiLabel((Rectangle){ 555, 245, 110, 10 }, "SLIDER_PADDING");
    GuiSpinner((Rectangle){ 670, 240, 90, 20 }, NULL, &style, 0, 14, false);
    GuiSetStyle(SCROLLBAR, SLIDER_PADDING, style);
    
    style = GuiCheckBox((Rectangle){ 565, 280, 20, 20 }, "ARROWS_VISIBLE", GuiGetStyle(SCROLLBAR, ARROWS_VISIBLE));
    GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, style);
    
    style = GuiGetStyle(SCROLLBAR, SLIDER_PADDING);
    GuiLabel((Rectangle){ 555, 325, 110, 10 }, "SLIDER_PADDING");
    GuiSpinner((Rectangle){ 670, 320, 90, 20 }, NULL, &style, 0, 14, false);
    GuiSetStyle(SCROLLBAR, SLIDER_PADDING, style);
    
    style = GuiGetStyle(SCROLLBAR, SLIDER_WIDTH);
    GuiLabel((Rectangle){ 555, 350, 110, 10 }, "SLIDER_WIDTH");
    GuiSpinner((Rectangle){ 670, 345, 90, 20 }, NULL, &style, 2, 100, false);
    GuiSetStyle(SCROLLBAR, SLIDER_WIDTH, style);
    
    const char *text = GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE? "SCROLLBAR: LEFT" : "SCROLLBAR: RIGHT";
    style = GuiToggle((Rectangle){ 560, 110, 200, 35 }, text, GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE));
    GuiSetStyle(LISTVIEW, SCROLLBAR_SIDE, style);
    //----------------------------------------------------------
    
    // ScrollBar style controls
    //----------------------------------------------------------
    GuiGroupBox((Rectangle){ 550, 20, 220, 135 }, "SCROLLPANEL STYLE");
    
    style = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
    GuiLabel((Rectangle){ 555, 35, 110, 10 }, "SCROLLBAR_WIDTH");
    GuiSpinner((Rectangle){ 670, 30, 90, 20 }, NULL, &style, 6, 30, false);
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, style);
    
    style = GuiGetStyle(DEFAULT, BORDER_WIDTH);
    GuiLabel((Rectangle){ 555, 60, 110, 10 }, "BORDER_WIDTH");
    GuiSpinner((Rectangle){ 670, 55, 90, 20 }, NULL, &style, 0, 20, false);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, style);
    //----------------------------------------------------------
}
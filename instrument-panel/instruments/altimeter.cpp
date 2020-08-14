#include <stdio.h>
#include <stdlib.h>
#include "altimeter.h"
#include "simvars.h"

altimeter::altimeter(int xPos, int yPos, int size) : instrument(xPos, yPos, size)
{
    setName("Altimeter");
    addVars();
    resize();

    strcpy(hpa, "1013");
    strcpy(inhg, "2992");
    strcpy(stringAltitude, "00000");
}

/// <summary>
/// Destroy and recreate all bitmaps as instrument has been resized
/// </summary>
void altimeter::resize()
{
    destroyBitmaps();

    // Create bitmaps scaled to correct size (original size is 800)
    scaleFactor = size / 800.0f;

    // 0 = Original (loaded) bitmap
    ALLEGRO_BITMAP* orig = loadBitmap("altimeter-pick.bmp");
    addBitmap(orig);

    // 1 = Destination bitmap (all other bitmaps get assembled to here)
    ALLEGRO_BITMAP* dest = al_create_bitmap(size, size);
    addBitmap(dest);

    // 2 = Another original (loaded) bitmap
    ALLEGRO_BITMAP* orig2 = loadBitmap("altimeter-dial.bmp");
    addBitmap(orig2);

    // 3 = Graduated transparency for altitude reels
    ALLEGRO_BITMAP* trans = al_create_bitmap(78 * scaleFactor, 393 * scaleFactor);
    addBitmap(trans);

    // 4 = Graduated transparency for pressure reels
    ALLEGRO_BITMAP* trans2 = al_create_bitmap(45 * scaleFactor, 162 * scaleFactor);
    addBitmap(trans2);

    // 5 = Shadow background
    ALLEGRO_BITMAP* shadowBackground = al_create_bitmap(800 * scaleFactor, 800 * scaleFactor);
    addBitmap(shadowBackground);

    // 6 = Shadow sprite
    ALLEGRO_BITMAP* shadow = al_create_bitmap(43 * scaleFactor, 562 * scaleFactor);
    al_set_target_bitmap(shadow);
    al_draw_scaled_bitmap(orig, 192, 649, 43, 562, 0, 0, 43 * scaleFactor, 562 * scaleFactor, 0);
    addBitmap(shadow);

    // 7 = Fill with background colour (takes 1 pixel of background and scales it)
    ALLEGRO_BITMAP* bg = al_create_bitmap(size, size);
    al_set_target_bitmap(bg);
    al_draw_scaled_bitmap(orig, 75, 61, 1, 1, 0, 0, size, size, 0);
    addBitmap(bg);

    // 8 = Pointer sprite
    ALLEGRO_BITMAP* pointer = al_create_bitmap(43, 562);
    al_set_target_bitmap(pointer);
    al_draw_bitmap_region(orig, 148, 649, 43, 562, 0, 0, 0);
    addBitmap(pointer);

    al_set_target_backbuffer(globals.display);
}

/// <summary>
/// Draw the instrument at the stored position
/// </summary>
void altimeter::render()
{
    // Use normal blender
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    // Draw stuff into dest bitmap
    al_set_target_bitmap(bitmaps[1]);

    // Fill with black
    al_draw_scaled_bitmap(bitmaps[2], 0, 0, 1, 1, 0, 0, size, size, 0);

    // Copy graduated pattern artwork into trans2 bitmap for the small digits
    al_set_target_bitmap(bitmaps[4]);
    al_draw_scaled_bitmap(bitmaps[0], 289, 0, 262, 254, 0, 0, var1, var2, 0);
    al_set_target_bitmap(bitmaps[1]);

    // Copy graduated pattern artwork into trans bitmap
    al_set_target_bitmap(bitmaps[3]);
    al_draw_scaled_bitmap(bitmaps[0], 289, 0, 262, 254, 0, 0, var6, var7, 0);
    al_set_target_bitmap(bitmaps[1]);

    // Draw hPa Reels (x4)
    al_draw_scaled_bitmap(bitmaps[0], 148, var3, 45, 162, var4, var5, var1, var2, 0);
    al_draw_scaled_bitmap(bitmaps[0], 148, var8, 45, 162, var9, var5, var1, var2, 0);
    al_draw_scaled_bitmap(bitmaps[0], 148, var10, 45, 162, var11, var5, var1, var2, 0);
    al_draw_scaled_bitmap(bitmaps[0], 148, var12, 45, 162, var13, var5, var1, var2, 0);

    // Draw IN HG Reels (x4)
    al_draw_scaled_bitmap(bitmaps[0], 148, var14, 45, 162, var4, var15, var1, var2, 0);
    al_draw_scaled_bitmap(bitmaps[0], 148, var16, 45, 162, var9, var15, var1, var2, 0);
    al_draw_scaled_bitmap(bitmaps[0], 148, var17, 45, 162, var11, var15, var1, var2, 0);
    al_draw_scaled_bitmap(bitmaps[0], 148, var18, 45, 162, var13, var15, var1, var2, 0);

    // Draw Altitude reels (x3)
    al_draw_scaled_bitmap(bitmaps[0], 0, var19, 73, 393, var20, var21, var22, var23, 0);
    al_draw_scaled_bitmap(bitmaps[0], 74, var24, 73, 393, var25, var21, var22, var23, 0);
    al_draw_scaled_bitmap(bitmaps[0], 74, var26, 73, 393, var27, var21, var22, var23, 0);

    // Set blender to multiply (shades of grey darken, white has no effect)
    al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, ALLEGRO_ZERO);

    // hPa Reels overlay graduated transparencies
    al_draw_bitmap(bitmaps[4], var4, var5, 0);
    al_draw_bitmap(bitmaps[4], var9, var5, 0);
    al_draw_bitmap(bitmaps[4], var11, var5, 0);
    al_draw_bitmap(bitmaps[4], var13, var5, 0);

    // IN HG Reels overlay the graduated transparencies
    al_draw_bitmap(bitmaps[4], var4, var15, 0);
    al_draw_bitmap(bitmaps[4], var9, var15, 0);
    al_draw_bitmap(bitmaps[4], var11, var15, 0);
    al_draw_bitmap(bitmaps[4], var13, var15, 0);

    // Altitutude Reels overlay the graduated transparencies
    al_draw_bitmap(bitmaps[3], var20, var21, 0);
    al_draw_bitmap(bitmaps[3], var25, var21, 0);
    al_draw_bitmap(bitmaps[3], var27, var21, 0);

    // Restore normal blender
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    if (globals.electrics)
    {
        // Power is on
        // Display power on/off
        al_draw_scaled_bitmap(bitmaps[0], 194, 0, 95, 31, var33, var34, var35, var36, 0);
    }
    else
    {
        // Power is off
        // Display power off/on
        al_draw_scaled_bitmap(bitmaps[0], 194, 30, 95, 31, var33, var34, var35, var36, 0);
    }

    // Display Dial
    al_draw_scaled_bitmap(bitmaps[2], 0, 0, 800, 800, 0, 0, size, size, 0);

    if (globals.enableShadows) {
        // Fill shadow_background bitmap with white
        al_set_target_bitmap(bitmaps[5]);
        al_draw_scaled_bitmap(bitmaps[0], 192, 649, 1, 1, 0, 0, var28, var28, 0);

        // Write shadow sprite to shadow background
        al_draw_rotated_bitmap(bitmaps[6], var30, var31, var37, var29, angle * AngleFactor, 0);
        al_set_target_bitmap(bitmaps[1]);

        // Draw shadow
        // Set blender to multiply (shades of grey darken, white has no effect)
        al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, ALLEGRO_ZERO);
        al_draw_bitmap(bitmaps[5], 0, 0, 0);
        // Restore normal blender
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    }

    //Draw needle pointer at angle
    al_draw_scaled_rotated_bitmap(bitmaps[8], 21, 322, var32, var32, scaleFactor, scaleFactor, angle * AngleFactor, 0);

    if (!globals.connected) {
        // Display 'Not Connected message'
        al_draw_scaled_bitmap(bitmaps[0], 289, 229, 240, 84, 162 * scaleFactor, 318 * scaleFactor, 480 * scaleFactor, 168 * scaleFactor, 0);
    }

    // Position dest bitmap on screen
    al_set_target_backbuffer(globals.display);
    al_draw_bitmap(bitmaps[1], xPos, yPos, 0);
}

/// <summary>
/// Fetch flightsim vars and then update all internal variables
/// that affect this instrument.
/// </summary>
void altimeter::update()
{
    // Check for position or size change
    long *settings = globals.simVars->readSettings(name, xPos, yPos, size);

    xPos = settings[0];
    yPos = settings[1];

    if (size != settings[2]) {
        size = settings[2];
        resize();
    }

    // Get latest FlightSim variables
    globals.connected = fetchVars();

    // Calculate values

    // Calculate what to add to pressure to keep needle in the correct position.
    long altitudeCorrection = 0;
    if (globals.externalControls) {
        altitudeCorrection = (long)(((((float)pressure2 / 0.3386389) / 16) - (((float)16208 / 0.3386389) / 16)) * 10);
        pressure = pressure2;
    }

    sprintf(hpa, "%4.4d", (int)((float)pressure / 16));
    sprintf(inhg, "%4.4d", (int)((((float)pressure / 0.3386389) / 16) + 0));

    if (globals.externalControls)
    {
        //Altitude Value
        units = 2;

        // If altitude is in meters then convert to feet
        altitudeTarget = (long)((float)altitude2 * 3.28084);
    }
    else
    {
        if (units == 0 || units == 1) {
            altitudeTarget = altitude1;
        }
        else if (units == 2) {
            //if altitude is in meters then convert to feet
            altitudeTarget = (long)((float)altitude1 * 3.28084);
        }
    }

    // If manually adjusting pressure cal then add correction offset to altitude
    if (globals.externalControls) {
        altitudeTarget += altitudeCorrection;
    }

    if (altitudeTarget < 0) {
        altitudeTarget = 0;
    }

    if (altitude - altitudeTarget > 500) altitude -= 200;
    else if (altitudeTarget - altitude > 500) altitude += 200;
    else if (altitude - altitudeTarget > 100) altitude -= 40;
    else if (altitudeTarget - altitude > 100) altitude += 40;
    else if (altitude - altitudeTarget > 50) altitude -= 20;
    else if (altitudeTarget - altitude > 50) altitude += 20;
    else if (altitude - altitudeTarget > 10) altitude -= 5;
    else if (altitudeTarget - altitude > 10) altitude += 5;
    else if (altitude - altitudeTarget > 5) altitude -= 2;
    else if (altitudeTarget - altitude > 5) altitude += 2;
    else if (altitude - altitudeTarget > 0) altitude -= 1;
    else if (altitudeTarget - altitude > 0) altitude += 1;

    sprintf(stringAltitude, "%5.5i", altitude);

    // Calculations for pressure displays
    char two_char[2], ones_char[2], tens_char[2];

    var1 = (int)(45 * scaleFactor);
    var2 = (int)(162 * scaleFactor);

    two_char[0] = hpa[0];
    two_char[1] = '\0';
    var3 = (int)((54 * (8 - atoi(two_char)) + 54));
    var4 = (int)(486 * scaleFactor);
    var5 = (int)(157 * scaleFactor);
    var6 = (int)(78 * scaleFactor);
    var7 = (int)(393 * scaleFactor);

    two_char[0] = hpa[1];
    two_char[1] = '\0';
    var8 = (int)((54 * (8 - atoi(two_char)) + 54));
    var9 = (int)(532 * scaleFactor);

    two_char[0] = hpa[2];
    two_char[1] = '\0';
    var10 = (int)((54 * (8 - atoi(two_char)) + 54));
    var11 = (int)(578 * scaleFactor);

    two_char[0] = hpa[3];
    two_char[1] = '\0';
    var12 = (int)((54 * (8 - atoi(two_char)) + 54));
    var13 = (int)(624 * scaleFactor);

    two_char[0] = inhg[0];
    two_char[1] = '\0';
    var14 = (int)((54 * (8 - atoi(two_char)) + 54));
    var15 = (int)(465 * scaleFactor);

    two_char[0] = inhg[1];
    two_char[1] = '\0';
    var16 = (int)((54 * (8 - atoi(two_char)) + 54));

    two_char[0] = inhg[2];
    two_char[1] = '\0';
    var17 = (int)((54 * (8 - atoi(two_char)) + 54));

    two_char[0] = inhg[3];
    two_char[1] = '\0';
    var18 = (int)((54 * (8 - atoi(two_char)) + 54));

    // Calculations for Altitude display
    two_char[0] = stringAltitude[0];
    two_char[1] = '\0';
    ones_char[0] = stringAltitude[4];
    ones_char[1] = '\0';
    tens_char[0] = stringAltitude[3];
    tens_char[1] = '\0';
    int reelOffset;
    if (stringAltitude[1] == '9' && stringAltitude[2] == '9' && stringAltitude[3] == '9') {
        reelOffset = (int)(atoi(tens_char) + (int)(10 * atoi(ones_char)));
    }
    else {
        reelOffset = 0;
    }
    var19 = (int)((131 * (8 - atoi(two_char)) + 262 - reelOffset));
    var20 = (int)(70 * scaleFactor);
    var21 = (int)(202 * scaleFactor);
    var22 = (int)(78 * scaleFactor);
    var23 = (int)(393 * scaleFactor);

    two_char[0] = stringAltitude[1];
    two_char[1] = '\0';
    ones_char[0] = stringAltitude[4];
    ones_char[1] = '\0';
    tens_char[0] = stringAltitude[3];
    tens_char[1] = '\0';
    if (stringAltitude[2] == '9' && stringAltitude[3] == '9') {
        reelOffset = (int)(atoi(tens_char) + (int)(10 * atoi(ones_char)));
    }
    else {
        reelOffset = 0;
    }
    var24 = (int)((131 * (8 - atoi(two_char)) + 262 - reelOffset));
    var25 = (int)(151 * scaleFactor);

    two_char[0] = stringAltitude[2];
    two_char[1] = '\0';
    ones_char[0] = stringAltitude[4];
    ones_char[1] = '\0';
    tens_char[0] = stringAltitude[3];
    tens_char[1] = '\0';
    if (stringAltitude[3] == '9') {
        reelOffset = (int)(atoi(tens_char) + (int)(10 * atoi(ones_char)));
    }
    else {
        reelOffset = 0;
    }
    var26 = (int)((131 * (8 - atoi(two_char)) + 262 - reelOffset));
    var27 = (int)(233 * scaleFactor);

    var28 = (int)(800 * scaleFactor);
    var29 = (int)(420 * scaleFactor);
    var37 = (int)(410 * scaleFactor);
    var30 = (int)(21 * scaleFactor);
    var31 = (int)(322 * scaleFactor);
    var32 = (int)(400 * scaleFactor);
    angle = (((float)(100 * atoi(two_char)) + (float)(10 * atoi(tens_char)) + (float)atoi(ones_char)) * .256256256) - 0.15;

    // Calculations for electrics on/off display
    var33 = (int)(277 * scaleFactor);
    var34 = (int)(681 * scaleFactor);
    var35 = (int)(98 * scaleFactor);
    var36 = (int)(33 * scaleFactor);
}

/// <summary>
/// Add FlightSim variables for this instrument (used for simulation mode)
/// </summary>
void altimeter::addVars()
{
    globals.simVars->addVar(name, "Pressure 1", 0x0330, false, 10, 16212);
    globals.simVars->addVar(name, "Pressure 2", 0x73E2, false, 10, 16212);
    globals.simVars->addVar(name, "Altitude Units (1=Ft, 2=M)", 0x0C18, false, 1, 2);
    globals.simVars->addVar(name, "Altitude 1", 0x3324, false, 1, 0);
    globals.simVars->addVar(name, "Altitude 2", 0x34B0, false, 1, 0);
}

/// <summary>
/// Use SDK to obtain latest values of all flightsim variables
/// that affect this instrument.
/// 
/// Returns false if flightsim is not connected.
/// </summary>
bool altimeter::fetchVars()
{
    bool success = true;
    DWORD result;

    // Value from FlightSim
    if (!globals.simVars->FSUIPC_Read(0x0330, 2, &pressure, &result)) {
        pressure = 16402;
        success = false;
    }

    if (!globals.simVars->FSUIPC_Read(0x0C18, 2, &units, &result)) {
        units = 2;
        return false;
    }

    if (!globals.simVars->FSUIPC_Read(0x3324, 4, &altitude1, &result)) {
        altitude1 = 0;
        success = false;
    }

    if (!globals.simVars->FSUIPC_Read(0x34B0, 8, &altitude2, &result)) {
        altitude2 = 0;
        success = false;
    }

    if (!globals.simVars->FSUIPC_Process(&result))
    {
        success = false;
    }

    // If adjusting pressure cal manually rather than reading FS primus cal knob setting
    if (globals.externalControls)
    {
        // Read pressure as hPa * 16
        if (!globals.simVars->FSUIPC_Read(0x73E2, 2, &pressure2, &result) || !globals.simVars->FSUIPC_Process(&result)) {
            pressure2 = 16208;;
            success = false;
        }

        if (pressure2 < 15168) {
            pressure2 = 15168;
            if (!globals.simVars->FSUIPC_Write(0x73E2, 2, &pressure2, &result) || !globals.simVars->FSUIPC_Process(&result)) {
                success = false;
            }
        }
        else if (pressure2 > 17344) {
            pressure2 = 17344;
            if (!globals.simVars->FSUIPC_Write(0x73E2, 2, &pressure2, &result) || !globals.simVars->FSUIPC_Process(&result)) {
                success = false;
            }
        }
    }
    
    return success;
}

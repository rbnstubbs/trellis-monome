    // Adafruit_Trellis_XY.h

    //  G. Kuetemeyer
    //  03/20/2014
    //  gk@pobox.com

    #if (ARDUINO >= 100)
    #include "Arduino.h"
    #else
    #include "WProgram.h"
    #endif

    void TrellisToXY(uint8_t trellisID, struct TileOffsets &TileOffsets, uint8_t *x, uint8_t *y);
    uint8_t XYToTrellis(uint8_t numKeys, struct TileOffsets &TileOffsets, uint8_t xIn, uint8_t yIn);

    //-----------------------------------------------------------
    // struct for mapping Trellis Tile XY offsets
    //
    // example with 4 Tiles in a square (8X8 buttons/leds)
    //
    // -----
    // |2|3|    layout for 8X8 matrix ----
    // |0|1|    Don't forget to initialize Trellises in order matching layouty...
    // -----
    //
    //
    // struct TileOffsets XYOffsets {
    //   { 0, 4, 0, 4 },
    //   { 0, 0, 4, 4 }
    //};

    struct TileOffsets {
       uint8_t xOffsets[8];
       uint8_t yOffsets[8];
    };

    // array for mapping trellis IDs for a single Tile to X/Y coordinates
    const uint8_t trellisIDs[16][2] = {
//        { 0, 3 },
//      { 1, 3 },
//      { 2, 3 },
//      { 3, 3 },
//      { 0, 2 },
//      { 1, 2 },
//      { 2, 2 },
//      { 3, 2 },
//      { 0, 1 },
//      { 1, 1 },
//      { 2, 1 },
//      { 3, 1 },
//      { 0, 0 },
//      { 1, 0 },
//      { 2, 0 },
//      { 3, 0 },
        { 3, 0 },
        { 2, 0 },
        { 1, 0 },
        { 0, 0 },
        { 3, 1 },
        { 2, 1 },
        { 1, 1 },
        { 0, 1 },
        { 3, 2 },
        { 2, 2 },
        { 1, 2 },
        { 0, 2 },
        { 3, 3 },
        { 2, 3 },
        { 1, 3 },
        { 0, 3 },
    };


    // array for mapping X/Y coordinates to single trellis Tile IDs
    const uint8_t XYTrellisIDs[4][4] = {
      { 12, 13, 14, 15 },
      {  8,  9, 10, 11 },
      {  4,  5,  6,  7 },
      {  0,  1,  2,  3 },
    };

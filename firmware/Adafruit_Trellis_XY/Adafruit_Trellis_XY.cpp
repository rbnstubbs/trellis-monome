    // Adafruit_Trellis_XY.cpp
    //
    // Library for mapping Trellis Tile IDs to XY coordinates
    // and mapping XY coordinates to Trellis Tile IDs
    //  G. Kuetemeyer
    //  03/20/2014
    //  gk@pobox.com

    #include <Adafruit_Trellis_XY.h>

    //----------------------------------------------------------------------
    // map Trellis button/led ids to X/Y coordinates
    void TrellisToXY(uint8_t trellisID, struct TileOffsets &TileOffsets, uint8_t *x, uint8_t *y) {
      uint8_t xOffset;
      uint8_t yOffset;
     
      // get trellis matrix offsets
      if(trellisID < 16) {
        xOffset = TileOffsets.xOffsets[0];
        yOffset = TileOffsets.yOffsets[0];
      }
      else if(trellisID > 15 && trellisID < 32) {
        trellisID = trellisID - 16;
        xOffset = TileOffsets.xOffsets[1];
        yOffset = TileOffsets.yOffsets[1];
      }
      else if(trellisID > 31 && trellisID < 48) {
        trellisID = trellisID - 32;
        xOffset = TileOffsets.xOffsets[2];
        yOffset = TileOffsets.yOffsets[2];
      } 
      else if(trellisID > 47 && trellisID < 64) {
        trellisID = trellisID - 48;
        xOffset = TileOffsets.xOffsets[3];
        yOffset = TileOffsets.yOffsets[3];
      }
      else if(trellisID > 63 && trellisID < 80) {
        trellisID = trellisID - 64;
        xOffset = TileOffsets.xOffsets[4];
        yOffset = TileOffsets.yOffsets[4];
      }
      else if(trellisID > 79 && trellisID < 96) {
        trellisID = trellisID - 80;
        xOffset = TileOffsets.xOffsets[5];
        yOffset = TileOffsets.yOffsets[5];
      } 
      else if(trellisID > 95 && trellisID < 112) {
        trellisID = trellisID - 96;
        xOffset = TileOffsets.xOffsets[6];
        yOffset = TileOffsets.yOffsets[6];
      }
      else if(trellisID > 111 && trellisID < 128) {
        trellisID = trellisID - 112;
        xOffset = TileOffsets.xOffsets[7];
        yOffset = TileOffsets.yOffsets[7];
      }   
      else {
        xOffset = TileOffsets.xOffsets[0];
        yOffset = TileOffsets.yOffsets[0];
      }
     
       // add in offsets
      *x = trellisIDs[trellisID][0] + xOffset;
      *y = trellisIDs[trellisID][1] + yOffset;   
    }

    //----------------------------------------------------------------------
    // map X/Y coordinates to Trellis button/led id
    // (admittedly brute force but really simple.
    // Just pass ids to TrellisToXY until we get a match
    uint8_t XYToTrellis(uint8_t numKeys, struct TileOffsets &TileOffsets, uint8_t xIn, uint8_t yIn) {
      uint8_t xTest;
      uint8_t yTest;
     
      for(uint8_t i = 0; i < numKeys + 1; i++) {
        TrellisToXY(i, TileOffsets, &xTest, &yTest);
        if(xTest == xIn && yTest == yIn) {
          return i;
        }
      }
      return 0; 
    }
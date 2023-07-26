//
// Created by Michael Berger on 7/14/23.
//

#include "bayer.h"

#include "bayer2.h"
#include "bayer4.h"
#include "bayer8.h"

int bayer_value(int rowIndex, int columnIndex, int bayerSize) {
    switch (bayerSize) {
        case BAYER_2:
            return BAYER_2X2[rowIndex % bayerSize][columnIndex % bayerSize];

        case BAYER_4:
            return BAYER_4X4[rowIndex % bayerSize][columnIndex % bayerSize];

        case BAYER_8:
            return BAYER_8X8[rowIndex % bayerSize][columnIndex % bayerSize];

        default:
            return 0;
    }
}

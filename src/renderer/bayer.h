//
// Created by Michael Berger on 7/14/23.
//

#ifndef INC_3D_BAYER_H
#define INC_3D_BAYER_H

enum BayerSize {
    BAYER_2 = 2,
    BAYER_4 = 4,
    BAYER_8 = 8,
};

/**
 * Returns the value of the Bayer matrix at the given row and column index.
 *
 * @param rowIndex The row index.
 * @param columnIndex The column index.
 * @param bayerSize The size of the Bayer matrix. BAER_2, BAYER_4, or BAYER_8.
 * @return The value of the Bayer matrix at the given row and column index.
 */
int bayer_value(int rowIndex, int columnIndex, int bayerSize);

#endif //INC_3D_BAYER_H

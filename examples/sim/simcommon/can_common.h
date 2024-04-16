#pragma once

#include <inttypes.h>

typedef struct CANCOMMON_FRAME_BITS_TYPE {
    uint32_t timestamp : 16;        // [0][15:0] free running counter
    uint32_t DLC : 4;               // [0][19:16] Length of Data bytes
    uint32_t RTR : 1;               // [0][20] Remote Transmition Request
    uint32_t IDE : 1;               // [0][21] ID Extended Bit
    uint32_t SRR : 1;               // [0][22] Substitute Remote Request
    uint32_t rsv023 : 1;            // [0][23]
    uint32_t CODE : 4;              // [0][27:24] Message buffer code: 0xC = Tx Data Frame (RTR must be 0)
    uint32_t rsv028 : 1;            // [0][28]
    uint32_t ESI : 1;               // [0][29] Error state indicator
    uint32_t BRS : 1;               // [0][30] Bit Rate Switch inside CAN FD frame
    uint32_t EDL : 1;               // [0][31] Extended Data Length: 1=CANFD
    uint32_t IDEXT : 18;            // [1][17:0]
    uint32_t IDSTD : 11;            // [1][28:18] Frame ID
    uint32_t PRIO : 3;              // [1][31:29] Local Prioirty
    uint8_t data[64];
} CANCOMMON_FRAME_BITS_TYPE;

typedef union CANCOMMON_FRAME_TYPE {
    uint8_t buf8[4*18];
    uint32_t buf32[18];
    CANCOMMON_FRAME_BITS_TYPE bits;
} CANCOMMON_FRAME_TYPE;


typedef struct CANCOMMON_MSG_TYPE {
    uint32_t busid;
    CANCOMMON_FRAME_TYPE frame;
} CANCOMMON_MSG_TYPE;


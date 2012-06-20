//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------
//
// mbSCRCRC.h - Memory bank class for the Scratchpad section of NVRAM iButtons and
//              1-Wire devices.
//
// Version: 2.10
//
#ifndef MBSCRCRC_H
#define MBSCRCRC_H
#include "ownet.h"


// Local function definitions
SMALLINT readPageScratchCRC(SMALLINT bank, int portnum, uchar *SNum, int page,
                            SMALLINT rd_cont, uchar *buff);
SMALLINT readPageExtraScratchCRC(SMALLINT bank, int portnum, uchar *SNum, int page,
                                 SMALLINT rd_cont, uchar *buff, uchar *extra);
SMALLINT readScratchPadCRC(int portnum, uchar *buff, int len, uchar *extra);
SMALLINT copyScratchPadCRC(int portnum, int str_add, int len);
char *getBankDescriptionScratchCRC(SMALLINT bank, uchar *SNum);
SMALLINT hasPageAutoCRCScratchCRC(SMALLINT bank, uchar *SNum);
#endif

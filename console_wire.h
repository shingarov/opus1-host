/*
 * console_wire.h -- API for console wire protocol parser.
 *
 * Copyright (c) 2009-2014 LADAREVO SOFTWARE INC.
 * LADAREVO ORGELBAU LAB
 *
 */

/*
 * host->console
 */
void setConsoleStopLED(unsigned x, unsigned y, unsigned status);
void set7SegmentLED(unsigned x, unsigned y, unsigned status);

/*
 * host->console
 */
void process_console_control(unsigned char x);


/*
 * Cyclic Redundancy Checking function prototypes
 *
 * $Header: crc.h 1.1 92/09/29 $
 * $Log:	crc.h,v $
 * Revision 1.1  92/09/29  18:02:32  duplain
 * Initial revision
 * 
 */

#ifndef __CRC_H
#define __CRC_H

extern uint32_t crc;
void calccrc(uint8_t byte);

#endif /* __CRC_H */

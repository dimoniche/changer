unsigned short crc16(unsigned char *buf, unsigned char len)
{
    register unsigned char crc_lo, crc_hi, byte;

    crc_lo = 0xFF;
    crc_hi = 0xFF;

    while (len--)
    {
        byte = *(buf++);
        byte ^= crc_hi;

        crc_hi = ( byte & 0x02 ) ? (crc_lo - 0x80) : crc_lo;

        if ( byte & 0x01 )
        crc_hi ^= 0xC0;

        crc_lo = byte;
        crc_lo >>= 1;
        crc_lo ^= byte;
        crc_lo >>= 1;
        byte ^= crc_lo;

        if ( byte & 0x08 )
        --byte;

        if ( byte & 0x40 )
        --byte;

        byte &= 0x01;

        if ( byte )
        crc_lo ^= 0xC0;

        crc_hi ^= byte;
    }

    return (unsigned short)(((unsigned short)crc_hi << 8) | crc_lo);
}
    
    
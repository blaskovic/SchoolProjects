/*******************************
 *
 * DNS Proxy
 * Author: Branislav Blaskovic
 * xblask00@stud.fit.vutbr.cz
 *
 * *****************************/

#include "packet.class.h"

Packet::Packet()
{
}

void Packet::set_packet(uint16_t *packet_to_set)
{
    packet = (TPacket_data *)packet_to_set;
    read_packet();
}

uint8_t Packet::get_piece(TPacket_data *packet, int position)
{
    // Get exact pieace of packet
    return *(((uint8_t*) (packet + 1)) + position);
}

string Packet::get_type()
{
    // Supported types
    switch(qtype)
    {
        case 1:   return string("A");
        case 2:   return string("NS");
        case 3:   return string("MD");
        case 4:   return string("MF");
        case 5:   return string("CNAME");
        case 6:   return string("SOA");
        case 7:   return string("MB");
        case 8:   return string("MG");
        case 9:   return string("MR");
        case 10:  return string("NULL");
        case 11:  return string("WKS");
        case 12:  return string("PTR");
        case 13:  return string("HINFO");
        case 14:  return string("MINFO");
        case 15:  return string("MX");
        case 16:  return string("TXT");
        case 17:  return string("RP");
        case 18:  return string("AFSDB");
        case 19:  return string("X25");
        case 20:  return string("ISDN");
        case 21:  return string("RT");
        case 22:  return string("NSAP");
        case 23:  return string("NSAP-PTR");
        case 24:  return string("SIG");
        case 25:  return string("KEY");
        case 26:  return string("PX");
        case 27:  return string("GPOS");
        case 28:  return string("AAAA");
        case 29:  return string("LOC");
        case 30:  return string("NXT");
        case 31:  return string("EID");
        case 32:  return string("NIMLOC");
        case 33:  return string("SRV");
        case 34:  return string("ATMA");
        case 35:  return string("NAPTR");
        case 36:  return string("KX");
        case 37:  return string("CERT");
        case 38:  return string("A6");
        case 39:  return string("DNAME");
        case 40:  return string("SINK");
        case 41:  return string("OPT");
        case 42:  return string("APL");
        case 43:  return string("DS");
        case 44:  return string("SSHFP");
        case 45:  return string("IPSECKEY");
        case 46:  return string("RRSIG");
        case 47:  return string("NSEC");
        case 48:  return string("DNSKEY");
        case 49:  return string("DHCID");
        case 50:  return string("NSEC3");
        case 51:  return string("NSEC3PARAM");
        case 52:  return string("TLSA");
        case 55:  return string("HIP");
        case 56:  return string("NINFO");
        case 57:  return string("RKEY");
        case 58:  return string("TALINK");
        case 59:  return string("CDS");
        case 99:  return string("SPF");
        case 249: return string("TKEY");
        case 250: return string("TSIG");
        case 251: return string("IXFR");
        case 252: return string("AXFR");
        case 253: return string("MAILB");
        case 254: return string("MAILA");
        case 255: return string("*");
        case 256: return string("URI");
        case 257: return string("CAA");
        case 32768: return string("TA");
        case 32769: return string("DVL");
        default: return string("Type: " + qtype);
    }
}

string Packet::get_domain()
{
    return domain_name;
}

void Packet::read_packet()
{
    // Prepare for reading
    int num_of_chars = get_piece(packet, 0);
    int offset = 1;
    domain_name = string("");

    // Loop till no more chars can be read
    while(num_of_chars > 0)
    {
        // Loop through 
        for(int i = 0; i < num_of_chars; i++)
        {
            domain_name = string(domain_name) + (char)get_piece(packet, offset + i);
        }

        // Add offset of read chars and read the amount of new ones
        offset += num_of_chars;
        num_of_chars = get_piece(packet, offset);
        
        // It's not the last block of chars
        if(num_of_chars)
        {
            domain_name = domain_name + string(".");
        }

        offset++;
    }

    // Read the type of packet
    qtype = (uint16_t) get_piece(packet, offset+1);
}


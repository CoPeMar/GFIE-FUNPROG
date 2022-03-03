/*
 * main.c
 *
 *  Created on: Feb 24, 2022
 *      Author: atcsol
 */


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

int main() {

    int fd;
    uint8_t read_byte, tc_bytes[256];

    uint16_t packet_id;
    uint16_t nbytes = 0;
    uint16_t packet_seq_ctrl;
    uint16_t packet_len;
    uint32_t df_header;
    uint16_t packet_err_ctrl;

    fd = open("single-tc-p2-2.bin", O_RDONLY);

    /* Read Packet ID and store it into packet_id */

    read(fd, &read_byte, 1);		// Read MSB
    tc_bytes[0] = read_byte;
    nbytes++;
    packet_id = read_byte;
    packet_id = packet_id << 8;     // Shift it 8 bits to the left

    read(fd, &read_byte, 1);            // Read LSB
    tc_bytes[1] = read_byte;
    nbytes++;
    packet_id = packet_id | read_byte;  // OR the MSB and the LSB

    read(fd, &read_byte, 1);
    tc_bytes[2] = read_byte;
    nbytes++;
    packet_seq_ctrl = read_byte;
    packet_seq_ctrl = packet_seq_ctrl << 8;

    read(fd, &read_byte, 1);
    tc_bytes[3] = read_byte;
    nbytes++;
    packet_seq_ctrl = packet_seq_ctrl | read_byte;

    read(fd, &read_byte, 1);
    tc_bytes[4] = read_byte;
    nbytes++;
    packet_len = read_byte;
    packet_len = packet_len << 8;

    read(fd, &read_byte, 1);
    tc_bytes[5] = read_byte;
    nbytes++;
    packet_len = packet_len | read_byte;

    read(fd, &read_byte, 1);
    tc_bytes[6] = read_byte;
    nbytes++;
    df_header = read_byte;
    df_header = df_header << 8;

    read(fd, &read_byte, 1);
    tc_bytes[7] = read_byte;
    nbytes++;
    df_header = df_header | read_byte;
    df_header = df_header << 8;

    read(fd, &read_byte, 1);
    tc_bytes[8] = read_byte;
    nbytes++;
    df_header = df_header | read_byte;
    df_header = df_header << 8;

    read(fd, &read_byte, 1);
    tc_bytes[9] = read_byte;
    nbytes++;
    df_header = df_header | read_byte;

    for(uint16_t i = 0; i < packet_len - 5; i = i + 1, nbytes = nbytes + 1){
        read(fd, &read_byte, 1);
        tc_bytes[10+i] = read_byte;
        }

    read(fd, &read_byte, 1);
    packet_err_ctrl = read_byte;
    packet_err_ctrl = packet_err_ctrl << 8;

    read(fd, &read_byte, 1);
    packet_err_ctrl = packet_err_ctrl | read_byte;

    uint16_t crc_value = 0xFFFF;

    for (uint16_t i = 0; i < nbytes; i++ ) {

        crc_value = crc_value ^ (tc_bytes[i] << 8);

        for (uint8_t j = 0; j < 8; j++) {

            if ((crc_value & 0x8000) != 0) {
                crc_value = (crc_value << 1) ^ 0x1021;
            } else {
                crc_value = crc_value << 1;
            }
        }
    }

    printf("Packet ID: 0x%X\n", packet_id);
    printf("Packet Sequence Control: 0x%X\n", packet_seq_ctrl);
    printf("Packet Length: 0x%X\n", packet_len);
    printf("Data Field Header: 0x%X\n", df_header);
    printf("Packet Error Control: 0x%X\n", packet_err_ctrl);
    printf("APID: 0x%X\n", (packet_id & 0x07FF));
    printf("Sequence flags: 0x%X\n", ((packet_seq_ctrl >> 14) & 0x0003));
    printf("Sequence count: %d\n", (packet_seq_ctrl & 0x3FFF));
    printf("Ack: 0x%X\n", ((df_header >> 24) & 0x000F));
    printf("Service type: %d\n", ((df_header >> 16) & 0x00FF));
    printf("Service subtype: %d\n", ((df_header >> 8) & 0x00FF));
    printf("Source ID: 0x%X\n", (df_header & 0x00FF));

    if (packet_err_ctrl == crc_value){
         	printf("Expected CRC value 0x%X, Calculated CRC value 0x%X: OK\n", packet_err_ctrl, crc_value);
         } else {
         	printf("Expected CRC value 0x%X, Calculated CRC value 0x%X: FAIL\n", packet_err_ctrl, crc_value);
         }

    /*uint8_t byte_to_write;

    uint16_t tm_packet_id = 0;

    fd = open("single-tm.bin", O_WRONLY | O_CREAT | O_TRUNC, 0664);

    tm_packet_id = tm_packet_id | (1 << 11);

    tm_packet_id = tm_packet_id | (0x32C);

    byte_to_write = (tm_packet_id & 0xFF00) >> 8;
    write(fd, &byte_to_write, 1);

    byte_to_write = (tm_packet_id & 0x00FF);
    write(fd, &byte_to_write, 1);

    tm_packet_id = (0x3 << 14);

    byte_to_write = (tm_packet_id & 0xFF00) >> 8;
    write(fd, &byte_to_write, 1);

    byte_to_write = (tm_packet_id & 0x00FF);
    write(fd, &byte_to_write, 1);

    tm_packet_id = 0x7;

    byte_to_write = (tm_packet_id & 0xFF00) >> 8;
    write(fd, &byte_to_write, 1);

    byte_to_write = (tm_packet_id & 0x00FF);
    write(fd, &byte_to_write, 1);

    tm_packet_id = 0x1001;

    byte_to_write = (tm_packet_id & 0xFF00) >> 8;
    write(fd, &byte_to_write, 1);

    byte_to_write = (tm_packet_id & 0x00FF);
    write(fd, &byte_to_write, 1);

    tm_packet_id = 0x0178;

    byte_to_write = (tm_packet_id & 0xFF00) >> 8;
    write(fd, &byte_to_write, 1);

    byte_to_write = (tm_packet_id & 0x00FF);
    write(fd, &byte_to_write, 1);

    tm_packet_id = packet_id;

    byte_to_write = (tm_packet_id & 0xFF00) >> 8;
    write(fd, &byte_to_write, 1);

    byte_to_write = (tm_packet_id & 0x00FF);
    write(fd, &byte_to_write, 1);

    tm_packet_id = packet_seq_ctrl;

    byte_to_write = (tm_packet_id & 0xFF00) >> 8;
    write(fd, &byte_to_write, 1);

    byte_to_write = (tm_packet_id & 0x00FF);
    write(fd, &byte_to_write, 1);

    close(fd);



    close(fd);
    */
    return 0;

}

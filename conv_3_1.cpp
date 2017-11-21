#include <stdint.h>
#define SIGN_EXTEND(start, end, x) (((x) & ((1ULL << (start)) - 1)) | (((x) & (1ULL << ((start) - 1))) ? (((1ULL << ((end) - (start))) - 1) << (start)) : 0))


#define MASK(width, expr) (((1ULL << (width)) - 1) & ((expr)))

void simulate(unsigned char* state) {

// Variable declarations

// Internal variables
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$zero_const_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$max_const_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$c1_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$add_w_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$waddr_eq_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$waddr_mux_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$veq_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$add_r_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$raddr_eq_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$raddr_mux_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$zero_const_out;
uint8_t  lb_p4_clamped_stencil_update_stream_wen_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$waddr$enMux_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$raddr$enMux_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$waddr$reg0_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_2$raddr$reg0_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$max_const_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$c1_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$add_w_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$waddr_eq_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$waddr_mux_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$waddr$enMux_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$waddr$reg0_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$veq_out;
uint16_t  lb_p4_clamped_stencil_update_stream$mem_2$mem_rdata;
uint16_t  lb_p4_clamped_stencil_update_stream$mem_1$mem_rdata;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$add_r_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$raddr_eq_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$raddr_mux_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$raddr$enMux_out;
uint8_t  lb_p4_clamped_stencil_update_stream$mem_1$raddr$reg0_out;
uint16_t  const7_7_out;
uint16_t  mul_3507_351_out;
uint16_t  const5_5_out;
uint16_t  mul_3465_347_out;
uint16_t  const3_3_out;
uint16_t  mul_3423_343_out;
uint16_t  const0_0_out;
uint16_t  add_340_343_344_out;
uint16_t  add_340_347_348_out;
uint16_t  add_340_351_352_out;

// Simulation code

// ----- Update outputs of sequential elements
lb_p4_clamped_stencil_update_stream$mem_2$waddr$reg0_out = *((uint8_t*)(state + 9));
lb_p4_clamped_stencil_update_stream$mem_2$raddr$reg0_out = *((uint8_t*)(state + 8));
lb_p4_clamped_stencil_update_stream$mem_2$mem_rdata = (((uint16_t*)(state + 30))[ *((uint8_t*)(state + 8)) ]);
lb_p4_clamped_stencil_update_stream$mem_1$waddr$reg0_out = *((uint8_t*)(state + 7));
lb_p4_clamped_stencil_update_stream$mem_1$raddr$reg0_out = *((uint8_t*)(state + 6));
lb_p4_clamped_stencil_update_stream$mem_1$mem_rdata = (((uint16_t*)(state + 10))[ *((uint8_t*)(state + 6)) ]);

// ----- Update combinational logic
lb_p4_clamped_stencil_update_stream$mem_2$zero_const_out = 0b0000;
lb_p4_clamped_stencil_update_stream$mem_2$max_const_out = 0b1010;
lb_p4_clamped_stencil_update_stream$mem_2$c1_out = 0b0001;
lb_p4_clamped_stencil_update_stream$mem_2$add_w_out = MASK( 4, (*((uint8_t*)(state + 9)) + /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$c1_out) );
lb_p4_clamped_stencil_update_stream$mem_2$add_r_out = MASK( 4, (*((uint8_t*)(state + 8)) + /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$c1_out) );
lb_p4_clamped_stencil_update_stream$mem_1$zero_const_out = 0b0000;
lb_p4_clamped_stencil_update_stream_wen_out = 1;
lb_p4_clamped_stencil_update_stream$mem_1$max_const_out = 0b1010;
lb_p4_clamped_stencil_update_stream$mem_1$c1_out = 0b0001;
lb_p4_clamped_stencil_update_stream$mem_1$add_w_out = MASK( 4, (*((uint8_t*)(state + 7)) + /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$c1_out) );
lb_p4_clamped_stencil_update_stream$mem_1$add_r_out = MASK( 4, (*((uint8_t*)(state + 6)) + /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$c1_out) );
*((uint16_t*)(state + 4)) = (((0b0000000000000000 + (lb_p4_clamped_stencil_update_stream$mem_2$mem_rdata * 0b0000000000000011)) + (lb_p4_clamped_stencil_update_stream$mem_1$mem_rdata * 0b0000000000000101)) + (*((uint16_t*)(state + 2)) * 0b0000000000000111));

// ----- Update stored state in sequential elements
*((uint8_t*)(state + 9)) = ((((*((uint8_t*)(state + 1)) == 0) && (*((uint8_t*)(state + 0)) == 1))) ? (/* LOCAL */lb_p4_clamped_stencil_update_stream_wen_out ? (MASK( 1, (/* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$add_w_out == /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$max_const_out) ) ? /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$zero_const_out : /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$add_w_out) : *((uint8_t*)(state + 9))) : *((uint8_t*)(state + 9)));
*((uint8_t*)(state + 8)) = ((((*((uint8_t*)(state + 1)) == 0) && (*((uint8_t*)(state + 0)) == 1))) ? (/* LOCAL */lb_p4_clamped_stencil_update_stream_wen_out ? (MASK( 1, (/* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$add_r_out == /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$max_const_out) ) ? /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$zero_const_out : /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$add_r_out) : *((uint8_t*)(state + 8))) : *((uint8_t*)(state + 8)));
*((uint8_t*)(state + 7)) = ((((*((uint8_t*)(state + 1)) == 0) && (*((uint8_t*)(state + 0)) == 1))) ? (/* LOCAL */lb_p4_clamped_stencil_update_stream_wen_out ? (MASK( 1, (/* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$add_w_out == /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$max_const_out) ) ? /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$zero_const_out : /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$add_w_out) : *((uint8_t*)(state + 7))) : *((uint8_t*)(state + 7)));
((uint16_t*)(state + 30))[ *((uint8_t*)(state + 9)) ] = ((((*((uint8_t*)(state + 1)) == 0) && (*((uint8_t*)(state + 0)) == 1)) && /* LOCAL */lb_p4_clamped_stencil_update_stream_wen_out) ? lb_p4_clamped_stencil_update_stream$mem_1$mem_rdata : ((uint16_t*)(state + 30))[ *((uint8_t*)(state + 9)) ]);
((uint16_t*)(state + 10))[ *((uint8_t*)(state + 7)) ] = ((((*((uint8_t*)(state + 1)) == 0) && (*((uint8_t*)(state + 0)) == 1)) && /* LOCAL */lb_p4_clamped_stencil_update_stream_wen_out) ? *((uint16_t*)(state + 2)) : ((uint16_t*)(state + 10))[ *((uint8_t*)(state + 7)) ]);
*((uint8_t*)(state + 6)) = ((((*((uint8_t*)(state + 1)) == 0) && (*((uint8_t*)(state + 0)) == 1))) ? (/* LOCAL */lb_p4_clamped_stencil_update_stream_wen_out ? (MASK( 1, (/* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$add_r_out == /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$max_const_out) ) ? /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$zero_const_out : /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$add_r_out) : *((uint8_t*)(state + 6))) : *((uint8_t*)(state + 6)));

// ----- Update outputs of sequential elements
lb_p4_clamped_stencil_update_stream$mem_2$waddr$reg0_out = *((uint8_t*)(state + 9));
lb_p4_clamped_stencil_update_stream$mem_2$raddr$reg0_out = *((uint8_t*)(state + 8));
lb_p4_clamped_stencil_update_stream$mem_2$mem_rdata = (((uint16_t*)(state + 30))[ *((uint8_t*)(state + 8)) ]);
lb_p4_clamped_stencil_update_stream$mem_1$waddr$reg0_out = *((uint8_t*)(state + 7));
lb_p4_clamped_stencil_update_stream$mem_1$raddr$reg0_out = *((uint8_t*)(state + 6));
lb_p4_clamped_stencil_update_stream$mem_1$mem_rdata = (((uint16_t*)(state + 10))[ *((uint8_t*)(state + 6)) ]);

// ----- Update combinational logic
lb_p4_clamped_stencil_update_stream$mem_2$zero_const_out = 0b0000;
lb_p4_clamped_stencil_update_stream$mem_2$max_const_out = 0b1010;
lb_p4_clamped_stencil_update_stream$mem_2$c1_out = 0b0001;
lb_p4_clamped_stencil_update_stream$mem_2$add_w_out = MASK( 4, (*((uint8_t*)(state + 9)) + /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$c1_out) );
lb_p4_clamped_stencil_update_stream$mem_2$add_r_out = MASK( 4, (*((uint8_t*)(state + 8)) + /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_2$c1_out) );
lb_p4_clamped_stencil_update_stream$mem_1$zero_const_out = 0b0000;
lb_p4_clamped_stencil_update_stream_wen_out = 1;
lb_p4_clamped_stencil_update_stream$mem_1$max_const_out = 0b1010;
lb_p4_clamped_stencil_update_stream$mem_1$c1_out = 0b0001;
lb_p4_clamped_stencil_update_stream$mem_1$add_w_out = MASK( 4, (*((uint8_t*)(state + 7)) + /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$c1_out) );
lb_p4_clamped_stencil_update_stream$mem_1$add_r_out = MASK( 4, (*((uint8_t*)(state + 6)) + /* LOCAL */lb_p4_clamped_stencil_update_stream$mem_1$c1_out) );
*((uint16_t*)(state + 4)) = (((0b0000000000000000 + (lb_p4_clamped_stencil_update_stream$mem_2$mem_rdata * 0b0000000000000011)) + (lb_p4_clamped_stencil_update_stream$mem_1$mem_rdata * 0b0000000000000101)) + (*((uint16_t*)(state + 2)) * 0b0000000000000111));

}


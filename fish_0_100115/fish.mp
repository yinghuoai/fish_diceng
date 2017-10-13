NOTE: AVR Code addresses are word addresses
All other addresses (including ones in FLASH) are byte addresses

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                        func_lit   008C   0020 =     32. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       008C  __func_lit_start
       00AC  __func_lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             lit   00AC   0028 =     40. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       00AC  __lit_start
       00D4  __lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                           idata   00D4   0323 =    803. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       00D4  __idata_start
       03F7  __idata_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   03F8   3752 =  14162. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       01FC  __text_start
       01FC  __start
       021E  _exit
       021F  _delay
       024C  _delay_1ms
       0256  _delay_nms
       0264  _port_init
       027D  _timer3_init
       0297  _timer1_init
       02A8  _uart0_init
       02B6  _init_devices
       02CA  _senddata
       02CE  _uart_putchar
       02D2  _uart_word
       02E5  _Sin
       0490  _init_static_offset
       04B9  _init_dynamic_offset
       04E5  _calculate_data
       0645  _init_data
       065E  _char2int
       0670  _one2two
       06AC  _two2one
       06E7  _one2three
       073F  _three2one
       0795  _one2six
       0844  _six2one
       08EE  _change_dd
       0905  _change_flag
       090F  _i2c_write
       0935  _i2c_read
       0966  _i2c_init
       0BBF  _uart0_rx_isr
       1074  _timer3_ovf_isr
       10CE  _timer1_ovf_isr
       111B  _main
       18EE  _TWI_Master_Initialise
       18F8  _TWI_Transceiver_Busy
       18FC  _TWI_Get_State_Info
       1902  _TWI_Start_Transceiver_With_Data
       192D  _TWI_Start_Transceiver
       193A  _TWI_Get_Data_From_Transceiver
       195A  _twi_isr
       1A35  push_arg4
       1A37  push_arg2
       1A3A  mod16s
       1A3E  div16s
       1A58  mod16u
       1A5A  div16u
       1A5B  xdiv16u
       1A74  mod8u
       1A76  div8u
       1A77  xdiv8u
       1A8C  elpm32
       1AA0  empy16s
       1AB0  pop_xgset003C
       1AB5  pop_xgset00FC
       1ABC  pop_xgsetF0FC
       1AC7  push_xgsetF0FC
       1AC9  push_xgset30FC
       1ACB  push_xgset00FC
       1ACD  push_xgset003C
       1AD2  push_xgset300C
       1AD7  pop_xgset300C
       1ADC  push_xgset303C
       1AE3  pop_xgset303C
       1AEA  push_xgsetF03C
       1AF3  pop_xgsetF03C
       1AFC  neg32
       1B05  fpsub
       1B07  fpadd
       1B08  fpadd_alt
       1B6F  fpadd2
       1B7A  fpsub1
       1B84  fpsub2
       1B8F  fpint
       1BC1  uint2fp
       1BC3  int2fp
       1BCC  ulong2fp
       1BCE  long2fp
       1BE5  fpdiv1
       1BEE  fpdiv2
       1BF8  fpdiv1x
       1C04  fpdiv2x
       1C7F  unpacks
       1C87  tstR16_R19
       1C8F  unpacku
       1CA8  normalize_and_pack
       1CDD  pack
       1CED  saveFPRegs
       1CFC  restoreFPRegs
       1D0B  fpmule
       1D5E  fpmule1
       1D68  fpmule2
       1D73  fpmule2x
       1D81  fpcmp
       1D8E  fpcmp1
       1D99  fpcmp2
       1DA5  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            data   0100   0323 =    803. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0100  __data_start
       0100  _Speed
       0101  _Instant
       0102  _redata_fan
       0103  _id_fan
       0104  _baocun_i
       0105  _t_dynamic
       0106  _Speed_dynamic
       0107  _n
       0108  _m
       0109  _s
       010A  _FISHID
       010B  _FISHID_init
       010C  _FISHID_I2C
       010F  _freqs
       0110  _freqs_init
       0111  _freqs_I2C
       0114  _Static_Offset_degree
       0117  _Static_Offset_degree_init
       011A  _degree_I2C
       011F  _Speed_tem_init
       012F  _Speed_tem_I2C
       0141  _Speed_tem_init1
       0149  _Speed_tem_init2
       0151  _Speed_tem_z
       0161  _Speed_tem_z1
       0169  _Speed_tem_z2
       0171  _Speed_tem_I2C1
       017B  _Speed_tem_I2C2
       0185  _Neighbouring_Offset_degree_init
       0188  _Neighbouring_Offset_degree_I2C
       018D  _Amplitude_degree_tem_init
       01BD  _Amplitude_degree_tem_init1
       01C5  _Amplitude_degree_tem_init2
       01CD  _Amplitude_degree_tem_init3
       01D5  _Amplitude_degree_tem_init4
       01DD  _Amplitude_degree_tem_init5
       01E5  _Amplitude_degree_tem_init6
       01ED  _Amplitude_degree_tem_z
       021D  _Amplitude_degree_tem_z1
       0225  _Amplitude_degree_tem_z2
       022D  _Amplitude_degree_tem_z3
       0235  _Amplitude_degree_tem_z4
       023D  _Amplitude_degree_tem_z5
       0245  _Amplitude_degree_tem_z6
       024D  _Amplitude_degree_tem_I2C1
       0257  _Amplitude_degree_tem_I2C2
       0261  _Amplitude_degree_tem_I2C3
       026B  _Amplitude_degree_tem_I2C4
       0275  _Amplitude_degree_tem_I2C5
       027F  _Amplitude_degree_tem_I2C6
       0289  _Dynamic_Offset_degree_origin_init
       02A1  _Dynamic_Offset_degree_origin_init1
       02A9  _Dynamic_Offset_degree_origin_init2
       02B1  _Dynamic_Offset_degree_origin_init3
       02B9  _Dynamic_Offset_degree_origin_z
       02D1  _Dynamic_Offset_degree_origin_z1
       02D9  _Dynamic_Offset_degree_origin_z2
       02E1  _Dynamic_Offset_degree_origin_z3
       02E9  _Dynamic_Offset_degree_origin_I2C1
       02F3  _Dynamic_Offset_degree_origin_I2C2
       02FD  _Dynamic_Offset_degree_origin_I2C3
       0307  _isSrlStart
       0308  _isSrlStartI2c
       0309  _addr
       030A  _addrI2c
       030B  _s_change
       030C  _d_change
       030D  _d_change_t
       030E  _d_change_t_v
       030F  _t_change
       0310  _ts_change
       0311  _a_change
       0312  _f_change
       0313  _id_change
       0314  _e_change
       0315  _i_change
       0316  _i2c_change
       0317  _isInstruction
       0318  _isInstructionI2c
       0319  _is_uart
       031A  _m_change
       031B  _m_cishu
       031C  _mm_cishu
       031D  _mmm_cishu
       031E  _a_cishu
       031F  _aa_cishu
       0320  _aaa_cishu
       0321  _cishu
       0322  _auto_cishu
       0323  _y_change
       0324  _y_changing
       0325  _yy_time
       0326  _y_cishu
       0327  _yy_cishu
       0328  _yyy_cishu
       0329  _cishu_y
       032A  _y_tem
       032B  _y_speed
       032C  _s_tem
       032D  _m_tem
       032E  _d_tem
       032F  _dd_tem
       0330  _flag
       0331  _i2c_canshu
       0332  _freq
       0336  _Speed_tem
       0346  _Dynamic_Offset_degree_origin
       035E  _Dynamic_Offset_degree_y
       03BE  _Joint_Angle_value
       03C4  _Static_Offset_value
       03CA  _Dynamic_Offset_degree
       03D0  _Dynamic_Offset_degree_char
       03D3  _Dynamic_Offset_value
       03D9  _Dynamic_Offset_value_per
       03DF  _Dynamic_Offset_value_last
       03E5  _Dynamic_Offset_value_t
       03EB  _Neighbouring_Offset_degree
       03EE  _Amplitude_degree
       03F1  _Amplitude_degree_tem
       0421  TWI_Master.c:TWI_state
       0422  _TWI_statusReg
       0423  __data_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             bss   0423   0011 =     17. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0423  _redata_i2c
       0423  __bss_start
       0424  _redata_tem
       0425  _redata
       0426  _PWM_Period_value
       0428  _MiddlePosition_value
       042A  _RightLimit_value
       042C  _LeftLimit_value
       042E  TWI_Master.c:TWI_bufPtr
       042F  TWI_Master.c:TWI_msgSize
       0430  _TWI_buf
       0434  __bss_end

Area                               Start  End    Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                          vector   0000   0088 =    136. bytes (abs,ovr,rom)

Files Linked      [ module(s) ]

C:\iccv7avr\lib\crtatmega.o	[ crtatmega.s ]
11fish.o	[ _11fish.c ]
TWI_Master.o	[ TWI_Master.c ]
<library>	[ asave.s, div16s.s, div16u.s, div8u.s, elpm32.s, emul16s.s, gpop2.s, gpop3.s, gpop5.s, gpush.s, gpushx.s ]
[ gpusha.s, gpushb.s, lneg.s, fpadd.s, fpadd2.s, fpsub1.s, fpsub2.s, fpint.s, int2fp.s, fpdiv_new.s, fputil.s ]
[ fpmule.s, fpmule1.s, fpmule2.s, fpmule2x.s, fpcmp.s, fpcmp1.s, fpcmp2.s ]

User Global Definitions

ram_end = 0x10ff
hwstk_size = 0x10

User Base Address Definitions

func_lit = 0x8c
data = 0x100
eeprom:0.4096


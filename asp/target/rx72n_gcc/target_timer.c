/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
 *  Copyright (C) 2003-2004 by Platform Development Center
 *                                          RICOH COMPANY,LTD. JAPAN
 *  Copyright (C) 2008-2010 by Witz Corporation, JAPAN
 *  Copyright (C) 2016- by Hisashi Hata, JAPAN
 *  Copyright (C) 2023 wolfSSL Inc.
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 */

/*
 *		タイマドライバ（RX72N)
 */

#include "kernel_impl.h"
#include "time_event.h"
#include <sil.h>
#include "target_timer.h"

/*
 *  タイマの初期化処理
 */
void
target_timer_initialize(intptr_t exinf)
{
  //unlock register access 
  sil_wrh_mem((void *)(SYSTEM_PRCR_ADDR), SYSTEM_PRKEY | SYSTEM_PRC1);
  /*
   * モジュールストップ機能の設定(CMT)
   */
  *SYSTEM_MSTPCRA_ADDR &= ~(SYSTEM_MSTPCRA_MSTPA15_CMT_U0); /* CMT0_UNIT 0 */
  //lock register access
  sil_wrh_mem((void *)(SYSTEM_PRCR_ADDR), SYSTEM_PRKEY );

  /* 
   * タイマ停止 
   */
  *CMT_CMSTR0_ADDR &= ~CMT_CMSTR0_STR0_BIT;

  /*
   * カウントアップに用いられるクロック設定
   * PCLKB/8を選択
   */
 // *CMT0_CMCR_ADDR = CMT_PCLK_DIV_8;
  *CMT0_CMCR_ADDR = (0x0001U);
  /* 
   * コンペアマッチタイマカウンタ設定
   */
  *CMT0_CMCNT_ADDR = 0U;

  /* 
   * コンペアマッチタイマ周期設定
   */
  /* CMT0 Toppers Setting 10msec  */
  *CMT0_CMCOR_ADDR = RX72N_CMCOR ;// CMCOR_PERIOD ;// 18749;//CMCOR_PERIOD;

  /*
   *  タイマ動作開始前の割込み要求をクリア
   */
	x_clear_int(INTNO_TIMER);

  /* 
   * コンペアマッチタイマ割り込みを許可
   */
   *CMT0_CMCR_ADDR |= CMT0_CMCR_CMIE_BIT;

  /* 
   * タイマ動作開始
   */
  *CMT_CMSTR0_ADDR |= CMT_CMSTR0_STR0_BIT;
}
void
target_timer_cmt1_initialize(intptr_t exinf)
{
	//to BSP initialize
}
void
target_timer_cmt2_initialize(intptr_t exinf)
{
	//to BSP initialize
}
/*
 *  タイマの停止処理
 */
void
target_timer_terminate(intptr_t exinf)
{
	/* 
	 * タイマ停止 
	 */
	*CMT_CMSTR0_ADDR &= ~CMT_CMSTR0_STR0_BIT;
}
void
target_timer_cmt1_terminate(intptr_t exinf)
{
 // renesas bsp
}
void
target_timer_cmt2_terminate(intptr_t exinf)
{
// renesas bsp
}

/*
 *  タイマ割込みハンドラ
 */
void target_timer_handler(void)
{
	i_begin_int(INTNO_TIMER);
	
	signal_time();				/* タイムティックの供給 */
	i_end_int(INTNO_TIMER);
}
void target_timer_cmt1_handler(void)
{
	i_begin_int(INHNO_TIMER_CMT1);

	signal_time();				/* タイムティックの供給 */
	timer_cm1_int_Wrapper();
	i_end_int(INHNO_TIMER_CMT1);
}
void target_timer_cmt2_handler(void)
{
	i_begin_int(INHNO_TIMER_CMT2);

	signal_time();				/* タイムティックの供給 */
	timer_cm2_int_Wrapper();
	i_end_int(INHNO_TIMER_CMT2);
}

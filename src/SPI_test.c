/***********************************************************************
*
*  FILE        : SPI_test.c
*  DATE        : 2024-10-16
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
/***********************************************************************************************************************
	Includes
***********************************************************************************************************************/
/* Code　Generation Module */
#include "r_smc_entry.h"
#include "Config_CMT0.h"
#include "Config_RSPI0.h"

/* For Debug(sprintf) */
#include <stdio.h>

/***********************************************************************************************************************
	DEFINE
***********************************************************************************************************************/
/* SPI Tx Command */
#define TX_CMD0			(0x000F)
#define TX_CMD1			(0x00F0)
#define TX_CMD2			(0x0F00)
#define TX_CMD3			(0xF000)

/* SPI Command Length */
#define TX_CMD_LEN_4	(4)


/***********************************************************************************************************************
	External RAM
***********************************************************************************************************************/

/***********************************************************************************************************************
	Public RAM
***********************************************************************************************************************/
/* SPI */
volatile uint16_t Rspi0_tx_buf[4];
volatile uint16_t Rspi0_rx_buf[4];

/***********************************************************************************************************************
	Static RAM
***********************************************************************************************************************/

/***********************************************************************************************************************
	ROM
***********************************************************************************************************************/

/***********************************************************************************************************************
	Function
***********************************************************************************************************************/
void main(void);
void CallBack_CMT_100ms(void);

/**********************************************************************************************************************
* Function Name: main
* Description  : タイマ/通信の開始とMainループ
*				 【実行内容】
*				 1.RSPI通信開始
*				 2.100[ms]周期(10[Hz])コンペアマッチタイマ割り込みの設定
* Arguments    : None
* Return Value : None
**********************************************************************************************************************/
void main(void)
{
	/* 1.RSPI通信開始 */
	R_Config_RSPI0_Start();

	/* 3.100[ms]周期(10[Hz])コンペアマッチタイマ割り込みの設定 */
	R_Config_CMT0_Start();

	while (1) {
		/* IDLE状態 */
		/* 割り込み待ち(何もしない) */
	}

}

/**********************************************************************************************************************
* Function Name: CallBack_CMT_100ms
* Description  : 100[ms]周期(10[Hz])コンペアマッチタイマ割り込み
*				 【実行内容】
*				 1.SPI送受信前処理
*				 2.SPI送受信開始
* Arguments    : None
* Return Value : None
**********************************************************************************************************************/
void CallBack_CMT_100ms(void)
{
	bool rx_end,tx_end;			/* 送受信完了フラグ */

	/* 送受信状態を取得 */
	R_BSP_InterruptsDisable();	/* 処理中の更新を避けるため割り込み禁止 */

	rx_end = Get_RSPI_RxEnd();
	tx_end = Get_RSPI_TxEnd();

	R_BSP_InterruptsEnable();	/* 割り込み許可 */


	/* 1.SPI送受信前処理 */
	if( rx_end == true ) {
		/* 受信完了フラグクリア */
		Clr_RSPI_RxEnd();

		/* 受信済みなら受信データをコンソールへ表示 */
		printf("RX Data[0]=%d\n", Rspi0_rx_buf[0]);
		printf("RX Data[1]=%d\n", Rspi0_rx_buf[1]);
		printf("RX Data[2]=%d\n", Rspi0_rx_buf[2]);
		printf("RX Data[3]=%d\n", Rspi0_rx_buf[3]);
	}

	/* 2.SPI送受信開始 */
	if( tx_end == true ) {
		/* 送信完了フラグクリア */
		Clr_RSPI_TxEnd();

		/* 送信済みなら送信データを更新し、送信処理を実行 */
		Rspi0_tx_buf[0] = TX_CMD0;
		Rspi0_tx_buf[1] = TX_CMD1;
		Rspi0_tx_buf[2] = TX_CMD2;
		Rspi0_tx_buf[3] = TX_CMD3;

		R_Config_RSPI0_Send_Receive((uint16_t *)Rspi0_tx_buf, TX_CMD_LEN_4, (uint16_t *)Rspi0_rx_buf);
	}
}
